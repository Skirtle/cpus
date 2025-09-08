#!/usr/bin/python

import argparse, subprocess, os, traceback
import simple_assembler as sa
from errors import *

parser = argparse.ArgumentParser(prog = "Intel 8080 assembler", description = "Assembles a file into an Intel 8080 readable machine code file")
# pcc related arguments
parser.add_argument("-i", "--input_filename", help = "The filename of the assembly code", dest = "input")
parser.add_argument("input", nargs="?", help = "The filename of the assembly code")
parser.add_argument("-o", "--output_filename", default = None, help = "The output filename")
parser.add_argument("-v", "--verbose", action = "store_true", default = False, help = "Print verbose information")
parser.add_argument("-N", "--no_output", action = "store_false", default = True, help = "Do not compile assembly code")
parser.add_argument("--version", action = "store_true", default = False, help = "Show the version of pcc")

# C related arguments
parser.add_argument("-r", "--run", action = "store_true", default = False, help = "Run the assembled file")
parser.add_argument("-c", "--compile_c", action = "store_true", default = False, help = "Recompile the C file with gcc cpu_intel-8080.c -o out.exe' by default' by default")
parser.add_argument("--compile_args", default = "-o out.exe", help = "Change the compile options for the C code")

args = parser.parse_args()

os_dict = {
    'nt': "Windows",
    "posix": "Linux"
}

op_os_dict = {
    'nt': "Linux",
    "posix": "Windows"
}

version = "0.1.0"

if (args.version):
    print(f"pcc {os_dict[os.name]} {version}")

if (args.input == None): 
    print(f"{BOLD}pcc:{RESET}{PINK} fatal error:{RESET} no input files\nassembly terminate.")
    exit()

in_filename = args.input
if (args.output_filename == None): out_filename = in_filename.split(".")[0] + ".bin" 
else: out_filename = args.output_filename

if __name__ == "__main__":
    if (args.no_output):
        commands = sa.read_file(in_filename, args.verbose)
        write_code = sa.write_file(out_filename, commands, args.verbose)
    
        if (write_code == 1): print(f"{BOLD}pcc:{RESET}{RED} fatal error:{RESET} could not open file {out_filename} to write\nassembly terminated.")
        elif (write_code == 2): print(f"{BOLD}pcc:{RESET}{RED} fatal error:{RESET} could not create file {out_filename}\nassembly terminated.")
        elif (write_code == 3): print(f"{BOLD}pcc:{RESET}{RED} fatal error:{RESET} could not write to file {out_filename}\nassembly terminated.")
    
    if (args.compile_c):
        if (args.verbose): print(f"{GREEN}LOG:{RESET} Recompiling C code with options '{args.compile_args}'")
        result = subprocess.run(["gcc", "cpu_intel-8080.c"] + args.compile_args.split(" "))
        if (args.verbose): print(f"{GREEN}Log:{RESET} Got exit code {result.returncode} from recompiling C code")
    
    if (args.run):
        try:
            open("./out.exe", "r") # Check if we can open the file
            if (args.verbose): print(f"{GREEN}Log:{RESET} Running file {out_filename}")
            result = subprocess.run(["./out.exe", out_filename])
        except FileNotFoundError:
            print(f"{RED}Fatal error:{RESET} Could not find out.exe. Did you use -N by mistake?")
        except OSError:
            print(f"{RED}Fatal error:{RESET} Could not run {out_filename}. Try running on {op_os_dict[os.name]} or compiling on {os_dict[os.name]}")
        except Exception as err:
            print(f"{RED}Warning:{RESET} Got an unaccounted for error: {err}")
            traceback.print_exc()
            exit(1)