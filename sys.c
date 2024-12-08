/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>
#include <utils.h>
#include <io.h>
#include <mm.h>
#include <mm_address.h>
#include <sched.h>
#include <p_stats.h>
#include <errno.h>
#include <cir_buff.h>

#define LECTURA 0
#define ESCRIPTURA 1

extern int available_sems[MAX_SEM];
extern struct sem_t sems[MAX_SEM];

extern Byte x, y;
extern Byte color;

extern struct list_head blocked;

int waitKey = 0;

void * get_ebp();
int sys_semDestroy (struct sem_t *s);

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF; 
  if (permissions!=ESCRIPTURA) return -EACCES; 
  return 0;
}

void user_to_system(void)
{
  update_stats(&(current()->p_stats.user_ticks), &(current()->p_stats.elapsed_total_ticks));
}

void system_to_user(void)
{
  update_stats(&(current()->p_stats.system_ticks), &(current()->p_stats.elapsed_total_ticks));
}

int sys_ni_syscall()
{
	return -ENOSYS; 
}

int sys_getpid()
{
	return current()->PID;
}

int global_PID=1000;
int global_TID=1000;

int ret_from_fork()
{
  return 0;
}

int sys_fork(void)
{
  struct list_head *lhcurrent = NULL;
  union task_union *uchild;
  
  /* Any free task_struct? */
  if (list_empty(&freequeue)) return -ENOMEM;

  lhcurrent=list_first(&freequeue);
  
  list_del(lhcurrent);
  
  uchild=(union task_union*)list_head_to_task_struct(lhcurrent);
  
  /* Copy the parent's task struct to child's */
  copy_data(current(), uchild, sizeof(union task_union));
  
  /* new pages dir */
  allocate_DIR((struct task_struct*)uchild);
  
  /* Allocate pages for DATA+STACK */
  int new_ph_pag, pag, i;
  page_table_entry *process_PT = get_PT(&uchild->task);
  page_table_entry *parent_PT = get_PT(current());
  for (pag=PAG_LOG_INIT_DATA; pag<1024; pag++)
  {
     if (is_assigned(parent_PT, pag)) {
      new_ph_pag=alloc_frame();
      if (new_ph_pag!=-1) /* One page allocated */
      {
        set_ss_pag(process_PT, pag, new_ph_pag);
      }
      else /* No more free pages left. Deallocate everything */
      {
        /* Deallocate allocated pages. Up to pag. */
        for (i=PAG_LOG_INIT_DATA; i<pag; i++)
        {
          if (is_assigned(parent_PT, i)) {
            free_frame(get_frame(process_PT, i));
            del_ss_pag(process_PT, i);
          }
        }
        /* Deallocate task_struct */
        list_add_tail(lhcurrent, &freequeue);
        
        /* Return error */
        return -EAGAIN; 
      }
    }
  }
  
  /* Copy parent's SYSTEM and CODE to child. */
  for (pag=0; pag<NUM_PAG_KERNEL; pag++)
  {
    set_ss_pag(process_PT, pag, get_frame(parent_PT, pag));
  }
  for (pag=0; pag<NUM_PAG_CODE; pag++)
  {
    set_ss_pag(process_PT, PAG_LOG_INIT_CODE+pag, get_frame(parent_PT, PAG_LOG_INIT_CODE+pag));
  }
  /* Copy parent's DATA to child. */
  page_table_entry *parent_DIR = get_DIR(current());
  set_ss_pag(parent_DIR, 1, (unsigned)process_PT>>12);
  for (pag=NUM_PAG_KERNEL+NUM_PAG_CODE; pag<NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA; pag++)
  {
    copy_data((void*)(pag<<12), (void*)((0x400+pag)<<12), PAGE_SIZE);
  }
  for (pag=PAG_LOG_INIT_DATA+NUM_PAG_DATA; pag<1024; pag++)
  {
    if (is_assigned(parent_PT, pag))
      copy_data((void*)(pag<<12), (void*)((0x400+pag)<<12), PAGE_SIZE);
  }
  del_ss_pag (parent_DIR, 1);
  
  /* Deny access to the child's memory space */
  set_cr3(get_DIR(current()));

  uchild->task.PID=++global_PID;
  uchild->task.TID=++global_TID;
  uchild->task.state=ST_READY;
  INIT_LIST_HEAD(&uchild->task.threads);
  INIT_LIST_HEAD(&uchild->task.sems);
  
  /* Add thread to thread list */
  list_add_tail (&uchild->task.anchor, &uchild->task.threads);

  int register_ebp;		/* frame pointer */
  /* Map Parent's ebp to child's stack */
  register_ebp = (int) get_ebp();
  register_ebp=(register_ebp - (int)current()) + (int)(uchild);

  uchild->task.register_esp=register_ebp + sizeof(DWord);

  DWord temp_ebp=*(DWord*)register_ebp;
  /* Prepare child stack for context switch */
  uchild->task.register_esp-=sizeof(DWord);
  *(DWord*)(uchild->task.register_esp)=(DWord)&ret_from_fork;
  uchild->task.register_esp-=sizeof(DWord);
  *(DWord*)(uchild->task.register_esp)=temp_ebp;

  /* Set stats to 0 */
  init_stats(&(uchild->task.p_stats));

  /* Queue child process into readyqueue */
  uchild->task.state=ST_READY;
  list_add_tail(&(uchild->task.list), &readyqueue);
  
  return uchild->task.PID;
}

