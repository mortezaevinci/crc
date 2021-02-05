#include "evinci_crc.h"

#ifdef INBOOTLOADER
unsigned long crcbitbybitfast_simplified(const unsigned char* p,const  int len,const unsigned long polynom,const  unsigned long crcinit,const  int bitorder) 
{
  // fast bit by bit algorithm without augmented zero bytes.
  // does not use lookup table, suited for polynom orders between 1...32.
  unsigned long crcmask = ((((unsigned long)1<<(bitorder-1))-1)<<1)|1;
  unsigned long crchighbit = (unsigned long)1<<(bitorder-1);
  unsigned long j, c, bit;
  unsigned long crc = crcinit;//crcinit_direct; //using direct approach

  for (int i=0; i<len; i++) 
  {
    c = (unsigned long)p[i];

    for (j=0x80; j; j>>=1) 
    {
      bit = crc & crchighbit;
      crc<<= 1;
      if (c & j) bit^= crchighbit;
      if (bit) crc^= polynom;
    }
  } 
  crc&= crcmask;
  return(crc);
}
#else

unsigned long reflect(unsigned long crc, int bitnum) 
{

	// reflects the lower 'bitnum' bits of 'crc'
	unsigned long i, j=1, crcout=0;

	for (i=(unsigned long)1<<(bitnum-1); i; i>>=1) 
	{
		if (crc & i) crcout|=j;
		j<<= 1;
	}
	return (crcout);
}

unsigned long crcbitbybitfast(const String p,const  int len,const unsigned long polynom,const  unsigned long crcinit,const  unsigned long crcxor,const  int bitorder,const  bool refout,const  bool refin) 
{
	// fast bit by bit algorithm without augmented zero bytes.
	// does not use lookup table, suited for polynom orders between 1...32.
	unsigned long crcmask = ((((unsigned long)1<<(bitorder-1))-1)<<1)|1;
	unsigned long crchighbit = (unsigned long)1<<(bitorder-1);
	unsigned long j, c, bit;
	unsigned long crc = crcinit;//crcinit_direct; //using direct approach

	for (int i=0; i<len; i++) 
	{
		c = (unsigned long)p[i];
		if (refin) c = reflect(c, 8);

		for (j=0x80; j; j>>=1) 
		{

			bit = crc & crchighbit;
			crc<<= 1;
			if (c & j) bit^= crchighbit;
			if (bit) crc^= polynom;
		}
	}	

	if (refout) crc=reflect(crc, bitorder);
	crc^= crcxor;
	crc&= crcmask;

	return(crc);
}

unsigned long crcbitbybitfast(const byte* p,const  int len,const unsigned long polynom,const  unsigned long crcinit,const  unsigned long crcxor,const  int bitorder,const  bool refout,const  bool refin) 
{
	// fast bit by bit algorithm without augmented zero bytes.
	// does not use lookup table, suited for polynom orders between 1...32.
	unsigned long crcmask = ((((unsigned long)1<<(bitorder-1))-1)<<1)|1;
	unsigned long crchighbit = (unsigned long)1<<(bitorder-1);

	unsigned long j, c, bit;
	unsigned long crc = crcinit;//crcinit_direct; //using direct approach

	for (int i=0; i<len; i++) 
	{
		c = (unsigned long)*p++;
		if (refin) c = reflect(c, 8);

		for (j=0x80; j; j>>=1) 
		{

			bit = crc & crchighbit;
			crc<<= 1;
			if (c & j) bit^= crchighbit;
			if (bit) crc^= polynom;
		}
	}	

	if (refout) crc=reflect(crc, bitorder);
	crc^= crcxor;
	crc&= crcmask;

	return(crc);
}


uint8_t CRC8_MAXIM(const byte *addr, uint8_t len, uint8_t offset)
{
     uint8_t crc=0;
     
     for (uint8_t i=offset; i<len;i++)
     {
           uint8_t inbyte = addr[i];
           for (uint8_t j=0;j<8;j++)
           {
                 uint8_t mix = (crc ^ inbyte) & 0x01;
                 crc >>= 1;
                 if (mix)
                       crc ^= 0x8C;
                 
                 inbyte >>= 1;
           }
     }
     return crc;
}

