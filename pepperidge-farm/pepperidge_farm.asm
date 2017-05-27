#define STDIN "#x00"
#define STDOUT "#x01"

#define O_RDONLY "#x00"

#define SEEK_SET "#x00"
#define SEEK_END "#x02"

#define SYS_OPEN "#x02"
#define SYS_READ "#x03"
#define SYS_WRITE "#x04"
#define SYS_CLOSE "#x05"
#define SYS_LSEEK "#x11"

#define I0_OFFSET "#x00"
#define I1_OFFSET "#x02"
#define I2_OFFSET "#x04"
#define I3_OFFSET "#x06"
#define I4_OFFSET "#x08"
#define I5_OFFSET "#x0A"
#define I6_OFFSET "#x0C"
#define I7_OFFSET "#x0E"
#define I8_OFFSET "#x10"
#define I9_OFFSET "#x12"
#define I10_OFFSET "#x14"
#define I11_OFFSET "#x16"
#define I12_OFFSET "#x18"
#define I13_OFFSET "#x1A"
#define I14_OFFSET "#x1C"
#define I15_OFFSET "#x1E"

.label open
    push    r2
    mov     r2, r1          ; open(filename, mode)
    mov     r1, r0
    movb    r0, SYS_OPEN
    syscall
    pop     r2
    ret

.label seek
    push    r3              ; lseek(fd, offset, whence)
    mov     r3, r2
    mov     r2, r1
    mov     r1, r0
    movb    r0, SYS_LSEEK
    syscall
    pop     r3
    ret

.label read
    push    r3
    mov     r3, r2          ; read(fd, buffer, size)
    mov     r2, r1
    mov     r1, r0
    movb    r0, SYS_READ
    syscall
    pop     r3
    ret

.label write
    push    r3
    mov     r3, r2          ; write(fd, buffer, size)
    mov     r2, r1
    mov     r1, r0
    movb    r0, SYS_WRITE
    syscall
    pop     r3
    ret

.label close
    push    r1
    mov     r1, r0          ; close(fd)
    movb    r0, SYS_CLOSE
    syscall
    pop     r1
    ret

.label strlen
    push    r1              ; save r1 and r2
    push    r2
    movl    r1, #0          ; initialize r1 and r2
    movl    r2, #0
    .label strlen_loop
        mov     r1, [r0]
        test    r1,r1
        inc     r2
        inc     r0
        jnz     :strlen_loop
    dec     r2              ; place result in r0
    mov     r0, r2
    pop     r2              ; replace r1 and r2
    pop     r1
    ret

.label print
    push    r0              ; save r0 and r1
    push    r1
    push    r2
    mov     r1, r0          ; save buffer pointer
    call    :strlen         ; strlen(buffer)
    mov     r2, r0
    movb    r0, STDOUT
    call    :write          ; write(stdout, buffer, size)
    pop     r2
    pop     r1
    pop     r0
    ret

.label input
    push    r2
    push    r3
    mov     r3, r1          ; read(stdin, buffer, size)
    mov     r2, r0
    movb    r1, STDIN
    movb    r0, SYS_READ
    syscall
    pop     r3
    pop     r2
    ret

.label failure
    movl    r0, :wrong
    call    :print
    ret

.label success
    ; r6 has fd
    ; r5 has buffer ptr
    ; r4 has buffer size
    movl    r0, :right      ; print(success)
    call    :print
    movb    r1, O_RDONLY    ; open(flag, O_RDONLY)
    movl    r0, :flag
    call    :open
    cmpl    r0, #xFFFF      ; handle error case
    jz      :success_error
    mov     r3, r0          ; lseek(fd, 0, SEEK_END)
    movl    r2, SEEK_END
    movl    r1, #0
    call    :seek
    mov     r6, r3
    mov     r4, r0          ; save file size
    inc     r4              ; buffer = alloca(file_size)
    sub     sp, r4
    mov     r5, sp
    movl    r2, SEEK_SET
    movl    r1, #0
    mov     r0, r3          ; lseek(fd, 0, SEEK_SET)
    call    :seek
    mov     r2, r4          ; read(fd, buffer, file_size)
    mov     r1, r5
    mov     r0, r3
    call    :read
    cmpl    r0, #xFFFF      ; handle error case
    jz      :success_error
    mov     r3, r4
    add     r4, r5          ; null-terminate string and print it
    dec     r4
    movb    [r4], #0
    mov     r0, r5
    call    :print
    mov     r0, r6          ; close(fd)
    call    :close
    jmps    :success_return
