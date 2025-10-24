/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  Utilities.h
 *  Originating Author:  
 *       Creation Date:  
 *
 ******************************************************************************/ 
 
 /*--------------------------------------------------------------------------*/
/*---------------------------[  Revision History  ]--------------------------*/
/*---------------------------------------------------------------------------*/
/*
 *  when?       who?    what?
 *  ----------- ------- ------------------------------------------------------
 *  
 *
 *---------------------------------------------------------------------------*/

/*  If we haven't included this file already.... */
#ifndef UTILITIES_H
#define UTILITIES_H

#include "Globals.h"
#include  "ProjectData.h" 
#include  "stdbool.h" 
    
#define NICAD_ADC_CHAN             0
#define TEMPERATURE_ADC_CHAN       1
#define ALKALINE_ADC_CHAN          2
  
#define DEPTH_SENS_ADC_CHAN        3  
  
#define ADC_CHANNELS               4
    
#define BEGIN_FLAG_0     0xAA
#define BEGIN_FLAG_1     0x55  // 2 begin flags, why?, I don't remember, must be getting old, definitely getting old.
#define CMD_FLAG_READ    0x33  // new reading
#define CMD_FLAG_CC      0x77  // calibration constants
#define ACK_FLAG_CC      0x78  // calibration constants
#define CMD_FLAG_AllData 0x87  // Requesting all data from the Xplorer2 (2 backets CC and Reading)
#define CMD_FLAG_SN      0x88  // serial number
#define CMD_FLAG_VER     0x89  // firmware version
#define CMD_SET_SN       0x90  // pc setting sn
#define CMD_FLAG_RESET   0x99  // software reset
#define DELIMITER_FLAG   '~'   // for receiving data from the ble module
#define END_FLAG         '\n'  // for receiving data from the ble module


extern void shut_down_test (void ) ;
extern void light_test (void ) ;
extern void raw_count_test (void ) ;
extern void key_pad_test (void );
extern void USB_store_test (void );
extern void SleepDelay128ms( void );
extern void set_adc_channel ( uint16_t chann );
extern float readADCVolts ( uint8_t channel );

extern void initSystem ( void );
extern void printTimeDate ( date_time_t date );
extern void getTimeDateStr ( date_time_t date, char * d_t_str );
extern void SendBLEData ( station_data_t * ble_data, bool isRecall );
extern void SendBLEDataCC ( ) ;
extern void SendBleSn();
extern void SendBleReset();
extern void reset_ble_menu();
extern void ResetBleModule(bool verbose);
extern void RequestBleVersion();
extern void ParseBleVersion(uint8* b);
extern void InitBleCharacteristics();
extern void setSerialNumber ( uint32 serial_number );
extern void ParseCalibrationConstants(uint32* b);
extern void SendBleCcAck();
extern void RTC_WriteTime ( date_time_t* d_time );  //write time and date from RTC
#endif




