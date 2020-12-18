
#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
using namespace std;

class memory
{
public:
    memory(uint32_t siz); // constructor prototype
    ~memory(); // destructor protopye
    bool check_address(uint32_t i) const; 
    uint32_t get_size() const; 
    uint8_t get8(uint32_t addr) const; 
    uint16_t get16(uint32_t addr) const; 
    uint32_t get32(uint32_t addr) const; 
    void set8(uint32_t addr, uint8_t val); 
    void set16(uint32_t addr, uint16_t val); 
    void set32(uint32_t addr, uint32_t val); 
    void dump() const; 
    bool load_file(const string& fname); 
private:
    uint8_t* mem; // memory simulator array
    uint32_t size; // size of memory
};

#endif