.label success_error
    call    :contact_admins
.label success_return
    add     sp, r3
    ret

.label decode_hexchar
    push    r1              ; save registers
    mov     r1, [r0]        ; get byte out of string
    cmpb    r1, #x30        ; if byte < '0', it is invalid
    jb      :decode_hexchar_error
    cmpb    r1, #x39        ; if byte < '9', it is a number
    ja      :decode_hexchar_notnum
    subb    r1, #x30        ; convert byte to bits
    jmps    :decode_hexchar_return
.label decode_hexchar_notnum
    cmpb    r1, #x46        ; if byte > 'F', it is invalid
    ja      :decode_hexchar_error
    cmpb    r1, #x40        ; if byte == '@', it is invalid
    jz      :decode_hexchar_error
    subb    r1, #x41        ; convert byte to bits
    addb    r1, #xA
    jmps    :decode_hexchar_return
.label decode_hexchar_error
    movb    r1, #xFF
.label decode_hexchar_return
    mov     r0, r1
    pop     r1
    ret

.label decode_hex
    push    r1              ; save registers
    push    r2
    push    r3
    xor     r2, r2          ; clear result
    xor     r3, r3          ; clear counter
    mov     r1, r0          ; keep string pointer
    .label decode_hex_loop
        mov     r0, r1          ; get next character
        call    :decode_hexchar ; decode_hexchar(buffer)
        cmpb    r0, #xFF
        jz      :decode_hex_fail
        mulb    r2, #x10        ; shift character into place
        add     r2, r0
        inc     r1
        inc     r3
        cmpb     r3, #4
        jnz     :decode_hex_loop
    jmps    :decode_hex_return
.label decode_hex_fail
    movb    r2, #0
.label decode_hex_return
    mov     r0, r2
    pop     r3
    pop     r2
    pop     r1
    ret

.label decode_input
    push    r2
    push    r3
    push    r4
    push    r5
    xor     r4, r4          ; count = 0
    mov     r3, r1          ; save pointer to output array
    mov     r2, r0          ; save pointer to input string
    .label decode_input_loop
        call    :decode_hex     ; decode 4 characters into 1 word
        mov     r5, r0          ; store it into output array
        divl    r5, #x100
        mov     [r3], r5
        inc     r3
        mov     [r3], r0
        inc     r3
        addb    r2, #4          ; advance input string
        mov     r1, r3          ; prepare for next loop iteration
        mov     r0, r2
        inc     r4
        cmpb    r4, #x10        ; loop as long as we haven't processed all input
        jnz     :decode_input_loop
    pop     r5
    pop     r4
    pop     r3
    pop     r2
    ret

.label get_word
    push    r1
    push    r2
    mov     r1, [r0]
    mull    r1, #x100
    inc     r0
    mov     r2, [r0]
    add     r1, r2
    mov     r0, r1
    pop     r2
    pop     r1
    ret

.label check_s0
    push    r1
    push    r2
    mov     r7, r0          ; get user's i0 value
    addb    r0, I0_OFFSET
    call    :get_word
    mov     r1, r0
    mov     r0, r7          ; get user's i10 value
    addb    r0, I10_OFFSET
    call    :get_word
    mov     r2, r0
    movl    r0, :s0         ; get s0 value
    call    :get_word
    xorl    r1, #x4936      ; s0 == (i0 ^ 0x4936) + i10
    add     r1, r2
    xor     r0, r1
    pop     r2
    pop     r1
    ret

