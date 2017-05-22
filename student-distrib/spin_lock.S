#define ASM  1
#include "x86_desc.h"

.global spin_lock
.global spin_unlock

lock:
	.byte 0

spin_lock:

	pushl %ebp
	movl %esp, %ebp
	movl 8(%ebp), %ecx
	movl $1, %eax

loop:
	lock xchgl %eax,(%ecx) 	# EAX’ = M[ECX]; M[ECX]’ = EAX
	testl %eax, %eax
	jnz loop
	leave
	ret

spin_unlock:
	pushl %ebp
	movl %esp, %ebp
	movl 8(%ebp), %ecx
	movl $0, %eax
	lock xchgl %eax, (%ecx)  
	leave 
	ret 
	



