#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  char c;

  gotoXY(0, 0);

  while(1) {

    getKey(&c, 10000);
    write(1, &c, 1);

    if (c == '.') {
      changeColor(15, 0);
    }

  }
}
