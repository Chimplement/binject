BITS 32

    push edx                ; save DT_FINI

    call get_ip             ; calculate position independent address of message
    add eax, message - $
    mov ecx, eax

    mov eax, 4              ; write
    mov ebx, 1              ; stdout
    mov edx, message_len
    int 0x80                ; syscall
    
    pop edx                 ; restore DT_FINI

    ret                     ; return to original entry point

get_ip:
    pop eax
    push eax
    ret

message:
    db "Hello, World!", 10
message_len: equ $ - message