unsigned int CRC_RTU(const byte *ptr, uint8_t length, uint8_t offset)
{
     unsigned int crc = 0xFFFF;
	for(uint8_t pos=offset; pos<length; pos++)
	{
		crc ^= ptr[pos];
		for(int i=8; i!=0; i--)
		{	
			if((crc & 0x0001) != 0)
			{
				crc >>=1;
				crc ^= 0xA001;
			}
			else{
				crc >>=1;
			}
		}
	}
	
    return crc;
}

uint16_t Get_crc16_tab_Val(uint16_t i, uint16_t crcConstant)
{
    int j;
    uint16_t crc, c;

	crc = 0;
	c   = i;

	for (j=0; j<8; j++) {

		if ( (crc ^ c) & 0x0001 ) crc = ( crc >> 1 ) ^ crcConstant;
		else                      crc =   crc >> 1;

		c = c >> 1;
	}

	return crc;
}

uint16_t update_crc_16(uint16_t crc, byte c, uint16_t crcConstant)
{

    uint16_t tmp, short_c;

    short_c = 0x00ff & (uint16_t) c;

    tmp =  crc       ^ short_c;
    crc = (crc >> 8) ^ Get_crc16_tab_Val((tmp & 0xff), crcConstant);

    return crc;
}

uint16_t Get_crc_16(const byte *ptr, uint8_t length, uint8_t offset, uint16_t initialValue)
{
	
	uint16_t crc_16 = initialValue; //initialValue should be zero for crc_16, and 0xffff for crc_16_modbus
	
	for(uint8_t pos=offset;pos<length;pos++)
	{
		crc_16 = update_crc_16(crc_16, ptr[pos],P_16);
	}
	
	return crc_16;
}

uint16_t Get_crc_16_modbus(const byte *ptr, uint8_t length, uint8_t offset)
{
	uint16_t crc_16_modbus  = 0xffff;
	crc_16_modbus = Get_crc_16(ptr, length, offset, crc_16_modbus);
	
	return crc_16_modbus;
}

uint16_t can_crc_next(uint16_t crc, byte data,uint8_t crcConstant)
{
    uint8_t i, j;

    crc ^= (uint16_t)data << 7;

    for (i = 0; i < 8; i++) 
    {
        crc <<= 1;
        if (crc & 0x8000) 
        {
            crc ^= crcConstant;
        }
    }

    return crc & 0x7fff;
}

uint16_t Get_crc_CANBUS(const byte *ptr,uint8_t length,uint8_t offset)
{
	 uint16_t crc;

    crc = 0;

    for (int i = offset; i < length; i++) {
        crc = can_crc_next(crc, ptr[i],P_CAN);
    }
    return crc;
}

uint16_t Get_crc_dnp(const byte *ptr, uint8_t length, uint8_t offset)
{
	uint16_t crc_dnp = 0; 
	uint16_t low_byte, high_byte;
	
	for(uint8_t pos=offset;pos<length;pos++)
	{
		crc_dnp = update_crc_16(crc_dnp, ptr[pos],P_DNP);
	}
	
	crc_dnp = ~crc_dnp;
	low_byte   = (crc_dnp & 0xff00) >> 8;
    high_byte  = (crc_dnp & 0x00ff) << 8;
    crc_dnp    = low_byte | high_byte;
	
	return crc_dnp;
}

uint16_t Get_crc_kermit(const byte *ptr, uint8_t length, uint8_t offset)
{
	uint16_t crc_kermit = 0; 
	uint16_t low_byte, high_byte;
	
	for(uint8_t pos=offset;pos<length;pos++)
	{
		crc_kermit = update_crc_16(crc_kermit, ptr[pos],P_KERMIT);
	}
	
    low_byte   = (crc_kermit & 0xff00) >> 8;
    high_byte  = (crc_kermit & 0x00ff) << 8;
    crc_kermit = low_byte | high_byte;
	
	return crc_kermit;
}

