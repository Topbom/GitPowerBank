#ifndef _IIC_H_
#define _IIC_H_

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef UINT
#define UINT unsigned int
#endif

#define EEPROM	0xa0
#define NACK_IIC 1
#define ACK_IIC 0


// ============== define addr ============ //


#define Check_0x55		0x55
#define Check_0xaa		0xaa



// ================================== //

extern unsigned char Read24C16(unsigned short addr16);
extern void Write24C16(unsigned short k,unsigned char ch);
extern void DigiVR_Write()  ;
#endif
