/*************************************************************************************
Author: Stella Benedicta Laksono
Date: 21 February 2014
Revision History :I - Functions approved
                  II- SPI communication available
                  III - Oxygen saturation code
This is sketch to generate Texas Instrument Pulse Oximetry Analogue Front End AFE4490
and photo transisor-NJL5501r
**************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <intrinsics.h>
#include <string.h>
#include "AFE4490.h"
#include <SPI.h>
#include <math.h>
#include "Average.h"

#define count 60
int IRheartsignal[count];
int Redheartsignal[count];
int IRdc[count];
int Reddc[count];
double difIRheartsig_dc;
double difREDheartsig_dc;
double powdifIR;
double powdifRed;
double IRac; 
double Redac;
double SpOpercentage;
double Ratio;

const int SOMI = 12; 
const int SIMO = 11; 
const int SCLK  = 13;
const int SPISTE = 9; 

void AFE4490Init (void);
void AFE4490Write (uint8_t address, uint32_t data);
uint32_t AFE4490Read (uint8_t address);

void setup()
{
   Serial.begin(9600);
    
   SPI.begin(); 
   pinMode (SOMI,INPUT);
   pinMode (SPISTE,OUTPUT);
   pinMode (SCLK, OUTPUT);
   pinMode (SIMO, OUTPUT);
   
   SPI.setClockDivider (SPI_CLOCK_DIV128);
   SPI.setDataMode (SPI_MODE1);
   SPI.setBitOrder (MSBFIRST);
 
   AFE4490Init (); 
   }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop()
{            for(int i = 0; i<count; i++) 
            {               
                   AFE4490Write(TIAGAIN,0x000000);	// CF = 5pF, RF = 500kR
                   AFE4490Write(TIA_AMB_GAIN,0x000005);	// Timers ON, average 3 samples 
                   AFE4490Write(LEDCNTRL,0x0011414);	// Switch to READ mode  
                   AFE4490Write(CONTROL2,0x000000);	// LED_RANGE=100mA, LED=50mA 
                   AFE4490Write(CONTROL1,0x010707);	// Timers ON, average 3 samples 
                   AFE4490Write(CONTROL0,0x000001);	// Switch to READ mode   
                  
                    Serial.println(i);
                    delay (1000);
                    if (digitalRead (SOMI)== HIGH)
                    {
                      Serial.println ("SOMI is connected, Reading register is proceed!");
                      IRheartsignal[i] = AFE4490Read(LED1VAL);
                      Redheartsignal[i] = AFE4490Read(LED2VAL);
                                                                                              
                      IRdc[i]= mean (IRheartsignal, count);
                      Reddc[i]= mean (Redheartsignal, count);
                      
                      difIRheartsig_dc= IRheartsignal[i] -IRdc[i];
                      difREDheartsig_dc = Redheartsignal[i] - Reddc[i];
                                                             
                      powdifIR = pow (difIRheartsig_dc, 2.0);
                      powdifRed = pow (difREDheartsig_dc, 2.0);
                      
                      IRac=  (powdifIR)/i; 
                      Redac= (powdifRed)/i; 
                                           
                      Ratio= (Redac/Reddc[i])/(IRac/IRdc[i]);
                      
                      SpOpercentage = 100 - (Ratio);                      
                                                                       
                    Serial.print("Red Data:"); 
                    Serial.println(AFE4490Read(LED2VAL)); 
                    Serial.print("Red Ambient Data:"); 
                    Serial.println(AFE4490Read(ALED2VAL)); 
                    Serial.print("Red Different:");
                    Serial.println(AFE4490Read(LED2ABSVAL));
                    Serial.print("IR Data:"); 
                    Serial.println(AFE4490Read(LED1VAL)); 
                    Serial.print("IR Ambient:"); 
                    Serial.println(AFE4490Read(ALED1VAL)); 
                    Serial.print("IR Different:"); 
                    Serial.println(AFE4490Read(LED1ABSVAL)); 
                    Serial.print("Ratio:"); 
                    Serial.print(Ratio); 
                    Serial.println(" "); 
                    
                    if ((SpOpercentage>100) ||(SpOpercentage<0))
                       {  
                       Serial.println("SpO2:--"); 
                       } 
                    else
                       {
                       Serial.print("");  
                       Serial.print("SpO2:"); 
                       Serial.print(SpOpercentage); 
                       Serial.print(" "); 
                       Serial.println("%");
                       }
                        
                    AFE4490Write(CONTROL0,0x000000);	// disable READ mode   
                    }
                    else
                    {
                    Serial.println ("SOMI is not connected, check the wire!");
                    }
            }
         
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AFE4490Init (void)
{ 
    Serial.println("AFE4490 Initialization Starts"); 
    AFE4490Write(CONTROL1, 0x000101);
    AFE4490Write(CONTROL2, 0x000000);  
    AFE4490Write(PRPCOUNT, 0X001F3F);

    AFE4490Write(LED2STC, 0X001770); //timer control
    AFE4490Write(LED2ENDC,0X001F3E); //timer control
    AFE4490Write(LED2LEDSTC,0X001770); //timer control
    AFE4490Write(LED2LEDENDC,0X001F3F); //timer control
    AFE4490Write(ALED2STC, 0X000000); //timer control
    AFE4490Write(ALED2ENDC, 0X0007CE); //timer control
    AFE4490Write(LED2CONVST,0X000002); //timer control
    AFE4490Write(LED2CONVEND, 0X0007CF); //timer control
    AFE4490Write(ALED2CONVST, 0X0007D2); //timer control
    AFE4490Write(ALED2CONVEND,0X000F9F); //timer control

    AFE4490Write(LED1STC, 0X0007D0); //timer control
    AFE4490Write(LED1ENDC, 0X000F9E); //timer control
    AFE4490Write(LED1LEDSTC, 0X0007D0); //timer control
    AFE4490Write(LED1LEDENDC, 0X000F9F); //timer control
    AFE4490Write(ALED1STC, 0X000FA0); //timer control
    AFE4490Write(ALED1ENDC, 0X00176E); //timer control
    AFE4490Write(LED1CONVST, 0X000FA2); //timer control
    AFE4490Write(LED1CONVEND, 0X00176F); //timer control
    AFE4490Write(ALED1CONVST, 0X001772); //timer control
    AFE4490Write(ALED1CONVEND, 0X001F3F); //timer control

    AFE4490Write(ADCRSTCNT0, 0X000000); //timer control
    AFE4490Write(ADCRSTENDCT0,0X000000); //timer control
    AFE4490Write(ADCRSTCNT1, 0X0007D0); //timer control
    AFE4490Write(ADCRSTENDCT1, 0X0007D0); //timer control
    AFE4490Write(ADCRSTCNT2, 0X000FA0); //timer control
    AFE4490Write(ADCRSTENDCT2, 0X000FA0); //timer control
    AFE4490Write(ADCRSTCNT3, 0X001770); //timer control
    AFE4490Write(ADCRSTENDCT3, 0X001770);
     
    delay(1000);
    Serial.println("AFE4490 Initialization Done"); 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void AFE4490Write (uint8_t address, uint32_t data)
{
    digitalWrite (SPISTE, LOW); // enable device
    SPI.transfer (address); // send address to device
    SPI.transfer ((data >> 16) & 0xFF); // write top 8 bits
    SPI.transfer ((data >> 8) & 0xFF); // write middle 8 bits
    SPI.transfer (data & 0xFF); // write bottom 8 bits    
    digitalWrite (SPISTE, HIGH); // disable device
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t AFE4490Read (uint8_t address)
{       
    uint32_t data=0;
    digitalWrite (SPISTE, LOW); // enable device
    SPI.transfer (address); // send address to device
    //SPI.transfer (data);
    data |= (SPI.transfer (0)<<16); // read top 8 bits data
    data |= (SPI.transfer (0)<<8); // read middle 8 bits  data
    data |= SPI.transfer (0); // read bottom 8 bits data
    digitalWrite (SPISTE, HIGH); // disable device
    return data; // return with 24 bits of read data
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
