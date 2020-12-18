
#include "hex.h"
#include "rv32i.h"
#include "memory.h"
#include "registerfile.h"
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <bitset>
using namespace std;
static constexpr int mnemonic_width = 8; // width used for formatting
static constexpr int instruction_width = 35; // width of instruction

static constexpr uint32_t XLEN = 32;
static constexpr uint32_t opcode_lui = 0b0110111;
static constexpr uint32_t opcode_auipc = 0b0010111;
static constexpr uint32_t opcode_jal = 0b1101111;
static constexpr uint32_t opcode_jalr = 0b1100111;
static constexpr uint32_t opcode_btype = 0b1100011;
static constexpr uint32_t opcode_rtype = 0b0110011;
static constexpr uint32_t opcode_itype = 0b0000011;
static constexpr uint32_t opcode_itype_imm_shamt = 0b0010011;
static constexpr uint32_t opcode_stype = 0b0100011;
static constexpr uint32_t opcode_ecall = 0b1110011;
static constexpr uint32_t opcode_fence = 0b0001111;
// I_TYPE LOAD
static constexpr uint32_t funct3_lb = 0b000;
static constexpr uint32_t funct3_lh = 0b001;
static constexpr uint32_t funct3_lw = 0b010;
static constexpr uint32_t funct3_lbu = 0b100;
static constexpr uint32_t funct3_lhu = 0b101;
// I-TYPE ALU
static constexpr uint32_t funct3_addi = 0b000;
static constexpr uint32_t funct3_slti = 0b010;
static constexpr uint32_t funct3_sltiu = 0b011;
static constexpr uint32_t funct3_xori = 0b100;
static constexpr uint32_t funct3_ori = 0b110;
static constexpr uint32_t funct3_andi = 0b111;
//I-TYPE SHAMT
static constexpr uint32_t funct3_slli = 0b001;
static constexpr uint32_t funct3_srli = 0b101;
static constexpr uint32_t funct3_srai = 0b101;
static constexpr uint32_t funct7_srli = 0b0000000;
static constexpr uint32_t funct7_srai = 0b0100000;
// R-TYPE
static constexpr uint32_t funct3_add = 0b000;
static constexpr uint32_t funct7_add = 0b0000000;
static constexpr uint32_t funct7_sub = 0b0100000;
static constexpr uint32_t funct3_sll = 0b001;
static constexpr uint32_t funct7_sll = 0b0000000;
static constexpr uint32_t funct3_slt = 0b010;
static constexpr uint32_t funct7_slt = 0b0000000;
static constexpr uint32_t funct3_sltu = 0b011;
static constexpr uint32_t funct7_sltu = 0b0000000;
static constexpr uint32_t funct3_xor = 0b100;
static constexpr uint32_t funct7_xor = 0b0000000;
static constexpr uint32_t funct3_srl = 0b101;
static constexpr uint32_t funct7_srl = 0b0000000;
static constexpr uint32_t funct3_sra = 0b101;
static constexpr uint32_t funct7_sra = 0b0100000;
static constexpr uint32_t funct3_or = 0b110;
static constexpr uint32_t funct7_or = 0b0000000;
static constexpr uint32_t funct3_and = 0b111;
static constexpr uint32_t funct7_and = 0b0000000;
// B-TYPE
static constexpr uint32_t funct3_beq = 0b000;
static constexpr uint32_t funct3_bne = 0b001;
static constexpr uint32_t funct3_blt = 0b100;
static constexpr uint32_t funct3_bge = 0b101;
static constexpr uint32_t funct3_bltu = 0b110;
static constexpr uint32_t funct3_bgeu = 0b111;
// s-TYPE
static constexpr uint32_t funct3_sb = 0b000;
static constexpr uint32_t funct3_sh = 0b001;
static constexpr uint32_t funct3_sw = 0b010;

/**
 * rv32i constructor
 * saves the m argumen in the mem variable which will be used later for disassembling
 * @param memory* m
 * @return nothing
 * @note
 * @warning
 * @bug
 *
 ********************************************************************************/
rv32i::rv32i(memory* m)
{
    mem = m;
}

/**
 * dissambles the instruction in the simulated memory
 * For every 32-bit word in the memory it prints the 32-bit hex address, fetches the 32-bit
 * instruction from memory at the address in the pc register, prints the instruction, passes
 * te fetched instruction to decode and render the instruction into a string.
 * @param none
 * @return none
 * @note
 * @warning
 * @bug
 ********************************************************************************/
void rv32i::disasm(void)
{
    while (pc < (mem->get_size()))
    {
        std::cout << hex32(pc) << " : "; // prints the 32-bit hex address
        std::cout << decode(mem->get32(pc)) << std::endl; // prints the decoded instructions
        pc += 4; // increment pc by 4
    }
}

/**
 * The purpose of this function is to return a string containing the disassembled instructions text.
 * Using a switch statement with the opcode as the swith expression for each case we format the
 * instruction and its arguments when the opcode is enough. When the opcode isnt enogh to determine
 * the instructions we use sub-switch statement to further decode the instruction . For invalid
 *instructions
 * we print an error message.
 * @param uint32_t insn
 * @return a string containing the disassembled instruction text
 * @note
 * @warning
 * @bug
 ********************************************************************************/