#define TAM_BUFFER 512

int sys_write(int fd, char *buffer, int nbytes) {
char localbuffer [TAM_BUFFER];
int bytes_left;
int ret;

	if ((ret = check_fd(fd, ESCRIPTURA)))
		return ret;
	if (nbytes < 0)
		return -EINVAL;
	if (!access_ok(VERIFY_READ, buffer, nbytes))
		return -EFAULT;
	
	bytes_left = nbytes;
	while (bytes_left > TAM_BUFFER) {
		copy_from_user(buffer, localbuffer, TAM_BUFFER);
		ret = sys_write_console(localbuffer, TAM_BUFFER);
		bytes_left-=ret;
		buffer+=ret;
	}
	if (bytes_left > 0) {
		copy_from_user(buffer, localbuffer,bytes_left);
		ret = sys_write_console(localbuffer, bytes_left);
		bytes_left-=ret;
	}
	return (nbytes-bytes_left);
}


extern int zeos_ticks;

int sys_gettime()
{
  return zeos_ticks;
}

void sys_exit()
{  
  int i;

  page_table_entry *process_PT = get_PT(current());

  pointing_dir[current()->dir]--;

  // Remove user stack pages
  int stack_size = current()->user_stack_size;
  int stack_page = current()->user_stack_page;
  for (i=stack_page; i<stack_page+stack_size; i++)
  {
    free_frame(get_frame(process_PT, i));
    del_ss_pag(process_PT, i);
  }
  
  // Remove from thread list
  list_del (&current()->anchor);
  
  // Find another thread on the list
  if (list_empty(&current()->threads)) {
    // We want to kill the last thread of the process
    // Deallocate all the propietary physical pages
    for (i=0; i<NUM_PAG_DATA; i++)
    {
      free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA+i));
      del_ss_pag(process_PT, PAG_LOG_INIT_DATA+i);
    }
  }
  else {
    set_cr3(get_DIR(current()));
  }
  
  /* Destroy semaphores */
  struct list_head *pos;
  list_for_each(pos, &current()->sems) {
  	struct sem_t *s = list_entry(pos, struct sem_t, anchor);
  	sys_semDestroy(s);
  }
  
  /* Free task_struct */
  list_add_tail(&(current()->list), &freequeue);
  
  current()->PID=-1;
  current()->TID=-1;
  
  /* Restarts execution of the next process */
  sched_next_rr();
}

/* System call to force a task switch */
int sys_yield()
{
  force_task_switch();
  return 0;
}

extern int remaining_quantum;

int sys_get_stats(int pid, struct stats *st)
{
  int i;
  
  if (!access_ok(VERIFY_WRITE, st, sizeof(struct stats))) return -EFAULT; 
  
  if (pid<0) return -EINVAL;
  for (i=0; i<NR_TASKS; i++)
  {
    if (task[i].task.PID==pid)
    {
      task[i].task.p_stats.remaining_ticks=remaining_quantum;
      copy_to_user(&(task[i].task.p_stats), st, sizeof(struct stats));
      return 0;
    }
  }
  return -ESRCH; /*ESRCH */
}

