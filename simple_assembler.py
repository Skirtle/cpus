import argparse
from errors import *

class Command:
    def __init__(self, name, op1, op2):
        self.name = name
        self.op1 = op1
        self.op2 = op2
        
    def __str__(self): return f"{self.name}{', ' + self.op1 if self.op1 != None else ''}{', ' + self.op2 if self.op2 != None else ''}"
    def __repr__(self): return f"{self.name}{' ' + self.op1 if self.op1 != None else ''}{' ' + self.op2 if self.op2 != None else ''}"
    
    def generate_opcode_list(self):
        if (self.name == "MVI"): 
            opcodes = f"{(0b00000110 | (self.get_register_hex(self.op1) << 3)):02x} {self.op2.lower()[2:]}"
            return opcodes.split(" ")
        elif (self.name == "MOV"): 
            opcodes = f"{(0b01000000 | (self.get_register_hex(self.op1) << 3) | self.get_register_hex(self.op2)):02x}"
            return opcodes.split(" ")
        elif (self.name == "ADD"): 
            opcodes = f"{(0b10000000 | self.get_register_hex(self.op1)):02x}"
            return opcodes.split(" ")
        elif (self.name == "ADI"): 
            opcodes = f"{(0b11000110):02x} {self.op1.lower()[2:]}"
            return opcodes.split(" ")
        elif (self.name == "ADC"): 
            opcodes = f"{(0b10001000):02x} {self.get_register_hex(self.op1):02x}"
            return opcodes.split(" ")
        elif (self.name == "ACI"): 
            opcodes = f"{(0b11001110):02x} {self.op1.lower()[2:]}"
            return opcodes.split(" ")
        elif (self.name == "OUT"):
            opcodes = f"{(0b11010011 ):02x} {self.op1.lower()}"
            return opcodes.split(" ")
        
        exit(f"{self.name} is not implemented yet, exitting")
        
    def get_register_hex(self, name):
        names = { "B": 0, "C": 1, "D": 2, "E": 3, "H": 4, "L": 5, "M": 6, "A": 7 }
        return names[name]

def read_file(filename: str, verbose = False):
    commands = []
    with open(filename, "r") as file:
        if (verbose): print(f"File {filename} found and opened")
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
            
            if (len(split_code) >= 1): command = split_code[0]
            if (len(split_code) >= 2): op1 = split_code[1]
            if (len(split_code) >= 3):  op2 = split_code[2]
            
            full_command = Command(command, op1, op2)
            commands.append(full_command)
    if (verbose): print(f"Done reading {filename}")
    return commands
        
def write_file(filename: str, commands: list, verbose = False, create_file = True):
    try:
        with open(filename, "br+") as file:
            if (verbose): print(f"Writing to file {filename}")
            hex_list = []
            for i in commands:
                opcode_list = i.generate_opcode_list()
                for opcode in opcode_list:
                    hex_list.append(int("0x" + opcode, 16))
            hex_list.append(int("0x76", 16))
        
            byte_list = bytearray(hex_list)
            file.write(byte_list)
        if (verbose): print(f"Wrote {len(byte_list)} bytes to {filename}")
    except FileNotFoundError:
        if (not create_file):
            print(f"{BOLD}pcc:{RESET}{RED} fatal error:{RESET} could not create file {filename}\nassembly terminate.")
            exit()
        print(f"{BOLD}pcc:{RESET}{YELLOW} warning:{RESET} creating file {filename}")
        open(filename, "w")
        write_file(filename, commands, verbose)
    except:
        print(f"{BOLD}pcc:{RESET}{RED} fatal error:{RESET} could not open file {filename}\nassembly terminate.")
        exit()
    
if __name__ == "__main__":
    print("Hello!")