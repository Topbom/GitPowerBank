 
//============================================================================   
// PROJECT      = Power Bank
// MODULE       = 
// SHORT DESC.  = -   
// DEVICE       = uPD78F9232   
// VERSION      = 1.0   
// DATE         = 07.01.2013    
// LAST CHANGE  = 
// Edit         = Topbom   
// ===========================================================================   
// 
//   
// ===========================================================================   
   
#pragma sfr   
#pragma DI   
#pragma EI   
#pragma NOP
#pragma HALT
#pragma STOP
                              
//     
#pragma interrupt INTLVI         ISR_Low_Voltage		   //
#pragma interrupt INTTM80      ISR_INTTM80                //
#pragma interrupt INTAD          ISR_INTADC                  // 
#pragma interrupt INTTMH1      ISR_TMH1_10ms   // 
#pragma interrupt INTP0           ISR_INTP0_OUT1         // set VOUT1     
#pragma interrupt INTP2           ISR_INTP2_SetKey           // DC13.2V
#pragma interrupt INTP3           ISR_INTP3_SelectKey         //  hold 2 seconds x\cam on/off White LED do Toggle

#pragma section @@CNST  OPT AT 80H   
const char OPTION=0x9C;     //internal osc 8Mhz  ,watchdos can Stop at STOP Mode

//const char OPTION=0x9D;     //internal osc 8Mhz  ,watchdos can't Stop at STOP Mode

#include "Power_Bank.h" 

   
//--Output---------------------------------------------------------------------------      									  
#define	 Led_Data                   P0.0    //Output
#define	 Led_Latch                  P12.1   //Output
#define	 Led_Enable                P12.2   //Output
#define	 Led_Clock                  P12.3   //Output
							
#define	 Pin_USB5V                  P0.2   //Output
#define	 Pin_BAT12V                P4.5   //Output   //NPN  "H":Enable

#define	 Pin_WLED                    P12.0   //Output  //PMOS
#define	 SleepLed                     P3.2    //Output  
#define	 Pin_PWRIN                   P4.4    //Output   
#define	 Pin_PWRSEG7               P4.6    //Output  

#define	 Pin_R19V                      P13.0    //Output   //NPN  "H":Enable
#define	 Pin_R13V                      P3.3   //Output   //NPN   

#define	 SYSPWR_SW                   P4.0    //Output   
//-----------input-----------------
#define	 ADC0                P2.0   //Input
#define	 ADC1                P2.1   //Input
#define	 ADC2                P2.2   //Input
#define	 ADC3                P2.3   //Input      

#define	 INTP0_DCJack_In               P3.0   //INPUT  INTP0
#define	 INTP2_SetKey                     P3.1   //INPUT  INTP2 
#define	 INTP3_SelectKey                 P4.1   //INPUT  INTP3  

#define	 PlugIn_ExtPwr                P0.1    //INPUT plug-in =1 ;
#define	 PlugIn13V                       P0.3   //INPUT    plug-in =1 ;
#define	 PlugIn19V                       P4.2    //INPUT    plug-in =1 ;
#define	 PlugIn12V                       P4.7   //INPUT    plug-in =1 ;
//---------IO defeine------------------------------------------------------
#define	 DCIN_Pin_IO                   PM0.1  // 0:Output ,1:Input 
#define	 Pin_BAT12V_IO                PM4.5   //0:Output ,1:Input 
#define	 Pin_USB5V_IO                  PM0.2   //0:Output ,1:Input 
#define	 Pin_PWRIN_IO                  PM4.4    //Output   
//-------------------------------
#define  SYSPWR_SW_ON       SYSPWR_SW=1   //NPN
#define  SYSPWR_SW_OFF     SYSPWR_SW=0   //

#define  BAT12V_ON       Pin_BAT12V=0  //NPN+NPN
#define  BAT12V_OFF     Pin_BAT12V=1 //

#define  R19V_ON	          Pin_R19V = 1    //
#define  R19V_OFF	   Pin_R19V = 0    //
   
#define  R13V_ON          Pin_R13V=1
#define  R13V_OFF         Pin_R13V=0

#define  SleepLed_ON           SleepLed=0
#define  SleepLed_OFF         SleepLed=1   //   

#define  USB5V_ON               Pin_USB5V=0  //NPN+NPN
#define  USB5V_OFF             Pin_USB5V=1

#define  EN_PWRIN_ON               Pin_PWRIN=0 
#define  EN_PWRIN_OFF             Pin_PWRIN=1

//----------------------------------------------------------

#define  Led_Data_H           Led_Data=1
#define  Led_Data_L           Led_Data=0

#define  Led_Clock_H        Led_Clock=1
#define  Led_Clock_L        Led_Clock=0

#define  Led_Disp_ON       Led_Enable=0 
#define  Led_Disp_OFF     Led_Enable=1



#define  EN_WLED_ON           Pin_WLED=1
#define  EN_WLED_OFF         Pin_WLED=0

#define  EN_LEDPWR_ON    Pin_PWRSEG7=1
#define  EN_LEDPWR_OFF   Pin_PWRSEG7=0 //0




//-------------------------------------------






//-----------------------------------------------------------------------------   
// Global variables   
//-----------------------------------------------------------------------------   
//unsigned char result;   

sreg unsigned int bit_word;
																											
#define Pre_Sleep_F	       bit_word.0 // //1:Entry Sleep 
#define H_L_Bit_F	       bit_word.1 //
#define Update_LED_F  	bit_word.2  // 1:12V o 13V or 19V have plugIn 
#define ChkOK_Cnt_F         bit_word.3  //
                       
#define INTP0_F  	              bit_word.4  //
#define INTP2_F  	              bit_word.5 //
#define OneS_F  	              bit_word.6 //
#define ADC_OK_F             bit_word.7 //


#define BatLow_F	                  bit_word.8
#define Bat_Toggle_F	           bit_word.9   // 
#define  Set_ADC_F               bit_word.10 //  1:ON
#define BatLow_LED_F  	    bit_word.11 

#define   Once_F          	     bit_word.12  
#define  WhiteLED_F 	            bit_word.13
#define  OutV_Flash_F            bit_word.14//
#define  USB5V_Diff_F            bit_word.15//

//-----------------------------------------------------------------------													
sreg unsigned int SW_Status;

#define PlugIN_12V_F           SW_Status.0 //  1:SW-ON
#define PlugIN_13V_F           SW_Status.1 //  1:SW-ON
#define PlugIN_19V_F           SW_Status.2 //  1:SW-ON
#define  Out1_Mult_F             SW_Status.3  //  1:ON

#define  NoAny_Plug_F          SW_Status.4  //  1:ON
#define  Out1_Diff_F  	           SW_Status.5  //  have change out voltage
#define  PWR_12V_F   	    SW_Status.6  //
#define  IN_USB5V_F              SW_Status.7  //  1:ON

#define  Effect_ExtPwr_F        SW_Status.8 // //1:ON have ExtPower DC-Jack Plug-IN + Voltage math
#define  ExtPwr_F                   SW_Status.9   //  1:ON   have ExtPower DC-Jack Plug-IN
#define  DisUpLoad_ADC2_F    SW_Status.10 // :  1:ON
#define  Charge_Mode_F 	      SW_Status.11   //set 1 when insert DCjack 12 or 13 or 19 any one

#define  BatChagre_Full_F        SW_Status.15
  



sreg unsigned int Danger_Flag;
#define Batt_VeryLow_F             Danger_Flag.0 //  
#define Shut_Down_F                Danger_Flag.1 //  
#define CBat_VLow_F                 Danger_Flag.2  // 
#define USB_VShort_F  	        Danger_Flag.3 // :  1:ON

#define OUT1_VShort_F  	       Danger_Flag.4 //  1:ON



unsigned char result10000;   
unsigned char result1000;   
unsigned char result100; 
unsigned char result10; 
unsigned char result1; 

unsigned char  HEXBuff; 

//unsigned int  DECBuff; 
unsigned char TM80_1S_CNT; 
unsigned char INTM1_10ms_CNT; 
unsigned char ADC_Voltage;          //

unsigned char BatCapa_LED;          // 0:< 20% ,1:<405,2:< 60% ,3:<80%,4:< 100%,5:>100%  for battery charge mod 
unsigned char ADC0_BAT12V;          //
unsigned char ADC1_Out1;          //
unsigned char ADC2_ExtPwr;     //
unsigned char ADC3_USB5V;      //
unsigned char SetKey;
unsigned char KEY2_SetKey;
unsigned char ADC_Select;  // 


unsigned char VOL_LOW;
unsigned char Out1_ShortV_ADC;
unsigned char INTP2_CNT;
unsigned char OverLoad_Cnt;
unsigned char Bat_Capa_Num;     // 0:very low ,1:20% ,2:40% ,3:60% 4:80: 5:100%
unsigned char Pre_Bat_Capa_Num;     // 0:very low ,1:20% ,2:40% ,3:60% 4:80: 5:100%
unsigned char Sleep_Time;
unsigned char BatVeryLow_Time;
unsigned char ChargeMode_Time;
unsigned char ADC_Vref_Cnt;

unsigned char USBDanger_Cnt;
unsigned char OUT1Danger_Cnt;
unsigned char Charge_Vlow_Time;
unsigned char Out1_VShort_Time;
unsigned char USB_VShort_Time;
unsigned char LowV_LVIM_G;

unsigned char Charge_VLow_ON;
unsigned char Charge_VLow_OFF;

unsigned char ADCRH_8bit;          //int
unsigned char OUT1_VShort_Cnt;        
unsigned char USB_VShort_Cnt;    
unsigned char Out1_First_ADC;   
unsigned char USB_First_ADC;  
unsigned char  OUT1_VShort_NO ;


/*
#1           #2          #3           #4           #5            #6                   #7              #8
BCLED3   BCLED5   BCLED7   BCLED9     BCLED11  BCLED15         BCLED16     BPLED1 
PWR        Ext_Pwr    USB         12V          19V          13V                  Error           Sleep/Bat_low (Flash)
(S.6)       (S.7)        (S.8)       (S.9)         (S.10)      (S.11)               (S.12)          (GPIO)

BCLED2   BCLED4   BCLED6   BCLED8    BCLED10   BCLED12          BCLED13    BPLED14
(S.5)       (S.4)       (S.3)        (S.2)        (S.1)         (S.13)                (S.15)       (S.14)                     
Charge_OK BAT1    BAT2       BAT3        BAT4         BAT5                  Mass-A     NC

                                                                                                                     BCLED1
                                                                                                                      (S.0)
                                                                                                                        NC

*/
sreg unsigned char    LED_LByte1;   //DM134 #1 Low-Byte   B07~B00  //1:ON  ,BAT_Level
sreg unsigned char    LED_HByte2;   //DM134 #1 High-Byte  B15~B08

#define LED_PWR          LED_LByte1.6    // #1
#define LED_ExtPWR     LED_LByte1.7    // #2
#define LED_USB           LED_HByte2.0   // #3
#define LED_12V           LED_HByte2.1   // #4
#define LED_19V           LED_HByte2.2   // #5
#define LED_13V           LED_HByte2.3   // #6
#define LED_Error        LED_HByte2.4   // #7
//GPIO  do Battery Low LED                    // #8
//--------------------------------------
#define LED_Chr_OK    LED_LByte1.5  //  #1
#define LED_BAT1         LED_LByte1.4  //  #2
#define LED_BAT2         LED_LByte1.3  //  #3
#define LED_BAT3         LED_LByte1.2  //  #4
#define LED_BAT4         LED_LByte1.1  //  #5
#define LED_BAT5         LED_HByte2.5  // #6
#define LED_Mass_a     LED_HByte2.7  //  #7
#define NCC                  LED_HByte2.6  //  #8
//=================================================

unsigned int Batt_Charge_ALLS;   //int 
unsigned int Batt_Charge_BlockS;   //int 
unsigned char  Batt_Full_WaitS;         //int   charge up to Full but must waitting time up
unsigned int Sacler_AdcVref;  //int
unsigned int ADC_ADCR;          //int
unsigned int Real_Vol;             //int
    
unsigned int Out1_Time;          //int
unsigned int USB_Time;             //int
unsigned int WhiteLED_Time;             //int
unsigned int Charge_Block_Max_Time;             //int



