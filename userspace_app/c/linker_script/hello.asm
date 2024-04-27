
	global _start

	section .text
_start:
	; strlen(message)
	mov	rdi, message
	call	strlen
	mov	rdx, rax

	; write(1, message, 13)
	mov	rax, 1
	mov	rdi, 1
	mov	rsi, message
	syscall

	; exit(0)
	mov	eax, 60
	xor	rdi, rdi
	syscall

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
	db	"Hello, World", 10
