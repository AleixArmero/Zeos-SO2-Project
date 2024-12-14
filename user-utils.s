# 0 "user-utils.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "user-utils.S"
# 1 "include/asm.h" 1
# 2 "user-utils.S" 2

.globl syscall_sysenter; .type syscall_sysenter, @function; .align 0; syscall_sysenter:
 push %ecx
 push %edx
 push $SYSENTER_RETURN
 push %ebp
 mov %esp, %ebp
 sysenter
.globl SYSENTER_RETURN; .type SYSENTER_RETURN, @function; .align 0; SYSENTER_RETURN:
 pop %ebp
 pop %edx
 pop %edx
 pop %ecx
 ret


.globl write; .type write, @function; .align 0; write:
 pushl %ebp
 movl %esp, %ebp
 pushl %ebx;
 movl $4, %eax
 movl 0x8(%ebp), %ebx;
 movl 0xC(%ebp), %ecx;
 movl 0x10(%ebp), %edx;
 call syscall_sysenter
 popl %ebx
 test %eax, %eax
 js nok
 popl %ebp
 ret


nok:
 neg %eax
 mov %eax, errno
 mov $-1, %eax
 popl %ebp
 ret


.globl gettime; .type gettime, @function; .align 0; gettime:
 pushl %ebp
 movl %esp, %ebp
 movl $10, %eax
 call syscall_sysenter
 popl %ebp
 ret


.globl getpid; .type getpid, @function; .align 0; getpid:
 pushl %ebp
 movl %esp, %ebp
 movl $20, %eax
 call syscall_sysenter
 popl %ebp
 ret


.globl fork; .type fork, @function; .align 0; fork:
 pushl %ebp
 movl %esp, %ebp
 movl $2, %eax
 call syscall_sysenter
 test %eax, %eax
 js nok
 popl %ebp
 ret


.globl exit; .type exit, @function; .align 0; exit:
 pushl %ebp
 movl %esp, %ebp
 movl $1, %eax
 call syscall_sysenter
 popl %ebp
 ret


.globl yield; .type yield, @function; .align 0; yield:
 pushl %ebp
 movl %esp, %ebp
 movl $13, %eax
 call syscall_sysenter
 popl %ebp
 ret


.globl get_stats; .type get_stats, @function; .align 0; get_stats:
 pushl %ebp
 movl %esp, %ebp
 pushl %ebx;
 movl $35, %eax
 movl 0x8(%ebp), %ebx;
 movl 0xC(%ebp), %ecx;
 call syscall_sysenter
 popl %ebx
 test %eax, %eax
 js nok
 popl %ebp
 ret


.globl getKey; .type getKey, @function; .align 0; getKey:
 pushl %ebp
 movl %esp, %ebp
 pushl %ebx
 movl $36, %eax
 movl 8(%ebp), %ebx
 movl 12(%ebp), %ecx
 call syscall_sysenter
 popl %ebx
 test %eax, %eax
 js nok
 popl %ebp
 ret


.globl gotoXY; .type gotoXY, @function; .align 0; gotoXY:
 pushl %ebp
 movl %esp, %ebp
 pushl %ebx
 movl $37, %eax
 movl 8(%ebp), %ebx
 movl 12(%ebp), %ecx
 call syscall_sysenter
 popl %ebx
 test %eax, %eax
 js nok
 popl %ebp
 ret


.globl changeColor; .type changeColor, @function; .align 0; changeColor:
 pushl %ebp
 movl %esp, %ebp
 pushl %ebx
 movl $38, %eax
 movl 8(%ebp), %ebx
 movl 12(%ebp), %ecx
 call syscall_sysenter
 popl %ebx
 test %eax, %eax
 js nok
 popl %ebp
 ret


.globl clrscr; .type clrscr, @function; .align 0; clrscr:
 pushl %ebp
 movl %esp, %ebp
 pushl %ebx
 movl $39, %eax
 movl 8(%ebp), %ebx
 call syscall_sysenter
 popl %ebx
 test %eax, %eax
 js nok
 popl %ebp
 ret


.globl create_thread; .type create_thread, @function; .align 0; create_thread:
 pushl %ebp
 movl %esp, %ebp
 pushl %ebx
 movl $40, %eax
 movl 8(%ebp), %ebx
 movl 12(%ebp), %ecx
 movl 16(%ebp), %edx
 call syscall_sysenter
 popl %ebx
 test %eax, %eax
 js nok
 popl %ebp
 ret


.globl semCreate; .type semCreate, @function; .align 0; semCreate:
 pushl %ebp
 movl %esp, %ebp
 pushl %ebx
 movl $41, %eax
 movl 8(%ebp), %ebx
 call syscall_sysenter
 popl %ebx
 popl %ebp
 ret


.globl semWait; .type semWait, @function; .align 0; semWait:
 pushl %ebp
 movl %esp, %ebp
 pushl %ebx
 movl $42, %eax
 movl 8(%ebp), %ebx
 call syscall_sysenter
 popl %ebx
 test %eax, %eax
 js nok
 popl %ebp
 ret


.globl semSignal; .type semSignal, @function; .align 0; semSignal:
 pushl %ebp
 movl %esp, %ebp
 pushl %ebx
 movl $43, %eax
 movl 8(%ebp), %ebx
 call syscall_sysenter
 popl %ebx
 test %eax, %eax
 js nok
 popl %ebp
 ret


.globl semDestroy; .type semDestroy, @function; .align 0; semDestroy:
 pushl %ebp
 movl %esp, %ebp
 pushl %ebx
 movl $44, %eax
 movl 8(%ebp), %ebx
 call syscall_sysenter
 popl %ebx
 test %eax, %eax
 js nok
 popl %ebp
 ret


.globl memRegGet; .type memRegGet, @function; .align 0; memRegGet:
 pushl %ebp
 movl %esp, %ebp
 pushl %ebx
 movl $45, %eax
 movl 8(%ebp), %ebx
 call syscall_sysenter
 popl %ebx
 popl %ebp
 ret


.globl memRegDel; .type memRegDel, @function; .align 0; memRegDel:
 pushl %ebp
 movl %esp, %ebp
 pushl %ebx
 movl $46, %eax
 movl 8(%ebp), %ebx
 call syscall_sysenter
 popl %ebx
 test %eax, %eax
 js nok
 popl %ebp
 ret

.globl SAVE_REGS; .type SAVE_REGS, @function; .align 0; SAVE_REGS:
      pushl %eax
      movl %eax, REGS
      lea REGS, %eax
      movl %ebp, 4(%eax)
      movl %edi, 8(%eax)
      movl %esi, 12(%eax)
      movl %edx, 16(%eax)
      movl %ecx, 20(%eax)
      movl %ebx, 24(%eax)
      popl %eax
      ret

.globl RESTORE_REGS; .type RESTORE_REGS, @function; .align 0; RESTORE_REGS:
      lea REGS , %eax
      movl 4(%eax) , %ebp
      movl 8(%eax) , %edi
      movl 12(%eax), %esi
      movl 16(%eax), %edx
      movl 20(%eax), %ecx
      movl 24(%eax), %ebx
      movl (%eax), %eax
      ret