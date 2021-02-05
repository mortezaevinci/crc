#ifndef EVINCI_CRC_H
#define EVINCI_CRC_H
#ifndef INBOOTLOADER
#include "Arduino.h"
#endif
#define P_16        0xA001// crc with 0xA001 polynomial and order 16
#define P_32        0xEDB88320L //extended crc with polynomial 0x4c11db7, order=32, init=xor=0xffffffff, refin=refout=1
#define P_CCITT     0x1021// crc with 0x1021 polynomial and order 16
#define P_DNP       0xA6BC// crc with 0xA6BC polynomial and order 16
#define P_KERMIT    0x8408// crc with 0x4599 polynomial and order 16 (Maybe, extended crc with 0x1021 poly, order,16 init=xor=0, refin=refout=1, reversed-ordered bytes crc)
#define P_SICK      0x8005 //extended crc with 0x8005 polynomial and order 16, init=0,xor=0,refin=refout=1,
#define P_CAN		0xc599 // crc with 0x4599 polynomial and order 15 (Order 15 and 4 instead of c, because then x^15 is implieds)

//CRC-CCITT(0x1D0F)= extebnded crc, 0x1021, order=16, refin=refout=0, init=0x1d0f, xor=0
//CRC-CCITT(0xFFFF)= extebnded crc, 0x1021, order=16, refin=refout=0, init=0xFFFF, xor=0
//CRC-CCITT(XMODEM)= extebnded crc, 0x1021, order=16, refin=refout=0, init=0x0000, xor=0
//CRC MODBUS=extended crc, 0x8005, order=16, refin=refout=1, init=0xffffm xor=0
#ifdef INBOOTLOADER
unsigned long crcbitbybitfast_simplified(const unsigned char* p,const  int len,const unsigned long polynom,const  unsigned long crcinit,const  int bitorder);
#else
unsigned long reflect(unsigned long crc, int bitnum);
unsigned long crcbitbybitfast(const byte* p, const int len,const  unsigned long polynom, const  unsigned long crcinit, const  unsigned long crcxor,const int bitorder,const bool refout,const bool refin);
unsigned long crcbitbybitfast(const String p,const  int len,const unsigned long polynom,const  unsigned long crcinit,const  unsigned long crcxor,const  int bitorder,const  bool refout,const  bool refin);
uint8_t CRC8_MAXIM(const byte *addr, uint8_t len, uint8_t offset);
uint16_t CRC_RTU(const byte *ptr, uint8_t length, uint8_t offset);
uint16_t Get_crc16_tab_Val(uint16_t i, uint16_t crcConstant);
uint16_t update_crc_16(uint16_t crc, byte c, uint16_t crcConstant);
uint16_t Get_crc_16(const byte *ptr, uint8_t length, uint8_t offset, uint16_t initialValue = 0);
//initial value=FFFF, reverse data bytes, and reverse crc before XOR
uint16_t Get_crc_16_modbus(const byte *ptr, uint8_t length, uint8_t offset);
uint16_t Get_crc_CANBUS(const byte *ptr,uint8_t length,uint8_t offset);
uint16_t Get_crc_sick(const byte *ptr, uint8_t length, uint8_t offset);
uint16_t Get_crc_dnp(const byte *ptr, uint8_t length, uint8_t offset);
uint16_t Get_crc_kermit(const byte *ptr, uint8_t length, uint8_t offset);
uint16_t update_crc_sick(uint16_t crc, byte c, byte prev_byte);
uint16_t Get_crc_ccitt_tab_Val(uint16_t i);
uint16_t update_crc_ccitt(uint16_t crc, byte c);
uint16_t Get_crc_ccitt(const byte *ptr, uint8_t length, uint8_t offset, uint16_t initValue); 
uint16_t Get_crc_ccitt_0000(const byte *ptr, uint8_t length, uint8_t offset); //initial value 0000,no reverse CRC b4 Final XOR,no reverse data bytes 
uint16_t Get_crc_ccitt_ffff(const byte *ptr, uint8_t length, uint8_t offset); // initial value ffff,no reverse CRC b4 Final XOR,no reverse data bytes 
uint16_t Get_crc_ccitt_1d0f(const byte *ptr, uint8_t length, uint8_t offset); //initial value 1d0f,no reverse CRC b4 Final XOR,no reverse data bytes 
uint32_t update_crc_32(uint32_t crc, byte c);
uint32_t Get_crc_32(const byte *ptr, uint8_t length, uint8_t offset);
uint8_t Get_checksum(const byte *ptr, uint8_t length, uint8_t offset);
#endif
#endif