//-----------------------------------------------------------------------------   
// Function prototyps   
//-----------------------------------------------------------------------------   

//__interrupt void ISR_TMH1_10ms(void);
__interrupt void ISR_INTP0_OUT1(void); 
//__interrupt void ISR_INTP2_SetKey(void); 
//__interrupt void ISR_INTP2_SetKey(void);
void INTAD (void);
void INTTM80(unsigned char interval)   ;
void INTTMH1(void);
void INTP0() ;
void INTP2() ;
void INTP3() ;

void init_CPU (void)   ;
void init_GPIO (void)   ;
void init_UART(void)   ;
void delay();  
void delay_1S() ;
void wait(void)  ;

void HEX_TO_UNPACK_HEX(unsigned int hexdata) ;
void HEX_TO_DEC_DIV100(unsigned int hexdata);
void BCD_TO_HEX_Byte();
void UART_SendString (char *ucpStr) ; 


void Cool_Init();
void PrintOut_ASCII(unsigned char cnt,char *ucpStr);
void Debug_Mess() ;


void  SetKey_PlugIn_Detec();
void IR_Byte_Out_MSB (unsigned char  bit_cnt, unsigned char Byte1) ;
void  Chk_bit_byte_MSB(unsigned char  index, unsigned char data1) ;



void Set_Led_Out();

void ADC_Mode_Set();
void Battery_Level();  
void Charge_Battery_Level();
void DisCharge_Battery_Level();


void LED_ALL_OFF();
void LED_ALL_ON();
void Voltage_Short_Detec();
void Sent_CMD_DispLED();
void Sleep_Mode();

void INTP0_PlugIN_Chk(void);
void VoltageOut();
void LED_Flash(unsigned char cnt);
void Inital_Set_OFF();
void Entry_Sleep_Chk();
void Charge_Mode_Func();
void BattVeryLowMode();
void Flag_SetLED();
void SleepLED_Flash(unsigned char cnt);
void WLED_Flash(unsigned char cnt);
void Power_OFF();
void Get_Battery_ADC(); 
void Get_Out1_ADC();
void Get_ExtPwr_ADC();
void Get_USB_ADC();
void Low_Voltage_Check();
void Factory_Mode();
void Out1_VShort_Deter();
void USB_VShort_Deter();
void Short_Delay() ;
	
	/*
void Chk_bit_byte_LSB(unsigned char  index,unsigned char data1);
void IR_Byte_Out_LSB (unsigned char  bit_cnt, unsigned char Byte1) ;

void CR_Return(void);


*/




	
//-----------------------------------------------------------------------------   
// Module:   init_CPU   
// Function: Initialization of CPU   
//-----------------------------------------------------------------------------   
void init_CPU (void)   
{   
// stop watchdog timer   
  WDTM = 0x70;   //bit4 set to h is STOP Watchdog
   
//time gererator settings   
  PCC = 0x00;           // set CPU time to fx   
  PPCC = 0x00;   
   
//  HSRCM = 0x00;           // high speed ring oscillator operates   
  LSRCM = 0x01;         // low speed ring oscillator stops   
   
  OSTS  = 0x00;         //shortest stabilisation time 2^10/fx   
   
// interrupt setting   
   IF0  = 0x00;   
   IF1  = 0x00;   
   MK0  = 0xFF;    
   MK1  = 0xFF;   
}   
   
//-----------------------------------------------------------------------------   
// Module:   init_GPIO   
// Function: Initialization of LED ports   
// 0:Output ,1:Input 
//-----------------------------------------------------------------------------   
void init_GPIO (void)   
{   

  P13.0=0;          // Output
  P13=1 ;
  
  PM0=0xFA  ;    //xxxx 1010 ,P0.7~P0.4 is no used  
  PU0=0xFF    ;  //xxxx 0101  

  
   PM12=0xF0;         // 0:Output  ,P12.7~P12.4 is no used
   PU12=0x0F;         // 0:Output  ,P12.7~P12.4 is no used

   PMC2=0x0f;
   PM2=0xFF;       // 1:Input   do ADC0~3  ,P2.7~P2.4 is no used
   PU2=0x0ff;
    // PU2=0x000;

  PM3=0x13 ;   // xxx1 0011    P3.7~P4.5 is no used
  PU3=0x1E ;   // xxx1 1110    P3.7~P4.5 is no used 

   // PM4=0x86;  // 1000 0110
    //P4.7 12Vin
    //P4.2 19Vin
  PM4.0=0;  // 
  PM4.1=1 ;       
  PM4.2=1;        //
 
  PM4.5=0;        //
  PM4.6=0;        //
  PM4.7=1;        //
  
   PU4=0x7F ;   // 0111 1011B


     USB5V_OFF;
     BAT12V_OFF;	 

}   
/*========================================================
AUDIO PLUG-Out  is Depond on INTP0
00:Failling    01:Rising   10:Stop   11:Both
========================================================*/
void INTP0()
{
   INTM0=0xe4;               //1110 0100  set INTP2:Rising(00) ,INTP1:Prohinited(10) ,INTP0:Rising(01) 
   PIF0= CLEAR;              // clear interrupt request flag   INTP0
   PMK0=INT_ENABLE;     //Enable INTP1 
}

/*========================================================

00:Failling    01:Rising   10:Stop   11:Both
========================================================*/
/*
void INTP1()
{
   INTM0=0xe4;               //1110 0100  set INTP2:Rising(00) ,INTP1:Prohinited(10) ,INTP0:Rising(01) 
   PIF1= CLEAR;              // clear interrupt request flag   INTP1
 //  PMK1=INT_ENABLE;     //Enable INTP1 
}
*/
/*========================================================
Set_Key
00:Failling    01:Rising   10:Stop   11:Both
========================================================*/
void INTP2()
{
   INTM0=0xe4;               //1110 0100  set INTP2:Rising(00) ,INTP1:Prohinited(10) ,INTP0:Rising(01) 
   PIF2 = CLEAR;             // clear interrupt request flag   INTP2 
   PMK2=INT_ENABLE;     //Enable INTP1 
}

//-----------------------------------------------------------------------------   
// Module:   init_ADC   
// Function: Initialization of AD converter   
//00:Failling    01:Rising   10:Stop   11:Both
//-----------------------------------------------------------------------------   
void INTAD (void)   
{   
  PMC2.0=1;               //set alternate function mode   
  PMC2.1=1;               //set alternate function mode  
  PMC2.2=1;               //set alternate function mode  
  PMC2.3=1;               //set alternate function mode    
  PM2.0=1;                //set port to input mode    P2.0   
  PM2.1=1;                //set port to input mode    P2.1 
  PM2.2=1;                //set port to input mode    P2.2   
  PM2.3=1;                //set port to input mode    P2.3
  ADM = 0x18 ;
  ADIF = 0;                     //clear interrupt request flag   
  ADMK = 0;                     //enable ADC interrupt   
}   

//-----------------------------------------------------------------------------   
// Module:   init_UART   
// Function: Initialization of UART6   
//-----------------------------------------------------------------------------   
void init_UART(void)   
{   
 
 // PM4.4=1;            //input mode for RxD   //d do not receive mod  



  PM4.4=0;  //output  RX

  PM4.3=0;            //output mode for TxD   
  PU4.3=1;             //set TxD output to high level 

  P4.3=1;
  P4.4=1;
  
   
  POWER6 = 1;               //enable internal clock operation   
  ASIM6 |= 0xE5;            //enable transmission   
                            //enable reception   
                            //no parity   
                            //character lenght of data = 8-bits   
                            //number of stop bits = 1   
   
#if BAUDRATE==9600          //initialization for baudrate = 9600   
  CKSR6 = 0x02;   
  BRGC6 = 104;   
#elif BAUDRATE==38400       //initialization for baudrate = 38400   
  CKSR6 = 0x00;   
  BRGC6 = 104;   
#elif BAUDRATE==115200      //initialization for baudrate = 115200   
  CKSR6 = 0x00;   
  BRGC6 = 35;   
#endif   
delay();
}   
// /*========================================================
//    Initial timer 80 subroutine 
// ========================================================*/
// void init_tm80_sub(void)
// {
//     TMC80=0x04;		// operation and output disable
// 	                // count clock select fx/1024=7812.5Hz
// 	CR80=(78-1);	// compare register set 78
// 					// INTTM80 frequency = 7812.5Hz/78=100.16Hz
// 	TMIF80=CLEAR;		// Interrupt request flag clear
// 	TMMK80=INT_ENABLE;	// Interrupt service enable
// 	
// 	TCE80=ENABLE;		// Timer 80 operation enable
// }                                //7812/195 = 40 Hz   --->  25ms
//
//-----------------------------------------------------------------------------   
// Module:   init_TM80   
// Function: Initialization of Timer80   

//  TCL801 TCL800 Selection of count clock of 8-bit timer 80
//                              fXP = 8.0 MHz   fXP = 10.0 MHz
//  0        0    fXP/2^6            125 kHz         156.3 kHz
//  0        1    fXP/2^8          31.25 kHz         39.06 kHz
//  1        0    fXP/2^10          7.81 kHz          9.77 kHz
//  1        1    fXP/2^16          0.12 kHz          0.15 kHz
//  Cautions 1. Be sure to set TMC80 after stopping the timer operation.
//  2. Be sure to clear bits 0 and 6 to 0.
//  Remark fXP:
//-----------------------------------------------------------------------------   
void INTTM80(unsigned char interval)   
{   
   TCE80 = CLEAR;                   // stop timer80   
   TMC80 = 0x06;        // set input clock to fxp / 2^16 =10hz @ 8MHz   
   CR80  = interval;        // set interval time   
   TCE80 = ENABLE;                   // start timer80   
   TMIF80 = CLEAR;                  // clear interrupt request flag   
   TMMK80=INT_ENABLE;                    // enable timer80 interrupt   
}                             //7812/195 = 40 Hz   --->  25ms

//-----------------------------------------------------------------------------   
// ISR:       ISR_INTTM80   
// Function: Interrupt service routine of Timer80   
// timer 1 seconds do Interrupt
//-----------------------------------------------------------------------------   
__interrupt void ISR_INTTM80(void)   
{   
   // ADCE = 1;                     //enable voltage generator   
   // ADS  = 0x00;                  //AD channel select  P2.0 
  // ADCS = 1;                     //start conversion   

   OneS_F=1 ; 
  TM80_1S_CNT++;
  //------------------  
  if ((Effect_ExtPwr_F) && (BatChagre_Full_F ==0))
  	{

#if Charge_test 	  	
  	  	{
                  Batt_Charge_BlockS=Batt_Charge_BlockS+200 ;  	  
				  
  	  	}
#else
           {
              Batt_Charge_BlockS++;
          }
#endif		   
  	}
//------------------  
  if (NoAny_Plug_F)
  	{
           Sleep_Time++;  
  	}
//------------------  
  if (Charge_Mode_F)
  	{
           Charge_Vlow_Time++;	  
		if (BatChagre_Full_F==0)
			{
			      Batt_Charge_ALLS++;  	
			}			   
  	}
//------------------  
if  ((Out1_Mult_F) & (OUT1_VShort_F==0))
{
 Out1_Time++;
}
//-------------------------------------------
if  (OUT1_VShort_F) 
{
 Out1_VShort_Time++;
}
//-------------------------------------------
if  (USB_VShort_F)
{
 USB_VShort_Time++;
}
//-------------------------------------------


if  ((IN_USB5V_F)  & (USB_VShort_F==0))
{
 USB_Time++;
}

if  (WhiteLED_F)
{
 WhiteLED_Time++;
}


//Pin_WLED=~Pin_WLED;   
	 /*
   if  (WhiteLED_F==0)
   	{
           Pin_WLED=~Pin_WLED ;
   	}
    */
}     
/*========================================================
   Initial timer H1 subroutine 
   TMHMD1 : TMHE1 CKS12 CKS11 CKS10 TMMD11 TMMD10 TOLEV1 TOEN1
   8MhZ=8000000 ,  
//  CKS12   CKS11   CKS10  Selection of count clock of 8-bit TMH1
//                                        fPRS = 2.0 MHz         fPRS = 8.0 MHz  
//  0        0       0       fPRS               2    MHz            8   MHz  
//  0        0       1       fPRS/2^2           500  KHz            2   MHz  
//  0        1       0       fPRS/2^4           125  KHz            500 KHz  
//  0        1       1       fPRS/2^6          31.25 KHz            125 KHz  //
//  1        0       0       fPRS/2^12          0.49 KHz           1953.125 Hz   //80000000/4096=1853.125  
//  1        0       1       fRL/2^7            1.88 KHz            
//  1        1       0       fRL/2^9            0.47  KHz            
//  1        1       1       fRL                240  KHz            

//  Cautions 1. Be sure to set TMC80 after stopping the timer operation.
//  2. Be sure to clear bits 0 and 6 to 0.
//  Remark fXP:   
========================================================*/ 
void INTTMH1(void)     //about 10ms
{
    TMHMD1=0x40;	                  // operation and output disable
			                        // count clock select fx/4096=1953.125Hz
	CMP01=(20-1);	          // compare register set 20
		                             // INTTMH1 frequency = 1953.125Hz/20=97.656Hz  //10.24ms
	TMIFH1=CLEAR;		 // Interrupt request flag clear
	TMMKH1=INT_ENABLE;	 // Interrupt service enable
	
	//TMHE1=ENABLE;		// Timer H0 operation enable
}