.label check_s1
    push    r1
    push    r2
    push    r3
    mov     r7, r0          ; get user's i1 value
    addb    r0, I1_OFFSET
    call    :get_word
    mov     r1, r0
    mov     r0, r7          ; get user's i3 value
    addb    r0, I3_OFFSET
    call    :get_word
    mov     r2, r0
    mov     r0, r7          ; get user's i4 value
    addb    r0, I4_OFFSET
    call    :get_word
    mov     r3, r0
    movl    r0, :s1         ; get s1 value
    call    :get_word
    xorl    r1, #x0FDF      ; s1 == (i1 ^ 0x0FDF) * i3 * i4
    mul     r2, r3
    mul     r1, r2
    xor     r0, r1
    pop     r3
    pop     r2
    pop     r1
    ret

.label check_s2
    push    r1
    push    r2
    push    r3
    mov     r7, r0          ; get user's i2 value
    addb    r0, I2_OFFSET
    call    :get_word
    mov     r1, r0
    mov     r0, r7          ; get user's i7 value
    addb    r0, I7_OFFSET
    call    :get_word
    mov     r2, r0
    mov     r0, r7          ; get user's i6 value
    addb    r0, I6_OFFSET
    call    :get_word
    mov     r3, r0
    movl    r0, :s2         ; get s2 value
    call    :get_word
    xorl    r1, #xC7DF      ; s2 == (i2 ^ 0xC7DF) + (i7 * i6)
    mul     r2, r3
    add     r1, r2
    xor     r0, r1
    pop     r3
    pop     r2
    pop     r1
    ret

.label check_s3
    push    r1
    mov     r7, r0          ; get user's i3 value
    addb    r0, I3_OFFSET
    call    :get_word
    mov     r1, r0
    movl    r0, :s3         ; get s3 value
    call    :get_word
    xorl    r1, #xC5DB      ; s3 == (i3 ^ 0xC5DB) + 0x14AA
    addl    r1, #x14AA
    xor     r0, r1
    pop     r1
    ret

.label check_s4
    push    r1
    push    r2
    mov     r7, r0          ; get user's i4 value
    addb    r0, I4_OFFSET
    call    :get_word
    mov     r1, r0
    mov     r0, r7          ; get user's i15 value
    addb    r0, I15_OFFSET
    call    :get_word
    mov     r2, r0
    movl    r0, :s4         ; get s4 value
    call    :get_word
    mul     r1, r2          ; s4 == i4 * i15
    xor     r0, r1
    pop     r2
    pop     r1
    ret

.label check_s5
    push    r1
    push    r2
    push    r3
    mov     r7, r0          ; get user's i5 value
    addb    r0, I5_OFFSET
    call    :get_word
    mov     r1, r0
    mov     r0, r7          ; get user's i3 value
    addb    r0, I3_OFFSET
    call    :get_word
    mov     r2, r0
    mov     r0, r7          ; get user's i6 value
    addb    r0, I6_OFFSET
    call    :get_word
    mov     r3, r0
    movl    r0, :s5         ; get s5 value
    call    :get_word
    add     r2, r3          ; s5 == i5 + i3 + i6
    add     r1, r2
    xor     r0, r1
    pop     r3
    pop     r2
    pop     r1
    ret

.label check_s6
    push    r1
    mov     r7, r0          ; get user's i6 value
    addb    r0, I6_OFFSET
    call    :get_word
    mov     r1, r0
    movl    r0, :s6         ; get s6 value
    call    :get_word
    addl    r1, #x5432      ; s6 == (i6 + 0x5432) | 0x3008
    orl     r1, #x3008
    xor     r0, r1
    pop     r1
    ret

.label check_s7
    push    r1
    mov     r7, r0          ; get user's i7 value
    addb    r0, I7_OFFSET
    call    :get_word
    mov     r1, r0
    movl    r0, :s7         ; get s7 value
    call    :get_word
    addl    r1, #x1212      ; s7 == i7 + 0x1212
    xor     r0, r1
    pop     r1
    ret

.label check_s8
    push    r1
    mov     r7, r0          ; get user's i8 value
    addb    r0, I8_OFFSET
    call    :get_word
    mov     r1, r0
    movl    r0, :s8         ; get s8 value
    call    :get_word
    xorl    r1, #x8703      ; s8 == i8 ^ 0x8703
    xor     r0, r1
    pop     r1
    ret

