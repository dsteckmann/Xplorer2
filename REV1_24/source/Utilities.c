

/**************************   ***************************************************************** ***************************/
//
// Title:       Elite Explorer
// Company:     Instrotek
//
// Document No.:
// Issue:
// Description:  
//
// Filename:   
// Author:      
//
// Processor:  
// Clock:	    12MHz	
//
//
// History: date, version, personal initials with comments about any changes
//
/**********************************************************************************************************************/

/*********************************************  INCLUDE FILES  ***************************************************/
#include "project.h"
#include "Globals.h"
#include "conversions.h"
#include "DataStructs.h"
#include "StoreFunctions.h"
#include "Alfat.h"
#include "prompts.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "Tests.h"
#include "Utilities.h"
#include "Interrupts.h"
#include "SDcard.h"
#include "UARTS.h"
#include "BlueTooth.h"
#include "Batteries.h"
#include <FS.h>
/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/
 extern uint8_t getCalibrationDepth ( uint8_t depth_inches );
 extern void initPulseCntStrt( void );
 
 extern void shut_down_state ( void );
 extern uint32 getSerialNumber ( void );
    extern uint16 BleVersionHi;
    extern uint16 BleVersionLo;
extern EEPROM_DATA_t  eepromData; 

/************************************************  LOCAL DEFINITIONS  *************************************************/


/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/


#ifndef language
#define language  1
#endif

// Make this a power of 2 for binary math
#define BUFFER_SIZE  16
#define BUFFER_SHIFT 4  // 2^4 = 16

typedef struct 
{
  int32_t filter[ADC_CHANNELS];
  int32_t average;
  uint8_t index;
} filter_t ;
typedef struct
{
 uint16 buffer_size;
 uint32 value_array[BUFFER_SIZE];
 uint16 value_index;
 uint32 adc_filter;
 uint8 filter_filled;
 volatile uint32 filtered_value;
} circular_buffer_t;

static circular_buffer_t filtered_temperature;
static int32 filt[ADC_CHANNELS] = {0,0,0,0};
static int32 filteredADC[ADC_CHANNELS];

/////UTILITY FUNCTIONS//////////////////////////////////////////////////////////
/******************************************************************************
 *  Name:           ParseCalibrationConstants
 *  PARAMETERS:     buffer with the data in it
 *  DESCRIPTION:    Parses and saves the calibration constants received from the PC app
 *****************************************************************************/ 
void ParseCalibrationConstants(uint32* b) {
    float* f;
    uint32 i = 0, len;
    uint8_t selection = 0, s2;
    len = b[0] / 4; // len is all the bytes, divide by 8 to get number of doubles, should be 42
    /*  Depths are 1-12 - 1 = 0-11  bs=1,2,3,4,5,6,7,8,9,10,11,12
        PC returns bs,1=ac,2,3,4,5,6,7,8,9,10,11,12,13=moisture
    */
    f = (float*)&b[1];
    for(i = 0; i < 39; i += 3, selection++) { // they come in the order of depth (bs-12) and A,B,C 3*14= 42 skip moist 
        s2 = selection;
        if(s2 ==1) continue; // this gauge does not have ac
        if(s2 != 0) { s2--; }
        eepromData.Constants.DEPTHS[s2].A = f[i];
        eepromData.Constants.DEPTHS[s2].B = f[i + 1];
        eepromData.Constants.DEPTHS[s2].C = f[i + 2];
    }
    eepromData.Constants.E_MOIST_CONST = f[i++];
    eepromData.Constants.F_MOIST_CONST = f[i++]; // next should be 0
    i += 2;  // i do not know
    len = b[i++]; // this is the date
    eepromData.Constants.CAL_DATE.isecond = (len & 0x1f) * 2; len = len >> 5;
    eepromData.Constants.CAL_DATE.iminute = (len & 0x3f); len =len >> 6;
    eepromData.Constants.CAL_DATE.ihour = (len & 0x1f); len = len >> 5;
    eepromData.Constants.CAL_DATE.iday = (len & 0x1f); len = len >> 5;
    eepromData.Constants.CAL_DATE.imonth = (len & 0x0f); len = len >> 4;
    eepromData.Constants.CAL_DATE.iyear = (len & 0x7f) + 2022;    
    EEpromWriteArray(  (uint8 *)&eepromData.Constants, sizeof(constants_t), offsetof( EEPROM_DATA_t,Constants ) );
    len = b[i]; // set the date flag
    len = len & 0xff;
    if(len != 0) {
        RTC_WriteTime(&eepromData.Constants.CAL_DATE);
    }
    ReadEepromData();
    SendBLEDataCC();
}
/******************************************************************************
 *  Name:           ParseBleVersion
 *  PARAMETERS:     buffer with the data in it
 *  DESCRIPTION:    Sends character to BlueToothUart then delays 9 ms
 *****************************************************************************/ 
void ParseBleVersion(uint8* b)
{
    memcpy(&BleVersionHi,&b[0],2);
    memcpy(&BleVersionLo,&b[2],2);
}
/******************************************************************************
 *  Name:           BlueToothUartPutCharDelay
 *  PARAMETERS:     character to send
 *  DESCRIPTION:    Sends character to BlueToothUart then delays 9 ms
 *****************************************************************************/ 
void BlueToothUartPutCharDelay(char c) {
    BlueToothUart_PutChar ( c );  
    CyDelay ( 9 );   
}
/******************************************************************************
 *  Name:           BlueToothUartPutPktHeaderDelay
 *  PARAMETERS:     2nd byte of the ID, and the len of the data to follow
 *  DESCRIPTION:    Sends header to BlueToothUart with intercharacter delay of 9 ms
 *****************************************************************************/ 