/*========================================================
    interrupt service routine (ISR) 
========================================================*/
__interrupt void ISR_TMH1_10ms(void) 
{

   NOP();
  INTM1_10ms_CNT++;  

}   

/*========================================================
    interrupt service routine (ISR) 
========================================================*/
__interrupt void ISR_INTP0_OUT1(void) 
{
   PMK0=INT_DISABLE;              // enable INTP2 interrupt  
   delay(); 
   INTP0_F=1;     
#if for_test ==1   //for test					   
   WLED_Flash(1)  ;   //for test
#endif	
   //PIF0 = CLEAR;                  // clear interrupt request flag   INTP2
   //PMK0=INT_ENABLE;              // enable INTP2 interrupt    
}    


/*========================================================
interrupt service routine (ISR) 
========================================================*/
__interrupt void ISR_INTP2_SetKey(void)  
{
   PMK2=INT_DISABLE;              // DISABLE INTP3 interrupt 
   delay();   
   INTP2_F=1;     
   //SetKey_PlugIn_Detec();

   	
 //  PIF2 = CLEAR;                  // clear interrupt request flag   INTP3
 // PMK2=INT_ENABLE;              // enable INTP3 interrupt  	      	   
}   


/*========================================================
00:Failling  01:Rising  10:Stop   11:Both
========================================================*/
void INTP3()
{
   INTM1=0x00;             //set INTP3 : Rising
   PIF3 = CLEAR;           // clear interrupt request flag   INTP3
   PMK3=INT_ENABLE;     //Enable INTP1 
   
  // PMK3=INT_ENABLE;   // enable INTP3 interrupt    
}
/*========================================================
    interrupt service routine (ISR) 
========================================================*/
__interrupt void ISR_INTP3_SelectKey(void) 
{

     PMK3=INT_DISABLE;              // Disable INTP2 interrupt    
     PIF3 = CLEAR;                  // clear interrupt request flag   INTP2
#if for_test ==1   //for test					   
WLED_Flash(3)  ;   //for test
#endif	
    //PMK3=INT_ENABLE;              // enable INTP2 interrupt 
}  //

//  /*========================================================
//     INTTM80 interrupt service routine (ISR) 
//  ========================================================*/
//  __interrupt void tm80_10ms_INT(void) 
//  {
//     TimerDebugPin^=HIGH_LEVEL;
//  }
  
   
//-----------------------------------------------------------------------------   
// ISR:       ISR_INTADC   
// Function: Interrupt service routine of ASD converter   
// EX:3.7v = 256(ADC) , 3.0 =207(ADC)
// 3.25V/255=12.745 mv/counter
// one seconds entry two times
//-----------------------------------------------------------------------------   
__interrupt void ISR_INTADC(void)   
{   
    unsigned short temp;   
    unsigned int data1;  
   // temp = ADCRH;              //load AD conversion result   
    ADCS = 0;                    //stop AD converter   
    ADCE = 0;                    //disable voltage generator   
    //ADC_ADCR=ADCR;              //
    ADCRH_8bit =ADCRH ;
    ADC_Voltage = ADCRH ;

  //  data1 = ADCR *  ADC_Vref_Cnt; //
  //  Sacler_AdcVref=data1;      //
 //   HEX_TO_DEC_DIV100(data1) ; // will div 100 scaler
  //  temp=ADC_Voltage;          //BYTE TO BYTE 
 //   BCD_TO_HEX_Byte()  ;   
  //  data1=HEXBuff;

	 /*
    switch(ADC_Select)
	       {
	 
               case 0:     //Battery  ADC0
               	      data1=data1+ADC0_Offset; //
       	              break;
       	                   
               case 1:     //Out1 ADC1
               	      data1=data1+ADC1_Offset ; //
                  
       	              break;
               case 2:     //DCIN  - ADC2
               	      data1=data1+ADC2_Offset ; //
       	              break;
       	              
     	        default : 
               case 3:     //USB  ADC3
               	       data1=data1+ADC3_Offset ; //
       	              break; 

     	   	}
   */
  //      Real_Vol = data1;
//	 ADC_Voltage = data1;
     ADC_OK_F =1;
}   

/*========================================================

========================================================*/
void INTLVI()
{
LVIM=0x80 ; //
LVIS=0x06 ; //  Vlvi  0x08=2.6V
LVIMK=INT_ENABLE ; //Enable
}
//-----------------------------------------------------------------------------   
// ISR:       
// Function: 
// 
//----------------------------------------------------------------------------- 
__interrupt void ISR_Low_Voltage(void) 
{
unsigned char i;
  LVIMK=INT_DISABLE ; //Disable
  LowV_LVIM_G = LVIM ;  
  // Low_Voltage_Check();
} 

//-----------------------------------------------------------------------------   
// Module:   init_ADC   
// Function: Initialization of AD converter   
//-----------------------------------------------------------------------------    
void Cool_Init()
  {   
  	TM80_1S_CNT=0;
	INTM1_10ms_CNT=0;
        Batt_Charge_BlockS=0x0;	
	
        bit_word=0x0000;
        SW_Status=0x0000;
	      ADC_OK_F =1;
	      Led_Clock_H;
	      Led_Data_H;
	      Led_Latch=0;
	      EN_WLED_OFF;  
        ADC_Select=0; 
     ChargeMode_Time=0;
     BatVeryLow_Time=0;
     Batt_Full_WaitS=0;
     Sleep_Time=0;
	 
         SleepLed_OFF;  	  
	 SYSPWR_SW_ON;
	 Charge_Block_Max_Time = Charge_Max_20 ;
	 Out1_Diff_F=1;
	 IN_USB5V_F=0;
	 BAT12V_OFF;
	 USB5V_OFF;        
       Out1_ShortV_ADC=0;		 

   }
//-----------------------------------------------------------------------------   
// Module:   main   
// Function: main program   
//-----------------------------------------------------------------------------   
void main(void)   
{    
unsigned char i,j; 
  DI(); // global interrupt disable     

  init_CPU();                   // cpu initialization  

  init_GPIO();           // led port initialization   
     INTLVI();

  INTAD();           // adc initialization     
  INTTM80(119);        // initialization of timer80  //120/120 = 1 Hz   --->  1ms   
                        //7812.5Hz/78=100.16Hz  //CR80=(78-1);
  INTTMH1();     // initialization of TMH1  10ms


  INTP0()  ;
  INTP2()  ;   //do Set-Key down

  
 INTP3();      // KEY2 clear interrupt request flag   INTP3  enable INTP3 interrupt    

  Cool_Init();

   Update_LED_F=1;
Factory_Mode();
    EI();     // global interrupt enable  




     LED_ALL_ON();      
     delay_1S()  ;	 
     delay_1S()  ;		 
do
{
    ADC_Mode_Set() ;
}
while (ADC_Select !=3) ;   //have read ADC0 


    INTP0_PlugIN_Chk();    
    DisCharge_Battery_Level()	;   
    Flag_SetLED();	

    Sent_CMD_DispLED();		


//------------------------------
#ifdef debug_out   
   init_UART();          // uart60 initialization     
  UART_SendString("*********************** Power Bank  20140901 v1.0 NECuPD9234 ****\r\n");     
    //LED_Flash(1);   //for test
#if for_test ==1   //for test					   
	WLED_Flash(3)  ;   //for test
#endif	
     Debug_Mess()  ;
#endif	

//delay_1S() ;

//WDTM = 0x67 ; //Low-Speed OSC , 546.13ms/period

//   WDTM = WatchDog_Set  ; //Low-Speed OSC , 546.13ms/period
 
	      SleepLed_OFF;	


   
//************************************* 
   // BattVeryLowMode();
//=============================================	
  while (1)  //   
  {  

	WDTE=0xAC ;  //Clean Watchdog
 Entry_Sleep_Chk();
 BattVeryLowMode();
 SetKey_PlugIn_Detec();     
 Low_Voltage_Check();
  

//-----------------------------
if ((NoAny_Plug_F) &&(Sleep_Time >Entey_Sleep_time ) &&(BatChagre_Full_F==0) )
    {
    Pre_Sleep_F=1;
#if for_test ==1   //for test					   
	WLED_Flash(2)  ;   //for test
#endif	    
	if (WhiteLED_F)
		{
		   EN_LEDPWR_OFF;
		   Sleep_Time =Entey_Sleep_time +1  ;
		}
	else
		{
                 Sleep_Time=0;
    	          Sleep_Mode();
		}
    }
else
       {
          Pre_Sleep_F=0;       
        //  EN_LEDPWR_ON;
        }
//---------------------------------
  ADC_Mode_Set(); //
//-----------
Voltage_Short_Detec();
//------------------------------------
    if ((OneS_F) | (INTP0_F)  | (INTP2_F) | (Update_LED_F))     // 1 seconds period
      {      
	 INTP0_F=0;
	 INTP2_F=0;	 
	 Update_LED_F=0;	

        INTP0_PlugIN_Chk();   
//----------------------------------------------
/*
#if for_test ==1   //for test					   

   if  (WhiteLED_F==0)  //for test
   	{
           Pin_WLED=~Pin_WLED ;
		  // SYSPWR_SW =~SYSPWR_SW;		   			   	   
   	}
			  
#endif
  */	
//---------------------------------------------



     if (Charge_Mode_F)    //Charge Mode  ,output voltage must over 12V
           {
              Flag_SetLED();		
              Charge_Mode_Func();  
           }	
    else
	 {
	     DisCharge_Battery_Level()	;	
            Flag_SetLED();	 //	 
            Sent_CMD_DispLED(); //					   
	 }
        OneS_F = 0 ;	 
#ifdef debug_out 
             Debug_Mess()  ;
#endif
     }  
//-------------------------------------------	
  }        
 }     
 

//-----------------------------------------------------------------------------   
// Module:   HEX_TO_DEC_DIV100   
// Function: EX:65535 = 6 5 5 3 5 / 100 =655
// data store: result10000=06,result1000=05,result100=05,result10=03,result1=05
// ADC_Voltage = 655
//----------------------------------------------------------------------------- 
void HEX_TO_DEC_DIV100(unsigned int hexdata)   
{   
    unsigned int data1;  
    ADC_Voltage = 0 ;
    data1 = hexdata;
    result10000 = data1/10000;       //decimal output   
    data1  = data1%10000;       //   
    result1000  = data1/1000;       //   
    data1       = data1 % 1000;      // 
    result100  = data1/100;       //   
    data1       = data1 % 100;      //    
    result10   = data1/10;       //    
    result1    = data1 % 10;      //     
   //DECBuff=0;
    ADC_Voltage=result10000*100+result1000*10+result100; //  change to dec data  ans=data/100 
}  
//-----------------------------------------------------------------------------   
// Module:   BCD_TO_HEX_Byte   
// Function: 
// data store :
//----------------------------------------------------------------------------- 
void BCD_TO_HEX_Byte()   
{   
    HEXBuff=0;    
    HEXBuff =result10000*100+ result1000*10+result100; //

} 

