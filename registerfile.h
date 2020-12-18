
#ifndef REGISTERFILE_H
#define REGISTERFILE_H
#include <iostream>
#include "memory.h"
#include "hex.h"
class registerfile
{
public:
    registerfile(); // constructor
    void reset(); 
    void set(uint32_t r, int32_t val); 
    int32_t get(uint32_t r) const; 
    void dump() const; // function dump prototype
private:
    int32_t* reg; // private array of int32_t elements
};
#endif