section .bss
    temp resd 1

section .data
    half dd 0.5
    one dd 1.0
    three dd 3.0
    twohalf dd 2.5
    ninehalf dd 9.5
   
section .text

global f1
global f2
global f3

global df1
global df2
global df3

global d2f1
global d2f2
global d2f3


;float f1(float x)
;Возвращает значение ф-ии f1 = 3(0.5/(x+1)+1)
f1:
    push ebp
    mov ebp, esp
    
    finit
    
    fld dword[half]  ;0.5
    fld dword[ebp+8]  ;x на вход
    fld1
    faddp st1, st0  ;x+1
    fdivp  ;0.5/(x+1)
           ;деление sti на st0
    fld1
    faddp  ;0.5/(x+1) + 1
    fmul dword[three]
    
    mov esp, ebp
    pop ebp
    ret
    
    
    
;float f2(float x)
;Возвращает значение ф-ии f2 = 2.5x-9.5
f2:
    push ebp
    mov ebp, esp
    
    finit
    
    fld dword[ebp+8]  ;x на вход
    fmul dword[twohalf]  ;2.5x
    fsub dword[ninehalf]  ;2.5x-9.5
    
    mov esp, ebp
    pop ebp
    ret
    
    
    
;float f3(float x)
;Возвращает значение ф-ии f3 = 5/x
f3:
    push ebp
    mov ebp, esp
    
    finit
    
    fld dword[three]
    fadd dword[twohalf]
    fsub dword[half]  ;5
    fld dword[ebp+8]  ;x на вход
    fdiv  ;5/x
    
    mov esp, ebp
    pop ebp
    ret
    
    

;float df1(float x)
;Возвращает значение ф-ии (f1)' = -1.5 / (x + 1)^2
df1:
    push ebp
    mov ebp, esp
    
    finit
    
    fld dword[half]
    fadd dword[one]
    fchs  ;-1.5
    fld dword[ebp+8]  ;x на вход
    fld1
    faddp st1, st0  ;x+1
    fmul st0, st0  ;(x+1)^2
    fdivp  ;-1.5/(x+1)^2
           ;деление sti на st0
    
    mov esp, ebp
    pop ebp
    ret
    
    
    
;float df2(float x)
;Возвращает значение ф-ии (f2)' = 2.5
df2:
    push ebp
    mov ebp, esp
    
    finit
    
    fld dword[twohalf]  ;2.5
    
    mov esp, ebp
    pop ebp
    ret
    
    
    
;float df3(float x)
;Возвращает значение ф-ии (f3)' = -5 / x^2
df3:
    push ebp
    mov ebp, esp
    
    finit
    
    fld dword[ebp+8]
    fld dword[ebp+8]
    fmulp  ;x^2
    fld dword[three]
    fadd dword[one]
    fadd dword[one]  ;5
    fdivrp  ;5/x^2
    fchs  ;-5/x^2
    
    mov esp, ebp
    pop ebp
    ret
    
    

;float d2f1(float x)
;Возвращает значение ф-ии (f1)'' = 3 / (x + 1)^3
d2f1:
    push ebp
    mov ebp, esp
    
    finit
    
    fld dword[ebp+8]  ;x на вход
    fadd dword[one]  ;x+1
    fmul st0, st0
    fmul dword[ebp+8]  ;(x+1)^3
    fld dword[three]
    fdivrp  ;3/(x+1)^3
            ;деление st0 на sti
    
    mov esp, ebp
    pop ebp
    ret
    
    
    
;float d2f2(float x)
;Возвращает значение ф-ии (f2)'' = 0
d2f2:
    push ebp
    mov ebp, esp
    
    finit
    
    fld1
    fsub dword[one]
    
    mov esp, ebp
    pop ebp
    ret
    
    
    
;float d2f3(float x)
;Возвращает значение ф-ии (f3)'' = 10 / x^3
d2f3:
    push ebp
    mov ebp, esp
    
    finit
    
    fld dword[ebp+8]
    fld dword[ebp+8]
    fmulp
    fmul dword[ebp+8]  ;x^3
    fld dword[ninehalf]
    fadd dword[half]
    fdivrp  ;10/x^3
    
    mov esp, ebp
    pop ebp
    ret