std::string rv32i::decode(uint32_t insn) const
{
    uint32_t opcode = get_opcode(insn); // gets the opcode bits
    uint32_t funct3 = get_funct3(insn);
    uint32_t funct7 = get_funct7(insn);
    int32_t imm_i = get_imm_i(insn);

    std::cout << hex32(insn) << "  "; // prints the instruction in hex
    switch (opcode)
    {
        default:
            return render_illegal_insn();
        case opcode_lui:
            return render_lui(insn);
            break;
        case opcode_auipc:
            return render_auipc(insn);
            break;
        case opcode_jal:
            return render_jal(insn);
            break;
        case opcode_jalr:
            return render_jalr(insn);
            break;
        case opcode_rtype:
            switch (funct3)
            {
                default:
                    return render_illegal_insn();
                case funct3_add:
                    switch (funct7)
                    {
                        default:
                            return render_illegal_insn();
                        case funct7_add:
                            return render_rtype(insn, "add");
                            break;
                        case funct7_sub:
                            return render_rtype(insn, "sub");
                            break;
                    }
                    assert(0 && "unhandled funct7");
                case funct3_sll:
                    return render_rtype(insn, "sll");
                    break;
                case funct3_slt:
                    return render_rtype(insn, "slt");
                    break;

                case funct3_sltu:
                    return render_rtype(insn, "sltu");
                    break;
                case funct3_xor:
                    return render_rtype(insn, "xor");
                    break;
                case funct3_srl:
                    switch (funct7)
                    {
                        default:
                            return render_illegal_insn();
                            break;
                        case funct7_srl:
                            return render_rtype(insn, "srl");
                            break;
                        case funct7_sra:
                            return render_rtype(insn, "sra");
                            break;
                    }
                    assert(0 && "unhandled funct7");
                case funct3_or:
                    return render_rtype(insn, "or");
                    break;
                case funct3_and:
                    return render_rtype(insn, "and");
                    break;
            }
            assert(0 && "unhandled funct3");
        case opcode_itype:
            switch (funct3)
            {
                default:
                    return render_illegal_insn();
                case funct3_lb:
                    return render_itype_load(insn, "lb");
                    break;
                case funct3_lh:
                    return render_itype_load(insn, "lh");
                    break;
                case funct3_lw:
                    return render_itype_load(insn, "lw");
                    break;
                case funct3_lbu:
                    return render_itype_load(insn, "lbu");
                    break;
                case funct3_lhu:
                    return render_itype_load(insn, "lhu");
                    break;
            }
            assert(0 && "unhandled funct3");
        case opcode_itype_imm_shamt:
            switch (funct3)
            {
                default:
                    return render_illegal_insn();
                case funct3_addi:
                    return render_itype_alu(insn, "addi", imm_i);
                    break;
                case funct3_slti:
                    return render_itype_alu(insn, "slti", imm_i);
                    break;
                case funct3_xori:
                    return render_itype_alu(insn, "xori", imm_i);
                    break;
                case funct3_sltiu:
                    return render_itype_alu(insn, "sltiu", imm_i);
                    break;
                case funct3_ori:
                    return render_itype_alu(insn, "ori", imm_i);
                    break;
                case funct3_andi:
                    return render_itype_alu(insn, "andi", imm_i);
                    break;
                case funct3_slli:
                    return render_itype_shamt(insn, "slli");
                    break;
                case funct3_srli:
                    switch (funct7)
                    {
                        default:
                            return render_illegal_insn();
                        case funct7_srli:
                            return render_itype_shamt(insn, "srli");
                            break;
                        case funct7_srai:
                            return render_itype_shamt(insn, "srai");
                            break;
                    }
                    assert(0 && "unhandled funct7");
            }
            assert(0 && "unhandled funct3");            
        case opcode_btype:
            switch (funct3)
            {
                default:
                    return render_illegal_insn();
                case funct3_beq:
                    return render_btype(insn, "beq");
                    break;
                case funct3_bne:
                    return render_btype(insn, "bne");
                    break;
                case funct3_blt:
                    return render_btype(insn, "blt");
                    break;
                case funct3_bge:
                    return render_btype(insn, "bge");
                    break;
                case funct3_bltu:
                    return render_btype(insn, "bltu");
                    break;
                case funct3_bgeu:
                    return render_btype(insn, "bgeu");
                    break;
            }
            assert(0 && "unhandled funct3");
        case opcode_stype:
            switch (funct3)
            {
                default:
                    return render_illegal_insn();
                case funct3_sb:
                    return render_stype(insn, "sb");
                    break;
                case funct3_sh:
                    return render_stype(insn, "sh");
                    break;
                case funct3_sw:
                    return render_stype(insn, "sw");
                    break;
            }
            assert(0 && "unhandled funct3");
        case opcode_ecall:
            switch (funct7 + get_rs2(insn))
            {
                default:
                    return render_illegal_insn();
                case 0b000000000000:
                    return "ecall";
                    break;
                case 0b000000000001:
                    return "ebreak";
                    break;
            }
        case opcode_fence:
            return render_fence(insn);
            break;
    }
    assert(0 && "unhandled opcode");
}

/**
 * Get the opcode
 * The function of this funciton is to extract the opcode from insn
 * @param uint32_t insn
 * @return opcode
 * @note
 * @warning
 * @bug
 ********************************************************************************/

uint32_t rv32i::get_opcode(uint32_t insn)
{
    return (insn & 0x0000007f); // extract bits 0-6
}
/*
Extract and return the rd field from the given instruction.
*/
/**
 * get the rd field
 * Extract and return the rd field from the given insn instruction
 * @param uint32_t insn
 * @return rd field
 * @note
 * @warning
 * @bug
 ********************************************************************************/

uint32_t rv32i::get_rd(uint32_t insn)
{

    return (insn & 0x00000f80) >> 7; // extract bits bits 11-7
}

/**
 * Get funct3 field
 * Extract and return the funct3 field from the insn instruction
 * @param uint32_t insn
 * @return funct3
 * @note
 * @warning
 * @bug
 ********************************************************************************/
uint32_t rv32i::get_funct3(uint32_t insn)
{
    return (insn & 0x00007000) >> 12; // extracts bits bits 14-12
}
/**
 * Get rs1 field
 * Extract and return the rs1 field from the givne instruction
 * @param uint32_t insn
 * @return rs1 field
 * @note
 * @warning
 * @bug
 ********************************************************************************/
uint32_t rv32i::get_rs1(uint32_t insn)
{
    return (insn & 0x000f8000) >> 15; // bits 19-15
}

/**
 * Get rs2 field
 * Extract and return the rs2 field from the given instruction
 * @param uint32_t insn
 * @return rs2 field
 * @note
 * @warning
 * @bug
 ********************************************************************************/
uint32_t rv32i::get_rs2(uint32_t insn)
{
    return (insn & 0x01f00000) >> 20; // bits 24-20
}
/**
 * Get funct7 field
 * Extract and return the funct7 field from the given instructions
 * @param uint32_t insn
 * @return funct7 field
 * @note
 * @warning
 * @bug
 ********************************************************************************/

uint32_t rv32i::get_funct7(uint32_t insn)
{
    return (insn & 0xfe000000) >> 25; // extracts bits 31-25
}
/**
 * Get the imm_i field
 * Extract and return the imm_i field from the given instruction
 * @param uint32_t insn
 * @return imm_i field
 * @note
 * @warning
 * @bug
 ********************************************************************************/
int32_t rv32i::get_imm_i(uint32_t insn)
{
    int32_t imm_i = (insn & 0xfff00000) >> 20; // extract imm bits 31-20
    // check to see if bit 31 is 1, if so fil bits 31 to 12 with 1ns
    if (insn & 0x80000000)
    {
        imm_i |= 0xfffff000; // fills bits 31-12 with 1
    }
    return imm_i;
}
/**
 * Get the imm_u field
 * Extract and return the imm_u field from the given instruction
 * @param uint32_t insn
 * @return imm_u field
 * @note
 * @warning
 * @bug
 ********************************************************************************/
int32_t rv32i::get_imm_u(uint32_t insn)
{
    return (insn & 0xfffff000); // returns imm_u (extracts the values in bits 31-12 )and 11-0 will
                                // be filled with 0s
}
/**
 * Get the imm_b field
 * Extract and return the imm_b field from the given instruction
 * @param uint32_t insn
 * @return imm_b field
 * @note
 * @warning
 * @bug
 ********************************************************************************/
int32_t rv32i::get_imm_b(uint32_t insn)
{
    int32_t imm_b = (insn & 0x00000080) << 4; // extract bit 7 and move it to bit 11
    imm_b |= (insn & 0x00000f00) >> 7; // extracts bits 11-8 and moves them into 4-1
    imm_b |= (insn & 0x7e000000) >> 20; // extract bits 31-25 and move them into 10-5
    imm_b |= (insn & 0x80000000) >> 19; // extract bit 31 and move it to bit 12
    // if bit 31 is a 1 then fill bits 31-13 with 1's otherwise they will be 0's
    if (insn & 0x80000000)
    {
        imm_b |= 0xffffe000;
    }
    return imm_b;
}
/**
 * Get the imm_s field
 * Extract and return the imm_s field from the given instruction
 * @param uint32_t insn
 * @return imm_s field
 * @note
 * @warning
 * @bug
 ********************************************************************************/
