NOP
MVI A, 0  ; A = 0
MVI B, 1  ; B = 1
MVI C, 2  ; C = 2
ADD B     ; A = A + B = 0 + 1 = 1
ADD C     ; A = A + C = 1 + 2 = 3
ADI 10    ; A = A + 10 = 3 + 10 = 13
SUB C     ; A = A - C = 13 - 2 = 11
SUB B     ; A = A - B = 11 - 1 = 10
MOV D, A  ; D = A = 10
ADI 245   ; A = A + 245 = 10 + 245 = 255
ADI 1     ; A = A + 1 = 255 + 1 = 0, AC = 1, CY = 1
ADC B     ; A = A + 1 + CY = 0 + 1 + 1 = 2
ADI 254   ; A = A + 254 = 2 + 254 = 0, AC = 1, CY = 1
ACI 1     ; A = A + 1 + CY = 0 + 1 + 1 = 2
SUI 3     ; A = A - 3 = 2 - 3 = 255, AC = 1, CY = 1
; Add the sub borrow variants here
SBB A
SBI 1
OUT 3
OUT 1
HLT
