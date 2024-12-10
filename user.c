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


  /*He limitat el nombre de variables dinamiques a 2 per poder fer probes fácilment*/

  write (1, "\nDinamic memory test...\n", 24);

  /*Primer comprobem que la reserva de memória es faci correctament i que arribi fins el limit*/

  char *m = memRegGet(1);

  m[0] = 1;
  m[1023] = 1;
  m[1023*4 + 3] = 1;

  /*Ara comprobem que el fill hereta la variable i que no en pot crear un altre (la funció retorna NULL), ja que tenim 2 variables
  dinàmiques en total (la del pare i la copia al fill). Fem servir un semàfor per assegurar que es fa com volem*/

  s = semCreate(0);

  int pid = fork();

  if (pid == 0) {
    m[0] = 2;
    char *n = memRegGet(1);
    itoa((int) n >> 12, buff);
    write(1, buff, strlen(buff));
    write(1, "\n", 1);
    m[0] = 1;
    n[0] = 1;
    semSignal(s);
    exit();
  }

  semWait(s);

  /*Comprovem ara que si tenim 2 variables dinàmiques no podem crear un fill, ja que no podrem crear 2 noves variables
  dinàmiques per el fill*/

  char *n = memRegGet(1);

  pid = fork();
  perror();
  write(1, "\n", 1);

  /*Comprovem que al borrar aquesta variable dinàmica podem tornar a crear un fill i de pas que les variables son ben esborrades*/

  memRegDel(n);
  pid = fork();
  if (pid == 0) {

    write(1, "Soy el hijo\n", 13);
    exit();

  }

  /*Per últim comprobem que accedir a una variable que hem esborrat provoca un page fault*/
  n[0] = 1;


  while(1) {  

    /*itoa((int)m[0], buff);
    write(1, buff, strlen(buff));*/

  }
}
