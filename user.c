#include <libc.h>

char buff[24];

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  write (1, "\nColor test", 11);
  gotoXY (39, 12);
  changeColor (14, 1); // fg yellow, bg blue
  write (1, "Color test", 10);
      
  
  while(1) {
    
  }
}
