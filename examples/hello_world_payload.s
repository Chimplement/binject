BITS 64

    push rdx

    mov rax, 1
    mov rdi, 1
    lea rsi, [rel message]
    mov rdx, message_len
    syscall
    
    pop rdx

    ret

message:
    db "Hello, World!", 10
message_len: equ $ - message