int32_t rv32i::get_imm_s(uint32_t insn)
{
    int32_t imm_s = (insn & 0xfe000000) >> 20; // extract bits 31-25 and move them into bits 11-5
    imm_s |= (insn & 0x00000f80) >> (7 - 0); // extract bits 11-7 and move them into bits 4-0
    // check bit 31, if its a 1 then fill bits 31-12 with 1's otherwise they will be 0's
    if (insn & 0x80000000)
    {
        imm_s |= 0xfffff000;
    }
    return imm_s;
}
/**
 * Get the imm_j field
 * Extract and return the imm_j field from the given instruction
 * @param uint32_t insn
 * @return imm_j field
 * @note
 * @warning
 * @bug
 ********************************************************************************/
int32_t rv32i::get_imm_j(uint32_t insn)
{
    int32_t imm_j
        = (insn & 0x000ff000); // extracts bits 19-12 and keeps them on the positions that they are
    imm_j |= (insn & 0x00100000) >> 9; // extracts bits 20 and moves it to position 11
    imm_j |= (insn & 0x7fe00000) >> 20; // extracts bits 30-21 and moves them to position 10-1
    imm_j |= (insn & 0x80000000) >> 11; // extracts bit 31 and moves it to position 20
    // if bit at position 31 is a 1 then fill positions 31 to 21 with 1's otherwise they will be 0's
    if (insn & 0x80000000)
    {
        imm_j |= 0xffe00000;
    }
    return imm_j;
}

/**
 * Default message for when decoding
 * Returns a string with the error message shown below were the
 * instruction mnemonic and arguments would show .
 * @param none
 * @return an error message as shown below
 * @note
 * @warning
 * @bug
 ********************************************************************************/
std::string rv32i::render_illegal_insn() const
{
    return "ERROR: UNIMPLEMENTED INSTRUCTION";
}
/** Formats the disassembled instruction text for the lui instruction
 * this function will return a formated text of the disassembled lui instruction
 * @param uint32_t insn
 * @return a string containing the disassembled instruction
 * @note
 * @warning
 * @bug
 ********************************************************************************/
std::string rv32i::render_lui(uint32_t insn) const
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);

    std::ostringstream os;
    os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << "lui"
       << "x" << std::dec << rd << ",0x" << std::hex << ((imm_u >> 12) & 0x0fffff);
    return os.str();
}
/** Formats the disassembled instruction text for the auipc instruction
 * this function will return a formated text of the disassembled auipc instruction
 * @param uint32_t insn
 * @return a string containing the disassembled instruction
 * @note
 * @warning
 * @bug
 ********************************************************************************/
std::string rv32i::render_auipc(uint32_t insn) const
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);

    std::ostringstream os;
    os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << "auipc"
       << "x" << std::dec << rd << ",0x" << std::hex << ((imm_u >> 12) & 0x0fffff);
    return os.str();
}
/** Formats the disassembled instruction text for the jal instruction
 * this function will return a formated text of the disassembled jal instruction
 * @param uint32_t insn
 * @return a string containing the disassembled instruction
 * @note
 * @warning
 * @bug
 ********************************************************************************/
std::string rv32i::render_jal(uint32_t insn) const
{
    uint32_t rd = get_rd(insn);
    int32_t imm_j = get_imm_j(insn);
    std::ostringstream os;
    os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << "jal"
       << "x" << std::dec << rd << ",0x" << std::hex << (imm_j + pc);
    return os.str();
}
/** Formats the disassembled instruction text for the jalr instruction
 * this function will return a formated text of the disassembled jalr instruction
 * @param uint32_t insn
 * @return a string containing the disassembled instruction
 * @note
 * @warning
 * @bug
 ********************************************************************************/
std::string rv32i::render_jalr(uint32_t insn) const
{
    uint32_t rd = get_rd(insn);
    int32_t imm_i = get_imm_i(insn);
    int32_t rs1 = get_rs1(insn);
    std::ostringstream os;
    os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << "jalr"
       << "x" << std::dec << rd << "," << imm_i << "(x" << rs1 << ")";
    return os.str();
}
/** Formats the disassembled instruction text for the R-type instructions
 * this function will return a formated text of the disassembled r-type instruction
 * @param uint32_t insn
 * @return a string containing the disassembled instruction
 * @note
 * @warning
 * @bug
 ********************************************************************************/
std::string rv32i::render_rtype(uint32_t insn, const char* mnemonic) const
{
    uint32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    std::ostringstream os;
    os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << mnemonic << "x" << std::dec
       << rd << ",x" << std::dec << rs1 << ",x" << std::dec << rs2;
    return os.str();
}
/** Formats the disassembled instruction text for the B-type instructions
 * this function will return a formated text of the disassembled B-type instructions
 * @param uint32_t insn
 * @return a string containing the disassembled instruction
 * @note
 * @warning
 * @bug
 ********************************************************************************/
std::string rv32i::render_btype(uint32_t insn, const char* mnemonic) const
{
    int32_t imm_b = get_imm_b(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    std::ostringstream os;
    os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << mnemonic << "x" << std::dec
       << rs1 << ",x" << rs2 << ","
       << "0x" << std::hex << (imm_b + pc);
    return os.str();
}
/** Formats the disassembled instruction text for the i-type load instructions
 * this function will return a formated text of the disassembled i-type load instructions
 * @param uint32_t insn
 * @return a string containing the disassembled instruction
 * @note
 * @warning
 * @bug
 ********************************************************************************/
std::string rv32i::render_itype_load(uint32_t insn, const char* mnemonic) const
{
    uint32_t rd = get_rd(insn);
    int32_t imm_i = get_imm_i(insn);
    int32_t rs1 = get_rs1(insn);
    std::ostringstream os;
    os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << mnemonic << "x" << std::dec
       << rd << "," << imm_i << "(x" << rs1 << ")";
    return os.str();
}
/** Formats the disassembled instruction text for the i-type shamt instructions
 * this function will return a formated text of the disassembled i-type shamt instructions
 * @param uint32_t insn
 * @return a string containing the disassembled instruction
 * @note
 * @warning
 * @bug
 ********************************************************************************/
std::string rv32i::render_itype_shamt(uint32_t insn, const char* mnemonic) const
{
    uint32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    std::ostringstream os;
    os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << mnemonic << "x" << std::dec
       << rd << ",x" << rs1 << "," << std::dec << rs2;
    return os.str();
}
/** Formats the disassembled instruction text for the i-type alu instructions
 * this function will return a formated text of the disassembled i-type alu instructions
 * @param uint32_t insn
 * @return a string containing the disassembled instruction
 * @note
 * @warning
 * @bug
 ********************************************************************************/
std::string rv32i::render_itype_alu(uint32_t insn, const char* mnemonic, int32_t imm_i) const
{
    uint32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    std::ostringstream os;
    os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << mnemonic << "x" << std::dec
       << rd << ",x" << rs1 << "," << imm_i;
    return os.str();
}
/** Formats the disassembled instruction text for the s-type instructions
 * this function will return a formated text of the disassembled s-type isntructions
 * @param uint32_t insn
 * @return a string containing the disassembled instruction
 * @note
 * @warning
 * @bug
 ********************************************************************************/
std::string rv32i::render_stype(uint32_t insn, const char* mnemonic) const
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_s = get_imm_s(insn);
    std::ostringstream os;
    os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << mnemonic << "x" << std::dec
       << rs2 << "," << imm_s << "(x" << rs1 << ")";
    return os.str();
}
/** Formats the disassembled instruction text for the fence instruction
 * this function will return a formated text of the disassembled fence instruction
 * @param uint32_t insn
 * @return a string containing the disassembled instruction
 * @note
 * @warning
 * @bug
 ********************************************************************************/