void BlueToothUartPutPktHeaderDelay(char beginFlag2, uint16 len) {
    uint8 lenMSB = (uint8)((len>>8) & 0x00FF);
    uint8 lenLSB = (uint8)(len & 0x00FF);
    BlueToothUartPutCharDelay ( BEGIN_FLAG_0 );  
    BlueToothUartPutCharDelay ( BEGIN_FLAG_1 );
    BlueToothUartPutCharDelay ( beginFlag2 );
    BlueToothUartPutCharDelay (lenMSB);
    BlueToothUartPutCharDelay (lenLSB);
}
/******************************************************************************
 *  Name:           BlueToothUartPutBuffer
 *  PARAMETERS:     uint8* pointer to Buffer to send and the len of the data to send
 *  DESCRIPTION:    Sends the buffers contets to BlueToothUart
 *****************************************************************************/ 
void BlueToothUartPutBuffer(uint8* buf, uint16 len) {
    int i;
    for( i =0; i < len ; i++ ) {
        BlueToothUart_PutChar(buf[i]);
        CyDelay(2);
    }
}
/******************************************************************************
 *  Name:           SendBLEDataCC
 *  DESCRIPTION:    Sends EepromData Constants to the BLE module
 *****************************************************************************/ 
void SendBLEDataCC() {
    uint16 len = sizeof(constants_t); // including extra bytes for new Serial Number
    uint32 sn = getSerialNumber();
    float v = VERSION;
    BlueToothUartPutPktHeaderDelay(CMD_FLAG_CC, len + 12);          // sn=4 (uint32), gaugetype = 1 (send 4 for upgrades), version = 4 (float)
    BlueToothUartPutBuffer((uint8*)(&eepromData.Constants), len);
    BlueToothUartPutBuffer((uint8*)(&sn), 4);                       // added to accomadate new size in the serial number
    sn = eepromData.gauge_type;
    BlueToothUartPutBuffer((uint8*)(&sn), 4);                       // new type can be used for more heads
    BlueToothUartPutBuffer((uint8*)(&v), sizeof(float));            // should be 4 bytes, version of the xp firmware
}
/******************************************************************************
 *  DESCRIPTION:    Sends Acknowlegement or calibration receipt to the PC.
 *****************************************************************************/ 
void SendBleCcAck() {
    uint32 sn = getSerialNumber();
    BlueToothUartPutPktHeaderDelay(ACK_FLAG_CC, 4);
    BlueToothUartPutBuffer((uint8*)(&eepromData.Constants), 4);
    BlueToothUartPutBuffer((uint8*)(&sn), 4);
}
/******************************************************************************
 *  Name:           RequestBleVersion
 *  DESCRIPTION:    Requests the BLE modules firmware version
 *****************************************************************************/ 
void RequestBleVersion() { 
    uint32 sn = getSerialNumber();
    BlueToothUartPutPktHeaderDelay(CMD_FLAG_VER, 4); // protocol nees 
    BlueToothUartPutBuffer((uint8*)(&sn), 4);
}
/******************************************************************************
 *  Name:           SendBleNewSn
 *  DESCRIPTION:    Sends The Gauge Serial Number to the BLE module
 *****************************************************************************/ 
void SendBleSn() { 
    uint32 sn = getSerialNumber();
    BlueToothUartPutPktHeaderDelay(CMD_FLAG_SN, 4);
    BlueToothUartPutBuffer((uint8*)(&sn), 4);
}
/******************************************************************************
 *  Name:           SendBleReset
 *  DESCRIPTION:    Sends The Gauge Serial Number to the BLE module
 *****************************************************************************/ 
void SendBleReset() {
    uint32 i = 8371;
    BlueToothUartPutPktHeaderDelay(CMD_FLAG_RESET, 4);
    BlueToothUartPutBuffer((uint8*)(&i), 4);
}
/******************************************************************************
 *  Name:           SendBLEData
 *  PARAMETERS:     pointer to StationData to send
 *  DESCRIPTION:    sends Station data to the BLE module
 *****************************************************************************/ 
void SendBLEData ( station_data_t * ble_data, bool isRecall )
{  // BUF CONTAINS DATA WITHOUT BEGIN AND END FLAGS
    uint16 len = sizeof(station_data_t);
    ble_data->battery_voltage[0] = readBatteryVoltage(NICAD) ;
    ble_data->battery_voltage[1] = readBatteryVoltage(ALK) ;
    BlueToothUartPutPktHeaderDelay(CMD_FLAG_READ, len + PROJ_NAME_LENGTH + sizeof(bool));
    BlueToothUartPutBuffer((uint8*)(ble_data), len);
    BlueToothUartPutBuffer((uint8*)eepromData.active_project_name, PROJ_NAME_LENGTH);
    BlueToothUartPutBuffer((uint8*)isRecall, sizeof(bool));
}
// return the temperature filtered data
uint32 getTemperature_FilteredData ( void )
{
 uint32 value = filtered_temperature.adc_filter;
 value /= BUFFER_SIZE;
 return value ;
}
static void init_data_filter ( circular_buffer_t * strct_data  )
{
  uint16 n;
  // set all values to zero
  for ( n = 0; n < BUFFER_SIZE; n++ )
  {
    strct_data->value_array[n] = 0;
  }
  strct_data->value_index = 0;
  strct_data->adc_filter = 0;
  strct_data->filter_filled = 0;
  strct_data->buffer_size = BUFFER_SIZE;
}
/*******************************************************************************
* Function Name: void circular_buffer_filter
********************************************************************************
*
* Summary: Implements a MA filter on the incoming data
*          It uses a circular buffer to make this fast.
*          Noise attenuation = sqrt(number of samples)
*          100 points has an attenuation of 10
*          64 has an attenuation of 8
*          1024 = 32
* Parameters:  
*
* Return:
*   None
*
*******************************************************************************/

static void circular_buffer_filter ( uint32 value, circular_buffer_t * stct_data )
{
  
  uint32 value32 = (uint32)(value) ;
  
  // the circular buffer has been filled
  if ( stct_data->filter_filled == 1 )
  {
    stct_data->adc_filter -= stct_data->value_array[stct_data->value_index];
    stct_data->adc_filter += value32;
    stct_data->value_array[stct_data->value_index++] = value32;
    if ( stct_data->value_index >= stct_data->buffer_size )
    {
      stct_data->value_index = 0;
    }
  }
  else
  {
    stct_data->adc_filter -= stct_data->value_array[stct_data->value_index];
    stct_data->adc_filter += value32;
    stct_data->value_array[stct_data->value_index++] = value32;

    
    // Check if the filter aray is filled
    if ( stct_data->value_index >= stct_data->buffer_size )
    {
      stct_data->value_index = 0;
      stct_data->filter_filled = 1;
    }
   
  }
  
}


