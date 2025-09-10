from errors import *

class Command:
    def __init__(self, name, op1, op2):
        self.name = name
        self.op1 = op1
        self.op2 = op2
        
    def __str__(self): return f"{self.name}{', ' + self.op1 if self.op1 != None else ''}{', ' + self.op2 if self.op2 != None else ''}"
    def __repr__(self): return f"{self.name}{' ' + self.op1 if self.op1 != None else ''}{' ' + self.op2 if self.op2 != None else ''}"
    
    def generate_opcode(self):
        if (self.name == "MVI"): 
            base_opcode = 0b00000110
            reg = self.get_register_hex(self.op1) << 3
            i_val = self.conv_hex(self.op2.lower())
            opcodes = f"{(base_opcode | reg):02x} {i_val[2:]}"
            return opcodes.split(" ")
        elif (self.name == "MOV"): 
            base_opcode = 0b01000000
            reg1 = self.get_register_hex(self.op1) << 3
            reg2 = self.get_register_hex(self.op2)
            opcodes = f"{(base_opcode | reg1 | reg2):02x}"
            return opcodes.split(" ")
        elif (self.name == "ADD"): 
            base_opcode = 0b10000000
            reg = self.get_register_hex(self.op1)
            opcodes = f"{(base_opcode | reg):02x}"
            return opcodes.split(" ")
        elif (self.name == "ADI"): 
            base_opcode = 0b11000110
            i_val = self.conv_hex(self.op1.lower())
            opcodes = f"{(base_opcode):02x} {i_val[2:]}"
            return opcodes.split(" ")
        elif (self.name == "ADC"): 
            base_opcode = 0b10001000
            reg = self.get_register_hex(self.op1)
            opcodes = f"{(base_opcode | reg):02x}"
            return opcodes.split(" ")
        elif (self.name == "ACI"): 
            base_opcode = 0b11001110
            i_val = self.conv_hex(self.op1.lower())
            opcodes = f"{(base_opcode):02x} {i_val[2:]}"
            return opcodes.split(" ")
        elif (self.name == "OUT"):
            base_opcode = 0b11010011
            port = self.conv_hex(self.op1.lower())
            opcodes = f"{(base_opcode):02x} {port[2:]}"
            return opcodes.split(" ")
        elif (self.name == "SUB"): 
            base_opcode = 0b10010000
            reg = self.get_register_hex(self.op1)
            opcodes = f"{(base_opcode | reg):02x}"
            return opcodes.split(" ")
        elif (self.name == "SUI"): 
            base_opcode = 0b11010110
            i_val = self.conv_hex(self.op1.lower())
            opcodes = f"{(base_opcode):02x} {i_val[2:]}"
            return opcodes.split(" ")
        elif (self.name == "SBB"): 
            base_opcode = 0b10011000
            reg = self.get_register_hex(self.op1)
            opcodes = f"{(base_opcode | reg):02x}"
            return opcodes.split(" ")
        elif (self.name == "SBI"): 
            base_opcode = 0b11011110
            i_val = self.conv_hex(self.op1.lower())
            opcodes = f"{(base_opcode):02x} {i_val[2:]}"
            return opcodes.split(" ")
        elif (self.name == "NOP"): return ["0"]
        elif (self.name == "HLT"): return ["76"]
        elif (self.name == "INR"): 
            base_opcode = 0b00000100
            reg = self.get_register_hex(self.op1) << 3
            opcodes = f"{(base_opcode | reg):02x}"
            return opcodes.split(" ")
        elif (self.name == "DCR"): 
            base_opcode = 0b00000101
            reg = self.get_register_hex(self.op1) << 3
            opcodes = f"{(base_opcode | reg):02x}"
            return opcodes.split(" ")
        elif (self.name == "ANA"): 
            base_opcode = 0b10100000
            reg = self.get_register_hex(self.op1)
            opcodes = f"{(base_opcode | reg):02x}"
            return opcodes.split(" ")
        elif (self.name == "ANI"): 
            base_opcode = 0b11100110
            i_val = self.conv_hex(self.op1.lower())
            opcodes = f"{(base_opcode):02} {i_val[2:]}"
            return opcodes.split(" ")
        elif (self.name == "ORA"): 
            base_opcode = 0b10110000
            reg = self.get_register_hex(self.op1)
            opcodes = f"{(base_opcode | reg):02x}"
            return opcodes.split(" ")
        elif (self.name == "ORI"): 
            base_opcode = 0b11110110
            i_val = self.conv_hex(self.op1.lower())
            opcodes = f"{(base_opcode):02} {i_val[2:]}"
            return opcodes.split(" ")
        elif (self.name == "XRA"): 
            base_opcode = 0b10101000
            reg = self.get_register_hex(self.op1)
            opcodes = f"{(base_opcode | reg):02x}"
            return opcodes.split(" ")
        elif (self.name == "XRI"): 
            base_opcode = 0b11101110
            i_val = self.conv_hex(self.op1.lower())
            opcodes = f"{(base_opcode):02} {i_val[2:]}"
            return opcodes.split(" ")
        
        
        exit(f"{self.name} is not implemented yet, exitting")
        
    def get_register_hex(self, name):
        names = { "B": 0, "C": 1, "D": 2, "E": 3, "H": 4, "L": 5, "M": 6, "A": 7 }
        return names[name]
    
    # Converts a number into hex, allowing for a user to use either decimal and/or hex in their assembly
    def conv_hex(self, num):
        if ("0x" in num): return num
        return hex(int(num))
        
            