std::string rv32i::render_fence(uint32_t insn) const
{
    string pred = ""; // pred starts as an empty string
    string succ = ""; // succ starts as an empty string

    // case where the bits ar high for pred = i
    if ((insn & 0x08000000) == 0x08000000 )
    {
        pred += 'i';
    }
    //case where the bits are high for succ =i
    if ((insn & 0x00800000) == 0x00800000)
    {
        succ += 'i';
    }
    // case where the bits are high for pred =o
    if ((insn & 0x04000000) == 0x04000000)
    {
        pred += 'o';
    }
    //case where the bits are high for succ = o
    if ((insn & 0x00400000) == 0x00400000)
    {
        succ += 'o';
    }
    // case where bits are high for pred = r
    if ((insn & 0x02000000) == 0x02000000)
    {
        pred += 'r';
    }
    //case where bits are high for succ = r
    if ((insn & 0x00200000) == 0x00200000)
    {
        succ += 'r';
    }
    // case where bits are high for pred =w
    if ((insn & 0x01000000) == 0x01000000)
    {
        pred += 'w';
    }
    //case where bits are high for succ = w
    if ((insn & 0x00100000) == 0x00100000)
    {
        succ += 'w';
    }
    std::ostringstream os;
    os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << "fence" << pred << ","
       << succ;
    return os.str();
}
/** Formats the disassembled instruction text for the ebreak instructions
 * this function will return a formated text of the disassembled ebreak instructions.
 * @param uint32_t insn
 * @return a string containing the disassembled instruction
 * @note
 * @warning
 * @bug
 ********************************************************************************/
std::string rv32i::render_ebreak() const
{
    std::ostringstream os;
    os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << "ebreak";
    return os.str();
}

/** Formats the disassembled instruction text for the ecall instruction
 * this function will return a formated text of the disassembled ecall instruction
 * @param uint32_t insn
 * @return a string containing the disassembled instruction
 * @note
 * @warning
 * @bug
 ********************************************************************************/
std::string rv32i::render_ecall() const
{
    std::ostringstream os;
    os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << "ecall";
    return os.str();
}
/**
 * Setter show_instructions
 * sets the show instructions to bool b
 * @param bool b
 * @return none
********************************************************************************/ 
void rv32i::set_show_instructions(bool b)
{
    show_instructions = b;
}
/**
 * Seetter set_show_registers
 * set show_registers to bool b
 * @param bool b
 * @return none
 ********************************************************************************/
void rv32i::set_show_registers(bool b)
{
    show_registers = b;
}
/**
 * getter is_halted
 * gets the value of the flag halt
 * @param none
 * @return bool halt
 ********************************************************************************/
bool rv32i::is_halted() const
{
    return halt;
}
/*
Reset the rv32i object and the registerfile.
To reset the rv32i hart, set the pc register to zero, the insn_counter to zero, and the halt flag to
false;
*/
/**
 * Reset the rv32i object and the register file
 * Does the reset by setting pc register to zero, insn_counter to 0 and halt flag to false
 * @param none
 * @return none
 ********************************************************************************/
void rv32i::reset()
{
    pc = 0;
    insn_counter = 0;
    halt = false;
}
/**
 * Dumps the state of the hart
 * This method which is a member of rv32i class will dump the sate of the hart. Will dump registers
 * and pc register
 * @param none
 * @return none
 ********************************************************************************/
void rv32i::dump() const
{
    regs.dump();
    cout << " pc " << hex32(pc) << std::endl;
}
/**
 * Execute the given RV32I instruction
 * This function executes the given rv32i instruction by making use of the get__xxx() methods to
 *extract
 * needed instruction fields and then call exec_xx() to print the instructions.
 * @param uint32_t insn, std::ostream* pos
 * @return none
 ********************************************************************************/
void rv32i::dcex(uint32_t insn, std::ostream* pos)
{

    uint32_t opcode = get_opcode(insn);
    uint32_t funct3 = get_funct3(insn);
    uint32_t funct7 = get_funct7(insn);
    switch (opcode)
    {
        default:
            exec_illegal_insn(insn, pos);
            return;
        case opcode_lui:
            exec_lui(insn, pos);
            return;
        case opcode_auipc:
            exec_auipc(insn, pos);
            return;
        case opcode_jal:
            exec_jal(insn, pos);
            return;
        case opcode_jalr:
            exec_jalr(insn, pos);
            return;
        case opcode_rtype:
            switch (funct3)
            {
                default:
                    exec_illegal_insn(insn, pos);
                    return;
                case funct3_add:
                    switch (funct7)
                    {
                        default:
                            render_illegal_insn();
                            return;
                        case funct7_add:
                            exec_add(insn, pos);
                            return;
                        case funct7_sub:
                            exec_sub(insn, pos);
                            return;
                    }
                    assert(0 && "unhandled funct7");
                case funct3_sll:
                    exec_sll(insn, pos);
                    return;
                case funct3_slt:
                    exec_slt(insn, pos);
                    return;
                case funct3_sltu:
                    exec_sltu(insn, pos);
                    return;
                case funct3_xor:
                    exec_xor(insn, pos);
                    return;
                case funct3_srl:
                    switch (funct7)
                    {
                        default:
                            render_illegal_insn();
                            return;
                        case funct7_srl:
                            exec_srl(insn, pos);
                            return;
                        case funct7_sra:
                            exec_sra(insn, pos);
                            return;
                    }
                case funct3_or:
                    exec_or(insn, pos);
                    return;
                case funct3_and:
                    exec_and(insn, pos);
                    return;
            }
        case opcode_btype:
            switch (funct3)
            {
                default:
                    exec_illegal_insn(insn, pos);
                    return;
                case funct3_beq:
                    exec_beq(insn, pos);
                    return;
                case funct3_bne:
                    exec_bne(insn, pos);
                    return;
                case funct3_blt:
                    exec_blt(insn, pos);
                    return;
                case funct3_bge:
                    exec_bge(insn, pos);
                    return;
                case funct3_bltu:
                    exec_bltu(insn, pos);
                    return;
                case funct3_bgeu:
                    exec_bgeu(insn, pos);
                    return;
                    assert(0 && "unhandled funct3");
            }
        case opcode_itype:
            switch (funct3)
            {
                default:
                    exec_illegal_insn(insn, pos);
                    return;
                case funct3_lb:
                    exec_lb(insn, pos);
                    return;
                case funct3_lh:
                    exec_lh(insn, pos);
                    return;
                case funct3_lw:
                    exec_lw(insn, pos);
                    return;
                case funct3_lbu:
                    exec_lbu(insn, pos);
                    return;
                case funct3_lhu:
                    exec_lhu(insn, pos);
                    return;
            }
        case opcode_itype_imm_shamt:
            switch (funct3)
            {
                default:
                    exec_illegal_insn(insn, pos);
                    return;
                case funct3_addi:
                    exec_addi(insn, pos);
                    return;
                    break;
                case funct3_slti:
                    exec_slti(insn, pos);
                    return;
                    break;
                case funct3_xori:
                    exec_xori(insn, pos);
                    return;
                    break;
                case funct3_sltiu:
                    exec_sltiu(insn, pos);
                    return;
                    break;
                case funct3_ori:
                    exec_ori(insn, pos);
                    return;
                    break;
                case funct3_andi:
                    exec_andi(insn, pos);
                    return;
                    break;
                case funct3_slli:
                    exec_slli(insn, pos);
                    return;
                    break;
                case funct3_srli:
                    switch (funct7)
                    {
                        default:
                            exec_illegal_insn(insn, pos);
                            return;
                        case funct7_srli:
                            exec_srli(insn, pos);
                            return;
                            break;
                        case funct7_srai:
                            exec_srai(insn, pos);
                            return;
                            break;
                    }
            }
        case opcode_stype:
            switch (funct3)
            {
                default:
                    exec_illegal_insn(insn, pos);
                    return;
                case funct3_sb:
                    exec_sb(insn, pos);
                    return;
                    break;
                case funct3_sh:
                    exec_sh(insn, pos);
                    return;
                    break;
                case funct3_sw:
                    exec_sw(insn, pos);
                    return;
                    break;
            }
        case opcode_fence:
            exec_fence(insn, pos);
            return;
            break;
        case opcode_ecall:
            switch (funct7 + get_rs2(insn))
            {
                default:
                    exec_illegal_insn(insn, pos);
                    return;
                case 0b000000000001:
                    exec_ebreak(insn, pos);
                    return;
                case 0b000000000000:
                    exec_ecall(insn, pos);
                    return;
            }
    }
    assert(0 && "unhandled opcode");
}
/**
 * function to take care of illegal cases
 * sets the halt flag to ture, if ostream* parameter is not nullptr then call render_illegal_insn()
 * to print the message
 * @param uint32_t insn, std::ostream* pos
 * @return none
 ********************************************************************************/