#ifdef debug_out 
  
//-----------------------------------------------------------------------------   
// Module:   HEX_TO_UNPACK_HEX   
// Function: conver HEX to HEX_ASCII ,EX:09AF
// data store result10000=0,result1000=0,result100=9,result10=A,result1=F
//----------------------------------------------------------------------------- 
void HEX_TO_UNPACK_HEX(unsigned int hexdata)   
{   
    unsigned int data1;  
    
    data1 = hexdata;
   // data1=0x09AF;
    result10000=0;
    
    result1000 = data1/(16*16*16);   // hexbcd output   
    data1      = data1%(16*16*16);   //   
    result100  = data1/(16*16);      //   
    data1      = data1%(16*16);    //  
    result10   = data1/16;           //   
    result1    = data1%16;         //        
} 



//-----------------------------------------------------------------------------   
// Module:   UART_SendChar   
// Function: Send char via UART6   
//-----------------------------------------------------------------------------   
void UART_SendChar (unsigned char ucData)   
{   
    TXB6 = ucData;              // load UART transmit buffer   
    while(!STIF6);              // wait for transmission finished   
    STIF6=0;   
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
       
}   
   
//-----------------------------------------------------------------------------   
// Module:   UART_GetChar   
// Function: Get char from UART6   
//-----------------------------------------------------------------------------   
  
unsigned char UART_GetChar(void)   
{   
     unsigned char receive_byte;   
     while(!SRIF6);               // wait for uart receive byte   
     receive_byte = RXB6;         // load UART receive buffer   
     SRIF6=0;   
     return (receive_byte);   
}   
   
//-----------------------------------------------------------------------------   
// Module:   UART_SendString   
// Function: Send string via UART6   
//-----------------------------------------------------------------------------   
void UART_SendString (char *ucpStr)   
{   
    unsigned char ucData;   
    while (1)   
    {   
        ucData = (unsigned char)(*ucpStr++);       
    if(ucData) UART_SendChar (ucData);     
    else break;   
    }   
WDTE=0xAC ;  //Clean Watchdog	
} 

//-----------------------------------------------------------------------------   
// Module:      
// Function: Send measured result via UART6   
//-----------------------------------------------------------------------------   

void PrintOut(void)   
{   
    UART_SendChar(result10000+0x30);    
    UART_SendChar(result1000+0x30);   
    UART_SendChar(result100+0x30);   
    UART_SendChar(result10+0x30);   
    UART_SendChar(result1+0x30);    
   // UART_SendString(" S\r\n");   
   
}    

//-----------------------------------------------------------------------------   
// Module:      
// Function: Send measured result via UART6   
//-----------------------------------------------------------------------------   
void PrintOut_ASCII(unsigned char cnt,char *ucpStr)   
{   
     unsigned char i,ucData; 
     for (i=0;i<cnt;i++)
     {
     	 ucData = (unsigned char)(*ucpStr++);   
     	 if ( ucData >= 0 && ucData <= 9) 
     	 UART_SendChar(ucData+0x30);  
     	 else 
     	 UART_SendChar(ucData+55);  
     	 
     }
} 

/* -------------------------------------------
Function:

---------------------------------------------- */
void Debug_Mess()   
{   


       UART_SendString("1 TM80_1S_CNT=");      //Byte 
       HEX_TO_DEC_DIV100(TM80_1S_CNT);
       PrintOut_ASCII(3,&result100);  
       UART_SendString("\r\n"); 


        UART_SendString("2 INTM1_10ms_CNT=");      //Byte 
       HEX_TO_DEC_DIV100(INTM1_10ms_CNT);
       PrintOut_ASCII(3,&result100);  
       UART_SendString("\r\n"); 	   

       UART_SendString("3 Sleep_Time=");      //Byte 
       HEX_TO_DEC_DIV100(Sleep_Time);
       PrintOut_ASCII(3,&result100);  
       UART_SendString("\r\n"); 	   


  
       UART_SendString("4 Charge_Block_Max_Time=");       //two byte
       HEX_TO_DEC_DIV100(Charge_Block_Max_Time);
       PrintOut_ASCII(5,&result10000); 
       UART_SendString("    Chr100_WaitS =240     ");      //Byte      	   
       UART_SendString("\r\n");  	  
	UART_SendString("  Batt_Charge_ALLS=");       //two byte
       HEX_TO_DEC_DIV100(Batt_Charge_ALLS);
       PrintOut_ASCII(5,&result10000); 	   	
       UART_SendString("  Batt_Charge_BlockS=");       //two byte
       HEX_TO_DEC_DIV100(Batt_Charge_BlockS);
       PrintOut_ASCII(5,&result10000); 
       UART_SendString("    Batt_Full_WaitS=");       //two byte	   
       HEX_TO_DEC_DIV100(Batt_Full_WaitS);
       PrintOut_ASCII(5,&result10000);	   
       UART_SendString("\r\n");  


       UART_SendString("5 ADC_Vref_Cnt/ADC_Select/ADCRH_8bit= ");   //Max 0~9   //Byte 
       HEX_TO_DEC_DIV100(ADC_Vref_Cnt);
       PrintOut_ASCII( 2,&result10);    
       UART_SendString(" ");        
       PrintOut_ASCII( 1,&ADC_Select);  
       UART_SendString(" "); 	   
       HEX_TO_UNPACK_HEX(ADCRH_8bit);
       PrintOut_ASCII( 2,&result10);  	     	     
       UART_SendString("\r\n");  

       UART_SendString("6 HEXBuff/Real_Vol=  ");   //Byte 
       HEX_TO_DEC_DIV100(HEXBuff);
       PrintOut_ASCII( 3,&result100);    
       UART_SendString(" "); 
       HEX_TO_DEC_DIV100(Real_Vol);
       PrintOut_ASCII( 5,&result10000); 	   
       UART_SendString("\r\n"); 
	   
	   
  
       UART_SendString("7 ADC0(Bat)/ADC1(Out1)/ADC2(Ext_PWR)/ADC3(USB)=  ");   //Byte 
       HEX_TO_UNPACK_HEX(ADC0_BAT12V);
       PrintOut_ASCII(2,&result10);        
       UART_SendString(" "); 
       HEX_TO_UNPACK_HEX(ADC1_Out1);
       PrintOut_ASCII(2,&result10);    
       UART_SendString(" "); 
       HEX_TO_UNPACK_HEX(ADC2_ExtPwr);
       PrintOut_ASCII(2,&result10);     	   
       UART_SendString(" "); 
       HEX_TO_UNPACK_HEX(ADC3_USB5V);
       PrintOut_ASCII(2,&result10);     	   
       UART_SendString("\r\n");  


       UART_SendString("8 Charge_Vlow_Time=");     //
       HEX_TO_DEC_DIV100(Charge_Vlow_Time);
       PrintOut_ASCII(3,&result100);  	   
       UART_SendString("\r\n");


       UART_SendString("9 Charge_VLow_ON/OFF =  ");   //Byte 
       HEX_TO_DEC_DIV100(Charge_VLow_ON);
       PrintOut_ASCII(3,&result100);        
       UART_SendString("/"); 
       HEX_TO_DEC_DIV100(Charge_VLow_OFF);
       PrintOut_ASCII(3,&result100); 
       UART_SendString("\r\n");   

	   
       UART_SendString("10 VOL_LOW=");      //Byte 
       HEX_TO_DEC_DIV100(VOL_LOW);
       PrintOut_ASCII(3,&result100);  
       UART_SendString("\r\n");                             


 
                    
      
       UART_SendString("11-----------Bit_Word_Flag=");    
       HEX_TO_UNPACK_HEX(bit_word);
       PrintOut_ASCII(4,&result1000); 
       UART_SendString("\r\n");
       
       UART_SendString("12----------- SW_Flag=");   
       HEX_TO_UNPACK_HEX(SW_Status);
       PrintOut_ASCII(4,&result1000); 
       UART_SendString("\r\n");    

       UART_SendString("-----------Danger_Flag_Flag=");    
       HEX_TO_UNPACK_HEX(Danger_Flag);
       PrintOut_ASCII(4,&result1000); 
       UART_SendString("\r\n");	   


       UART_SendString("13 Bat_Capa_Num BatCapa_LED  Pre_Bat_Capa_Num  =");     //for test
       PrintOut_ASCII(1,&Bat_Capa_Num);  
       UART_SendString(" "); 	
       PrintOut_ASCII(1,&BatCapa_LED)	;   
       UART_SendString(" "); 	
       PrintOut_ASCII(1,&Pre_Bat_Capa_Num);  	   
       UART_SendString("\r\n");       	

       UART_SendString("14 BatVeryLow_Time=");     //Max 0~9   //Byte 
       HEX_TO_DEC_DIV100(BatVeryLow_Time);
       PrintOut_ASCII(3,&result100);  	   
       UART_SendString("\r\n"); 	   

       UART_SendString("15 Danger USB/OUT Cnt=");     //Max 0~9   //Byte 	
       PrintOut_ASCII(1,&USBDanger_Cnt)	;   
       UART_SendString("/");    
       PrintOut_ASCII(1,&OUT1Danger_Cnt)	;     	   
       UART_SendString("\r\n"); 	


       UART_SendString("16 Out1_Time=");       //two byte
       HEX_TO_DEC_DIV100(Out1_Time);
       PrintOut_ASCII(5,&result10000); 
       UART_SendString("\r\n"); 	

	 UART_SendString("17 USB_Time=");       //two byte
       HEX_TO_DEC_DIV100(USB_Time);
       PrintOut_ASCII(5,&result10000);	   
       UART_SendString("\r\n");  
	   
	 UART_SendString("18 WhiteLED_Time=");       //two byte
       HEX_TO_DEC_DIV100(WhiteLED_Time);
       PrintOut_ASCII(5,&result10000);	   
       UART_SendString("\r\n");



	UART_SendString("19 LowV_LVIM_G=");     //
       HEX_TO_UNPACK_HEX(LowV_LVIM_G);
       PrintOut_ASCII(2,&result10); 
       UART_SendString("\r\n");
   


       UART_SendString("20 OUT1_VShort_Cnt/Out1_VShort_Time/Out1_First_ADC/Out1_ShortV_ADC/OUT1_VShort_NO\r\n");     //
       UART_SendString("------ ");	       
        PrintOut_ASCII(1,&OUT1_VShort_Cnt)	;   
       UART_SendString("      /"); 
	HEX_TO_DEC_DIV100(Out1_VShort_Time);
       PrintOut_ASCII(3,&result100);  	   
       UART_SendString("      /");	    
       HEX_TO_UNPACK_HEX(Out1_First_ADC);
       PrintOut_ASCII(2,&result10);  	   
       UART_SendString("      /");
       HEX_TO_UNPACK_HEX(Out1_ShortV_ADC);
       PrintOut_ASCII(2,&result10);    
       UART_SendString("      /");   
       HEX_TO_DEC_DIV100(OUT1_VShort_NO);
       PrintOut_ASCII(3,&result100);  	   
       UART_SendString("\r\n");	   

	UART_SendString("21 USB_VShort_Time=");     //
       HEX_TO_DEC_DIV100(USB_VShort_Time);
       PrintOut_ASCII(3,&result100);  	   
       UART_SendString("\r\n");
	   
       UART_SendString("22 USB_VShort_Cnt/USB_First_ADC\r\n");     //Max 0~9   //Byte 	
       UART_SendString("------ ");	       
       PrintOut_ASCII(1,&USB_VShort_Cnt)	;     	   
       UART_SendString("      /");
       HEX_TO_UNPACK_HEX(USB_First_ADC);
       PrintOut_ASCII(2,&result100);  	   
       UART_SendString("\r\n");

	   




	   
       UART_SendString("\r\n");     
   
 }      



 
 
/* -------------------------------------------
Function:

---------------------------------------------- */
void SleepLED_Flash(unsigned char cnt)
{
  unsigned short i;   
  for (i=0;i<cnt;i++)
    {
       SleepLed_ON ;
        delay_1S();	 	   
       SleepLed_OFF    ;	  	  
       delay_1S();  		  
    }    
} 
 
