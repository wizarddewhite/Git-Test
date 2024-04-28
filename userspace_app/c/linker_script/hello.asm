
	global _start

	section .text
_start:
	; print(message)
	mov	rdi, message
	call	print

	mov	rdi, message
	add	rdi, 14
	call	print

	; exit(0)
	mov	eax, 60
	xor	rdi, rdi
	syscall

; ------------------------------------------------------------------------
; int print(char* p_str);
; ------------------------------------------------------------------------
print:
	push	rdi

	; strlen(p_str)
	call	strlen
	mov	rdx, rax

	; write(1, p_str, strlen(p_str))
	mov	rax, 1
	mov	rdi, 1
	pop	rsi
	syscall

	ret
; ------------------------------------------------------------------------

; ------------------------------------------------------------------------
; int strlen(char* p_str);
; ------------------------------------------------------------------------
strlen:
	mov	rax, 0			; 字符串长度开始是 0
	mov	rsi, rdi		; rdi 是函数参数,rsi 指向首地址

.1:
	cmp	byte [rsi], 0		; 看 rsi 指向的字符是否是 '\0'
	jz	.2			; 如果是 '\0'，程序结束
	inc	rsi			; 如果不是 '\0'，rsi 指向下一个字符
	inc	rax			;	  并且，eax 自加一
	jmp	.1			; 如此循环

.2:
	ret				; 函数结束，返回
; ------------------------------------------------------------------------


	section .data
message:
	db	"Hello, World", 10, 0
	db	"This is a test for nasm", 10, 0
