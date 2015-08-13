



#define  debug_out 

#define  for_test  0
#define  Charge_test  0
//Seconds counter*10 , Battery Voltage for DC Power Supply ,Plig-in DcJack (No Power )
//------------------------------------------------

//------------------------------------------------
/* Constant value definition */
#define TRUE			       1
#define FALSE		       0
#define CLEAR			0
#define INT_DISABLE		1
#define INT_ENABLE		0
#define DISABLE			0  
#define ENABLE		       1
#define LOW_LEVEL		0
#define HIGH_LEVEL		1 
#define INPUT_MODE		1
#define OUTPUT_MODE	0
#define H_PRIORITY		0
#define L_PRIORITY		1
#define OutModePin                        0  //Output
#define InModePin                          1  // Input

//***************************  chose baudrate   *******************************   
//#define BAUDRATE 9600   
//#define BAUDRATE 38400   
#define BAUDRATE 115200   


#define ADC_Vref_Normal           25  //No Loading or plug-in Ext Power
#define ADC_Vref_ExtPwr           25  //No Loading or plug-in Ext Power
#define ADC_Vref_Load              25
#define USB_Short_WTime                   3 //25/5=5 seconds
#define Out1_12V_Short_WTime             1 //25/5=5 seconds
#define Out1_13V_Short_WTime             1 //25/5=5 seconds
#define Out1_19V_Short_WTime             1 //25/5=5 seconds

#define USB_Danger_Cnt             3 //25/5=5 seconds
#define Out1_Danger_Cnt          3       //25/5=5 seconds

#define WatchDog_Set            Dis_WatchDog // 0x6F //25/5=5 seconds
#define Dis_WatchDog             0x70 //25/5=5 seconds



#define Out_5V_OffSet           0  //  1/(2/12) = 6 
#define Out_9V_OffSet           0  //  1/(2/12) = 6 
#define Out_12V_OffSet          0  //  1/(2/12) = 6 
#define Out_19V_OffSet          0  //  1/(2/12) = 6 
#define Out3_13V_OffSet          0  //  1/(2/12) = 6 
#define USB_OffSet                   0  //  1/(2/12) = 6 
#define Battery_OffSet             0  //  1/(2/12) = 6 


#define Entey_Sleep_time     5  //
#define DangerUSB_Num       4  //
#define DangerOUT1_Num     4 //

#define Out1_Restart     5 //
#define USB_Restart     5 //


#define CBat_VLow_ON           25   // Seconds
#define CBat_VLow_OFF          CBat_VLow_ON+12   // Seconds
#define CBat_VLow_OFF_Cnt         12   // Seconds
  
#define Out1_VShort_WaitS         20    //60   // Seconds watch
#define USB_VShort_WaitS          20   //60   // Seconds
//-----------------------------

#define KeyWaitTimeS            20  //about 2 seconds

#define Seg7_LEDON   1 
#define Seg7_LEDOFF  0

#if 0
#define Bat_Offset                5     // 10k:3.3K
#define Batt_00                 141      //  9V
#define Batt_20                 161     // 10V
#define Batt_40                 165     // 10.5V
#define Batt_60                 171    //11.5V
#define Batt_80                 175    //12
#define Batt_100               178    //12.5v

#define Charge_00                 151      //9v
#define Charge_20                 161     // 10V  Batt_20+Bat_Offset 
#define Charge_40                 168     // 11     //  battery low point
#define Charge_60                 174     //11.5v
#define Charge_80                 176    //12v
#define Charge_100               178    //12.5v
#else  //add diode + 0.3R 
#define Bat_Offset                5      // 10k:3.3K
#define Batt_00                 0x098   // 141      //  9V
#define Batt_20                 0x09C   //161     // 10V
#define Batt_40                 0x0A1//164     // 10.5V
#define Batt_60                 0x0A6   //   //11.5V
#define Batt_80                 0x0AB   //   //12
#define Batt_100               0x0B1   //  //12.5v

#define Charge_00            0x098    //     151      //9v     604
#define Charge_20            0x0A6    //     161     // 10V   644  Batt_20+Bat_Offset 
#define Charge_40            0x0AA     //     164     // 11     656  //  battery low point
#define Charge_60            0x0AE     //     168     //11.5v  672
#define Charge_80            0x0B0     //12.2V        170    //12v     680
#define Charge_100          0x0B2    //  176    //12.8v  704


#endif


#define Charge_Max_20               6*3600    //43200   unit:hour
#define Charge_Max_40               5*3600    //36000   unit:hour
#define Charge_Max_60               4*3600    //28800   unit:hour
#define Charge_Max_80               3*3600    //21600   unit:hour watch
#define Charge_Max_100             1*3600    //14400   unit:hour  watch

#if Charge_test
#define   Chr100_WaitS              15   //     unit:  max 256  watch
#else
#define   Chr100_WaitS              4*60   //     unit:  max 256  watch
#endif
#define Charge_VLow1           30*60    //   
#define Charge_VLow2           Charge_VLow1+30*60    //   
#define Charge_VLow3           Charge_VLow2+30*60  
#define Charge_VLow4           Charge_VLow3+30*60    //   
#define Charge_VLow5           Charge_VLow4+30*60    //   

#define Charge_VLow6           60*60    //   


#define Charge_VL1_ON          15
#define Charge_VL2_ON          30

#define Charge_VL3_ON          30
#define Charge_VL4_ON          40
#define Charge_VL5_ON          50

#define Charge_VL6_ON          60

#define Charge_VL1_OFF          Charge_VL1_ON+20
#define Charge_VL2_OFF          Charge_VL2_ON+15

#define Charge_VL3_OFF          Charge_VL3_ON+10
#define Charge_VL4_OFF          Charge_VL4_ON+10
#define Charge_VL5_OFF          Charge_VL5_ON+15

#define Charge_VL6_OFF          Charge_VL6_ON+10




#define V3_USB5V_LOW        0x070   //90 // 
#define V3_USB5V                 0x095   //148   // 
#define V3_USB5V_HIGH       0x0C8  //210   // no Zener2.4V (188)


#define V1_Cut_LOW            0x50  // 

#define V1_DC12V_LOW             0x70   //130   // 
#define V1_DC12V_LOAD           0x93    // 143   //  have Zener2.4V (231)
#define V1_DC12V_HIGH           0x0A8   // 168   //  no Zener2.4V (158)

#define V1_DC13V_LOW             0x075 // 130   //  
#define V1_DC13V_LOAD           0x09D // 154   //  have Zener2.4V (231)
#define V1_DC13V_HIGH           0x0B0 //  168   //  no Zener2.4V (165)

#define V1_DC19V_LOW            0x0A0 //  165   // 
#define V1_DC19V_LOAD          0x0BD //  186   //  have Zener2.4V (231)
#define V1_DC19V_HIGH          0x0D0 //   200   //  no Zener2.4V (231)

//----------------
#define ExtPwr_ADC2_Low    0x40 // 100  //100   // Battery charge min voltage level 
#define ExtPwr_ADC2            0x0a0 //    175   // 
#define ExtPwr_ADC2_High   0x0d0 //  200   //


#define ShortTest_Cnt           30   // 12  > 13.2 

//#define   Buzr_Cnt    11  //



