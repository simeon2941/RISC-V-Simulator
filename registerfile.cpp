
#include "registerfile.h"
/**
 * reset function
 * Which sets register 0 to 0 and the rest of registers to 0xf0f0f0f0
 * @param none
 * @return none
 *************************************************************************************************************/
void registerfile::reset()
{
    reg = new int32_t[32]; // allocate memory
    reg[0] = 0x00000000; // set register 0 to 0x000000
    for (uint32_t i = 1; i < 32; i++)
    {
        reg[i] = 0xf0f0f0f0; //set the rest of the registers to 0xf0f0f0f0
    }
}
/**
 * Constructor registerfile
 * Registerfile calls reset() which does the job of initiliazin reg 0 to 0 and the rest of regs to
 *0xf0f0f0f0
 * @param none
 * @return none
 *************************************************************************************************************/
registerfile::registerfile()
{
    reset(); //call reset 
}
/**
 * Sets register r to the given value
 * This function sets register r to the given value, if r is zero then we do nothing
 * @param uint32_t r, int32_t val
 * @return nothing
 *************************************************************************************************************/
void registerfile::set(uint32_t r, int32_t val)
{
    if (r != 0)
    {
        reg[r] = val; //put val into reg[r]
    }
}
/**
 * Return the value of register r
 * This function will return the value of register r if r is zero then return zero.
 * @param uint32_t r
 * @return the value of register r
 *************************************************************************************************************/
int32_t registerfile::get(uint32_t r) const
{
    if (r == 0)
    {
        return 0; //return 0 if its zero
    }
    else
    {
        return reg[r]; //return the value at reg[r]
    }
}
/**
 * Dump the registers
 * this function with dump the values of the 32 registers printing 8 registers
 * per line.
 * @param none
 * @return none
 *************************************************************************************************************/
void registerfile::dump() const
{
    int32_t count = 0;
    string s = " ";
    uint32_t i = 0;
    while (i < 32)
    {
        if (i > 9)
        {
            s = ""; //no spaces
        }
        if (count == 8)
        {
            cout << endl; //print a new line after printing 8 registers
            count = 0;
        }
        if ((i % 8) == 0)
        {
            cout << s << "x" << i << " "; //format the numbers x0 x8 x16 x24
        }
        count++;
        (count % 8 == 0) ? cout << hex32(reg[i]) : cout << hex32(reg[i]) << " "; // print the values of registers
        i++;
    }
    cout << endl;
}