/* -------------------------------------------
Function:

---------------------------------------------- */
void LED_Flash(unsigned char cnt)
{
  unsigned short i;   
  for (i=0;i<cnt;i++)
    {
 
      LED_LByte1=0xff ;
      LED_HByte2=0xff ;
      Sent_CMD_DispLED(); ; 
      delay(); 
      delay(); 	   
       SleepLed_ON    ;	  
      LED_LByte1=0x00 ;
      LED_HByte2=0x00 ; 
      Sent_CMD_DispLED(); 
       SleepLed_OFF    ;	  	  
      delay(); 	
      delay(); 		  
    }    
} 
#endif
/* -------------------------------------------
Function:

---------------------------------------------- */
void Flag_SetLED()
{

      LED_LByte1=0x00 ;    // all led off
      LED_HByte2=0x00 ;  // all led off
	  
          LED_PWR=Seg7_LEDON ;   //OUT3-LED	
           ADC_Vref_Cnt=ADC_Vref_Normal  ;
		   
        if (BatChagre_Full_F)
        	{
		  LED_Chr_OK=Seg7_LEDON  ;  //
        	}
//-------------------		  

	  if (IN_USB5V_F)
	  	{
   	           ADC_Vref_Cnt=ADC_Vref_Load  ;		
			   
	  	   if (USB_VShort_F)
	  	   	{
		   	 OutV_Flash_F =~OutV_Flash_F;
			  if (OutV_Flash_F)
			  	{

	                        LED_USB=Seg7_LEDON  ;  // OUT2-LED DC13.2V 	
			  	}
			  else
			  	{

	                        LED_USB=Seg7_LEDOFF ;  // OUT2-LED DC13.2V 	
			  	}
	  	   	}
		   else
		   	{
	                   LED_USB=Seg7_LEDON  ;  // OUT2-LED DC13.2V 		                 
		   	}

			    
	       }	  
//-------------------		  
	  if (PlugIN_12V_F)
	  	{
	  	 ADC_Vref_Cnt=ADC_Vref_Load  ;	//	          
	  	   if (OUT1_VShort_F)
	  	   	{
		   	 OutV_Flash_F =~OutV_Flash_F;
			  if (OutV_Flash_F)
			  	{

			            LED_12V=Seg7_LEDON  ;  // OUT2-LED DC13.2V 	
			  	}
			  else
			  	{

			           LED_12V=Seg7_LEDOFF  ;  // OUT2-LED DC13.2V 	
			  	}
	  	   	}
		   else
		   	{
	                 LED_12V=Seg7_LEDON  ;  // OUT2-LED DC13.2V 	    	                 
		   	}
	       }
 
//-------------------		  
	  if (PlugIN_19V_F)
	  	{	         
 
	  	 ADC_Vref_Cnt=ADC_Vref_Load  ;	//	          
	  	   if (OUT1_VShort_F)
	  	   	{
		   	// OutV_Flash_F =~OutV_Flash_F;
			  if (OutV_Flash_F)
			  	{

			             LED_19V=Seg7_LEDON  ;  // OUT2-LED DC13.2V 	
			             OutV_Flash_F=0;
			  	}
			  else
			  	{

			           LED_19V=Seg7_LEDOFF  ;  // OUT2-LED DC13.2V 
			             OutV_Flash_F=1;
			  	}
	  	   	}
		   else
		   	{
	                 LED_19V=Seg7_LEDON  ;  // OUT2-LED DC13.2V 	 	                 
		   	}			   
	       }	  

//-------------------		  
	  if (PlugIN_13V_F)
	  	{
 
   	           ADC_Vref_Cnt=ADC_Vref_Load ;		//          
          
	  	   if (OUT1_VShort_F)
	  	   	{
		   	 OutV_Flash_F =~OutV_Flash_F;
			  if (OutV_Flash_F)
			  	{

	                         LED_13V=Seg7_LEDON  ;  // OUT2-LED DC13.2V 	
			  	}
			  else
			  	{

	                          LED_13V=Seg7_LEDOFF  ;  // OUT2-LED DC13.2V 	
			  	}
	  	   	}
		   else
		   	{
	                    LED_13V=Seg7_LEDON  ;  // OUT2-LED DC13.2V 			                 
		   	}			   
	       }

//-------------------		  
	  if (ExtPwr_F)     //led On only
	  	{
	          LED_ExtPWR=Seg7_LEDON  ;  // OUT2-LED DC13.2V 	  
	          ADC_Vref_Cnt=ADC_Vref_ExtPwr ;	//

	       } 
//-------------------		  	    
          //Bat_Capa_Num=5;   // for test
          switch(Bat_Capa_Num)
	                        {
                     default :	 
	             case 0:				           // Battery Very Low					 	        
                      	      break;  	
 //----------------------  	 
	             case 1:						 // Battery 20%	
                                     //   LED_BAT1=Seg7_LEDON ; 		
	             				  if (BatLow_F)
	             				          {
                                                       LED_BAT1=Seg7_LEDON ; 	
						          }
					          else
						           {
						               LED_BAT1=Seg7_LEDOFF ; 	   					  
						            }										
                     	        break;
//----------------------
              	case 2:						 // Battery 40%
                                 LED_BAT1=Seg7_LEDON ; 		              	
                                 LED_BAT2=Seg7_LEDON ; 		

	                             break;
//----------------------		
              	case 3:						 // Battery 60%
                                 LED_BAT1=Seg7_LEDON ; 	
                                 LED_BAT2=Seg7_LEDON ; 		              	
                                 LED_BAT3=Seg7_LEDON ; 		

	                             break;
//----------------------	
              	case 4:						 // Battery 80%
                                 LED_BAT1=Seg7_LEDON ; 	
                                 LED_BAT2=Seg7_LEDON ; 	
                                 LED_BAT3=Seg7_LEDON ; 	              	
                                 LED_BAT4=Seg7_LEDON ; 	
	                             break;
//----------------------	
              	case 5:						 // Battery 100%
                                 LED_BAT1=Seg7_LEDON ; 	
                                 LED_BAT2=Seg7_LEDON ; 	
                                 LED_BAT3=Seg7_LEDON ; 	
                                 LED_BAT4=Seg7_LEDON ; 									 
                                 LED_BAT5=Seg7_LEDON ; 		

	                             break;
//----------------------		         
	          }						 	
	    			
 //    Sent_CMD_DispLED();		
 }

 
/* -------------------------------------------
Function:

---------------------------------------------- */
void Sent_CMD_DispLED()
    {
	 if (Pre_Sleep_F==0)
	    	{
	       	// WDTE=0xAC ;  //Clean Watchdog
   	  	    IR_Byte_Out_MSB(8,LED_HByte2) ;
   	  	    IR_Byte_Out_MSB(8,LED_LByte1) ;        
		    Set_Led_Out();	
	    	}
     }
/* -------------------------------------------
Function:

---------------------------------------------- */
void Set_Led_Out()
 {
 	Led_Enable=1;
 	wait();
	NOP();
	NOP();
	NOP();
	NOP();	 	
 	Led_Latch=0;
 	wait();
	NOP();
	NOP();
	NOP();
	NOP();	
 	Led_Latch=1;
 	wait();
	NOP();
	NOP();
	NOP();
	NOP();
 	Led_Latch=0;

 	wait();
	NOP();
	NOP();
	NOP();
	NOP();
 	Led_Enable=0;
 	wait();
	NOP();
	NOP();
	NOP();
	NOP();
 	
 }
/* -------------------------------------------
Function:

---------------------------------------------- */ 	    
void IR_Byte_Out_MSB (unsigned char  bit_cnt, unsigned char Byte1)  
{
 unsigned char  i;
 for (i=0;i<bit_cnt;i++)
 	{ 	  

          Chk_bit_byte_MSB(i,Byte1);
	    if ( H_L_Bit_F)
       	{
//bit 1		 
              Led_Data_H ;
 	wait();
	NOP();
	NOP();
	NOP();
	NOP();
              Led_Clock_H  ;    
 	wait();
	NOP();
	NOP();
	NOP();
	NOP();             
              Led_Clock_L  ;
 	wait();
	NOP();
	NOP();
	NOP();
	NOP();
         	}
       else
//bit 0
       	{
              Led_Data_L ;
        wait();
	NOP();
	NOP();
	NOP();
	NOP();
              Led_Clock_H  ;    
 	wait();
	NOP();
	NOP();
	NOP();
	NOP();            
              Led_Clock_L  ;
 	wait();
	NOP();
	NOP();
	NOP();
	NOP();

       	}
 	}
}

/* -------------------------------------------
Function:

---------------------------------------------- */
void  Chk_bit_byte_MSB(unsigned char  index, unsigned char data1)   
{   
 H_L_Bit_F=0; 
switch(index)
   {
      case 0 :
                   if  (data1 & 0x80)          //bit7
                   	{
     	                 H_L_Bit_F=1; 
                   	}
	  	         break;
      case 1 :
                   if  (data1 & 0x40)          //bit6
                   	{
     	                 H_L_Bit_F=1; 
                   	}
	  	    break;
      case 2 :
                   if  (data1 & 0x20)          //bit5
                   	{
     	                 H_L_Bit_F=1; 
                   	}
	  	    break;
      case 3 :
                   if  (data1 & 0x10)          //bit4
                   	{
     	                 H_L_Bit_F=1; 
                   	}
	  	    break;
      case 4 :
                   if  (data1 & 0x08)          //bit3
                   	{
     	                 H_L_Bit_F=1; 
                   	}
	  	    break;
      case 5 :
                   if  (data1 & 0x04)          //bit2
                   	{
     	                 H_L_Bit_F=1; 
                   	}
	  	    break;
      case 6 :
                   if  (data1 & 0x02)          //bit1
                   	{
     	                 H_L_Bit_F=1; 
                   	}
	  	    break;
      case 7 :
                   if  (data1 & 0x01)          //bit0
                   	{
     	                 H_L_Bit_F=1; 
                   	}
	  	    break;			
   }


}


/* -------------------------------------------
Function:

---------------------------------------------- */
void WLED_Flash(unsigned char cnt)
{
  unsigned short i;   
  for (i=0;i<cnt;i++)
    {
WDTE=0xAC ;  //Clean Watchdog    
      EN_WLED_ON ;  
      delay(); 
      delay(); 	  
      delay(); 	  	  
      EN_WLED_OFF ; 
      delay(); 	
      delay(); 	
      delay(); 	  	  
    }    
} 


