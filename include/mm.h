/*
 * mm.h - Capçalera del mòdul de gestió de memòria
 */

#ifndef __MM_H__
#define __MM_H__

#include <types.h>
#include <mm_address.h>
#include <sched.h>

 
#define FREE_FRAME 0
#define USED_FRAME 1
/* Bytemap to mark the free physical pages */
extern Byte phys_mem[TOTAL_PAGES];


extern page_table_entry dir_pages[NR_TASKS][TOTAL_PAGES];
extern int pointing_dir[NR_TASKS];

int init_frames( void );
int alloc_frame( void );
void free_frame( unsigned int frame );
void set_user_pages( struct task_struct *task );


extern Descriptor  *gdt;

extern TSS         tss; 

void init_mm();
void set_cr3(page_table_entry *dir);

void setGdt();

void setTSS();

void set_ss_pag(page_table_entry *PT, unsigned page,unsigned frame);
void del_ss_pag(page_table_entry *PT, unsigned page);
unsigned int get_frame(page_table_entry *PT, unsigned int page);
unsigned int is_assigned (page_table_entry *PT, unsigned int logical_page);

#endif  /* __MM_H__ */
