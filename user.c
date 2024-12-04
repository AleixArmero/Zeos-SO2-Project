#include <libc.h>

char buff[24];

void *test (void *value)
{
	write(1, "\nThread param= ",16);
	write(1, (char *)value, strlen((char *)value));
	write(1, "\n", 1);
	
	exit();
	write(1, "Not to be written",  17);
	return 0;
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  write (1, "\nFork threat test...\n", 21);
  
  int pid = fork ();
  int tid = 0;
  if (pid > 0)
  	write (1, "Parent ok!\n", 11);
  else if (pid == 0) {
  	write (1, "Child ok!\n", 10);
  	char value2[] = "thread 1\n";
  	char value3[] = "thread 2\n";
  	create_thread ((void *) test, 2, (void *) value2);
  	tid = create_thread ((void *) test, 2, (void *) value3);
  }
  else
  	write (1, "Fork error!\n", 12);
  
  itoa (tid, buff);
  write (1, "\nThread TID =", 14);
  write (1, buff, strlen(buff));
  write (1, "\n", 1);    
  
  while(1) {}
}