/* -------------------------------------------
Function:

---------------------------------------------- */
void ADC_Mode_Set()
{
  unsigned char  i;	
   // ADCE = 1;                     //enable voltage generator   
  //  ADS  = 0x00;                  //AD channel select  P2.0 
  //  ADCS = 1;                     //start conversion 
 if (ADC_OK_F) 
    {
      ADCS = 0;                    //stop AD converter   
      ADCE = 0;                    //disable voltage generator   

    if (Set_ADC_F==0)
    	{
     	   switch(ADC_Select)
	     {	 
               case 0:     //Battery  ADC0     
               		
               	      ADC0_BAT12V=ADCRH_8bit  ; //ADC_Voltage; //

               	      ADC_Select=1;                  //enable Next ADC
                           ADS  = 0x01;                  //AD channel select  P2.1 
       	              break;
//-------------------------------------       	                   
               case 1:     //out1 12v/13v/19v
                            if  (Out1_Mult_F)   //   ((PlugIN_12V_F) |(PlugIN_13V_F)  |(PlugIN_19V_F))
                            	{                            	  
                                     ADC1_Out1=ADCRH_8bit;  //ADC_Voltage; //								  
                            	}
				else
					{
					  ADC1_Out1=0;
					}				
                          	          ADC_Select=2;        //enable Next ADC
                                      ADS  = 0x02;            //AD channel select  P2.2                   
				
       	              break;
//-------------------------------------   					  
               case 2:     //EXt_Power        
#if  Charge_test
           if ((ExtPwr_F) &&  (BatChagre_Full_F==0))
                   {
                          ADC2_ExtPwr=0x0AD;	   // for test	
                          Effect_ExtPwr_F=1 ;	
				if (Bat_Capa_Num <=4)
				    {
                                   Charge_Mode_F=1;           
				     }
			   // BatChagre_Full_F=0;
                   } 
	else if (ExtPwr_F==0)
		   	{
                            ADC2_ExtPwr=0 ;		   	
                            Effect_ExtPwr_F=0 ;	
		              Charge_Mode_F=0;
			       BatChagre_Full_F=0;

		   	}
#else			   
                          if (DisUpLoad_ADC2_F==0)
                          	{
                                 if ((ExtPwr_F) &&  (BatChagre_Full_F==0))
                            	 {
                            	   ADC2_ExtPwr=ADCRH_8bit;
                            		if ((ADC2_ExtPwr>ExtPwr_ADC2_Low) &&  (ADC2_ExtPwr < 0xF0) )
				                   {				
                                                  Effect_ExtPwr_F=1 ;	
				                     if (Bat_Capa_Num <=4)
				                           {
                                                              Charge_Mode_F=1;    
				                           }									 
				                    } 
                          	            	//else if (ExtPwr_F==0)
                          	            	else  //  if (Batt_VeryLow_F==0)
                          	            	     { 
                                                  Effect_ExtPwr_F=0 ;	
		                                   Charge_Mode_F=0;
                                                  ADC2_ExtPwr=0 ;	
				                      BAT12V_OFF	;		
		                                     OUT1_VShort_F=1 ; 		
			                              OUT1_VShort_Cnt++;											 
		                                     OUT1_VShort_NO+=13;	
                               	            	}
                          	         //  ADC2_ExtPwr=100;	   // for test						  
                            	}
				else  
					{
						ADC2_ExtPwr=0  ;
				       }
                        }	
  #endif						  
                	         ADC_Select=3;                	//enable Next ADC
                             ADS  = 0x03;                  //AD channel select  P2.3                   

       	              break;
//-------------------------------------   					         	              
     	         default : 
               case 3:     //USB             
                          if (IN_USB5V_F) 
                          	   {                          	      
                                      ADC3_USB5V=ADCRH_8bit; //								   
                          	     }            
				else
					{
					  ADC3_USB5V=0;
					}
               	      ADC_Select=0;                	//enable Next ADC
                            ADS  = 0x00;                  //AD channel select  P2.0 
       	              break;  
      	      	
     	   	}
    	}
//---------------------------		   
     	   	         ADC_OK_F=0;
		         ADCE = 1;                     //enable voltage generator   
		            NOP();
		            NOP();					
                       ADCS = 1;                     //start conversion 
		            NOP();
		            NOP();					
              
//---------------------------------	
 	    }
      }  //

/* -------------------------------------------
Function:

---------------------------------------------- */
void Get_Battery_ADC()
{

     	  ADC_OK_F=0;
      do
      	{
         ADC_Select=0;                  //enable Next ADC
         ADS  = 0x00;                  //AD channel select  P2.1 
         ADCE = 1;                     //enable voltage generator   
         ADCS = 1;                     //start conversion          

      	}
	  while  (ADC_OK_F == 0) ;
	   ADC_Mode_Set()   ;	 	  
		 
}     
/* -------------------------------------------
Function:

---------------------------------------------- */
void Get_Out1_ADC()
{


     	   ADC_OK_F=0;
	  Set_ADC_F=1 ;

         ADC_Select=1;                  //enable Next ADC
         ADS  = 0x01;                  //AD channel select  P2.1 	  

         BAT12V_ON	;    
	 //Short_Delay();		 
 
      do
      	{

         ADCE = 1;                     //enable voltage generator   
         NOP();
         NOP();              
         NOP();         
         ADCS = 1;                     //start conversion   	
         NOP();
         NOP();              
         NOP();         

      	}
	  while  (ADC_OK_F == 0) ;
	 ADC1_Out1=ADCRH_8bit; //	
	  Set_ADC_F=0 ;	 
	  // ADC_Mode_Set()   ;
     	//ADC_OK_F=0;
      // ADCS = 0;                    //stop AD converter      
      // ADCE = 0;                    //disable voltage generator   	 
	 
}

/* -------------------------------------------
Function:

---------------------------------------------- */
void Get_ExtPwr_ADC()
{
      	  ADC_OK_F=0;
      do
      	{
         ADC_Select=2;                  //enable Next ADC
         ADS  = 0x02;                  //AD channel select  P2.1 
         ADCE = 1;                     //enable voltage generator   
         ADCS = 1;                     //start conversion          

      	}
	  while  (ADC_OK_F == 0) ;
	   ADC_Mode_Set()   ;	 
	  }
/* -------------------------------------------
Function:

---------------------------------------------- */
void Get_USB_ADC()
{ 
     	  ADC_OK_F=0;
         USB5V_ON	; 	
			Short_Delay();
         ADC_Select=3;                  //enable Next ADC
         ADS  = 0x03;                  //AD channel select  P2.1 	
      do
      	{

         ADCE = 1;                     //enable voltage generator   
         NOP();
         NOP();              
         NOP();   		 
         ADCS = 1;                     //start conversion      
         NOP();
         NOP();   
         NOP();		 
      	}
	  while  (ADC_OK_F == 0) ;
	 ADC3_USB5V=ADCRH_8bit; //		  
	     //  ADC_Mode_Set()   ;
}
/* -------------------------------------------
Function:

---------------------------------------------- */
void LED_ALL_OFF()
 {

       LED_LByte1=0x00 ;    // all led off
      LED_HByte2=0x00 ;  // all led off
     Sent_CMD_DispLED();	      
   EN_LEDPWR_OFF;


 }  
 
/* -------------------------------------------
Function:

---------------------------------------------- */
void LED_ALL_ON()
 {
 	      EN_LEDPWR_ON;
	SleepLed_ON ;
       LED_LByte1=0xFF ;    // all led off
      LED_HByte2=0xFF ;  // all led off
     Sent_CMD_DispLED();	   


 }  
/* -------------------------------------------
Function:
  
---------------------------------------------- */
void Voltage_Short_Detec() 
{
	unsigned char i;  
//--------USB5V------  
WDTE=0xAC ;  //Clean Watchdog
//---------------------------------------- 
          VoltageOut();	  
         Out1_VShort_Deter();
         USB_VShort_Deter();
//===========================
 
 } 	

    
 

/* -------------------------------------------
Function:

---------------------------------------------- */
 /*
void Battery_Low_Detec()
 {
 	unsigned char i; 
    if (BatLow_F)
    {

        SleepLed =~SleepLed;
    }
	
  }
*/
 //-----------------------------------------------------------------------------   
// Module:   wait   
// Function: generates a wait loop   
//----------------------------------------------------------------------------- 
void Power_OFF()
{
   if	(ExtPwr_F)    //have plug-in but no power ,so must set to input mode
   	{                             //until exit dcjack . 
   	 DCIN_Pin_IO = OutModePin ;   // 0:Output ,1:Input 
   	 PlugIn_ExtPwr=0;
   	 PU0=0xFD    ;  //xxxx 1101  
   	  EN_PWRIN_ON ; 	  
   	 
   	}
   else
   	{

         EN_PWRIN_OFF ;
       }
//--------------
  if (WhiteLED_F)
	 {
             EN_WLED_ON;				 
	  }
   else
   	{
   	   EN_WLED_OFF;
   	}
//--------------
   BAT12V_OFF;   	
   USB5V_OFF;	
   EN_LEDPWR_OFF;

}
 //-----------------------------------------------------------------------------   
// Module:   wait   
// Function: generates a wait loop   
//----------------------------------------------------------------------------- 
void Sleep_Mode()
{
       Batt_Charge_ALLS=0;
	DisUpLoad_ADC2_F =0;			
        LED_ALL_OFF();	 		
       POWER6=DISABLE;		/* Enable operation of internal clock */ 
       RXE6=DISABLE;		/* Receive Operation enable */	 
       TXE6=DISABLE;		/* Receive Operation enable */	
	 Power_OFF();	
	
       INTP0()  ;
       INTP2()  ;   //do Set-Key down
       INTP3();       

	   if ((Out1_Mult_F) &&  (Batt_VeryLow_F))	
	   	{
                 INTM0=0x0EC;               //1110 0100  set INTP2:Rising(00) ,INTP1:Prohinited(10) ,INTP0:Rising(11) 

		
}
	   
	 Batt_VeryLow_F=0;	
       ADCS = 0;                    //stop AD converter   
       ADCE = 0;                    //disable voltage generator   	

     
	PMC2=0x0;
        PM2 =0xff ;
	

         PU4.7=0;
	  PU4.2=0;

	PM4.5=1;


	P0.0=0;
	PM12.1=0;  //
	PM12.2=0;
	PM12.3=0;	
	PM0.0=0;

       Pin_USB5V_IO=1;
       Pin_BAT12V_IO=1;

	   EN_PWRIN_ON ;
	//  P4.4=0 ;
	  //	PM4.4=1 ; //
	  
	  P4.3=0;


        SYSPWR_SW_OFF;
        WDTM = Dis_WatchDog   ; //Disable WatchDog


	NOP();	
	NOP();
	NOP();
    
    	//HALT();   
    	STOP(); 		
	NOP();
	NOP();	
	NOP();
	NOP();
	NOP();	
	NOP();	
	delay();
       SYSPWR_SW_ON;
	DCIN_Pin_IO = InModePin ;   // 0:Output ,1:Input 
       EN_LEDPWR_ON;		
    
      init_GPIO();           // led port initialization   
         init_UART();          // uart60 initialization     
      POWER6=ENABLE;		/* Enable operation of internal clock */ 	 
      RXE6=ENABLE;			/* Receive Operation enable */	  
	TXE6=ENABLE;			/* Receive Operation enable */	         
	//PM2 =0xff;
WDTM = WatchDog_Set ; //Low-Speed OSC , 546.13ms/period	
	      WDTE=0xAC ;  //Enable Watchdog

EN_PWRIN_ON;
	      
 } 
 
  //-----------------------------------------------------------------------------   
// Module:   wait   
// Function: generates a wait loop   
//-----------------------------------------------------------------------------   
void wait(void)   
{   
  unsigned char a=0 ;  
  while(a!=0xff)   
  {   
    a++;   
  }   
}  
//-----------------------------------------------------------------------------   
// Module:   delay   
// Function: generates a delay loop   
//-----------------------------------------------------------------------------   
void delay(void)   
{   
  unsigned char a=0;   
  while(a!=0x40)   
  {   
    a++; 
    WDTE=0xAC ;  //Clean Watchdog
    wait();  
  }  
}    
//-----------------------------------------------------------------------------   
// Module:   delay   
// Function: generates a delay loop   
//-----------------------------------------------------------------------------   
void delay_1S(void)   
{   
  unsigned char a=0;   
  while(a!=0x0a)   
  {   
    a++; 
    WDTE=0xAC ;  //Clean Watchdog
    delay();  
  }   
}  

