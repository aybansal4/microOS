global write_string
global write_line
global read_line

SECTION .bss
buffer: resb 256        ; line buffer

SECTION .text

; write_string(char* str)
write_string:
    push rdi
.next_char:
    mov al, [rdi]
    cmp al, 0
    je .done
    mov rbx, 0xB8000
    mov ah, 0x0F
    mov [rbx], ax
    add rdi, 1
    jmp .next_char
.done:
    pop rdi
    ret

; write_line(char* str)
write_line:
    call write_string
    mov rdi, nl
    call write_string
    ret
nl: db 0x0D,0x0A,0

; read_line() -> returns pointer to static buffer
read_line:
    mov rsi, buffer
.read_loop:
.wait_key:
    in al, 0x64
    test al, 1
    jz .wait_key
    in al, 0x60

    cmp al, 0x1C
    je .done

    cmp al, 0x0E
    je .backspace

    ; naive scancode -> ASCII mapping (expand as needed)
    mov bl, al
    sub bl, 0x1D
    add bl, 'a' - 1

    mov [rsi], bl
    inc rsi

    mov rdi, rsi
    dec rdi
    call write_string
    jmp .read_loop

.backspace:
    cmp rsi, buffer
    je .read_loop
    dec rsi
    mov byte [rsi], 0
    mov rdi, bs
    call write_string
    jmp .read_loop

.done:
    mov byte [rsi], 0
    mov rax, buffer
    ret

bs: db 0x08,0x20,0x08,0
