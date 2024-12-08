#include <libc.h>

char buff[24];
int value = 0;
sem_t *s;

void *test (void *v)
{
	write(1, "\nThread waiting...\n",19);
	
	int e = semWait(s);
	if (e < 0) perror();
	value++;
	e = semSignal(s);
	if (e < 0) perror();

	write(1, "\nThread pass!\n", 14);
	
	exit();
	return 0;
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  write (1, "\nSemaphore test...\n", 19);
  
  s = semCreate(0);
  
  int pid = fork ();
  if (pid > 0) {
  	while (gettime() < 1000);
  	exit();
  }
  else if (pid == 0) {
  	create_thread ((void *) test, 1, (void *) &pid);
  	create_thread ((void *) test, 1, (void *) &pid);
  }
  else
  	write (1, "Fork error!\n", 12);
  
  write (1, "\nTest finished!\n", 16);
  
  while(1) {}
}
