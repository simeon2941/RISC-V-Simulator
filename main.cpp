
#include "hex.h"
#include "memory.h"
#include "rv32i.h"
#include "registerfile.h"
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
/**  usage() prints summary of how to invoke the program from a shell prompt.
 * usage() will print a description of all the possible command-line arguments that the program
 * takes.
 * @paramP
 * @return None
 *************************************************************************************************************/
static void usage()
{
    cerr << "Usage: rv32i [-d] [-i] [-l execution-limit] [-m hex-mem-size] [-r] [-z] infile" << endl;
    cerr << "   -d show a disassembly before simulation begins(default not disassemble)." << endl;
    cerr << "   -i Show instruction printing during execution(default do not print instructions). "<< endl;
    cerr << "   -l specify the maximum limit (default = no limit)" << endl;
    cerr << "   -m specify memory size (default = 0x10000)" << endl;
    cerr << "   -r show a dump of the hart (GP-rgisters and PC) status" << endl;
    cerr << "   -z show a dump of the hart status and memory after the simulation has halted."<< endl;
    exit(1);
}
/**
 * Read a file of RV32I instructions and execute them.
********************************************************************/
int main(int argc, char** argv)
{
    uint32_t memory_limit = 0x10000; // default memory size = 64k
    uint64_t execution_limit = 0; // 0 is for infinite-limit
    bool show_disassembly = false; // flag for show_disassembly
    bool show_instructions = false; // flag for show_instruction
    bool show_option_r = false; // flag for show a dumo of the hart (gp registers and pc)
    bool show_option_z = false; // flag for show a dump of the hart after simulation has halted
    int opt;
    // while loop to get all the inputed arguments
    while ((opt = getopt(argc, argv, "m:dil:rz")) != -1)
    {
        switch (opt) // switch case to see which arguments where procided by the user
        {
            case 'd':
                show_disassembly = true; // if the option-d is included change the flag to true
                break;
            case 'i':
                show_instructions = true; // if the option -i is entered change the flag to true
                break;
            case 'l':
                execution_limit = std::stoul(optarg, nullptr,
                    10); // if the option -l is given the execution limit will be the new value
                break;
            case 'm':
                memory_limit = std::stoul(
                    optarg, nullptr, 16); //-m the memory_limit will be the entered value
                break;
            case 'r':
                show_option_r = true; // if the option -r is entered change the value to true
                break;
            case 'z':
                show_option_z = true; // if the option -z is entered change the value to true
                break;
            default: /* '?' */
                usage();
        }
    }
    // give the memory the size entered after -m
    memory mem(memory_limit);
    if (!mem.load_file(argv[optind]))
        usage();

    rv32i sim(&mem);
    // call set_show_instructions to set the value of show_instructions
    sim.set_show_instructions(show_instructions);
    // call set_show_option_registers to set the value of show_option_r
    sim.set_show_registers(show_option_r);
    // if -d is entered call disasm() and reset()
    if (show_disassembly)
    {
        sim.disasm();
        sim.reset();
    }
    // call run with execution_limit as its parameter
    sim.run(execution_limit);
    // if -z is entered call dump() for the simulation and memory
    if (show_option_z)
    {
        sim.dump();
        mem.dump();
    }
    return 0;
}