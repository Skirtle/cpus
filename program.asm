; SZ0A0P1C
MVI A, 15
OUT 3 ; A = 0b00001111 0x0F 15, flag should be 0x02, parity not checked yet

INR A
OUT 3 ; A = 0b00010000 0x10 16, flag should be 0x12, parity = 0

INR A
OUT 3 ; A = 0b00010001 0x11 17, flag should be 0x06, parity = 1

DCR A
OUT 3 ; A = 0b00010000 0x10 16, flag should be 0x02, parity = 0

DCR A
OUT 3 ; A = 0b00001111 0x0F 15, flag should be 0x16, parity = 1