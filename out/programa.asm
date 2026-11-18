bits 32
extern _printf
global _main

section .data
_format_int db 37, 100, 10, 0
_format_real db 37, 102, 10, 0
_format_str db 37, 115, 10, 0
_cw_normal dw 0x037f
_cw_truncar dw 0x0f7f
_tmp_entero dd 0
_tmp_real_izquierdo dq 0.0
_tmp_real_derecho dq 0.0
_tmp_real_resultado dq 0.0
_tmp_arg0 dq 0.0
_tmp_arg1 dq 0.0
_v_suma_a dd 0
_v_suma_b dd 0
_v_main_x dd 0
_v_main_y dq 0
_v_main_i dd 0
_cte_real_0 dq __float64__(0.0)
_cte_real_1 dq __float64__(2.5)
_cadena_0 db 111, 107, 0
_cadena_1 db 110, 111, 0

section .text
_paso_0:
_fn_suma:
    push ebp
    mov ebp, esp
    push ebx
    mov eax, [ebp + 8]
    mov [_v_suma_a], eax
    mov eax, [ebp + 12]
    mov [_v_suma_b], eax
_paso_1:
_paso_2:
_paso_3:
_paso_4:
_paso_5:
    push dword [_v_suma_a]
_paso_6:
    push dword [_v_suma_b]
_paso_7:
    pop ebx
    pop eax
    add eax, ebx
    push eax
_paso_8:
    pop dword [_tmp_entero]
    mov eax, [_tmp_entero]
    mov ebx, [ebp - 4]
    leave
    ret
_paso_9:
    xor eax, eax
    mov ebx, [ebp - 4]
    leave
    ret
_paso_10:
_main:
    push ebp
    mov ebp, esp
    push ebx
_paso_11:
    push dword [_v_main_x]
_paso_12:
    push dword 3
_paso_13:
    pop dword [_v_main_x]
    add esp, 4
_paso_14:
    sub esp, 8
    fld qword [_v_main_y]
    fstp qword [esp]
_paso_15:
    sub esp, 8
    fld qword [_cte_real_0]
    fstp qword [esp]
_paso_16:
    fld qword [esp]
    add esp, 8
    fstp qword [_v_main_y]
    add esp, 8
_paso_17:
    sub esp, 8
    fld qword [_v_main_y]
    fstp qword [esp]
_paso_18:
    push dword [_v_main_x]
_paso_19:
    sub esp, 8
    fld qword [_cte_real_1]
    fstp qword [esp]
_paso_20:
    fld qword [esp]
    add esp, 8
    fstp qword [_tmp_real_derecho]
    pop dword [_tmp_entero]
    fild dword [_tmp_entero]
    fstp qword [_tmp_real_izquierdo]
    fld qword [_tmp_real_izquierdo]
    fadd qword [_tmp_real_derecho]
    sub esp, 8
    fstp qword [esp]
_paso_21:
    fld qword [esp]
    add esp, 8
    fstp qword [_v_main_y]
    add esp, 8
_paso_22:
    push dword [_v_main_x]
_paso_23:
    push dword 5
_paso_24:
    pop ebx
    pop eax
    cmp eax, ebx
    setl al
    movzx eax, al
    push eax
_paso_25:
    pop eax
    test eax, eax
    jz _paso_31
_paso_26:
_paso_27:
    push dword _cadena_0
    push dword _format_str
    call _printf
    add esp, 8
_paso_28:
    sub esp, 8
    fld qword [_v_main_y]
    fstp qword [esp]
_paso_29:
    fld qword [esp]
    add esp, 8
    fstp qword [_tmp_real_resultado]
    push dword [_tmp_real_resultado + 4]
    push dword [_tmp_real_resultado]
    push dword _format_real
    call _printf
    add esp, 12
_paso_30:
    jmp _paso_33
_paso_31:
_paso_32:
    push dword _cadena_1
    push dword _format_str
    call _printf
    add esp, 8
_paso_33:
    push dword [_v_main_i]
_paso_34:
    push dword 3
_paso_35:
    pop dword [_v_main_i]
    add esp, 4
_paso_36:
    push dword [_v_main_i]
_paso_37:
    push dword 0
_paso_38:
    pop ebx
    pop eax
    cmp eax, ebx
    setg al
    movzx eax, al
    push eax
_paso_39:
    pop eax
    test eax, eax
    jz _paso_50
_paso_40:
    push dword [_v_main_x]
_paso_41:
    push dword [_v_main_x]
_paso_42:
    push dword 1
_paso_43:
    pop ebx
    pop eax
    sub eax, ebx
    push eax
_paso_44:
    pop dword [_v_main_x]
    add esp, 4
_paso_45:
    push dword [_v_main_i]
_paso_46:
    push dword 1
_paso_47:
    pop ebx
    pop eax
    sub eax, ebx
    push eax
_paso_48:
