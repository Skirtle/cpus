; Initialize A and B for ADD tests
MVI A, 0x0F   ; A = 15 (low nibble full)
MVI B, 0x01   ; B = 1
ADD B         ; Test AC = 1 (0x0F + 0x01), CY = 0

MVI B, 0xF0   ; B = 240
ADC B         ; Test ADC with carry 0, expect CY = 1 (48 + 240), AC irrelevant

ADI 0x0F      ; Add immediate 0x0F, test AC = 1 (48 + 15 = 63), CY = 0
ACI 0x02      ; Add immediate 2 with carry 0 or 1 depending on previous CY, test both AC and CY

; Reset A for another scenario
MVI A, 0x80   ; Test sign bit
ADI 0x80      ; Overflow to check S and CY

MVI A, 0x69
OUT 0 ; Write A to output port 0