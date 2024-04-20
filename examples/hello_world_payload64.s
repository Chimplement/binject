BITS 64

    push rdx                ; save DT_FINI

    mov rax, 1              ; write
    mov rdi, 1              ; stdout
    lea rsi, [rel message]
    mov rdx, message_len
    syscall
    
    pop rdx                 ; restore DT_FINI

    ret                     ; return to original entry point

message:
    db "Hello, World!", 10
message_len: equ $ - message