global loader       ; Entry point

    KERNEL_STACK_SIZE equ 4096
    MAGIC_NUMBER equ 0x1BADB002
    FLAGS        equ 0x0
    CHECKSUM     equ -MAGIC_NUMBER

section .bss
    align 4
    kernel_stack:
        resb KERNEL_STACK_SIZE

section .text:
    align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM
    mov esp, kernel_stack + KERNEL_STACK_SIZE

    extern sum_of_three
    push dword 3
    push dword 2
    push dword 1
    call sum_of_three

loader:
    mov eax, 0xCAFEBABE
.loop:
    jmp .loop
