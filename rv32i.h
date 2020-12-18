
#ifndef RV32I_H
#define RV32I_H
#include "registerfile.h"
#include "hex.h"
#include "memory.h"
class rv32i
{
public:
    bool show_instructions = false; 
    bool show_registers= false;
    rv32i(memory* m);
    void disasm(void);
    std::string decode(uint32_t insn) const; // function decode prototype
    static uint32_t get_opcode(uint32_t insn); 
    static uint32_t get_rd(uint32_t insn); 
    static uint32_t get_funct3(uint32_t insn); 
    static uint32_t get_rs1(uint32_t insn); 
    static uint32_t get_rs2(uint32_t insn); 
    static uint32_t get_funct7(uint32_t insn); 
    static int32_t get_imm_i(uint32_t insn); 
    static int32_t get_imm_u(uint32_t insn); 
    static int32_t get_imm_b(uint32_t insn); 
    static int32_t get_imm_s(uint32_t insn); 
    static int32_t get_imm_j(uint32_t insn); 
    std::string render_illegal_insn() const; 
    std::string render_lui(uint32_t insn) const; 
    std::string render_auipc(uint32_t insn) const; 
    std::string render_jal(uint32_t insn) const; 
    std::string render_jalr(uint32_t insn) const;
    std::string render_rtype(uint32_t insn, const char* mnemonic) const; 
    std::string render_btype(uint32_t insn, const char* mnemonic) const; 
    std::string render_itype_load(uint32_t insn, const char* mnemonic) const; 
    std::string render_itype_alu(uint32_t insn, const char* mnemonic, int32_t imm_i) const; 
    std::string render_itype_shamt(uint32_t insn, const char* mnemonic) const; 
    std::string render_stype(uint32_t insn, const char* mnemonic) const; 
    std::string render_fence(uint32_t insn) const; 
    std::string render_ecall() const; 
    std::string render_ebreak() const; 
    static constexpr uint32_t XLEN = 32; 
    void exec_illegal_insn(uint32_t insn, std::ostream* pos); 
    void exec_lui(uint32_t insn, std::ostream* pos) ; 
    void exec_auipc(uint32_t insn, std::ostream* pos) ; 
    void exec_jal(uint32_t insn, std::ostream* pos); 
    void exec_jalr(uint32_t insn, std::ostream* pos); 
    void exec_add(uint32_t insn, std::ostream* pos); 
    void exec_addi(uint32_t insn, std::ostream* pos); 
    void exec_and(uint32_t insn, std::ostream* pos); 
    void exec_andi(uint32_t insn, std::ostream* pos); 
    void exec_beq(uint32_t insn, std::ostream* pos); 
    void exec_bge(uint32_t insn, std::ostream* pos);
    void exec_bgeu(uint32_t insn, std::ostream* pos); 
    void exec_blt(uint32_t insn, std::ostream* pos); 
    void exec_bltu(uint32_t insn, std::ostream* pos); 
    void exec_bne(uint32_t insn, std::ostream* pos); 
    void exec_lb(uint32_t insn, std::ostream* pos); 
    void exec_lbu(uint32_t insn, std::ostream* pos); 
    void exec_lh(uint32_t insn, std::ostream* pos);  
    void exec_lhu(uint32_t insn, std::ostream* pos); 
    void exec_lw(uint32_t insn, std::ostream* pos); 
    void exec_or(uint32_t insn, std::ostream* pos); 
    void exec_ori(uint32_t insn, std::ostream* pos); 
    void exec_sb(uint32_t insn, std::ostream* pos); 
    void exec_sh(uint32_t insn, std::ostream* pos); 
    void exec_sll(uint32_t insn, std::ostream* pos); 
    void exec_slli(uint32_t insn, std::ostream* pos); 
    void exec_slt(uint32_t insn, std::ostream* pos) ; 
    void exec_slti(uint32_t insn, std::ostream* pos); 
    void exec_sltiu(uint32_t insn, std::ostream* pos); 
    void exec_sltu(uint32_t insn, std::ostream* pos); 
    void exec_sra(uint32_t insn, std::ostream* pos);  
    void exec_srai(uint32_t insn, std::ostream* pos); 
    void exec_srl(uint32_t insn, std::ostream* pos); 
    void exec_srli(uint32_t insn, std::ostream* pos); 
    void exec_sub(uint32_t insn, std::ostream* pos); 
    void exec_sw(uint32_t insn, std::ostream* pos); 
    void exec_xor(uint32_t insn, std::ostream* pos); 
    void exec_xori(uint32_t insn, std::ostream* pos); 
    void exec_fence(uint32_t insn,std::ostream* pos); 
    void exec_ecall(uint32_t insn,std::ostream* pos); 
    void exec_ebreak(uint32_t insn,std::ostream* pos); 
    void reset(); // reset prototype
    void dump() const; // dump prototype    
    void set_show_instructions(bool b); 
    void set_show_registers(bool b); 
    bool is_halted() const; 
    void dcex(uint32_t insn, std::ostream*); //dcex prototype
    void tick(); 
    void run(uint64_t limit); //run prototype
private:
    memory* mem; // pointer pointing to memory object
    uint32_t pc = 0; // contains the address of instruction being decoded
    registerfile regs; 
    bool halt = false; 
    uint64_t insn_counter; // insn_counter to keep track of how many instructins are executed 
};

#endif