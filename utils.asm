global write_string
global write_line
global read_line

SECTION .bss
buffer: resb 256        ; line buffer for read_line
cursor_offset: resq 1   ; VGA cursor offset

SECTION .text

; ---------------------------------------------------------
; write_string(char* str)
; prints until null byte
; ---------------------------------------------------------
write_string:
    mov rsi, rdi          ; rsi = string pointer
.next_char:
    mov al, [rsi]
    cmp al, 0
    je .done
    mov rbx, 0xB8000
    mov rcx, [cursor_offset]
    mov ah, 0x0F          ; white on black
    mov [rbx + rcx*2], ax
    inc rcx
    mov [cursor_offset], rcx
    inc rsi
    jmp .next_char
.done:
    ret

; ---------------------------------------------------------
; write_line(char* str)
; prints string then newline
; ---------------------------------------------------------
write_line:
    call write_string
    mov rdi, nl
    call write_string
    ret
nl: db 0x0D,0x0A,0      ; CR+LF

; ---------------------------------------------------------
; read_line() -> returns pointer to static buffer
; reads until Enter, stores 0-terminated string
; ---------------------------------------------------------
read_line:
    mov rsi, buffer
.read_loop:
.wait_key:
    in al, 0x64
    test al, 1
    jz .wait_key
    in al, 0x60           ; read scancode

    cmp al, 0x1C           ; Enter
    je .done

    cmp al, 0x0E           ; Backspace
    je .backspace

    ; naive scancode → ASCII mapping (a-z only)
    mov bl, al
    sub bl, 0x1E
    add bl, 'a'-1
    mov [rsi], bl
    inc rsi

    mov rdi, rsi
    dec rdi
    call write_string       ; echo typed char
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

bs: db 0x08,0x20,0x08,0   ; backspace visual