uint16_t update_crc_sick(uint16_t crc, byte c, byte prev_byte)
{
    uint16_t short_c, short_p;

    short_c  =   0x00ff & (uint16_t) c;
    short_p  = ( 0x00ff & (uint16_t) prev_byte ) << 8;

    if ( crc & 0x8000 ) crc = ( crc << 1 ) ^ P_SICK;
    else                crc =   crc << 1;

    crc &= 0xffff;
    crc ^= ( short_c | short_p );

    return crc;
}

uint16_t Get_crc_sick(const byte *ptr, uint8_t length, uint8_t offset)
{
	uint16_t crc_sick = 0; 
	uint16_t low_byte, high_byte;
	
	byte prev_byte = 0;
	
	for(uint8_t pos=offset;pos<length;pos++)
	{
		crc_sick = update_crc_sick(crc_sick, ptr[pos], prev_byte);
		prev_byte = ptr[pos];
	}
	
	low_byte   = (crc_sick & 0xff00) >> 8;
	high_byte  = (crc_sick & 0x00ff) << 8;
	crc_sick   = low_byte | high_byte;
	
	return crc_sick;
}

uint16_t Get_crc_ccitt_tab_Val(uint16_t i)
{
    int j;
    uint16_t crc, c;

	crc = 0;
	c   = i << 8;

	for (j=0; j<8; j++) {

		if ( (crc ^ c) & 0x8000 ) crc = ( crc << 1 ) ^ P_CCITT;
		else                      crc =   crc << 1;

		c = c << 1;
	}

	return crc;
}

uint16_t update_crc_ccitt(uint16_t crc, byte c){

    uint16_t tmp, short_c;

    short_c = 0x00ff & (uint16_t) c;
	
	tmp = (crc >> 8) ^ short_c;
    crc = (crc << 8) ^ Get_crc_ccitt_tab_Val(tmp);

    return crc;
}

uint16_t Get_crc_ccitt(const byte *ptr, uint8_t length, uint8_t offset, uint16_t initValue)
{
	uint16_t crc_ccitt = initValue; 
	//uint16_t low_byte, high_byte;
	
	for(uint8_t pos=offset;pos<length;pos++)
	{
		crc_ccitt = update_crc_ccitt(crc_ccitt, ptr[pos]);
	}
	
	return crc_ccitt;
}

uint16_t Get_crc_ccitt_0000(const byte *ptr, uint8_t length, uint8_t offset)
{
	uint16_t initValue = 0;
	return Get_crc_ccitt(ptr, length, offset, initValue);
}

uint16_t Get_crc_ccitt_ffff(const byte *ptr, uint8_t length, uint8_t offset)
{
	uint16_t initValue = 0xffff;
	return Get_crc_ccitt(ptr, length, offset, initValue);
}

uint16_t Get_crc_ccitt_1d0f(const byte *ptr, uint8_t length, uint8_t offset)
{
	uint16_t initValue = 0x1d0f;
	return Get_crc_ccitt(ptr, length, offset, initValue);
}

uint32_t Get_crc32_tab_Val(uint32_t i)
{
    int j;
    uint32_t crc;

	crc = i;

	for (j=0; j<8; j++) {

			if ( crc & 0x00000001L ) crc = ( crc >> 1 ) ^ P_32;
            else                     crc =   crc >> 1;
	}

	return crc;
}

uint32_t update_crc_32(uint32_t crc, byte c)
{

    uint32_t tmp, long_c;

    long_c = 0x000000ffL & (uint32_t) c;

    tmp = crc ^ long_c;
    crc = (crc >> 8) ^ Get_crc32_tab_Val((tmp & 0xff));

    return crc;
}

uint32_t Get_crc_32(const byte *ptr, uint8_t length, uint8_t offset)
{
	uint32_t crc_32 = 0xffffffffL;
	
	for(uint8_t pos=offset;pos<length;pos++)
	{
		crc_32 = update_crc_32(crc_32, ptr[pos]);
	}
	
	crc_32    ^= 0xffffffffL;
	
	return crc_32;
}

uint8_t Get_checksum(const byte *ptr, uint8_t length, uint8_t offset)
{
	uint8_t crc = 0;
	
	for(uint8_t pos=offset;pos<length;pos++)
	{
		crc += ptr[pos];
		crc %= 256;
	}
	
	return crc;
}

#endif //separate the inbootloader stuff