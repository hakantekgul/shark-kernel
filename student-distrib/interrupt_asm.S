#define ASM  1
#include "x86_desc.h"

.global system_call_handler

#define INT_HANDLER(name,goto_function)\
.GLOBL name                            ;\
name:                                  ;\
	pushl %ebp                         ;\
	pushl %esi                         ;\
	pushl %edi                         ;\
	pushl %ebx                         ;\
	pushl %ecx                         ;\
	pushl %edx                         ;\
	pushl %eax                         ;\
	pushfl                             ;\
	call goto_function                 ;\
	popfl                              ;\
	popl %eax                          ;\
	popl %edx                          ;\
	popl %ecx                          ;\
	popl %ebx                          ;\
	popl %edi                          ;\
	popl %esi                          ;\
	popl %ebp                          ;\
	iret                               ;\

INT_HANDLER(keyboard_int, keyboard_handler1);
INT_HANDLER(rtc_int,handle_rtc_interrupt);
INT_HANDLER(pit_int, pit_interrupt_handler);
INT_HANDLER(mouse_int, mouse_handler);

syscall_jmptable:

.long 0x0, halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

#### system call handler x86 function
#### provides asm linkage for system calls
#### pushes all registers and flags, checks for invalid syscall number, then restores everything and irets
system_call_handler:

# system call calling convention: EBX, ECX, EDX, ESI, EDI, EBP
	
	pushl %ebx #push all registers 
	pushl %ecx
	pushl %edx
	pushl %esi
	pushl %edi
	pushl %ebp 
	pushfl

	pushl %edx #Argument 3
	pushl %ecx #Argument 2
	pushl %ebx #Argument 1

	cmpl $9, %eax #9 is the highest system call 
	jg invalid
	cmpl $1, %eax
	jl invalid 

	
	call *syscall_jmptable(,%eax,4)
	
	jmp ret


invalid: 

	movl $-1, %eax 

ret: 
	addl $12, %esp #pop arguments from stack

	popfl
	popl %ebp
	popl %edi
	popl %esi
	popl %edx
	popl %ecx
	popl %ebx

	iret 



