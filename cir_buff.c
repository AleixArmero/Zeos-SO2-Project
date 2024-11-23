#include <cir_buff.h>

#define MAX_BUFF 3

char keyBuff[MAX_BUFF];
int  keyHead, keyTail = 0;
int  numKeys = 0;

// indica si el buffer circular está vacío
int buff_empty ()
{
    return numKeys == 0;
}

// retorna el primer carácter i mueve la cabeza
// a la siguiente posición
char buff_head () {
    char c = '\0';
   
    if (numKeys > 0) {
        c = keyBuff[keyHead];
        keyHead = (keyHead + 1)%MAX_BUFF;
        numKeys--;
    }
   
   return c;
}

// introduce un carácter en el buffer circular
void buff_tail (char c)
{
    if (keyTail == keyHead && numKeys > 0) {
        // la cola ha atrapado la cabeza
        // hay que sobreescribir una tecla
        keyHead = (keyHead + 1)%MAX_BUFF;
    }

	// escribimos la nueva tecla
	keyBuff[keyTail] = c;
    keyTail = (keyTail + 1)%MAX_BUFF;
	  
	// tenemos una tecla más en el buffer
	if (numKeys < MAX_BUFF)
	    numKeys++;
}