/* -------------------------------------------
Function:
do LED or USB Out Taggle Detector
press key over 2  seconds will do LED Toggle else USB 5V out Toggle
;
---------------------------------------------- */
void SetKey_PlugIn_Detec(void)
{
 unsigned char i=0; 
  
    
     do
       {
          delay();	  
          i++;
       }
   while((INTP2_SetKey== 0) && (i <KeyWaitTimeS) ) ;  //
   if (i >= KeyWaitTimeS)
   	  {
   	  	 WhiteLED_F=~WhiteLED_F; 
   	  	 if (WhiteLED_F)
   	  	        {
   	  	       	EN_WLED_ON;
   	  	        }
   	  	  else
   	  	       {
   	  	       	EN_WLED_OFF;
   	  	      }

   	  }   
    else if (i >= 2) 
            {
               IN_USB5V_F= ~IN_USB5V_F   ;
	        Update_LED_F=1;
              if 	(IN_USB5V_F)
              	{
			  LED_USB=Seg7_LEDON  ;
                     //  USB5V_ON	;	
                     USB5V_Diff_F=1;
              	}
	       else
	       	{
		   	  LED_USB=Seg7_LEDOFF  ;
			  USB5V_OFF	; 	
			  USB5V_Diff_F=0;
	       	}
	         Sent_CMD_DispLED();
			  Update_LED_F=0;
            }
//----------------------
     do     //waitting key release 
       { 
          wait();  
	   WDTE=0xAC ;  //Clean Watchdog
       }
   while(INTP2_SetKey == 0) ;  //
}
/* -------------------------------------------
Function:
  
---------------------------------------------- */
void INTP0_PlugIN_Chk(void)
{ 
					
	  if (PlugIn12V)
		{			 
			if (PlugIN_12V_F==0)
				{
				  Out1_Diff_F=1;
		                OUT1Danger_Cnt=0; 				  
	                        BAT12V_OFF	;	
				  ADC1_Out1=0;
				  
				}                       				
                     PlugIN_12V_F=1 ;						 
		}   
	  else  
	  	{
	  	  PlugIN_12V_F=0 ;
	  	}
//------------------------------
	  if (PlugIn13V)
		{	
			if ((PlugIN_13V_F==0) && (Out1_Diff_F==0))
				{
				  Out1_Diff_F=1;
		                OUT1Danger_Cnt=0; 					  
	                       BAT12V_OFF	;	
				  ADC1_Out1=0;						   
				}                       					
                     PlugIN_13V_F=1 ;					 
		}
	  else  
	  	{
	  	  PlugIN_13V_F=0 ;
	  	}	  
//------------------------------	  
	 if (PlugIn19V)
		{							 
			if ((PlugIN_19V_F==0) && (Out1_Diff_F==0))
				{
				  Out1_Diff_F=1;
		                OUT1Danger_Cnt=0; 					  
	                       BAT12V_OFF	;	
				  ADC1_Out1=0;						   
				}                       					
                     PlugIN_19V_F=1 ;				   
		} 
	  else 
	  	{
	  	  PlugIN_19V_F=0 ;
	  	}	 
// ---disable ------must no any plig-in 12V/13V/19V 	 
	 if ((PlugIN_12V_F==0) & (PlugIN_13V_F==0) && (PlugIN_19V_F==0))
	 	{
	 	 OUT1_VShort_F=0;
		 OUT1Danger_Cnt=0; 
               Out1_Mult_F=0 ;		 
 		       
	 	}
//----------------------------------------------------- 
}

/* -------------------------------------------
Function:
if DCJackIn_Cnt=1 then Out It 
if DCJackIn_Cnt (!=0 && >=2) tjen Out Min-Voltage
---------------------------------------------- */
void VoltageOut()
{
 

    if (OUT1_VShort_F == 0)
	 {	
	    if (PlugIN_12V_F)
		{
                     R13V_OFF;		
                     R19V_OFF;							 
                     Out1_Mult_F=1 ;	 
					 
		}
	  else  if (PlugIN_13V_F)
		{
                     R19V_OFF;		
                     R13V_ON;									 
                     Out1_Mult_F=1 ;	 					 
		}	
	  else  if (PlugIN_19V_F)
		{
                     R13V_OFF;		
                     R19V_ON;									 
                     Out1_Mult_F=1 ;	 					 
		}
	  if (Out1_Mult_F==0)
	  	{
	  	 BAT12V_OFF	;	
           //    Out1_ShortV_ADC=0;		
          //     Out1_First_ADC=0;			   
		     OUT1_VShort_Cnt=0;
		     Out1_VShort_Time=0;
 
	  	}
	   }
 

//---------------------------------	 
  if (DisUpLoad_ADC2_F==0) 
    {
	 if (PlugIn_ExtPwr) 	
		{	
		    ExtPwr_F=1;
		    EN_PWRIN_ON;
		}	 
	 else   //remove Dc-Jack for Ext Power
	 	{
                     Effect_ExtPwr_F=0 ;	
                     Charge_Mode_F=0;	
                      ExtPwr_F=0;		
		       EN_PWRIN_OFF;
			BatChagre_Full_F=0 ;

                            Batt_Full_WaitS=0;  
	 	} 
    }
//--------------------------------------	
 if (IN_USB5V_F==0)
 	{
 	  USB5V_OFF;
       //  Out1_ShortV_ADC=0;
         USBDanger_Cnt=0;
     //    USB_First_ADC=0;
         USB_VShort_Cnt=0;

 	}

//------------------------- 
 }   


/*======================================= 

======================================== */
void Inital_Set_OFF()
{
	 BAT12V_OFF	;
        PIF0 = CLEAR;                  // clear interrupt request flag   INTP2
        PM3.0=1                         ; // INTP0 set Input            
        PMK0=INT_ENABLE;              // enable INTP2 interrupt    
        PMK2=INT_ENABLE;              // enable INTP2 interrupt            
        R13V_OFF;		
        R19V_OFF;							 
	           
}
/* -------------------------------------------
   
---------------------------------------------- */
void Entry_Sleep_Chk()
{

 if ((IN_USB5V_F) |(Out1_Mult_F) | (Effect_ExtPwr_F))
  	{
               NoAny_Plug_F=0;
         	 SleepLed_OFF ;
	        Pre_Sleep_F=0;
  	}
  else if (NoAny_Plug_F==0)
  	{
  	  NoAny_Plug_F=1;
	  Sleep_Time=0;
  	}
}
/* -------------------------------------------

---------------------------------------------- */
void Charge_Mode_Func()
{

	if ((BatChagre_Full_F) && (Bat_Capa_Num <= 4))
		{
		   BatChagre_Full_F=0;
		}


// Sleep_Time=0;
   if ((Batt_Full_WaitS > Chr100_WaitS) |  (Batt_Charge_BlockS> Charge_Block_Max_Time) && (BatChagre_Full_F==0))
                  {
		          BatChagre_Full_F=1 ;
			   LED_Chr_OK=Seg7_LEDON  ;  // OUT2-LED DC13.2V 
			   LED_BAT5=Seg7_LEDON ;   
			   Batt_Full_WaitS=0 ;
			   Batt_Charge_BlockS=0 ;
			    Sleep_Time=0;
			   
			}
//------charge up to 100%
else
{

     //   Sleep_Time=0;
	   if (ADC0_BAT12V >= Charge_100)  	
	   	{
	   	  Batt_Full_WaitS++  ;		
	   	}
	   	
//------------------------		 
  if (ADC0_BAT12V >= Charge_80)  	
  	{
                 LED_BAT1=Seg7_LEDON ; 	
                 LED_BAT2=Seg7_LEDON ; 	
                 LED_BAT3=Seg7_LEDON ; 	
                 LED_BAT4=Seg7_LEDON ; 				                 		 
  	          ChkOK_Cnt_F=1;	
        if  (BatChagre_Full_F==0)                      
         	{                       	
                    if (  Bat_Toggle_F)
            	             {
                               Bat_Toggle_F=0;            	    
                               LED_BAT5=Seg7_LEDON ;               	
            	              }
	             else
			        {
                                Bat_Toggle_F=1;            	    
                                LED_BAT5=Seg7_LEDOFF;               			
			       }
		 
               }
  	}
//-----Battery Charge <100% ---------  
  else 
  	{
	    Batt_Full_WaitS=0; 
  	    ChkOK_Cnt_F=0;  	
           LED_BAT1=Seg7_LEDOFF ; 	
           LED_BAT2=Seg7_LEDOFF ; 	
           LED_BAT3=Seg7_LEDOFF ; 	
           LED_BAT4=Seg7_LEDOFF ; 									 
           LED_BAT5=Seg7_LEDOFF ; 		   
           switch(BatCapa_LED)
	          {
                    default :	 
	             case 0:				           // Battery Very Low	
	             case 1:		             
                              //   LED_BAT1=Seg7_LEDOFF ; 	
                                 LED_BAT2=Seg7_LEDOFF ; 	
                                 LED_BAT3=Seg7_LEDOFF ; 	
                                 LED_BAT4=Seg7_LEDOFF ; 									 
                                 LED_BAT5=Seg7_LEDOFF ; 							
                      	   //   break;  	
 //----------------------  	 
	           //  case 1:						 // Battery 20%	
                                 LED_BAT1=Seg7_LEDON ; 	             
                     	        break;
//----------------------
              	case 2:						 // Battery 40%
                                 LED_BAT1=Seg7_LEDON ; 		              	
                                 LED_BAT2=Seg7_LEDON ; 		

	                             break;
//----------------------		
              	case 3:						 // Battery 60%
                                 LED_BAT1=Seg7_LEDON ; 	
                                 LED_BAT2=Seg7_LEDON ; 		              	
                                 LED_BAT3=Seg7_LEDON ; 		

	                             break;
//----------------------	
              	case 4:						 // Battery 80%
                                 LED_BAT1=Seg7_LEDON ; 	
                                 LED_BAT2=Seg7_LEDON ; 	
                                 LED_BAT3=Seg7_LEDON ; 	              	
                                 LED_BAT4=Seg7_LEDON ; 	
	                             break;
//----------------------	
              	case 5:						 // Battery 100%
                                 LED_BAT1=Seg7_LEDON ; 	
                                 LED_BAT2=Seg7_LEDON ; 	
                                 LED_BAT3=Seg7_LEDON ; 	
                                 LED_BAT4=Seg7_LEDON ; 									 
                                 LED_BAT5=Seg7_LEDON ; 	                                
	                          break;
           	    }			   
  	     }
}
//=================================================  
    Sent_CMD_DispLED();
    BatCapa_LED++;	

	if    (CBat_VLow_F)
		{
		   if (Charge_Vlow_Time < Charge_VLow_ON)
		   	{
		   	      EN_PWRIN_ON  ;
				DisUpLoad_ADC2_F =0;			  			  			  	
		   	}
		   else  if (Charge_Vlow_Time < Charge_VLow_OFF)
		   	{
			   DisUpLoad_ADC2_F =1;
		   	    EN_PWRIN_OFF ;			   
		   	}
		   else 
		   	{
		   	  Charge_Vlow_Time=0;
		   	}
		}
//-------------------------	
	else
		{
		 DisUpLoad_ADC2_F =0;			
		}
//--------------------------
if (BatCapa_LED >=6)
	{
		Charge_Battery_Level()		;			  
	}	
}
 /* -------------------------------------------
Function:
check battey low flag & Get Battery Bar  to  LED_LByte1 & LED_HByte2
---------------------------------------------- */
void Charge_Battery_Level()
 {  //--------------  
   if ((Effect_ExtPwr_F) &  (DisUpLoad_ADC2_F==0) ) //Battery charge mode
     {

		  if ((ADC0_BAT12V > 5 )&& ( ADC0_BAT12V < Charge_00))  //

		  	{
		  	   Batt_VeryLow_F=1;  //must up to ADC0_BAT12V >= Charge_40 will disable out voltage
                         BAT12V_OFF	;
    	                  Charge_Block_Max_Time = Charge_Max_20 ;           
                         BatLow_F=~BatLow_F ;			  
                          Bat_Capa_Num=1 ;				
		            Charge_VLow_ON = Charge_VL4_ON ;
             	            Charge_VLow_OFF = Charge_VL4_OFF ;				
			   
		  	}

		  
         else  if ((ADC0_BAT12V >= Charge_00 )&& ( ADC0_BAT12V < Charge_20))  //
          {         
          	            CBat_VLow_F=1;	
    	        Charge_Block_Max_Time = Charge_Max_20 ;          
                BatLow_F=~BatLow_F ;			  
                Bat_Capa_Num=1 ;  					
				
			     Charge_VLow_ON = Charge_VL5_ON ;	
			     Charge_VLow_OFF = Charge_VL5_OFF ;				   
			   
          }
		 
			

   //-------------------    		  
     else  if ((ADC0_BAT12V >= Charge_20)&& ( ADC0_BAT12V < Charge_40))
	        {	     
	            CBat_VLow_F=0;	  	        
                  Bat_Capa_Num=2 ;		     	  	    
				//   Charge_VLow_ON = Charge_VL4_ON ;	
			      //    Charge_VLow_OFF = Charge_VL4_OFF ;
	        	if (Pre_Bat_Capa_Num != Bat_Capa_Num)
	        		{
    	                        Charge_Block_Max_Time = Charge_Max_40	  	;  
				   Batt_Charge_BlockS=0;
	        		}      
	        }		  
   //-------------------    
     else  if ((ADC0_BAT12V >= Charge_40)&& ( ADC0_BAT12V < Charge_60))
	        {	     
	            CBat_VLow_F=0;	  
				Batt_VeryLow_F=0;  
                  Bat_Capa_Num=3 ;		
	        		//  Charge_VLow_ON = Charge_VL5_ON ;
			       //  Charge_VLow_OFF = Charge_VL5_OFF ;				  
	        
	        	if (Pre_Bat_Capa_Num != Bat_Capa_Num)
	        		{
	        		    OUT1_VShort_Cnt=0;
				   USB_VShort_Cnt=0;
    	                        Charge_Block_Max_Time = Charge_Max_60	  	;  
				   Batt_Charge_BlockS=0;

	        		}      
	        }
   //-------------------    
     else  if ((ADC0_BAT12V >= Charge_60)&& ( ADC0_BAT12V < Charge_80))
	        {	      
	        	   CBat_VLow_F=0;	  
				   	Batt_VeryLow_F=0;  
                       Bat_Capa_Num=4 ;	 
	        	if (Pre_Bat_Capa_Num != Bat_Capa_Num)
	        		{
    	                        Charge_Block_Max_Time = Charge_Max_80	  	;  
				   Batt_Charge_BlockS=0;								
	        		}				  
         	    
	        }	 
     else  if ((ADC0_BAT12V >= Charge_80)&& ( ADC0_BAT12V < 245))
	        {	     
	            CBat_VLow_F=0;	  
					Batt_VeryLow_F=0;  
                  Bat_Capa_Num=5 ;	       	    	        
	        	if (Pre_Bat_Capa_Num != Bat_Capa_Num)
	        		{
    	                        Charge_Block_Max_Time = Charge_Max_100	  	;    
				   Batt_Charge_BlockS=0;								
	        		}
	         }    	

          
   	}
//-------------------------------------------
        Pre_Bat_Capa_Num = Bat_Capa_Num;	
	 BatCapa_LED=Bat_Capa_Num;
 }

 /* -------------------------------------------
Function:
check battey low flag & Get Battery Bar  to  LED_LByte1 & LED_HByte2
---------------------------------------------- */
void DisCharge_Battery_Level()
 {	  	   
 	            CBat_VLow_F=0;	
//--------------------------------	 
      if ((ADC0_BAT12V > 5 )&& ( ADC0_BAT12V < Batt_00))  //
          {         
          	Batt_VeryLow_F=1;
              Bat_Capa_Num=0 ;  		
	    //   SleepLed =~SleepLed;
          }
   //-------------------    
     else  if ((ADC0_BAT12V >= Batt_00)&& ( ADC0_BAT12V < Batt_20))
	        {
                   if   (Batt_VeryLow_F)
                   	{
	        	 OUT1_VShort_Cnt=0;
		        USB_VShort_Cnt=0;
                   	}
				   
          	    Batt_VeryLow_F=0;	        
                   BatLow_F=~BatLow_F ;
                   Bat_Capa_Num=1 ;			      	  	    
	        }
   //-------------------    
     else  if ((ADC0_BAT12V >= Batt_20)&& ( ADC0_BAT12V < Batt_40))
	        {	    
          	    Batt_VeryLow_F=0;	        
                  Bat_Capa_Num=2 ;	        	  	    
	         }
   //-------------------    
     else  if ((ADC0_BAT12V >= Batt_40)&& ( ADC0_BAT12V < Batt_60))
	        {	
          	    Batt_VeryLow_F=0;	        
                  Bat_Capa_Num=3 ;		     	  	    
	        }
   //-------------------    
     else  if ((ADC0_BAT12V >= Batt_60)&& ( ADC0_BAT12V < Batt_80))
	        {	     
          	    Batt_VeryLow_F=0;	        
                  Bat_Capa_Num=4 ;	          	    
	        }	 
     else  if ((ADC0_BAT12V >= Batt_80)&& ( ADC0_BAT12V < 245))
	        {	
          	    Batt_VeryLow_F=0;	        
                  Bat_Capa_Num=5 ;	       	    
	         }	    

     	  BatCapa_LED=Bat_Capa_Num;
}
	 