void rv32i::exec_illegal_insn(uint32_t insn, std::ostream* pos)
{
    halt = true; // set the halt flag to true
    if (pos != nullptr) // if pos is not nulltpr call render_illegal_insn()
    {
        render_illegal_insn();
    }
}
/**
 * Execute lui instruction
 * IT executes the LUI RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_lui(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    int32_t imm_u = get_imm_u(insn); // get imm_u
    if (pos)
    {
        std::string s = render_lui(insn); // call render_lui
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << std::dec << rd << " = " << hex0x32(imm_u);
    }
    regs.set(rd, imm_u); // set rd to imm_u
    pc += 4; // increment pc by 4
}
/**
 * Execute auipc instruction
 * IT executes the AUIPC RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_auipc(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    int32_t imm_u = get_imm_u(insn) + pc; // get imm_u + pc
    if (pos)
    {
        std::string s = render_auipc(insn);
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = " << hex0x32(pc) << " + " << hex0x32(get_imm_u(insn)) << " = "
             << hex0x32(imm_u);
    }
    regs.set(rd, imm_u); // set rd to imm_u
    pc += 4; // increment pc by 4
}
/**
 * Execute jal instruction
 * IT executes the JAL RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_jal(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // rd
    uint32_t imm_j = get_imm_j(insn); // imm_j
    uint32_t old_pc = pc;
    if (pos)
    {
        std::string s = render_jal(insn);
        s.resize(instruction_width, ' ');
        pc += imm_j;
        *pos << s << "// "
             << "x" << to_string(rd) << " = " << hex0x32(old_pc + 4) << ", "
             << " pc = " << hex0x32(old_pc) << " + " << hex0x32(imm_j) << " = " << hex0x32(pc);
    }
    regs.set(rd, old_pc + 4);
    pc = old_pc + imm_j;
}
/**
 * Execute jalr instruction
 * IT executes the JALR RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_jalr(uint32_t insn, std::ostream* pos)
{
   uint32_t rd = get_rd(insn); //get rd
   uint32_t rs1 = get_rs1(insn); //register rs1
   uint32_t imm_i = get_imm_i(insn); //get imm_i
   uint32_t old_pc = pc; //old pc value
   pc = (regs.get(rs1) + imm_i) & 0xfffffffe; // increment pc 
   if (pos)
   {
    std::string s = render_jalr(insn) ;
     s.resize(instruction_width,' ');
     *pos << s << "// x" << to_string(rd) <<" = "<<hex0x32(old_pc+4) << ", pc = (" << hex0x32(imm_i)
     <<" + " << hex0x32(regs.get(rs1)) << ") & 0xfffffffe" << " = " << hex0x32(pc);

   }
   regs.set(rd,old_pc+4); //set rd to old_pc +4
} 
/**
 * Execute add instruction
 * IT executes the add RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_add(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // rs1
    uint32_t rs2 = regs.get(get_rs2(insn)); // rs2
    if (pos)
    {
        std::string s = render_rtype(insn, "add");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = " << hex0x32(rs1) << " + " << hex0x32(rs2) << " = "
             << hex0x32(rs1 + rs2);
    }
    regs.set(rd, (rs1 + rs2)); // set rd to (rs1+rs2)
    pc += 4; // increment pc by 4
}
/**
 * Execute addi instruction
 * IT executes the ADDI RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_addi(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // rs1
    int32_t imm_i = get_imm_i(insn); // imm_i
    regs.set(rd, (rs1 + imm_i)); // set rd to rs1+imm_i
    pc += 4; // increment pc by 4
    if (pos)
    {
        std::string s = render_itype_alu(insn, "addi", imm_i);
        ;
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = " << hex0x32(rs1) << " + " << hex0x32(imm_i) << " = "
             << hex0x32(regs.get(rd));
    }
}
/**
 * Execute srli instruction
 * IT executes the SRLI RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_srli(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // rs1
    uint32_t imm_i = get_imm_i(insn); // imm_i
    regs.set(rd, rs1 >> imm_i); // set rd to rs1>>imm_i
    pc += 4; // increment pc by 4
    if (pos)
    {
        std::string s = render_itype_shamt(insn, "srli");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = " << hex0x32(rs1) << " >> " << imm_i << " = "
             << hex0x32(regs.get(rd));
    }
}
/**
 * Execute and instruction
 * IT executes the AND RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_and(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // rs1
    uint32_t rs2 = regs.get(get_rs2(insn)); // rs2
    if (pos)
    {
        std::string s = render_rtype(insn, "and");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = " << hex0x32(rs1) << " & " << hex0x32(rs2) << " = "
             << hex0x32(rs1 & rs2);
    }
    regs.set(rd, (rs1 & rs2)); // set rd to rs1 & rs2
    pc += 4; // increment pc by 4
}
/**
 * Execute andi instruction
 * IT executes the ANDI RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_andi(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // rd
    uint32_t imm_i = get_imm_i(insn); // imm_i
    uint32_t rs1 = regs.get(get_rs1(insn)); // rs1
    if (pos)
    {
        std::string s = render_itype_alu(insn, "andi", imm_i);
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = " << hex0x32(rs1) << " & " << hex0x32(imm_i) << " = "
             << hex0x32(rs1 & imm_i);
    }
    regs.set(rd, (rs1 & imm_i)); // set rd to rs1&imm_i
    pc += 4; // increment pc by 4
}
/**
 * Execute beq instruction
 * IT executes the BEQ RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_beq(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = regs.get(get_rs1(insn)); // get register rs1
    uint32_t rs2 = regs.get(get_rs2(insn)); // get register rs2
    int32_t imm_b = get_imm_b(insn); // get imm_b
    if (pos)
    {
        std::string s = render_btype(insn, "beq");
        ;
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "pc += (" << hex0x32(regs.get(get_rs1(insn)))
             << " == " << hex0x32(regs.get(get_rs2(insn))) << " ? " << hex0x32(imm_b)
             << " : 4) = " << hex0x32(rs1 == rs2 ? pc += imm_b : pc += 4);
    }
    if (pos == nullptr)
    {
        (rs1 == rs2 ? pc += imm_b : pc += 4);
    }
}
/**
 * Execute bge instruction
 * IT executes the BGE RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_bge(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = regs.get(get_rs1(insn)); // get register rs1
    int32_t rs2 = regs.get(get_rs2(insn)); // get register rs2
    int32_t imm_b = get_imm_b(insn); // get imm_b
    if (pos)
    {
        std::string s = render_btype(insn, "bge");
        s.resize(instruction_width, ' ');
        *pos << s << "// " << std::dec << "pc += (" << hex0x32(rs1) << " >= " << hex0x32(rs2)
             << " ? " << hex0x32(imm_b)
             << " : 4) = " << hex0x32(rs1 >= rs2 ? pc += imm_b : pc += 4);
    }
    if (pos == nullptr)
    {
        (rs1 >= rs2 ? pc += imm_b : pc += 4); // increment pc
    }
}
/**
 * Execute bgeu instruction
 * IT executes the LUI RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_bgeu(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = regs.get(get_rs1(insn)); // get register rs1
    uint32_t rs2 = regs.get(get_rs2(insn)); // get register rs2
    int32_t imm_b = get_imm_b(insn); // get imm_b
    if (pos)
    {
        std::string s = render_btype(insn, "bgeu");
        s.resize(instruction_width, ' ');
        *pos << s << "// " << std::dec << "pc += (" << hex0x32(rs1) << " >=U " << hex0x32(rs2)
             << " ? " << hex0x32(imm_b)
             << " : 4) = " << hex0x32(rs1 >= rs2 ? pc += imm_b : pc += 4);
    }
    if (pos == nullptr)
    {
        (rs1 >= rs2 ? pc += imm_b : pc += 4); // increment pc
    }
}
/**
 * Execute blt instruction
 * IT executes the BLT RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_blt(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = regs.get(get_rs1(insn)); // get register rs1
    int32_t rs2 = regs.get(get_rs2(insn)); // get register rs1
    uint32_t imm_b = get_imm_b(insn); // get imm_b
    if (pos)
    {
        std::string s = render_btype(insn, "blt");
        s.resize(instruction_width, ' ');
        *pos << s << "// " << std::dec << "pc += (" << hex0x32(rs1) << " < " << hex0x32(rs2)
             << " ? " << hex0x32(imm_b) << " : 4) = " << hex0x32(rs1 < rs2 ? pc += imm_b : pc += 4);
    }
    if (pos == nullptr)
    {
        (rs1 < rs2 ? pc += imm_b : pc += 4); // increment pc
    }
}
/**
 * Execute bltu instruction
 * IT executes the bltu RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_bltu(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = regs.get(get_rs1(insn)); // get register rs1
    uint32_t rs2 = regs.get(get_rs2(insn)); // get register rs2
    int32_t imm_b = get_imm_b(insn); // get imm_b
    if (pos)
    {
        std::string s = render_btype(insn, "bltu");
        s.resize(instruction_width, ' ');
        *pos << s << "// " << std::dec << "pc += (" << hex0x32(rs1) << " <U " << hex0x32(rs2)
             << " ? " << hex0x32(imm_b)
             << " : 4) = " << hex0x32((rs1 < rs2 ? pc += imm_b : pc += 4));
    }
    if (pos == nullptr)
    {
        (rs1 < rs2 ? pc += imm_b : pc += 4); // increment pc
    }
}
/**
 * Execute bne instruction
 * IT executes the BNE RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_bne(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = regs.get(get_rs1(insn)); // get register rs1
    uint32_t rs2 = regs.get(get_rs2(insn)); // get register rs2
    int32_t imm_b = get_imm_b(insn); // get imm_b
    if (pos)
    {
        std::string s = render_btype(insn, "bne");
        s.resize(instruction_width, ' ');
        *pos << s << "// " << std::dec << "pc += (" << hex0x32(rs1) << " != " << hex0x32(rs2)
             << " ? " << hex0x32(imm_b)
             << " : 4) = " << hex0x32(rs1 != rs2 ? pc += imm_b : pc += 4);
    }
    if (pos == nullptr)
    {
        (rs1 != rs2 ? pc += imm_b : pc += 4); // increment pc
    }
}
/**
 * Execute lb instruction
 * IT executes the LB RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_lb(uint32_t insn, std::ostream* pos)
{

    uint32_t rd = get_rd(insn); // rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    int32_t imm_i = get_imm_i(insn); // imm_i
    int32_t address = mem->get8(rs1 + imm_i); // memory address
    // check the MSB if its set to 1 | with 0xFFFFFF00
    if ((address & 0x00000080) == 0x00000080)
    {
        address = mem->get8(rs1 + imm_i) | 0xFFFFFF00;
    }
    regs.set(rd, address); // set rd to addr
    pc += 4; // icrement pc by 4
    if (pos)
    {
        std::string s = render_itype_load(insn, "lb");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = sx(m8(" << hex0x32(rs1) << " + " << hex0x32(imm_i)
             << " )) = " << hex0x32(regs.get(rd));
    }
}
/**
 * Execute lbu instruction
 * IT executes the LBU RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_lbu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // get register rs1
    uint32_t imm_i = get_imm_i(insn); // get imm_i
    regs.set(rd, mem->get8((rs1 + imm_i))); // set rd to mem->get8(rs1+imm_i)
    rd = mem->get8((rs1 + imm_i)); // get8(rs1 + imm_i)
    pc += 4; // increment pc by 4
    if (pos)
    {
        std::string s = render_itype_load(insn, "lbu");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << get_rd(insn) << " = zx(m8(" << hex0x32(rs1) << " + " << hex0x32(imm_i)
             << " )) = " << hex0x32(rd);
    }
}
/**
 * Execute lh instruction
 * IT executes the LH RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos
 * @return none
 ********************************************************************************/
