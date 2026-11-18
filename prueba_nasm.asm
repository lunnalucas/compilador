global _main
extern _printf
extern _exit

section .data
    mensaje db "¡Prueba de NASM exitosa! El valor es: %d", 10, 0
    valor   dd 42

section .text
_main:
    ; Pasamos el número 42 y el mensaje a printf
    push dword [valor]
    push mensaje
    call _printf
    add esp, 8      ; Limpiamos la pila

    ; Salimos del programa limpiamente
    push 0
    call _exit