def read_file(filename: str, verbose = False):
    commands = []
    with open(filename, "r") as file:
        if (verbose): print(f"{GREEN}Log:{RESET} File {filename} found and opened")
        comment_char = ";"
        for line in file:
            line = line.strip()
            
            # Remove comments
            line = line.split(comment_char)[0].strip()
            if (line == ""): continue
            
            # Split command and operands
            line = line.replace(",", "")
            split_code = line.split(" ")
            command = None
            op1 = None
            op2 = None
            
            if (len(split_code) >= 1): command = split_code[0].upper()
            if (len(split_code) >= 2): op1 = split_code[1].upper()
            if (len(split_code) >= 3): op2 = split_code[2].upper()
            
            full_command = Command(command, op1, op2)
            commands.append(full_command)
    
    if (verbose): print(f"{GREEN}Log:{RESET} Done reading {filename}")
    return commands
        
def write_file(filename: str, commands: list, verbose = False) -> int:
    try: 
        file = open(filename, "bw+")
        if (verbose): print(f"{GREEN}Log:{RESET} Writing to file {filename}")
        hex_list = []
        for i in commands:
            opcode_list = i.generate_opcode()
            for opcode in opcode_list:
                new_opcode = int(opcode, 16)
                hex_list.append(new_opcode)
        
        if (hex_list[-1] != 118): hex_list.append(int("0x76", 16)) # Manually add HLT to end
        
        byte_list = bytearray(hex_list)
        file.write(byte_list)
        if (verbose): print(f"{GREEN}Log:{RESET} Wrote {len(byte_list)} bytes to {filename}")
        file.close()
    
    except FileNotFoundError:
        if (verbose): print(f"{YELLOW}Warning:{RESET} creating file {filename}")
        f = open(filename, "w") # Create the file and try again
        f.close()
        write_file(filename, commands, verbose)
    
    except ValueError as err:
        print(f"Got exception {type(err).__name__}, exitting with code 3 ({err})")
        return 3
    
    except Exception as err:
        print(f"Got exception {err}, {type(err).__name__}, exitting with code 1") 
        return 1
    
    return 0
    
if __name__ == "__main__":
    print("You're in simple_assembler.py!")
    comms = read_file("program.asm", True)
    write_file("program.bin", comms, True)