.label check_s9
    push    r1
    push    r2
    mov     r7, r0          ; get user's i9 value
    addb    r0, I9_OFFSET
    call    :get_word
    mov     r1, r0
    mov     r0, r7          ; get user's i10 value
    addb    r0, I10_OFFSET
    call    :get_word
    mov     r2, r0
    movl    r0, :s9         ; get s9 value
    call    :get_word
    xorl    r2, #x0A52      ; s9 == (i9 + 0x4004) + (i10 ^ 0x0A52)
    addl    r1, #x4004
    add     r1, r2
    xor     r0, r1
    pop     r2
    pop     r1
    ret

.label check_s10
    push    r1
    mov     r7, r0          ; get user's i10 value
    addb    r0, I10_OFFSET
    call    :get_word
    mov     r1, r0
    movl    r0, :s10        ; get s10 value
    call    :get_word
    xor     r0, r0          ; s10 has no constraints
    pop     r1
    ret

.label check_s11
    push    r1
    push    r2
    mov     r7, r0          ; get user's i11 value
    addb    r0, I11_OFFSET
    call    :get_word
    mov     r1, r0
    mov     r0, r7          ; get user's i8 value
    addb    r0, I8_OFFSET
    call    :get_word
    mov     r2, r0
    movl    r0, :s11        ; get s11 value
    call    :get_word
    add     r1, r2          ; s11 == i11 + i8
    xor     r0, r1
    pop     r2
    pop     r1
    ret

.label check_s12
    push    r1
    mov     r7, r0          ; get user's i12 value
    addb    r0, I12_OFFSET
    call    :get_word
    mov     r1, r0
    movl    r0, :s12        ; get s12 value
    call    :get_word
    xor     r0, r1          ; s12 == i12
    pop     r1
    ret

.label check_s13
    push    r1
    mov     r7, r0          ; get user's i13 value
    addb    r0, I13_OFFSET
    call    :get_word
    mov     r1, r0
    movl    r0, :s13        ; get s13 value
    call    :get_word
    xorl    r1, #x863C      ; s13 = (i13 ^ 0x863C) + 0x1234
    addl    r1, #x1234
    xor     r0, r1
    pop     r1
    ret

.label check_s14
    push    r1
    push    r2
    push    r3
    mov     r7, r0          ; get user's i14 value
    addb    r0, I14_OFFSET
    call    :get_word
    mov     r1, r0
    mov     r0, r7          ; get user's i4 value
    addb    r0, I4_OFFSET
    call    :get_word
    mov     r2, r0
    mov     r0, r7          ; get user's i9 value
    addb    r0, I9_OFFSET
    call    :get_word
    mov     r3, r0
    movl    r0, :s14        ; get s14 value
    call    :get_word
    add     r2, r3          ; s14 == i14 + i4 + i9
    add     r1, r2
    xor     r0, r1
    pop     r3
    pop     r2
    pop     r1
    ret

.label check_s15
    push    r1
    push    r2
    mov     r7, r0          ; get user's i15 value
    addb    r0, I15_OFFSET
    call    :get_word
    mov     r1, r0
    mov     r0, r7          ; get user's i0 value
    addb    r0, I0_OFFSET
    call    :get_word
    mov     r2, r0
    movl    r0, :s15        ; get s15 value
    call    :get_word
    andl    r1, #x0F00      ; s15 == (i15 & 0x0F00) + i0
    add     r1, r2
    xor     r0, r1
    pop     r2
    pop     r1
    ret

