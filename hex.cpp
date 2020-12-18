

#include "hex.h"

/** 
*  string hex8(uint8_t i) takes an uint8_t i and prints the 2 hex digits of the argument i
*  hex8 will return the 2 hex digits representing the 8 bits of uint8_t i argument
* @param x uint8_t i
* @return 2 hex digits of argument i
* @note
* @warning
* @bug
*************************************************************************************************************/
std::string hex8(uint8_t i)
{
    std::ostringstream os;
    os << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(i);
    return os.str();
}
/** 
* string hex32(uint8_t i) takes an uint32_t i and prints the 8 hex digits of the argument i
* hex32 will return the 8 hex digits representing the 32 bits of uint32_t i argument
* @param x uint32_t i
* @return 8 hex digits of argument i
* @note
* @warning
* @bug
*************************************************************************************************************/
std::string hex32(uint32_t i)
{
    std::ostringstream os;
    os << std::hex << std::setfill('0') << std::setw(8) << static_cast<uint32_t>(i);
    return os.str();
}
/**
* string hex32(uint8_t i) takes an uint32_t i and prints "0x" and the 8 hex digits of the
* argument i
* hex32 will print out 0x with the 8 hex digits of the argument i
* @param x uint32_t i
* @return 8 hex digits of argument i with string"0x"in front
* @note
* @warning
* @bug
*************************************************************************************************************/
std::string hex0x32(uint32_t i)
{
    return std::string("0x") + hex32(i);
}
