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
* db   - Data byte (16-bit)
* lb   - Low byte of a 16-bit value
* hb   - High byte of a 16-bit value
* pa   - Port address (8-bit)

## Dest and source register fields
* 111  - A
* 000  - B
* 001  - C
* 010  - D
* 011  - E
* 100  - H
* 101  - L
* 110  - M

#### Sources
* [Encodings](http://dunfield.classiccmp.org//r/8080.txt?utm_source=chatgpt.com)