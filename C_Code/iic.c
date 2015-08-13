

   
#pragma sfr   
#pragma DI   
#pragma EI   
#pragma NOP

#define	 I2C_SCL      P4.0   //Output  Set_Mid
#define	 I2C_SDA      P4.7   //Output  Sel_LSB


#define EEPROM	  0xa0
#define NACK_IIC    1
#define ACK_IIC      0
#define I2C_WR       0
#define I2C_RD       1
#define I2C_WR_ADDr       0x5c


sreg unsigned char I2C_BitF;   //DM134 #1 High-Byte  B15~B08
#define I2C_ACK       I2C_BitF.0
// ============== define addr ============ //


#define Check_0x55		0x55
#define Check_0xaa		0xaa
//------------------------------
void IIC_Start();
void IIC_Stop();
void IIC_Tx(unsigned char Data);
void IIC_Rx(unsigned char Ack);
void WaitEEprom();
void ReadIIC(unsigned char device,unsigned char addr);
void WriteIIC(unsigned char device,unsigned char addr,unsigned char ch);
void Read24C16(unsigned char addr16);
void Write24C16(unsigned char k,unsigned char ch);
void I2C_wait();
void I2C_delay_ms() ;
void DigiVR_Write()  ;
//------------------------------
void IIC_Start()
{

	PM4.7=0;       // I2C_SDA Output 
  PU4.7=1;
  I2C_wait();
  	
	I2C_SDA = 1;	
	I2C_wait();	
	I2C_SCL = 1;	
	I2C_wait();
	I2C_SDA = 0;
	I2C_wait();
	I2C_SCL = 0;
	I2C_wait();
}
//------------------------------
void IIC_Stop()
{
	PM4.7=0;       // I2C_SDA Output 
  PU4.7=1;
  I2C_wait();
	   
	I2C_SDA = 0;
	I2C_wait();
	I2C_SCL = 1;
	I2C_wait();
	I2C_SDA = 1;
	I2C_wait();
}
//------------------------------
void IIC_Tx(unsigned char Data)
{
	unsigned char AckLoop;
	unsigned char i;
	PM4.7=0;       // I2C_SDA Output 
  PU4.7=1;		
  I2C_wait();
	//   Data=0x5c;
	for(i=0;i<8;i++)
		{
		if ( (Data & 0x80)== 0x80)
		I2C_SDA=1;
		else
		I2C_SDA=0;	
		
		I2C_wait();
		I2C_SCL = 1;
		I2C_wait();
		I2C_SCL = 0;	 
		I2C_wait();		
		Data <<= 1;
		}
//Get ACK
	PM4.7=1;       // I2C_SDA Input  KEY 
  PU4.7=1;
  I2C_wait();		   
	I2C_SDA = 1;	
//       I2C_wait();	
//	I2C_SCL = 1;
//       I2C_wait();	
	for(i=0;i<5;i++)
		{		
   	I2C_SCL = 1;	
   	I2C_wait();		
   	I2C_wait();	   		
		if(I2C_SDA==0)
			{
			I2C_ACK = 0;
			break;
			}
     I2C_SCL = 0;	
   	I2C_wait();	     
		 I2C_wait();	
		}
    	I2C_SCL = 0;	
}
//------------------------------
void  IIC_Rx(unsigned char Ack)
{
	unsigned char Data;
	unsigned char i;
	Data = 0;
	PM4.7=0;       // I2C_SDA Output 
       PU4.7=1;	
       		I2C_wait();
	for(i=0;i<8;i++)
		{
		Data <<= 1;
		I2C_SCL = 0;
		I2C_SDA = 1;
		I2C_SCL = 1;
		I2C_wait();
		if(I2C_SDA)
			Data |= 1;
		else
			Data &= 0xfe;
		I2C_SCL = 0;
		I2C_wait();
		}
	I2C_SCL = 0;
	if (Ack==0)
	I2C_SDA = 0;
	else 
	I2C_SDA = 0;		
	I2C_SCL = 1;
	I2C_wait();
	I2C_SCL = 0;
	I2C_wait();
//	return Data;
}
//------------------------------
void WaitEEprom()
{
	unsigned char i,ack;
	for(i=0;i<200;i++)  //10ms 
		{
		IIC_Start();
		//ack = IIC_Tx(0xa0);	//polling ACKnowledge
		IIC_Stop();
		if(ack == ACK_IIC)
			{
			I2C_wait();
			break;
			}
		}
}
//------------------------------
void ReadIIC(unsigned char device,unsigned char addr)
{
	unsigned char value;
	IIC_Start();
	IIC_Tx(device);
	IIC_Tx(addr);
	I2C_SDA = 1;	//repeat start
	I2C_wait();
	I2C_SCL = 1;
	I2C_wait();
	I2C_SDA = 0;
	I2C_wait();
	I2C_SCL = 0;
	I2C_wait();
	IIC_Tx(device+1);
	//value = IIC_Rx(NACK_IIC);	//nack
	IIC_Stop();
//	return value;
}
//------------------------------
void WriteIIC(unsigned char device,unsigned char addr,unsigned char ch)
{
	IIC_Start();
	IIC_Tx(device);
	IIC_Tx(addr);
	IIC_Tx(ch);
	IIC_Stop();
}
//------------------------------
void Read24C16(unsigned char addr16)
{
	unsigned char ch;
	unsigned char device,addr;
	addr = addr16 & 0x0ff;
	device = (addr16 >> 7) & 0x0e;
	device |= EEPROM;
	//ch = ReadIIC(device,addr);
	//return ch;
}
//------------------------------
void Write24C16(unsigned char k,unsigned char ch)
{
unsigned char device,addr;
	addr = (unsigned char)k;
	device = (unsigned char)((k>>7)&0x0e);
	device |= EEPROM;
	WriteIIC(device,addr,ch);
	WaitEEprom();
}

//-----------------------------------------------------------------------------   
// Module:   wait   
// Function: generates a wait loop   
//-----------------------------------------------------------------------------   
void I2C_wait()   
{   
  char a=0;   
  while(a!=0x10)   
  {   
    a++;   
  }   
} 

//-----------------------------------------------------------------------------   
// Module:   wait   
// Function: generates a wait loop   
//-----------------------------------------------------------------------------   
void I2C_delay_ms()   
{   
  unsigned int a=0;   
  while(a!=0x1000)   
  {   
    a++;   
  }   
} 


//-----------------------------------------------------------------------------   
// Module:      
// Function:  
//-----------------------------------------------------------------------------   
void DigiVR_Write()   
{   
  unsigned char a=0;   
  IIC_Start();
  IIC_Tx(0x5c);
 // IIC_Tx(0x00);
 // IIC_Tx(0x40);
  IIC_Stop();
  
} 


//---------------