void rv32i::exec_lh(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn); // rd
    int32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    int32_t imm_i = get_imm_i(insn); // imm_i
    int32_t address = (mem->get16(rs1 + imm_i)); // memory address
    // if msb is 1 then | with 0xffff0000
    if ((address & 0x00008000) == 0x00008000)
    {
        address = (mem->get16(rs1 + imm_i) | 0xffff0000);
    }
    regs.set(rd, address); // set rd to address
    pc += 4; // increment pc by 4
    if (pos)
    {
        std::string s = render_itype_load(insn, "lh");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = sx(m16(" << hex0x32(rs1) << " + " << hex0x32(imm_i)
             << ")) = " << hex0x32(regs.get(rd));
    }
}
/**
 * Execute lhu instruction
 * IT executes the LHU RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_lhu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    uint32_t imm_i = get_imm_i(insn); // get imm_i
    regs.set(rd, mem->get16((rs1 + imm_i))); // set rd to memory address get16(rs1+imm_i)
    pc += 4; // increment pc with 4
    if (pos)
    {
        std::string s = render_itype_load(insn, "lhu");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = zx(m16(" << hex0x32(rs1) << " + " << hex0x32(imm_i)
             << " )) = " << hex0x32(mem->get16(rs1 + imm_i));
    }
}
/**
 * Execute lw instruction
 * IT executes the LW RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_lw(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    uint32_t imm_i = get_imm_i(insn); // imm_i
    regs.set(rd, mem->get32(rs1 + imm_i)); // set rd to memory address get32(rs1+imm_i)
    pc += 4; // increment pc by 4
    if (pos)
    {
        std::string s = render_itype_load(insn, "lw");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = sx(m32(" << hex0x32(rs1) << " + " << hex0x32(imm_i)
             << " )) = " << hex0x32(regs.get(rd));
    }
}
/**
 * Execute or instruction
 * IT executes the OR RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_or(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    uint32_t rs2 = regs.get(get_rs2(insn)); // register rs2
    if (pos)
    {
        std::string s = render_rtype(insn, "or");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = " << hex0x32(rs1) << " | " << hex0x32(rs2) << " = "
             << hex0x32(rs1 | rs2);
    }
    regs.set(rd, (rs1 | rs2)); // set rd to rs1 | rs2
    pc += 4; // increment pc by 4
}
/**
 * Execute ori instruction
 * IT executes the ORI RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_ori(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    uint32_t imm_i = get_imm_i(insn); // get imm_i
    if (pos)
    {
        std::string s = render_itype_alu(insn, "ori", imm_i);
        ;
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = " << hex0x32(rs1) << " | " << hex0x32(imm_i) << " = "
             << hex0x32(rs1 | imm_i);
    }
    regs.set(rd, (rs1 | imm_i)); // set rd to rs1 | imm_i
    pc += 4; // increment pc by 4
}
/**
 * Execute sb instruction
 * IT executes the SB RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_sb(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    uint32_t rs2 = regs.get(get_rs2(insn)); // register rs2
    uint32_t imm_s = get_imm_s(insn); // imm_s
    if (pos)
    {
        std::string s = render_stype(insn, "sb");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "m8(" << hex0x32(rs1) << " + " << hex0x32(imm_s)
             << ") = " << hex0x32(rs2 & 0x000000ff);
    }
    mem->set8(rs1 + imm_s, rs2 & 0x000000ff); // set memory at address rs1+imm_S to rs2&0x000000ff
    pc += 4; // increment pc by 4
}
/**
 * Execute sh instruction
 * IT executes the LUI RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_sh(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    uint32_t imm_s = get_imm_s(insn);
    // unsigned short mask = (1 << (16-0))-1;
    uint32_t addr = regs.get(rs1) + imm_s;
    uint32_t target = regs.get(rs2) & 0x0000ffff;
    if (pos)
    {
        std::string s = render_stype(insn, "sh");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "m16(" << hex0x32(regs.get(rs1)) << " + " << hex0x32(imm_s)
             << ") = " << hex0x32(target);
    }
    mem->set16(addr, target); // set addr to target
    pc += 4; // incremet pc by 4
}
/**
 * Execute sll instruction
 * IT executes the SLL RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_sll(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    uint32_t rs2 = regs.get(get_rs2(insn)); // register rs2
    regs.set(rd, rs1 << (rs2 % XLEN)); // set rd to rs1<<(Rs2%xlen)
    pc += 4; // increment pc by 4
    if (pos)
    {
        std::string s = render_rtype(insn, "sll");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = " << hex0x32(rs1) << " << " << rs2 % XLEN << " = "
             << hex0x32(regs.get(rd));
    }
}
/**
 * Execute slli instruction
 * IT executes the SLLI RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_slli(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    uint32_t imm_i = get_imm_i(insn); // get imm_i
    regs.set(rd, rs1 << imm_i); // set rd to rs1 << imm_i
    pc += 4; // increment pc by 4
    if (pos)
    {
        std::string s = render_itype_shamt(insn, "slli");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = " << hex0x32(rs1) << " << " << imm_i << " = "
             << hex0x32(regs.get(rd));
    }
}
/**
 * Execute slt instruction
 * IT executes the SLT RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_slt(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    int32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    int32_t rs2 = regs.get(get_rs2(insn)); // register rs2
    (rs1 < rs2 ? regs.set(rd, 1) : regs.set(rd, 0)); // set regs 1 or 0 based on condition rs1 < rs2
    pc += 4;
    if (pos)
    {
        std::string s = render_rtype(insn, "slt");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = (" << hex0x32(rs1) << " < " << hex0x32(rs2)
             << ") ? 1 : 0 = " << hex0x32(regs.get(rd));
    }
}
/**
 * Execute slti instruction
 * IT executes the SLTI RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_slti(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    int32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    int32_t imm_i = get_imm_i(insn); // get imm_i
    if (rs1 < imm_i)
    {
        regs.set(rd, 1);
    }
    else
    {
        regs.set(rd, 0);
    }
    (rs1 < imm_i ? regs.set(rd, 1)
                 : regs.set(rd, 0)); // set rd to 0 or 1 based on condition rs1 <imm_i
    pc += 4; // increment pc by 4
    if (pos)
    {
        std::string s = render_itype_alu(insn, "slti", imm_i);
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = (" << hex0x32(rs1) << " < " << imm_i
             << ") ? 1 : 0 = " << hex0x32(regs.get(rd));
    }
}
/**
 * Execute sltiu instruction
 * IT executes the SLTIU RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_sltiu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    uint32_t imm_i = get_imm_i(insn); // get imm_i
    (rs1 < imm_i ? regs.set(rd, 1)
                 : regs.set(rd, 0)); // set rd to 0 or 1 based to condition rs1 < imm_i
    pc += 4; // increment pc by 4
    if (pos)
    {
        std::string s = render_itype_alu(insn, "sltiu", imm_i);
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = (" << hex0x32(rs1) << " <U " << imm_i
             << ") ? 1 : 0 =" << hex0x32(regs.get(rd));
    }
}
/**
 * Execute sltu instruction
 * IT executes the SLTU RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_sltu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    uint32_t rs2 = regs.get(get_rs2(insn)); // register rs2
    (rs1 < rs2 ? regs.set(rd, 1) : regs.set(rd, 0)); // set rd to 1 or 0 based on cond rs1 < rs2
    pc += 4; // increment pc by 4
    if (pos)
    {
        std::string s = render_rtype(insn, "sltu");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = (" << hex0x32(rs1) << " <U " << hex0x32(rs2)
             << ") ? 1 : 0 = " << hex0x32(regs.get(rd));
    }
}
/**
 * Execute sra instruction
 * IT executes the SRAI RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_sra(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // ged rd
    int32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    int32_t rs2 = regs.get(get_rs2(insn)); // register rs2
    regs.set(rd, rs1 >> rs2); // set rd to rs1>>rs2
    pc += 4; // increment pc by 4
    if (pos)
    {
        std::string s = render_rtype(insn, "sra");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = " << hex0x32(rs1) << " >> " << (rs2 % XLEN) << " = "
             << hex0x32(regs.get(rd));
    }
}
/**
 * Execute srai instruction
 * IT executes the SRAI RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_srai(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    int32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    uint32_t imm_i = get_imm_i(insn); // get imm_i
    regs.set(rd, rs1 >> imm_i); // set rd to rs1 >>imm_i
    pc += 4; // increment pc by 4
    if (pos)
    {
        std::string s = render_itype_shamt(insn, "srai");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = " << hex0x32(rs1) << " >> " << (imm_i & 0X0000001f) << " = "
             << hex0x32(regs.get(rd));
    }
}
/**
 * Execute srl instruction
 * IT executes the SRL RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_srl(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    uint32_t rs2 = regs.get(get_rs2(insn)); // register rs2
    regs.set(rd, rs1 >> rs2); // set rd to rs1 >> rs2
    pc += 4; // increment pc by 4
    if (pos)
    {
        std::string s = render_rtype(insn, "srl");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = " << hex0x32(rs1) << " >> " << (rs2 % XLEN) << " = "
             << hex0x32(regs.get(rd));
    }
}
/**
 * Execute sub instruction
 * IT executes the SUB RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_sub(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    uint32_t rs2 = regs.get(get_rs2(insn)); // register rs2
    regs.set(rd, rs1 - rs2); // set rd to rs1-rs2
    pc += 4; // increment pc by 4
    if (pos)
    {
        std::string s = render_rtype(insn, "sub");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = " << hex0x32(rs1) << " - " << hex0x32(rs2) << " = "
             << hex0x32(rs1 - rs2);
    }
}
/**
 * Execute sw instruction
 * IT executes the SW RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_sw(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    uint32_t rs2 = regs.get(get_rs2(insn)); // register rs2
    uint32_t imm_s = get_imm_s(insn); // get imm_s
    if (pos)
    {
        std::string s = render_stype(insn, "sw");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "m32(" << hex0x32(rs1) << " + " << hex0x32(imm_s)
             << ") = " << hex0x32(rs2 & 0xffffffff);
    }
    mem->set32((rs1 + imm_s), rs2 & 0xffffffff); // set memory at rs1+imms to rs2 & 0xffffffff
    pc += 4; // increment pc by 4
}
/**
 * Execute xor instruction
 * IT executes the XOR RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_xor(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    uint32_t rs2 = regs.get(get_rs2(insn)); // register rs2
    if (pos)
    {
        std::string s = render_rtype(insn, "xor");
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = " << hex0x32(rs1) << " ^ " << hex0x32(rs2) << " = "
             << hex0x32(rs1 ^ rs2);
    }
    regs.set(rd, rs1 ^ rs2); // set rd to rs1^rs2
    pc += 4; // increment pc by 4
}
/**
 * Execute xori instruction
 * IT executes the xori RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_xori(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); // get rd
    uint32_t rs1 = regs.get(get_rs1(insn)); // register rs1
    uint32_t imm_i = get_imm_i(insn); // get imm_i
    if (pos)
    {
        std::string s = render_itype_alu(insn, "xori", imm_i);
        ;
        s.resize(instruction_width, ' ');
        *pos << s << "// "
             << "x" << rd << " = " << hex0x32(rs1) << " ^ " << hex0x32(imm_i) << " = "
             << hex0x32(rs1 ^ imm_i);
    }
    regs.set(rd, (rs1 ^ imm_i)); // set rd to rs1 ^ imm_i
    pc += 4; // incremet pc by 4
}
/**
 * Execute fence instruction
 * IT executes the fence RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_fence(uint32_t insn, std::ostream* pos)
{
    if (pos)
    {
        std::string s = render_fence(insn);
        s.resize(instruction_width, ' ');
        *pos << s << "// fence ";
    }
    pc += 4; // incremet pc by 4
}
/**
 * Execute fence instruction
 * IT executes the fence RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_ecall(uint32_t insn, std::ostream* pos)
{
    if (pos)
    {
        std::string s = render_ecall();
    }
    pc += 4; // incremet pc by 4
}
/**
 * Execute ebreak instruction
 * IT executes the EBREAK RV32I instruction, renders the details of what it has simulated.
 * it uses render() helper functions
 * @param uint32_t insn, std::ostream* pos)
 * @return none
 ********************************************************************************/
void rv32i::exec_ebreak(uint32_t insn, std::ostream* pos)
{
    if (pos)
    {
        std::string s = render_ebreak();
        s.resize(instruction_width, ' ');
        *pos << s << "// HALT \n";
    }
    halt = true; // set halt flag to ture
    std::cout << "Execution terminated by EBREAK instruction";
}
/**
 * Function tick executes 1 instruction
 * if the halt flag is true than returns without doing anything, else it increments
 * insn_counter. If show_register is true it dumps the sate of hart otherwise does nothing
 * it fetches an instruction from the memory at address of pc regiser. If show instruction
 * is true then print the value of pc regiser and fetched instruction, call dcex(insn,&std::cout)
 * to execute instruction and render the instruction and simulation details else call
 * dcex(insn,nullptr) to execute the instruction without rendering anything
 * @param none
 * @return none
 ********************************************************************************/
void rv32i::tick()
{
    if (is_halted())
    {
        return; // if halt ==true return
    }
    else
    {
        ++insn_counter; // increment insn_counter
        if (show_registers == true)
        {
            dump(); // if show_register true dump()
        }
        uint32_t insn = mem->get32(pc); // fetch an instruction
        if (show_instructions)
        {
            std::cout << hex32(pc) << ": "; // print pc
            std::cout << hex32(insn) << "  "; // print instructon
            dcex(insn, &std::cout); // call dcex
            std::cout << endl;
        }
        else
        {
            dcex(insn, nullptr); // if show_instruciton ==false  execute instruction without
                                 // rendering anything
        }
    }
}
/**
 * run-loop
 * call reset to reset pc insn_counter and halt flag then set register 2 to 0x00000100
 * Enter a loop that will cal tick() until the halt flag is set or limit number of
 * instructions have been executed. When loop is completed print out how many
 * instruction where executed
 * @param uint64_t limit
 * @return none
 ********************************************************************************/
void rv32i::run(uint64_t limit)
{
    reset(); // rest pc,insnscounter and halt
    regs.set(2, mem->get_size()); // set reg 2 to 0x00000100
    while ((limit == 0 || (limit != 0 && insn_counter < limit)) && !is_halted())
    {
        tick(); // cal tick
    }
    if (show_instructions == false)
    {
        std::cout << endl;
    }
    std::cout << insn_counter << " instructions executed" << std::endl;
}