/******************************************************************************
 *
 *  Name: readADC()
 *
 *  PARAMETERS: ADC channel
 *
 *  DESCRIPTION: Gets the filtered value, then converts it to a voltage 
 *            
 *  RETURNS:    voltage as a float value
 *
 *****************************************************************************/ 

float readADCVolts ( uint8_t channel )
{

  int32 k;
  float volts;
  
  k  = filteredADC[channel];
  
  if ( channel == DEPTH_SENS_ADC_CHAN )
  {
    volts = ADC_SAR_CountsTo_Volts(k);
  }
  else if ( channel == TEMPERATURE_ADC_CHAN )
  {
   volts = ADC_DelSig_CountsTo_Volts ( (int32)getTemperature_FilteredData() );
  }
  else
  {
    volts = ADC_DelSig_CountsTo_Volts(k);
  }  
  
  
  return volts;
   
 

}

/*******************************************************************************
* Function Name: LowPassFilter()
*
*
* Summary:
*  IIR filter for the adc values
*  At 33HZ smple rate, it takes about 3 seconds to settle down 
*
* Parameters:  THe latest adc value and the RTD channel
*
* Return:
*   The filtered ADC value
*
*******************************************************************************/
void LowPassFilter(int32 input, uint8 channel)
{
   
    input <<= 8;
    filt[channel] = filt[channel] + (((input-filt[channel]) >> 8) * 17); // higher the multiplier the faster the filter
    filteredADC[channel] = (filt[channel]>>8) + ((filt[channel] & 0x00000080) >> 7);
    
}

/*******************************************************************************
* Function Name: LowPassFilter()
*
*
* Summary:
*  IIR filter for the adc values
*  At 33Hz sample rate, it takes about 1.5s to settle down
*  Low Pass Filter 2 simplies the IIR filter. It probably looses a little resolution
*  Changing the multiplier coeffcient, changes the step response. The larger, the faster the response 
* Parameters:  THe latest adc value. 
*  
* Return:
*   The filtered ADC value
*
*******************************************************************************/
void LowPassFilter2(int32 input, uint8 channel)
{
   
    input <<= 8; // prescale * 256 to keep precision
    filt[channel] = filt[channel] + (((input-filt[channel]) >> 8) * 32); // higher the multiplier the faster the filter
    
    // This removes the prescale
    filteredADC[channel] = (filt[channel]>>8) ; 
    
}
/*******************************************************************************
* Function Name: ADC_RTD_ISR
*
*
* Summary:
*   The default Interrupt Service Routine for ADC.
*
*  The ADC will be halted. THe 32 bit result is read. THis also clears the interrupt flag
*  The analog mux will be incremented to the next channel. The next conversion is started.  
*
* Parameters:  
*
* Return:
*   None
*
*******************************************************************************/
static CY_ISR( ADC_ISR)
{
  static uint8_t mux = 0;
  static int32_t adc;
  static int32_t adc_sar32;
  static int16 sar_adc;
  //static float volts;

  ADC_DelSig_StopConvert();
   
  adc = ADC_DelSig_GetResult32();
 // volts = ADC_DelSig_CountsTo_Volts (adc);
  
  // filter the ADC value. Use the filteredADC value to get voltage
  LowPassFilter((uint32)adc,mux);
 
  if ( mux == TEMPERATURE_ADC_CHAN )
  {
    circular_buffer_filter ( adc, &filtered_temperature);
  }  
  
  mux +=1 ;
  if ( mux > ALKALINE_ADC_CHAN )
  {
    mux = 0;
  }
   
  AMux_Select(mux);

  ADC_DelSig_StartConvert();
 
  // get SAR value and start next conversion
  ADC_SAR_StopConvert();
  sar_adc = ADC_SAR_GetResult16();
  adc_sar32 = (int32_t)sar_adc;

  ADC_SAR_StartConvert();
  filteredADC[DEPTH_SENS_ADC_CHAN] = adc_sar32;
  LowPassFilter(adc_sar32,DEPTH_SENS_ADC_CHAN);
  
}

/******************************************************************************
 *
 *  Name: initADC ( )
 *
 *  PARAMETERS: None
 *
 *  DESCRIPTION: Initialize the DelSig ADC. Start ISR to Read values
 *               
 *               
 *               CH0 - NICAD_ADC
 *               CH1 - TEMP_SENS_ADC      
 *               CH2 - ALK_ADC   
 *               CH2 - DEPTH_SENS_ELITE
 *              
 *
 *  RETURNS:None
 *
 *****************************************************************************/ 

void initADC ( void )
{

  Opamp_1_Start();
  AMux_Start();
  AMux_Select(0);
  
  init_data_filter ( &filtered_temperature );
  
  // Start the ADC
  ADC_DelSig_Start();
  
  // Enable the ADC interrupt,and give the ISR vestor
  // This must be after ADC_START
  ADC_DelSig_IRQ_StartEx( ADC_ISR );
  
  // Start the conversion    
  ADC_DelSig_StartConvert();

  ADC_SAR_Start();
  ADC_SAR_StartConvert();  



}