.label check
    subb    sp, #32
    mov     r2, sp          ; save copy of pointer to new array
    mov     r1, sp
    call    :decode_input
    mov     r0, r2
    call    :check_s0       ; check i0 value against s0
    test    r0, r0
    jnz     :check_fail
    mov     r0, r2
    call    :check_s1       ; check i1 value against s1
    test    r0, r0
    jnz     :check_fail
    mov     r0, r2
    call    :check_s2       ; check i2 value against s2
    test    r0, r0
    jnz     :check_fail
    mov     r0, r2
    call    :check_s3       ; check i3 value against s3
    test    r0, r0
    jnz     :check_fail
    mov     r0, r2
    call    :check_s4       ; check i4 value against s4
    test    r0, r0
    jnz     :check_fail
    mov     r0, r2
    call    :check_s5       ; check i5 value against s5
    test    r0, r0
    jnz     :check_fail
    mov     r0, r2
    call    :check_s6       ; check i6 value against s6
    test    r0, r0
    jnz     :check_fail
    mov     r0, r2
    call    :check_s7       ; check i7 value against s7
    test    r0, r0
    jnz     :check_fail
    mov     r0, r2
    call    :check_s8       ; check i8 value against s8
    test    r0, r0
    jnz     :check_fail
    mov     r0, r2
    call    :check_s9       ; check i9 value against s9
    test    r0, r0
    jnz     :check_fail
    mov     r0, r2
    call    :check_s10      ; check i10 value against s10
    test    r0, r0
    jnz     :check_fail
    mov     r0, r2
    call    :check_s11      ; check i11 value against s11
    test    r0, r0
    jnz     :check_fail
    mov     r0, r2
    call    :check_s12      ; check i12 value against s12
    test    r0, r0
    jnz     :check_fail
    mov     r0, r2
    call    :check_s13      ; check i13 value against s13
    test    r0, r0
    jnz     :check_fail
    mov     r0, r2
    call    :check_s14      ; check i14 value against s14
    test    r0, r0
    jnz     :check_fail
    mov     r0, r2
    call    :check_s15      ; check i15 value against s15
    test    r0, r0
    jnz     :check_fail
    call    :success
    jmps    :check_ret
.label check_fail
    call    :failure
.label check_ret
    addb    sp, #32
    ret

.label main
    movl    r0, :prompt     ; print(prompt)
    call    :print
    subb    sp, #64         ; char buffer[64]
    mov     r2, sp          ; save string pointer for future call
    movb    r1, #64         ; input(&buffer, 0x40)
    mov     r0, sp
    call    :input
    mov     r0, r2          ; check(buffer)
    call    :check
    addb    sp, #64
    end

.label contact_admins
    movl    r0, :contact_us
    call    :print
    ret
    
.label contact_us
.db "If you think you have the right answer and are seeing this on the server, please contact an admin.",0x0A,0

.label prompt
.db "Enter your registration code: ",0

.label wrong
.db "Code does not match any known registered users",0x0A,0

.label right
.db "Thank you for your patronage!",0x0A,"Your username is: ",0

.label flag
.db "flag",0

.label fake0
.db 0x3C, 0xE3, 0x5A, 0x9E

.label s13
.db 0x91, 0x13

.label fake1
.db 0x8C, 0x05, 0xE4, 0x2E

.label s10
.db 0x0A, 0x52

.label s1
.db 0x3D, 0xD8

.label s0
.db 0x7C, 0xF5

.label fake2
.db 0x9F, 0x4B, 0x9F, 0x06, 0xD7, 0xA8, 0xE9, 0xA0, 0xA6, 0x36, 0xA6, 0x49

.label s8
.db 0x31, 0x36

.label s11
.db 0x93, 0x08

.label s2
.db 0xEB, 0x70

.label fake3
.db 0xEB, 0xBF

.label s5
.db 0xDF, 0x28

.label s3
.db 0x50, 0x0D

.label s4
.db 0x7B, 0xE8

.label fake4
.db 0x96, 0xFA, 0xE8, 0xB7, 0xA5, 0xDF, 0xC2, 0x4D

.label s14
.db 0xF0, 0xB8

.label s6
.db 0x3B, 0x78

.label s9
.db 0x62, 0x72

.label fake5
.db 0xB7, 0x48

.label s7
.db 0x16, 0x97

.label fake6
.db 0xF0, 0x19, 0x2D, 0x6B

.label s12
.db 0x08, 0x5B

.label s15
.db 0x9F, 0x94

.label fake7
.db 0x49, 0x87, 0xE6, 0x24, 0x37, 0x52, 0xEF, 0xB8
