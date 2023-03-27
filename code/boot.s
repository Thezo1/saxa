global loader       ; Entry point

    KERNEL_STACK_SIZE equ 4096
    MAGIC_NUMBER equ 0x1BADB002
    FLAGS        equ 0x0
    CHECKSUM     equ -MAGIC_NUMBER

; NOTE(Thezo): Exported instructions

global outb         ; Output instructions
    ; send bytes to the output port
    ; stack: [esp + 8] last argument, the data byte(14 or 15)
    ;        [esp + 4] first argument, the port number
    ;        [esp ] return address
    outb:
        mov al, [esp + 8]
        mov dx, [esp + 4]
        out dx, al
        ret

global inb           ; Input instructions
    ; returns bytes from the input port
    ;  stack: [esp + 4] first argument, address of the I/O port
    ;         [esp ] return address
    inb:
        mov dx, [esp + 4]
        in al, dx
        ret

; NOTE(Thezo): Exported instructions

section .bss
    align 4
    kernel_stack:
        resb KERNEL_STACK_SIZE

section .text:
    align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM
loader:
    mov esp, kernel_stack + KERNEL_STACK_SIZE
    extern main
    call main

    mov eax, 0xCAFEBABE     ; Definetly keeping this

.loop:
    jmp .loop
