MVI A, 30 ; A = 30
MVI B, 10 ; B = 10
MVI C, 5  ; C = 5
SUB C     ; A = A + C = 30 - 5 = 25
OUT 0     ; Print A = 25
SUB B     ; A = A - B = 25 - 10 = 15
OUT 0     ; Print A = 15
SUI 15    ; A = A - 15 = 15
OUT 0     ; Print A = 0