/******************************************************************************
 *
 *  Name: initTimers ( void )
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Initialize the timers
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void initTimers ( void )
{
 
 // key scan timer
 initInterrupts();
  //Counter_Divider_Start();
}  



/******************************************************************************
 *
 *  Name: initIOLines ( void )
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Initialize the GPIO
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void initIOLines ( void )
{
 uint8 n;
 
 DIS_YES_ON_Write ( 1 ); // Disable the YES/ON key from toggling the LTC2951 power controller.
 MICRO_POWER_ENABLE() ;

 GM_TUBE_ENABLE();    // Turn on both tubes
 KEY_B_LIGHT_DISABLE(); 

 for ( n=0; n < 4; n++ )
 {
  output_high( BUZZER ); 
  CyDelay(50);
  output_low( BUZZER ); 
  CyDelay(50);
 }


}

/******************************************************************************
 *
 *  Name: getPCB_Rev()
 *  PARAMETERS: 
 *
 *  DESCRIPTION: Read the rev 0,1,2 inputs to get the PCB revision number
           
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
void getPCB_Rev ( void )
{
  uint8 rev = 0;
  
  rev = REV_0_Read() & 0x01;
  rev |= ( REV_1_Read() & 0x01 ) << 1 ;
  rev |= ( REV_2_Read() & 0x01 ) << 2 ;
  PCB_REV = rev;
}


/******************************************************************************
 *
 *  Name: initSystem()
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: initializt the GPIO lines
 *               
 *
 *
 *
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
void initSystem ( void )
{

  initIOLines();
  
  CHARGER_ON_IO_Write ( 0 ); // turn on charger enable
  
  getPCB_Rev();
  
  initTimers ( );
  init_lcd ( );
 
  initPulseCntStrt(  );
  I2C_1_Start();
  I2C_1_DisableInt();
  //initUARTS();      // moved to main.c to support the Plus depth sensor comms.
  initAlfat();
   
  SDstart();
  // Check three times to see if card is detected.
  if ( SD_CARD_DETECT_Read() == SD_CARD_OUT) 
  {
    CyDelay(250);
    if ( SD_CARD_DETECT_Read() == SD_CARD_OUT) 
    {
      CyDelay(250);
      if ( SD_CARD_DETECT_Read() == SD_CARD_OUT) 
      {
        CLEAR_DISP;
        LCD_position(LINE1+8);
        //_LCD_PRINT("SD Card Not Detected");
        _LCD_PRINT("SDND");
        CyDelay ( 350 );
      }   
    }    
  }
  else
  {
    CreateDir( "Project" );
  }  
 SDstop(0);

  // Copy EEPROM NV memory data to RAM copy nv_storage
  getNVFromEEProm();
  
  // Set the idle shutdown time to a minimum of 1 hour.
  if ( NV_RAM_MEMBER_RD (SHUT_DOWN_TIME_HOURS ) < 1 )
  {
    NV_MEMBER_STORE ( SHUT_DOWN_TIME_HOURS, 1 );
  }
  
  display_version();
  delay_ms(1500);
  
#if 0    
    /* Start components */
  
    Global_IE();
    FS_FILE *nfile = null;
    static FS_FILE *Sfile = null;
    char p_name[30];
    getActiveProjectEE ( p_name ); // get the current project name from EEPROM;
    SD_Diag();
  
    
#endif 

 initADC ( ); 
 
 // turn on the current source if in a 3440.
 if ( NV_RAM_MEMBER_RD(depth_strip_type ) == DEPTH_STRIP_3440 )
 {
  // Init but disconnect the depth mux.
  AMux_DEPTH_Init();
  AMux_DEPTH_Select(0); // connect the current source to the 3440 depth strip.
  IDAC8_DEPTH_Start();  // turn on the current source for the depth strip.

 }
 initDepthVoltages(); // calculate the volt at each depth
 
 BlueToothUart_Start();

}

/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

Bool check_temp(Bool display)  // Checks Internal Temperature of Gauge
{
  //uint8_t key_timer = 249;
  float voltage = 0;     
  enum buttons button;
  float temp_calc;
  

  BB_ON_Write ( 1 ); // Turn on BASE board 
  CyDelay ( 50 );
  Global_ID();
  init_data_filter ( &filtered_temperature );
  Global_IE();  
  
  while ( filtered_temperature.filter_filled != 1 )
  {};
  
  Global_ID();
  voltage = readADCVolts(TEMPERATURE_ADC_CHAN);
  Global_IE();  
  // °C = (Vout - 500mV) / 10 mv/°C
  temp_calc = (voltage - .5) * 100;

  if ( Spec_flags.self_test)
  {
    BB_ON_Write ( 0 ); // Turn off BASE board 
    if((temp_calc < 65.0)  && (temp_calc > -35.0))
      return(1);  
    else
      return(0);
  }
  else
  {
    if(Flags.diag)      
    {
       NV_MEMBER_STORE(d_temp,temp_calc);  // store value to memory during diagnostic test   
    }
    
    if( !display )
    {   
      
      if ( temp_calc >= 65.0 )                           
      {    
        warning_text(0);  //TEXT// display "  *** WARNING ***\n   Gauge Too HOT" LINE2,3       
        hold_buzzer();
        delay_ms(3000);      
        CLEAR_DISP;;
      }   
    }
    else
    {
      gauge_temp_text();  //TEXT// display "Gauge Temp =" LINE2     
      ESC_to_Exit(LINE4); //TEXT// display "ESC to Exit"
 
      while(1)
      {
        LCD_position(LINE2+13);
        
        sprintf ( lcdstr, "%3.0f %cC ",(double)temp_calc,0xDF);
        LCD_print ( lcdstr );
        Global_ID();
        voltage = readADCVolts(TEMPERATURE_ADC_CHAN);
        Global_IE(); 
        // °C = (Vout - 500mV) / 10 mv/°C
        temp_calc = (voltage - .5) * 100;
        
        button = getKey ( 1500 );
        
        if((button == ENTER) || (button == ESC) || (button == MENU))
		    {
		     break;
	     	};
      }    
    }   
  }
  BB_ON_Write ( 0 ); // Turn off BASE board   
  return(1); 
}