/* -------------------------------------------
when Battery lowest to Bat00  that plug-in to charge only.
20\4 = 5 seconds
---------------------------------------------- */
void BattVeryLowMode()
{  //watch
   if   ((Batt_VeryLow_F) & (Effect_ExtPwr_F==0) & (CBat_VLow_F==0))  //no Plug-in  Ext-power
  	{
                  BatVeryLow_Time++; //
                  if (BatVeryLow_Time >20 )
                     {	      
                        Shut_Down_F=1;  	
		          WhiteLED_F=0;       
	                 SleepLED_Flash(3);	 					 
                      //  Batt_VeryLow_F=0;			
                        BatVeryLow_Time=0;	
			   SleepLed_ON ;						
	                 Sleep_Mode();	
                     }
  	}
  else  
	    {
	       BatVeryLow_Time=0;	
	        SleepLed_OFF;	
		Shut_Down_F=0;  	
  	   }  
 
}  

/*======================================
       Low Voltage
======================================*/
void Low_Voltage_Check()
{
           if (LVIF) 
             	{
#ifdef debug_out 
     Debug_Mess()  ;
#endif	
			   WhiteLED_F=0;
               
	                 SleepLed_ON;	   
	              //   SleepLED_Flash(5);	   					 		
                        BatVeryLow_Time=0;	
	                 Sleep_Mode();	             	
             	  
            	}
	    LVIMK=INT_ENABLE ; //Enable
}

/*======================================
       Low Voltage
======================================*/
void Factory_Mode()
{
 unsigned char i=0; 
//--Test Mode----insert DC13V Jack + preee Key2 (3 Seconds)-------------------------------------
     do
       {
          delay();	  
          i++;
       }
   while((INTP2_SetKey== 0) && (i <KeyWaitTimeS)&& (PlugIn13V)  ) ;  //
             LED_ALL_OFF();
      while(INTP2_SetKey== 0) ;
	  	
      if (i>=KeyWaitTimeS)   
  	{
          for (i=0;i<3;i++)
   	   {

   	     IN_USB5V_F=1;
   	      EN_WLED_ON;
	      EN_LEDPWR_ON;
              LED_ALL_ON();      
	      SleepLed_ON;	
	      PlugIN_19V_F=1;
             VoltageOut();			 
             delay_1S();	
             delay_1S();		
             delay_1S();					 
	      PlugIN_13V_F=1;
             VoltageOut();			 
             delay_1S();	
             delay_1S();	
             delay_1S();				 
	      PlugIN_12V_F=1;
             VoltageOut();			 
             delay_1S();		
             delay_1S();	
             delay_1S();				 
			 
              LED_ALL_OFF();      	      
	       SleepLed_OFF;	
	       Power_OFF();
             delay_1S();	
             delay_1S();	
             delay_1S();				 
	      IN_USB5V_F=0;
	      PlugIN_12V_F=0;
	      PlugIN_13V_F=0;
	      PlugIN_19V_F=0;		  
          }
  	}
}
/*======== ==============================
              
======================================*/
void Out1_VShort_Deter()
{
 if (OUT1_VShort_F)  
 	{
       	ADC1_Out1=0; 
       	BAT12V_OFF	;
	    if (OUT1_VShort_Cnt >= Out1_Restart) //shoet times up to 5 will stop output Voltage
	   	{                                       //Release until remove all Dc=jack plug-in
	   	  OUT1_VShort_F=1;
	   	}		 
	  else  if (Out1_VShort_Time  > Out1_VShort_WaitS)
	   	{
	   	     OUT1_VShort_F=0;
		     Out1_Diff_F=1;
		     Out1_VShort_Time=0;
	   	} 	   	  
 	} 
//--------------
else if ((Out1_Diff_F) && (OUT1_VShort_F ==0) &&(Batt_VeryLow_F==0))
	  {      
	  	  Out1_Diff_F=0;	  
		  ADC1_Out1=0;
                 if (Out1_Mult_F) 
                 	{
                        TMHE1=ENABLE;		// Timer H0 operation enable
                        INTM1_10ms_CNT=0;
		          Get_Out1_ADC();

			   Out1_First_ADC = ADC1_Out1 ;			  
			   if ((ADC1_Out1 < V1_Cut_LOW) || (ADC1_Out1 > V1_DC19V_HIGH))
			   	{
			   	  BAT12V_OFF	; 
                              TMHE1=DISABLE;		// Timer H0 operation Disable				 
				  OUT1_VShort_F=1;
				  OUT1_VShort_Cnt++; 
			        OUT1_VShort_NO++;				  
			   	}				  
                 	} 			 			  
	  	} 

 //--------OUT1 12V------------------------------------------------------------------- 
// 12V  select one only  // can't change 12V/13V/19V/ output arrange 
 else if (PlugIN_12V_F) 
   	{
   	if  (((ADC1_Out1> 1) &&(ADC1_Out1 < V1_DC12V_LOW )) || (ADC1_Out1 > V1_DC12V_HIGH))	
   		{
   		          BAT12V_OFF;      
                         Out1_ShortV_ADC=ADC1_Out1;  						   
			   // LED_12V=Seg7_LEDOFF  ;  // 
   			    //Sent_CMD_DispLED();	     
			    OUT1_VShort_F=1 ;
			    OUT1_VShort_Cnt++;
			    OUT1_VShort_NO+=10;
	       }	
	
         }  
//--------OUT1 13V---------------------------------------------------------------------   
 else if (PlugIN_13V_F)
   	{
   	if  (((ADC1_Out1> 1) &&(ADC1_Out1 < V1_DC13V_LOW )) || (ADC1_Out1 > V1_DC13V_HIGH))	
   		{
   		           BAT12V_OFF;     			   
                         Out1_ShortV_ADC=ADC1_Out1;  							       
			    OUT1_VShort_F=1 ;
			    OUT1_VShort_Cnt++;	
			    OUT1_VShort_NO+=10;				
	         }	

	} 
 //-----------------------------------------------------------------------
//--------OUT1 19V------   
 else  if (PlugIN_19V_F)
   {
   	if  (((ADC1_Out1> 1) &&(ADC1_Out1 < V1_DC19V_LOW )) || (ADC1_Out1 > V1_DC19V_HIGH))
	    {
              BAT12V_OFF;      					   
              Out1_ShortV_ADC=ADC1_Out1;  							     
		OUT1_VShort_F=1 ; 		
	       OUT1_VShort_Cnt++;
			    OUT1_VShort_NO+=10;		   
	     }	
 
    }  

	  
//------------------------------------- 
   
}


/*======================================

======================================*/
void USB_VShort_Deter()
{	   

     if (USB_VShort_F) 
 	{
       	USB5V_OFF	;
	    if (USB_VShort_Cnt >= USB_Restart )   //shoet times up to 4 will stop output Voltage
	   	{                                       //Release until remove all Dc=jack plug-in
	   	  USB_VShort_F=1;
		  USB_VShort_Time=0; 
	   	}		
	  else  if (USB_VShort_Time  > USB_VShort_WaitS)
	   	{
	   	     USB_VShort_F=0;
		     USB_VShort_Time=0;
		     USB5V_Diff_F=1;
	   	}	   	
 	}
//--------------
else if (USB5V_Diff_F )
	  {     	  
                 if (IN_USB5V_F) 
                 	{
                 	  USB5V_Diff_F=0;
						 
		          Get_USB_ADC();	  
			   USB_First_ADC = ADC3_USB5V;			  
			   if ((ADC3_USB5V < V3_USB5V_LOW)  || (ADC3_USB5V > V3_USB5V_HIGH))
			   	{
			   	    USB5V_OFF	;
			   	}				  
                 	} 			 			  
	  	} 	 
//---------------
 else if (IN_USB5V_F)
 {
	
   if  (((ADC3_USB5V > 1) &&(ADC3_USB5V < V3_USB5V_LOW )) || (ADC3_USB5V > V3_USB5V_HIGH))
       {	
		     USB5V_OFF;          
                   USB_First_ADC=ADC3_USB5V;  									   				  								    
	           USB_VShort_F=1 ;
	           USB_VShort_Cnt++;						  
	}	
 //  else
//		{
//	 	  USB5V_ON;
//	 	}		   
		
    }
//-------------------------------------- 
}

//-----------------------------------------------------------------------------   
// Module:   wait   
// Function: generates a wait loop   
//-----------------------------------------------------------------------------   
void Short_Delay()   
{   
  unsigned char a=0 ;  
  while(a <= 0xD0)   
  {   
    a++;   
  }   
}  
//  -----END--------------------

