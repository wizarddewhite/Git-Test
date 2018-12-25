# https://0xax.gitbooks.io/linux-insides/SysCall/linux-syscall-1.html
# gcc -c hello-ia64.s 
# ld -o hello hello-ia64.o
.data

msg:
	.ascii "Hello World!\n"
	len = . - msg

.text
	.global _start

_start:
	movq $1, %rax
	movq $1, %rdi
	movq $msg, %rsi
	movq $len, %rdx
	syscall

	movq $60, %rax
	xorq %rdi, %rdi
	syscall