/******************************************************************************
 *
 *  Name: shut_down_test ()
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Does a HARD shut down
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void shut_down_test (void ) 
{
 
 char time = 0;
 enum buttons button;
 
 //"  Hold OFF Key Down "        
 //"    for 25secs      "        
 //"                    "
 //"  Press ESC to Exit "    
 shut_down_txt();

 button = getKey ( TIME_DELAY_MAX ) ;

 // If OFF/NO key was hit do test
 if ( button == NO )
 {
   
   //"  TEST IN PROCESS   ");        
   //" Keep Pressing OFF  ");        
   //"  Press ESC to Exit ");  
   
   press_off_txt();
   LCD_position (LINE3) ;

   // Disable auto shut down
   Controls.shut_dwn = FALSE; 
   // Keyscan disabled
   KEYSCAN_DISABLE();  
   while(1)
   {
  
    delay_ms ( 1300 );
    LCD_position (LINE3+ 8) ;
    if(Features.language_f)
    {
      _LCD_PRINTF( "%u sec. ",++time );
    
    }
    else
    {
      _LCD_PRINTF( "%u seg.",++time ); 
    }  
    
    scan_keys();  
    button = getLastKey();
    
    if ( button == ESC )               // ENTER was pressed, reset count
    { 
      break;
    }  
   }
 } 

  // re-enable auto shut down and key scan
  Controls.shut_dwn = TRUE; 
  KEYSCAN_ENABLE() ;

}

/******************************************************************************
 *
 *  Name: light_test ()
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Turns on the LCD light and LEDs for 5 seconds 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void light_test (void ) 
{
  enum buttons button;
  
  // Press START 
  // to turn on LIGHT
  // for 5 seconds
  light_test_txt();
  
  while(1)
  {
    button = getKey( TIME_DELAY_MAX );
    if ( button == ESC )
    {
      return;
    }
    else if ( button == ENTER )
    {
      break;
    }
  }
  
  lights_are_on_txt(  );
  
  KEY_B_LIGHT_ENABLE()  ;  
  
  delay_ms ( 4000 );
 

  KEY_B_LIGHT_DISABLE();
}

/******************************************************************************
 *
 *  Name: raw_count_test ()
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: takes a 5 second count 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void raw_count_test (void ) 
{
  uint16_t moisture_cnt = 0;
  uint32_t density_1_2_cnt = 0;
  uint32_t density_cnt = 0;
  
  enum buttons button;
    
  // Press START to take 10 second count
  count_test_txt();
  
  while(1)
  {
    button = getKey( TIME_DELAY_MAX );
    if ( button == ESC )
    {
      return;
    }
    else if ( button == ENTER )
    {
      break;
    }
  }
  
  CLEAR_DISP;
  Spec_flags.self_test = TRUE;
  self_test_text(2);  // display "     Tube Test\n    In Progress" on LINE2 and LINE3    
  
  GM_TUBE_ENABLE();           // enable 2nd GM tube
  // run count for 5 seconds
  measurePulses ( LINE3, 15, &moisture_cnt, &density_1_2_cnt,tst_depth_g);  
  
  GM_TUBE_DISABLE();           // disable 2nd GM tube
  // run count for 5 seconds
  measurePulses ( LINE3, 15, &moisture_cnt, &density_cnt, tst_depth_g);  
  Spec_flags.self_test = FALSE;
  
  display_count_text ( moisture_cnt, density_cnt, density_1_2_cnt );

  while(1)
  {
    button = getKey( TIME_DELAY_MAX );
    break;
  }

}

/******************************************************************************
 *
 *  Name: USB_store_test ()
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Press a key and the KEY name appears.
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void USB_store_test (void )  
{
  char j;
  char date_string[30];
  char name_string[30];
  char temp_str[30];
  enum buttons button;
  FILE_PARAMETERS fp; 
  uint8_t n;
  
    // turn on power
    AlfatStart();
    //center the string
    
    char alfat_error = NO_ERROR;
    if ( AlfatGetVersion( temp_str ) != 0x3030 )
    {
      alfat_error = ERROR;
    }  
    
    temp_str[6] = 0;
      
    if ( NO_ERROR == alfat_error )
    {
      CLEAR_DISP;
      printOnLCDLineLocString ( 1, centerStart( strlen(temp_str)), temp_str );
      CyDelay(2000);
    }  
    else
    {
      printOnLCDLineLocString ( 1,centerStart(11), "ALFAT Error");
      CyDelay(2000);
      AlfatStop();   
      return;
    }
   
    
    USB_text(0); // display "  Insert External\n Drive in USB Port\n     Press ENTER" on LINE1, LINE2 and LINE4 
    while ( 1 )
    {
      button = getKey ( TIME_DELAY_MAX );
      
      if((button == ENTER) || (button == ESC))
      {
        break;
      }  
        
    }      
    
    if(button == ESC)
    {
     AlfatStop();   
     return;
    }  
    
    
    // If the drive can be mounted, stat test data will be stored.
    j = 0;
    while ( !check_for_USB() && j < 5 ) 
    { 
     
     USB_text(2);  // display " No USB Device "
     delay_ms ( 100 );  
     j++;   
    }; 
    
    if ( j >= 5 )
    {
      // Put ALFAT in reduced power mode
      AlfatStop();   
      return;
    }
    
    USB_text(1);  // display "    Writing Data\n    To USB Drive" on LINE2 and LINE3
    hold_buzzer();
    
    if ( initialize_USB( TRUE ) )
    {
      // make the file name string
      nullString(name_string, sizeof(name_string) );
      strcat( name_string, "USB STORE_" );
      
      // get the current time, and turn into a string
      read_RTC( &date_time_g );
      getTimeDateStr ( date_time_g, date_string );
      
      // replace the ":" with a "_"
      for ( n = 0; n < strlen(date_string); n++ )
      {
        if ( ( date_string[n] == ':' ) || (date_string[n] == '/') )
        {
          date_string[n] = '_';
        }
      }
      
      // add date to string
      strcat( name_string, date_string );
      
      // open a file
      USB_open_file  ( name_string , &fp );  
    
      // make header
      // get the current time, and turn into a string
      read_RTC( &date_time_g );
      getTimeDateStr ( date_time_g, date_string );
      sprintf( temp_str, "%s\t\r\n", date_string );
      AlfatWriteStr(&fp,temp_str);
    
      nullString(temp_str, sizeof(temp_str));
      sprintf( temp_str, "Storage Test\t\r\n" );
      
      AlfatWriteStr(&fp,temp_str);
      // close file     
      AlfatFlushData(fp.fileHandle);
      AlfatCloseFile(fp.fileHandle);  
      CyDelay ( 1500 );
    }   
   
    AlfatStop();   
  
}

/******************************************************************************
 *
 *  Name: key_padtest()
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Press a key and the KEY name appears.
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void key_pad_test (void )  
{
    enum buttons button;
    
    press_key_text();     // PRESS any KEY
    
    do
    {
      LCD_PrintBlanksAtPosition ( 20, LINE3 );
      button = getKey ( TIME_DELAY_MAX );
      
      switch ( button )
      {
        case ZERO:
                  LCD_PrintAtPositionCentered ( "0", LINE3+10 );
                  break;
        case ONE:
                  LCD_PrintAtPositionCentered ( "1", LINE3+10 );
                  break;
        case TWO:
                  LCD_PrintAtPositionCentered ( "2", LINE3+10 );
                  break;
        case THREE:
                  LCD_PrintAtPositionCentered ( "3", LINE3+10 );
                  break;
        case FOUR:
                  LCD_PrintAtPositionCentered ( "4", LINE3+10 );
                  break;
        case FIVE:
                  LCD_PrintAtPositionCentered ( "5", LINE3+10 );
                  break;          
        case SIX:
                  LCD_PrintAtPositionCentered ( "6", LINE3+10 );
                  break;
        case SEVEN:
                  LCD_PrintAtPositionCentered ( "7", LINE3+10 );
                  break;
        case EIGHT:
                  LCD_PrintAtPositionCentered ( "8", LINE3+10 );
                  break;
        case NINE:
                  LCD_PrintAtPositionCentered ( "9", LINE3+10 );
                  break;
        case UP:
                  LCD_PrintAtPositionCentered ( "UP", LINE3+10 );
                  break;
        case DOWN:
                  LCD_PrintAtPositionCentered ( "DOWN", LINE3+10 );
                  break;          
        case PRINT:
                  LCD_PrintAtPositionCentered ( "PRINT", LINE3+10 );
                  break;          
        case CE:
                  LCD_PrintAtPositionCentered ( "CE", LINE3+10 );
                  break;
        case ESC:
                  LCD_PrintAtPositionCentered ( "ESC", LINE3+10 );
                  break;
        case PROJ:
                  LCD_PrintAtPositionCentered ( "PROJ", LINE3+10 );
                  break;
        case MENU:
                  LCD_PrintAtPositionCentered ( "MENU", LINE3+10 );
                  break;
        case STORE:
                  LCD_PrintAtPositionCentered ( "STORE", LINE3+10 );
                  break;
        case YES:
                  LCD_PrintAtPositionCentered ( "YES", LINE3+10 );
                  break;                                                        
        case NO:
                  LCD_PrintAtPositionCentered ( "NO", LINE3+10 );
                  break;                                                        
        case STAND:
                  LCD_PrintAtPositionCentered ( "STD", LINE3+10 );
                  break; 
        case TIME:
                  LCD_PrintAtPositionCentered ( "TIME", LINE3+10 );
                  break;           
        case MAPR:
                  LCD_PrintAtPositionCentered ( "MAX/PR", LINE3+10 );
                  break;           
        case ENTER:
                  LCD_PrintAtPositionCentered ( "START/ENTER", LINE3+10 );
                  break;           
        
       default:   LCD_PrintAtPositionCentered ( "UNKNOWN KEY", LINE3+10 );
                  break;   

      }
      delay_ms(500);
      
    } while ( ESC != button );
    
    
}


/******************************************************************************
 *  Name: enter_depth_slope(void)
 *  
 *  PARAMETERS: Enter Depth Slope  
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 
 
void enter_depth_slope(void)  // 
{ 
  float  num_temp;
  char number_ptr[20] =  NULL_NAME_STRING ;
  enum buttons button;
    while(1)
    {
      // This is in kg_m3 units
      num_temp = NV_RAM_MEMBER_RD(ds_slope);    
      
      CLEAR_DISP;
      LCD_position(LINE1);
      _LCD_PRINTF ( "Depth Slope: %.3f",num_temp);

      change_val_text();       //TEXT// display "Change Value?" LINE2
      Press_YES_or_NO(LINE3);  //TEXT// display "Press YES or NO"
      ESC_to_Exit(LINE4);      //TEXT// display "ESC to Exit" 
      
      while(1)
      {
        button = getKey(TIME_DELAY_MAX);
        if((button == ESC) || (button == YES) || (button == NO))
        {
          break;
        }  
      }    
      if(button != YES)   // NO or ESC was pressed, exit to menu
      {
        break;
      }  
      
      enter_value_text(7);   //TEXT// display "Enter Value for\nDepth SLope:" LINE1,2
      Enter_to_Accept(LINE3);  //TEXT// display "YES to Accept" 
      ESC_to_Exit(LINE4);    //TEXT// display "ESC to Exit"
       
      num_temp = enterNumber ( number_ptr, LINE2 + 13, 6 );
      
     // If esc hit, don't save a new ENTRY
      if ( getLastKey() == ESC )
      {
        break;      
      }      

      NV_MEMBER_STORE(ds_slope,num_temp);
      
      CLEAR_DISP;
      LCD_position(LINE1);
      _LCD_PRINTF ( "Depth Slope: %.3f",num_temp);
      delay_ms(1500);
      
      break;   
    }
 
}


/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void check_depth (void )  
{
  float  value;
  uint8_t depth;
  enum buttons button;
  
  // turn on depth power
  //DS_POWER_ON()  ;
  
  // let power stabilize
  delay_ms ( 40 );
  
  // get the voltage
  value = readADCVolts(DEPTH_SENS_ADC_CHAN);

  depth_voltage_text(); //TEXT// display "DS Voltage =" LINE2      
  ESC_to_Exit(LINE4);  //TEXT// display "ESC to Exit"
 
  while(1)
  {
    
    if (  GAUGE_3440 == NV_RAM_MEMBER_RD (gauge_type ) || (  GAUGE_3500 == NV_RAM_MEMBER_RD (gauge_type )) )
    {
      LCD_position(LINE2+13);
      _LCD_PRINT ( "       ");
      LCD_position(LINE2+13);
      _LCD_PRINTF ( "%.3f", (double)value);
    }
    else if (  GAUGE_3440_PLUS == NV_RAM_MEMBER_RD (gauge_type ) )
    {
      LCD_position(LINE2);
      _LCD_PRINT ( "             ");
    }

  
    depth = get_depth_auto (0); 

    LCD_position(LINE3+13);
    _LCD_PRINT ( "       ");
    LCD_position(LINE3+13);
    
    if ( depth == 0 )
    {
      _LCD_PRINT ( "SAFE");
      
    }
    else if ( depth == 1 )
    {
      _LCD_PRINT ( "BS");
    }
    else if ( depth == 13 )
    {
      _LCD_PRINT ( "INVALID");
    }
    else
    {
      _LCD_PRINTF ( "%d", depth );
    }
    
    button = getKey(1500); 
       
    if((button == ENTER) || (button == ESC) || (button == MENU))
    {
      break;  
    }    
    
    value = readADCVolts ( DEPTH_SENS_ADC_CHAN );

   } 
  
  if ( NV_RAM_MEMBER_RD(depth_strip_type ) == DEPTH_STRIP_3440 )
  {
    // If 3440 may need to change slope
    enter_depth_slope();
  }  

}




/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void shut_down_text(void)  // sets OFF_MODE and powers OFF gauge
{
  uint8 n;
  
  isr_ON_OFF_Disable();
  isrTIMER_1_Disable();
  
  NV_MEMBER_STORE(OFF_MODE, 0);                    // update status of shutdown, 0 = operator shutdown

  Controls.shut_dwn = TRUE;                           // set when "auto" shut off is enabled   
      
  // write all flag settings to memory      
  NV_MEMBER_STORE( OFFSET_SETTINGS,Offsets);
       
  //NV_MEMBER_STORE( FEATURE_SETTINGS, Features );
         
  NV_MEMBER_STORE( FLAG_SETTINGS, Flags );          
     
 Controls_U.controls_bitfield = &Controls;     
 NV_MEMBER_STORE( CONTROL_SETTINGS, Controls );     
 Global_ID();

 shut_down_text_text();  //TEXT// display "      Shutdown" LINE2  
 delay_ms(500);
 for ( n=0; n < 5; n++ )
 {
  output_high( BUZZER ); 
  CyDelay(100);
  output_low( BUZZER ); 
  CyDelay(100);
 }

  delay_ms(1500);
  while(1) 
  {
   MICRO_POWER_DISABLE();   
   delay_ms(1000);  
  };    
  
  
}


/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void print_string_backward(char *temp_str, uint8_t position)  //prints string from right to left starting at position 
{  
  uint8_t length;
  length = strlen(temp_str);
  LCD_position(position + (1 - length));
  _LCD_PRINTF ( "%s",temp_str);
}




/******************************************************************************
 *
 *  Name: get_constant()
 *
 *  PARAMETERS: ch = 'a', 'b', 'c'
 *              depth = BS to DEPTH_12_INCH 
 *
 *  DESCRIPTION: The values are in the RAM copy of NV memory
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
DOUBLE_FLOAT get_constant(char ch, uint8_t depth)  // returns selected depth constant from memory 
{

 // the values for the depth strip are 
 // Constants offsets are stored as BS = 0, (2" = 1), ....,(12" = 11 ), (13" = 12)
 uint8_t depths;
 
 depths =	getCalibrationDepth(depth);
 
 	// Check for invalid depth
 	if ( depths > MAX_DEPTHS )
 	{
 	  return 0;
 	}
 	
  if ( ch =='a' )
  {
     return  NV_RAM_MEMBER_RD ( Constants.DEPTHS[depths].A );
  }   
  else if ( ch =='b' )
  {
     return  NV_RAM_MEMBER_RD ( Constants.DEPTHS[depths].B );
  }            
  else
  {
     return  NV_RAM_MEMBER_RD ( Constants.DEPTHS[depths].C );  
  }   

    
}




/******************************************************************************
 *
 *  Name: review_std_counts(void)
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: There are up to 30 standard counts stored
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void review_std_counts(void)
{
  Bool auto_scroll_advance;
  
  uint8_t i, j, number_of_counts, index;
  
  uint16_t DSC[30],MSC[30],loop_cnt;
  
  date_time_t TSC[30];
  
  enum buttons button;
  
  
  index = NV_RAM_MEMBER_RD(stand_test.std_index); // index is always 1-30
  index -= 1;  //match index to array element number
  number_of_counts = NV_RAM_MEMBER_RD(stand_test.std_counts);   
  if(number_of_counts != 0)
  {        
    i = index;
    j = 0;
   
    do   //read all valid counts from memory
    {
      DSC[i] = NV_RAM_MEMBER_RD(stand_test.dense_count_x[i] );  //each count is 2 bytes long
      MSC[i] = NV_RAM_MEMBER_RD(stand_test.moist_count_x[i] );  //each count is 2 bytes long    
      TSC[i] = NV_RAM_MEMBER_RD(stand_test.date_count_x[i]  );   //each date is 4 bytes long
      i = (i!=0) ? (i-1) : 29;
      j++;
    } while((i!=index) && (j<number_of_counts));  //index is array element with most recent data      
   
    i = index;
    j = 1;
   
    while(1)                                   //remains in display mode until button is pressed
    {
      CLEAR_DISP;
      LCD_position(LINE1);
      
      printTimeDate ( TSC[i] );
  
      LCD_position(LINE2);
      
       sprintf( lcdstr, "D %u  M %u", DSC[i],MSC[i] ) ; 
       LCD_print ( lcdstr ) ;
    
      if(2*j <= number_of_counts)       
      {          
        i = (i!=0) ? (i-1) : number_of_counts - 1;
        
        LCD_position(LINE3);
        printTimeDate ( TSC[i] );
        
        LCD_position(LINE4);
        sprintf (lcdstr, "D %u  M %u",DSC[i],MSC[i]);
        LCD_print ( lcdstr );
        
      }       
      auto_scroll_advance = FALSE;     
      if(Features.auto_scroll)
      {
        loop_cnt = 0;
        while(1)
        {          
          button = getKey(50);
          if( (button == STORE) || (button == ESC) || (button == MENU) || (button == UP) || (button == DOWN) || (++loop_cnt >= 100))
            break;
        }
        if(loop_cnt >= 100)
          auto_scroll_advance = TRUE;
        else
          auto_scroll_advance = FALSE;
      }
      else
      {
        while(1)
        {
          button = getKey(TIME_DELAY_MAX);
          if((button == ESC) || (button == UP) || (button == DOWN) || (button == MENU) || (button == STORE) )
            break;
        }        
      }
      if((button == DOWN) || auto_scroll_advance)     
      {
        i = (i!=0) ? (i-1) : number_of_counts - 1;         
        j = (j+1)%((number_of_counts+1)/2);
        if(j==0)
          j = (number_of_counts+1)/2;
      }     
      else if ( button == STORE )
      {
       storeStdCountsToUSB( 1 );
      }
      else if(button == UP)       
      {       
        if(number_of_counts > 2)
        {
          if((i<(number_of_counts-2)) && i!=0)
            i = (i+3)%30;
          else if(i!=0)
            i = (i+3)%((number_of_counts+1)/2);
          else
            i = ((number_of_counts%2)==0) ? i+3 : i+2;             
        }
        else
        {
          i = (i+1)%number_of_counts;
        }   
        j -= 1;
        if(j==0)
        {
           j = (number_of_counts+1)/2;
        }   
      }
      else
        break;
    } 
  }
  // There are no standard counts available
  else
  {
    CLEAR_DISP;
    LCD_position(LINE1);
    LCD_print ( "No Valid Std. Counts" ) ;  
    LCD_position( LINE2 );
    LCD_print ( "Take a new STD Count" ) ;  
    CyDelay (2000);
  }
}
/************************************************************************/
//  Functions Name: InitBleCharacteristics()
//  Description:  Fills BLE characteristics
/***************************************************************************/
void InitBleCharacteristics() {
    station_data_t s;
    SendBLEDataCC ();
    s.offset_mask = 0;
    s.date = NV_RAM_MEMBER_RD(LAST_TEST_TIME);
    s.density = 0;
    s.density_count = NV_RAM_MEMBER_RD ( D_CNT_AVG );
    s.density_stand = NV_RAM_MEMBER_RD (DEN_STAND);
    s.depth = NV_RAM_MEMBER_RD(LAST_TEST_DEPTH);
    if (depth_setting >= 13) { s.depth = 0; }
    memcpy ( &s.gps_read, &eepromData.LAST_GPS_READING , sizeof(GPSDATA) );
    s.MA = NV_RAM_MEMBER_RD(MARSHALL);
    s.MCR = 0;
    s.moisture = 0;
    s.moisture_count = NV_RAM_MEMBER_RD ( M_CNT_AVG );
    s.moisture_stand = NV_RAM_MEMBER_RD (MOIST_STAND);
    s.name[0] = '\0';
    s.PR = NV_RAM_MEMBER_RD(PROCTOR);
    if ( Features.SI_units == FALSE ) { // assume this has been saved and read by now
         s.units           = PCF;
    }
    else if ( Features.kg_gcc_units == GCC_UNITS ) {
        s.units           = GM_CC;
    }
    else { s.units          = KG_M3; }    
    if( Spec_flags.spec_cal_flag) { //  I think the flags are useless at this point
        s.offset_mask     |= SPECIAL_CAL_BIT;
    }
    if(Offsets.den_offset_pos) { s.offset_mask |= DENSITY_OFFSET_BIT; }
    if(Offsets.moist_offset_pos) { s.offset_mask |= MOISTURE_OFFSET_BIT; }
    if(Offsets.tren_offset_pos) { s.offset_mask |= TRENCH_OFFSET_BIT; }
    
    // If 3440 set gauge type bit
    if ((eepromData.gauge_type  == GAUGE_3440  ) || (eepromData.gauge_type  == GAUGE_3440_PLUS  ) )
    { s.offset_mask |= TR_GAUGE_BIT; }
    
    s.den_off = NV_RAM_MEMBER_RD(D_OFFSET);
    s.k_value  = NV_RAM_MEMBER_RD ( K_VALUE );
    s.t_offset        =  NV_RAM_MEMBER_RD ( T_OFFSET );
    s.kk_value = NV_RAM_MEMBER_RD ( KK_VALUE );
    s.bottom_den = NV_RAM_MEMBER_RD ( BOTTOM_DENS );
    s.DT = 0;
    CyDelay(100);
    SendBLEData ( &s, true );
}
/******************************************************************************
 *  Name:           reset_ble_menu
 *  PARAMETERS:     bool verbose determines if the display is updated
 *  DESCRIPTION:    forces the BLE module to do a software reset
 *****************************************************************************/
void ResetBleModule(bool verbose){
    char buf[NUMBER_CHARS1];
    int i;
    SendBleReset(); // button other than YES was pressed, exit to menu
    CLEAR_DISP;
    printAtPosLCD(LINE1, "     Resetting");
    printAtPosLCD(LINE2, "      Module");
    i = 1;
    while(i > -1) {
        snprintf(buf,NUMBER_CHARS1,"         %02d         ",i--);
        printAtPosLCD(LINE3,buf);
        CyDelay(900);
    }
    SendBleSn();
}
void reset_ble_menu(){
    char buf[20];
    enum buttons button;
    snprintf(buf,20,"    Ver. %d.%d",BleVersionHi,BleVersionLo);
    CLEAR_DISP;
    printAtPosLCD(LINE1,buf);
    printAtPosLCD(LINE2, "   Reset the BLE");  
    printAtPosLCD(LINE3, "    Module ?");
    printAtPosLCD(LINE4, "  Press YES or NO"); 
    CyDelay (2000);
    while(1) {
        button = getKey(TIME_DELAY_MAX);
        if((button == ESC) || (button == YES) || (button == NO)) { break ; }
    }    
    if(button == YES) {
        ResetBleModule(true);
    }
}