int sys_getKey (char *b, int timeout)
{
    // comprobamos que la dirección dónde se guarda la tecla corresponde
    // al espacio de datos del usuario
    if (!access_ok(VERIFY_WRITE, b, sizeof(char)))
        return -EINVAL;
    
    if (!buff_empty() && waitKey < buff_size())
        *b = buff_head();
    else {
        // si el buffer circular está vacío o no hay suficientes teclas...
        if (timeout <= 0)
            return -EINVAL;
        // esperamos una nueva tecla
        waitKey++;
        current()->p_stats.remaining_ticks = timeout;
        current()->state = ST_BLOCKED;
        list_add_tail (&current()->list, &blocked);
        sched_next_rr ();
        waitKey--;
        // comprobamos si hay tecla
        if (buff_empty ())
           return -EAGAIN;
        // leemos el buffer circular
        *b = buff_head();
    }
    
    return 0;
}

int sys_gotoXY(int new_x, int new_y) {
  if (x < 0 || y < 0 || x > 79 || y > 24)
    return -EINVAL;

  x = (Byte) new_x;
  y = (Byte) new_y;

  return 0;

}


//Background and foreground in this screen just can have values from 0 to F.
int sys_changeColor(int fg, int bg) {
  
  if (fg < 0 || bg < 0 || fg > 15 || bg > 7)
    return -EINVAL;

  //Word *screen = (Word *)0xb8000;

  color = ((Byte)bg << 4) | (Byte)fg;
  
  // we must change color only for next prints
  /*for (int i = 0; i < 25; i++) {
    for (int j = 0; j < 80; j++) {
      //We get the character so we can change its color using the bg and fg given
      char c = screen[i*80 + j] & 0x00FF;
      screen[i*80 + j] = (c & 0x00FF) | ((bg << 4) | fg) << 8;
    }
  }*/

  return 0;
  
}


//b ha de ser o null o una matriu 80x25x2 (on aquests 2 final es el caracter i el color)
int sys_clrscr(char* b) {
  // comprobamos si toda la matriz se encuentra en el espacio del usuario
  if (b != NULL && !access_ok(VERIFY_READ, b, sizeof(char[25][80][2])))
     return -EINVAL;
     
  Word *screen = (Word *)0xb8000;
  
  if (b == NULL) {
    // imprimimos la matriz por defecto
    for (int i = 0; i < 25; i++) {
      for (int j = 0; j < 80; j++) {
        screen[i*80 + j] = 0x0000;
      }
    }

  }

  else {

    /*for (int i = 0; i < 25; i++) {
      for (int j = 0; j < 80; j++) {
        screen[i*80 + j] = (b[i*80 + j*2] & 0x00FF) | (b[i*80 + j*2 + 1]) << 8;
      }
    }*/
    
    // imprimimos la matriz del usuario
    copy_from_user (b, screen, 80*25*2);

  }

  x = 0;
  y = 0;

  return 0;

}

