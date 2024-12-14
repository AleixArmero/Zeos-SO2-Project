#define MAX_ZONES 20

/* La idea seria tenir un array de vm_struct y que cada procés (o thread) tingués la seva própia,
però llavors al crear un thread o un procés l'haurem d'inicialitzar com toqui. En cada PCB posem el punter
que apunta al vm_struct i ja tindriem accés sempre. El problema es que crec que els threads i el procés haurien de compartir
l'struct, ja que també comparteixen taula de págines, però no estic del tot segur.

Amb aixó tindriem per a cada procés (o thread) un array, que hauriem de mantenir ordenat quan fer una inserció
o borrem, que per a cada adreça tenim el nombre de pagines, cosa que ens facilita després esborrar tota aquella zona,
cosa que fa que qual volguem implementar la memória dinàmica només fará falta consultar aquesta taula i facilment podrem
veure quina es la zona de memória podem reservar i, quan es faci el alliberament, també podrem alliberar facilment les
pagines físiques reservades, ja que ja sabrem quantes págines ocupa la zona reservada 

Pel que fa la la inicialització, quan es fa un fork al nou PCB se li haurá d'assignar un punter. Per veure si s'esta fent servir o no
he afegit un camp using que només pot valer 0 o 1. Un cop assignat, no sé si s'hauria de copiar completament tota l'estructura, ja que 
no se si els stacks reservats pels threads també han de ser transpasats al nou procés.

En cas de no haber de transpasar també els threads, s'hauria d'afegir un nou camp al vm_area_struct per distingir quina zona es de thread
i quina es de memória dinàmica.

He hagut de definir un nombre de zones de memória reservades màxim perque no tenim memória dinàmica en el kernel, es a dir, no podem reservar
mes memória de la que tenim per el kernel, sinó que aquesta es fixa, per tant si o si necessitem un array fix. De nombre màxim de zones he posat
un per a cada possible PCB (10) y després 10 extres per que en cas de tenir tot threats, encara es puguin reservar unes quantes zones de memoria
per memória dinàmica.

Pots cambiar els structs com vulguis i et sigui necesari, no hi ha cap problema.

Suposant que tenim el punter a un d'aquests arrays a la PCB y que les insercions i esborraments es fan bé, a l'hora de crear un thread hauriem de (apart de
fer les comprobacions sobre els paràmetres pasats a la funció) mirar l'array per veure si hi ha algún espai entre dues areas on hi cápiga el nou stack i
guardar aquesta direcció, o en cas de que no hi hagi agafar la direcció final i posar el stack a partir d'allá. Amb aquesta direcció guardada, tot es igual
que en el fork però aquest cop absolutament totes les zones de memória son compartides. De fet, la taula de pàgines és compartida, així que crec que no es
necesari fer la copia de tot, sinó només assignar la mateixa taula de pàgines. Un cop fet aixó, a més també s'hauria de reservar la nova zona de memória
desde el procés principal (que com es comparteix la taula de pàgines, es el mateix que si ho fes el thread) i ja només faltaria cambiar el esp del thread per
que apunti al fons de la nova zona i el seu eip perque apunti a la funció que li toca fer (això ja estaba ben implementat crec).

Després, al fer l'exit del thread, el primer que hauriem de fer es detectar que realment és un thread (amb algún boolea en el PCB o el que sigui) i
després fer el mateix que fariem amb el exit d'un procés però a més esborrant la zona de memòria que li habiem assignat com a stack. Per detectar en
quina zona de memória es troba l'stack podriem, per exemple, guardar en el PCB la direcció de memória on comença el seu stack (per fer-ho fàcil).
Al compartir-se el punter a l'array de zones de memória virtual, si l'eliminem del del thread, quan ho volguem mirar des del procés tampoc hi serà,
per tant no hem de fer res des del procés. El mateix passa amb les pàgines, donat que la taula de pàgines es compartida, al esborrar la zona d'stack
des del thread, el procés també la tindrá esborrada.
*/

struct vm_area_struct {
    unsigned long vm_start;         // Dirección de inicio del área
    unsigned long N;                // Numero de paginas de el area
};

struct vm_struct {
  struct vm_area_struct areas[MAX_ZONES];
  unsigned long count;
  unsigned int using;
};