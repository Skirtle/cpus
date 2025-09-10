# 8080 instruction encoding

## Conventions in instruction source
* D    - Destination register (8-bit)
* S    - Source register (8-bit)
* RP   - Register pair (16-bit)
* \#   - 8 or 16 bit immediate operand 
* a    - 16-bit memory address
* P    - 8-bit port address (what the fuck is this?)
* ccc  - Conditional

## Conventions in instruction encoding
* db   - Data byte (8-bit)
* lb   - Low byte of a 16-bit value
* hb   - High byte of a 16-bit value
* pa   - Port address (8-bit)

## Dest and source register fields
* 111  - A (Accumulator)
* 000  - B
* 001  - C
* 010  - D
* 011  - E
* 100  - H
* 101  - L
* 110  - M (Memory reference through address in H:L)

## Register pair 'RP' fields
* 00   - BC (B:C as 16-bit register)
* 01   - DE (D:E as 16-bit register)
* 10   - HL (H:L as 16-bit register)
* 11   - SP (Stack pointer, refers to PSQ (FLAGS:A) for PUSH/POP)

## Condition code 'CCC' fields: (FLAGS: S Z x A x P x C)
* 000  - NZ  ('Z'ero flag not set)
* 001  - Z   ('Z'ero flag set)
* 010  - NC  ('C'arry flag not set)
* 011  - C   ('C'arry flag set)
* 100  - PO  ('P'arity flag not set - ODD)
* 101  - PE  ('P'arity flag set - EVEN)
* 110  - P   ('S'ign flag not set - POSITIVE)
* 111  - M   ('S'ign flag set - MINUS)

## Instructions

| Instruction |      Encoding     |  Flags  |               Description             | Implemented |
|:-----------:|:-----------------:|:-------:|:-------------------------------------:|:-----------:|
| MOV D,S     | 01DDDSSS          | -       | Move register to register             | Yes         |
| MVI D,#     | 00DDD110 db       | -       | Move immediate to register            | Yes         |
| LXI RP,#    | 00RP0001 lb hb    | -       | Load register pair immediate          | No          |
| LDA a       | 00111010 lb hb    | -       | Load A from memory                    | No          |
| STA a       | 00110010 lb hb    | -       | Store A to memory                     | No          |
| LHLD a      | 00101010 lb hb    | -       | Load H:L from memory                  | No          |
| SHLD a      | 00100010 lb hb    | -       | Store H:L to memory                   | No          |
| LDAX RP     | 00RP1010 *1       | -       | Load indirect through BC or DE        | No          |
| STAX RP     | 00RP0010 *1       | -       | Store indirect through BC or DE       | No          |
| XCHG        | 11101011          | -       | Exchange DE and HL content            | No          |
| ADD S       | 10000SSS          | ZSPCA   | Add register to A                     | Yes         |
| ADI #       | 11000110 db       | ZSCPA   | Add immediate to A                    | Yes         |
| ADC S       | 10001SSS          | ZSCPA   | Add register to A with carry          | Yes         |
| ACI #       | 11001110 db       | ZSCPA   | Add immediate to A with carry         | Yes         |
| SUB S       | 10010SSS          | ZSCPA   | Subtract register from A              | Yes         |
| SUI #       | 11010110 db       | ZSCPA   | Subtract immediate from A             | Yes         |
| SBB S       | 10011SSS          | ZSCPA   | Subtract register from A with borrow  | No          |
| SBI #       | 11011110 db       | ZSCPA   | Subtract immediate from A with borrow | No          |
| INR D       | 00DDD100          | ZSPA    | Increment register                    | Yes         |
| DCR D       | 00DDD101          | ZSPA    | Decrement register                    | Yes         |
| INX RP      | 00RP0011          | -       | Increment register pair               | No          |
| DCX RP      | 00RP1011          | -       | Decrement register pair               | No          |
| DAD RP      | 00RP1001          | C       | Add register pair to HL (16 bit add)  | No          |
| DAA         | 00100111          | ZSPCA   | Decimal Adjust accumulator            | No          |
| ANA S       | 10100SSS          | ZSCPA   | AND register with A                   | No          |
| ANI #       | 11100110 db       | ZSPCA   | AND immediate with A                  | No          |
| ORA S       | 10110SSS          | ZSPCA   | OR  register with A                   | No          |
| ORI #       | 11110110          | ZSPCA   | OR  immediate with A                  | No          |
| XRA S       | 10101SSS          | ZSPCA   | ExclusiveOR register with A           | No          |
| XRI #       | 11101110 db       | ZSPCA   | ExclusiveOR immediate with A          | No          |
| CMP S       | 10111SSS          | ZSPCA   | Compare register with A               | No          |
| CPI #       | 11111110          | ZSPCA   | Compare immediate with A              | No          |
| RLC         | 00000111          | C       | Rotate A left                         | No          |
| RRC         | 00001111          | C       | Rotate A right                        | No          |
| RAL         | 00010111          | C       | Rotate A left through carry           | No          |
| RAR         | 00011111          | C       | Rotate A right through carry          | No          |
| CMA         | 00101111          | -       | Compliment A                          | No          |
| CMC         | 00111111          | C       | Compliment Carry flag                 | No          |
| STC         | 00110111          | C       | Set Carry flag                        | No          |
| JMP a       | 11000011 lb hb    | -       | Unconditional jump                    | No          |
| Jccc a      | 11CCC010 lb hb    | -       | Conditional jump                      | No          |
| CALL a      | 11001101 lb hb    | -       | Unconditional subroutine call         | No          |
| Cccc a      | 11CCC100 lb hb    | -       | Conditional subroutine call           | No          |
| RET         | 11001001          | -       | Unconditional return from subroutine  | No          |
| Rccc        | 11CCC000          | -       | Conditional return from subroutine    | No          |
| RST n       | 11NNN111          | -       | Restart (Call n*8)                    | No          |
| PCHL        | 11101001          | -       | Jump to address in H:L                | No          |
| PUSH RP     | 11RP0101 *2       | -       | Push register pair on the stack       | No          |
| POP RP      | 11RP0001 *2       | *2      | Pop  register pair from the stack     | No          |
| XTHL        | 11100011          | -       | Swap H:L with top word on stack       | No          |
| SPHL        | 11111001          | -       | Set SP to content of H:L              | No          |
| IN p        | 11011011 pa       | -       | Read input port into A                | No          |
| OUT p       | 11010011 pa       | -       | Write A to output port                | Sure        |
| EI          | 11111011          | -       | Enable interrupts                     | No          |
| DI          | 11110011          | -       | Disable interrupts                    | No          |
| HLT         | 01110110          | -       | Halt processor                        | Yes         |
| NOP         | 00000000          | -       | No operation                          | Yes         |

###### *1 - Only RP=00(BC) and 01(DE) are allowed for LDAX/STAX
###### *2 - RP=11 refers to PSW for PUSH/POP (cannot push/pop SP). When PSW is POP'd, ALL flags are affected

#### Sources
* [Encodings](http://dunfield.classiccmp.org//r/8080.txt)
* [General information](https://en.wikipedia.org/wiki/Intel_8080#Flags)

