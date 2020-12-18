
#include "memory.h"
#include "hex.h"


// An array of bytes representing the simulated memory
uint8_t* mem;
// size variable which will be used in the dump() function
uint32_t size;

/** 
* memory constructor
* Allocates size bytes for the memory array and initiliazes every byte to 0xa5
* @param uint32_t siz
* @return 
* @note
* @warning
* @bug
*************************************************************************************************************/
memory::memory(uint32_t siz)
{
    // rounds the length up
    size = (siz + 15) & 0xfffffff0;
    // allocates siz bytes for the array
    mem = new uint8_t[size];
    // initiliazes every byte to 0xa5
    for (unsigned int i = 0; i < size; i++)
    {
        mem[i] = 0xa5;
    }
}

/** 
* destructor frees up the memory that was allocated on the constructor
* @param
* @return
* @note
* @warning
* @bug
*************************************************************************************************************/
memory::~memory()
{
    // frees the memory allocated on the constructor
    delete[] mem;
}

/** 
*  bool_check_addres(uint32_t i) check if the given address is in the simulated memory
*  return true if the address is in the simulated memory and when its not on the simulated memory,
*  prints a warning and returns false
* @param x uint32_t i
* @return true or false
* @note
* @warning
* @bug
*************************************************************************************************************/
bool memory::check_address(uint32_t i) const
{
    if (i < size)
    {
        return true;
    }
    else
    {
        std::cout << "WARNING: Address out of range: " << hex0x32(i) << std::endl;
        return false;
    }
}

/**  
* get_size() return the rounded up siz value
* @param none
* @return rounded up siz value
* @note
* @warning
* @bug
*************************************************************************************************************/
uint32_t memory::get_size() const
{
    return size;
}

/** 
* memory::get8(uint32_t addr) check if the given addr is in the mem array
* if the addr is inside the mem array then return the value of the byte, if not
* return zero.
* @param uint32_t addr
* @return value of the bye from the memoryat that addres or zero
* @note
* @warning
* @bug
*************************************************************************************************************/
uint8_t memory::get8(uint32_t addr) const
{
    if (check_address(addr))
    {
        return mem[addr];
    }
    else
    {
        return 0;
    }
}

/** 
* memory::get16(uint32_t addr) get the two bytes from get8 and combines them in little-endian
* to create a 16-bit return value
* @param uint32_t addr
* @return 16-bit in little endian
* @note
* @warning
* @bug
*************************************************************************************************************/
uint16_t memory::get16(uint32_t addr) const
{

    int16_t value = (uint8_t)get8(addr);
    value =value | get8(addr+1)<<8;
    return value;
}

/**
* memory::get32(uint32_t addr) get the four bytes from get16 and combines them in little-endian
* to create a 32-bit return value
* @param uint32_t addr
* @return 32-bit in little endian
* @note
* @warning
* @bug
*************************************************************************************************************/
uint32_t memory::get32(uint32_t addr) const
{
    uint32_t value = (uint32_t)get16(addr);
    value = value | ((uint32_t)get16(addr+2))<<16;
    return value;
}

/**  
* memory::set8(uint32_t addr, uint8_t val) sets the bytes in the simulated memory
* it calls check_address() to verfy that the addr argumn is valid if addr is valid then set
* the byte in the simulated memory  to the given value val  if its not valid then return to
* the caller
* @param uint32_t addr, uint8_t val
* @return nothing
* @note
* @warning
* @bug
*************************************************************************************************************/
void memory::set8(uint32_t addr, uint8_t val)
{
    if (check_address(addr))
    {
        mem[addr] = val;
    }
    else
    {
        return;
    }
}

/**  
* memory::set16(uint32_t addr,uint16_t val) stores the given value in little endian
*calls set8 twice to storve the given value val in little endian starting at add
* @param uint32_t addr, uint16_t val
* @return nothing
* @note
* @warning
* @bug
*************************************************************************************************************/
void memory::set16(uint32_t addr, uint16_t val)
{
    set8(addr, val&0x00ff0000);
    set8(addr+1, (val>>8)&0x00ff);
    mem[addr] = val;
}

/**  
* memory::set32(uint32_t addr,uint32_t val) stores the given value into memory in little endian
* it calls set16() twice and stores the given value into memory in little endian format starting
* at addr
* @param uint32_t addr, uint32_t val
* @return nothing
* @note
* @warning
* @bug
*************************************************************************************************************/
void memory::set32(uint32_t addr, uint32_t val)
{
    set16(addr, val&0x0000ffff);
    set16(addr+2, (val>>16)&0x0000ffff);
    mem[addr] = val;
}

/**  
* memory::dump() dumps whats on the stimulated memory
* dump() dumps the entire contents of the simulated memory in hex with the ascii on the right
* @param
* @return None
* @note
* @warning
* @bug
*************************************************************************************************************/
void memory::dump() const
{
    // char array of 17
    char ascii[17];
    ascii[16] = 0;
    // loop that goes from 0 to the size of the simulated memory
    for (uint32_t i = 0; i < size; i++)
    {
        // print whats on the
        if (i % 16 == 0)
        {
            if (i != 0)
            {
                std::cout << " *";
                // loop through the ascii array and print whats there
                    std::cout << ascii;   
                std::cout << "*" << endl;
            }
            std::cout << hex32(i) << ":"; // prints register number
        }
        // ch is the value of the byte at i
        uint8_t ch = get8(i);
        // print 2 spaces every 8 bytes and 1 space after every 1 byte
        std::cout << (i % 16 == 8 ? "  " : " ") << hex8(ch);
        // if its printable print that character otherwise print '.'
        ascii[i % 16] = isprint(ch) ? ch : '.';
    }
    std::cout << " *";
    // loop through the ascii array and print whats there
    
        std::cout << ascii;
    
    std::cout << "*" << endl;
}

/** 
* bool memory::load_file() opens the file in binary mode and reads its contents in simulated
* memory
* if the file cant be openes print an error saying cant open the file and return false, otherwise if
* its able to open the file, read the file one byte at a time, we check the bytes with check_address before
* writting. if check_address returns true we keep on writing otherwise we print Program too big, we close the
* file and turn false. if it loads ok we close the file and return true;
* @param &fname
* @return true or false
* @note
* @warning
* @bug
*************************************************************************************************************/
bool memory::load_file(const std::string& fname)
{
    char val;
    int index = 0;
    // opens a file in binary mode
    std::ifstream infile(fname, std::ios::in | std::ios::binary);

    // checks if file can be opened or not
    if (!infile)
    {
        std::cerr << "Can't open file " << fname << " for reading." << std::endl;
        return false;
    }
    // get 1 character at a time
    while (infile.get(val))
    {
        // if the address is available
        if (check_address(index))
        {
            // if the address is available sets val into memory[index]
            mem[index] = val;
        }
        else
        {
            // if the check_address return false then we print program too big and close the file
            std::cerr << "Program too big." << std::endl;
            infile.close();
            return false;
        }
        // increment the index
        index++;
    }
    // close thefile
    infile.close();
    return true;
}