int sys_create_thread (void * (*function)(void *param), int N, void *param)
{

  if (N < 1 || N > 1024 - PAG_LOG_INIT_DATA+NUM_PAG_DATA)
      return -EINVAL;
  
  // Param can be a pointer to a semaphore
  if (!access_ok(VERIFY_READ, function, sizeof(void *)))
      return -EINVAL;

  /*Mirem que ala funció es trobi en la zona de codi, ja que es posible que ens donin una direcció a una funció en la zona
  de kernel (crec, no estic massa segur)*/

  struct list_head *lhcurrent = NULL;
  union task_union *uchild;
  
  /* Any free task_struct? */
  if (list_empty(&freequeue)) return -ENOMEM;

  lhcurrent=list_first(&freequeue);
  
  list_del(lhcurrent);
  
  uchild=(union task_union*)list_head_to_task_struct(lhcurrent);
  
  /* Copy the parent's task struct to child's */
  copy_data(current(), uchild, sizeof(union task_union));
  uchild->task.user_stack_size = N;
  
  /* Allocate pages for STACK */
  int new_ph_pag, i;
  page_table_entry *process_PT = get_PT(&uchild->task);
  int found = 0;
  int pag = PAG_LOG_INIT_DATA+NUM_PAG_DATA-1;
  while (pag < 1024 && !found)
  {
    pag++;
    int available = 0;
    while (available < N && pag+N<1024)
    {
      if (!is_assigned(process_PT, pag))
        available++;
      else
        available=0;
      pag++;
    }
    if (available == N)
      found = 1;
  }
  // if found, pag points to the next page of last log pag of the gap
  
  if (found)
  {
    for (i = pag-N; i < pag; i++)
    {
      new_ph_pag=alloc_frame();
      if (new_ph_pag!=-1) /* One page allocated */
      {
        set_ss_pag(process_PT, i, new_ph_pag);
      }
      else /* No more free pages left. Deallocate everything */
      {
        /* Deallocate allocated pages. */
        for (int j = pag - N; j < pag + i; j++)
        {
          free_frame(get_frame(process_PT, j));
          del_ss_pag(process_PT, j);
        }
        /* Deallocate task_struct */
        list_add_tail(lhcurrent, &freequeue);
      
        /* Return error */
        return -EAGAIN; 
      }
    }
  }
  else
  {
    list_add_tail(lhcurrent, &freequeue);
    return -EAGAIN;
  }

  uchild->task.TID=++global_TID;
  uchild->task.state=ST_READY;
  uchild->task.user_stack_page = pag-N;
  INIT_LIST_HEAD(&uchild->task.sems);

  pointing_dir[current()->dir]++;
  
  /* Add thread to thread list */
  list_add_tail (&uchild->task.anchor, &current()->threads);

  /* Set up thread user stack throught parent */
  unsigned *top = (unsigned *)(((uchild->task.user_stack_page+uchild->task.user_stack_size)<<12)-8);
  *top = 0;
  *(top+1) = (unsigned) param;
  
  // Set up the system stack
  uchild->stack[KERNEL_STACK_SIZE-2] = (unsigned) top;
  uchild->stack[KERNEL_STACK_SIZE-5] = (unsigned) function;

  int register_ebp;		/* frame pointer */
  /* Map Parent's ebp to child's stack */
  register_ebp = (int) get_ebp();
  register_ebp = (register_ebp - (int)current()) + (int)(uchild);

  uchild->task.register_esp=register_ebp;

  /* Set stats to 0 */
  init_stats(&(uchild->task.p_stats));

  /* Queue child process into readyqueue */
  uchild->task.state=ST_READY;
  list_add_tail(&(uchild->task.list), &readyqueue);
  
  return uchild->task.TID;
}

struct sem_t *sys_semCreate (int initial_value)
{
  if (initial_value < 0)
    return NULL;
    
  int i = 0;
  int found = 0;
  
  /* Trying to find an available semaphore */
  while (!found && i < MAX_SEM) {
    if (available_sems[i] == 0) {
      available_sems[i] = 1;
      found = 1;
    }
    i++;
  }
  if (!found)
    return NULL;

  /* Available semaphore found */
  struct sem_t *s = &sems[i-1];
  s->count = initial_value;
  s->parent = current();
  list_add_tail(&s->anchor, &current()->sems);
  INIT_LIST_HEAD(&s->blocked);
  return s;
}

int sys_semWait (struct sem_t *s) {
  int position = ((int) (s - sems)) / sizeof(struct sem_t);
  if (position < 0 || position >= MAX_SEM || !available_sems[position])
    return -EINVAL;
    
  s->count--;
  if (s->count < 0) {
    update_process_state_rr(current(), &s->blocked);
    sched_next_rr();
  }
  
  return 0;
}

int sys_semSignal (struct sem_t *s) {
  int position = ((int) (s - sems)) / sizeof(struct sem_t);
  if (position < 0 || position >= MAX_SEM || !available_sems[position])
    return -EINVAL;
    
  s->count++;
  if (s->count >= 0 && !list_empty(&s->blocked)) {
    struct list_head *l = list_first (&s->blocked);
    struct task_struct *t = list_head_to_task_struct(l);
    update_process_state_rr(t,&readyqueue);
  }
  
  return 0;
}

int sys_semDestroy (struct sem_t *s) {
  int position = ((int) (s - sems)) / sizeof(struct sem_t);
  if (position < 0 || position >= MAX_SEM || !available_sems[position] || s->parent != current())
    return -EINVAL;
  
  struct list_head *pos, *n;
  list_for_each_safe (pos, n, &s->blocked) {
    struct task_struct *t = list_head_to_task_struct (pos);
    update_process_state_rr(t,&readyqueue);
  }
  available_sems[position] = 0;
  
  return 0;
}
