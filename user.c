#include <libc.h>

char buff[24];
sem_t *s;
char *md;
int value = 0;

void *test (void *v)
{
	write(1, "\nThread waiting...\n",19);
	
	int e = semWait(s);
	if (e < 0) perror();
	if (value == 0) {
		md = memRegGet(1);
		md = "TEST!\n";
		value++;
	}
	else
		write (1, md, strlen(md));
		
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


  /*He limitat el nombre de variables dinamiques a 2 per poder fer probes fácilment*/

  write (1, "\nDinamic memory test...\n", 24);

  s = semCreate(1);

  int pid = fork();

  if (pid == 0) {
    create_thread(test, 1, &value);
    create_thread(test, 1, &value);
    exit();
  }


  while(1) { }
}
