/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  
 *  Originating Author:  
 *       Creation Date:  
 *
 ******************************************************************************/
 
 
/*---------------------------------------------------------------------------*/
/*---------------------------[  Revision History  ]--------------------------*/
/*---------------------------------------------------------------------------*/
/*
 *  when?       who?    what?
 *  ----------- ------- ------------------------------------------------------
 *  10/21/2010  CC      Original
 *
 *---------------------------------------------------------------------------*/

#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

#include "Globals.h"

extern void SavePartialEepromData(uint8* array,int32 len,int32 offset);  
extern void getNVFromEEProm (void );
extern void EEpromReadArray(uint8 *array, uint32 len,uint32 eepromOffset);
//*************************************************************************************************************************

// Limites to project storage 

#define NULL_NAME_STRING {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

#define PROJECT_NOT_VALID 0
#define AUTO_NUMBER_ON     1
#define AUTO_NUMBER_OFF    0

#define MAX_DEPTHS      13

#define BS              0
#define DEPTH_2_INCH    1
#define DEPTH_3_INCH    2
#define DEPTH_4_INCH    3
#define DEPTH_5_INCH    4
#define DEPTH_6_INCH    5
#define DEPTH_7_INCH    6

#define DEPTH_8_INCH    7
#define DEPTH_9_INCH    8
#define DEPTH_10_INCH   9
#define DEPTH_11_INCH   10
#define DEPTH_12_INCH   11


#pragma pack(1) 
typedef struct s_depth_cal
{
  DOUBLE_FLOAT  A;
  DOUBLE_FLOAT  B;
  DOUBLE_FLOAT  C;
} t_depth_cal_const_abs;  

#pragma pack(1) 
typedef struct constants_s
{
  t_depth_cal_const_abs  DEPTHS[MAX_DEPTHS];   
  DOUBLE_FLOAT           E_MOIST_CONST;             //
  DOUBLE_FLOAT           F_MOIST_CONST;             //
  uint16_t               SERIAL_NUM;
  date_time_t  CAL_DATE           ;                 // 4 BYTES saves the date of the last calibration;
  float       SPECIALCAL_B        ;                 // special cal constant for B value
} constants_t;


////////////////////////Memory locations for avg standard test values///////////
#pragma pack(1) 
typedef struct avg_stand_test_s
{
  uint16_t     dense_count_1    ;          
  uint16_t     dense_count_2    ;          
  uint16_t     dense_count_3    ;          
  uint16_t     moist_count_1    ;         
  uint16_t     moist_count_2    ;         
  uint16_t     moist_count_3    ;         
  uint16_t     std_number       ;          

  uint16_t    dense_count_x[30] ;          // save the last 30 std counts 
  uint16_t    moist_count_x[30] ;          // save the last 30 std counts 
  date_time_t date_count_x[30]  ;          // save the last 30 std count dates 
  uint16_t    std_index         ;          // next place to save standard value 
  uint16_t    std_counts        ;          // save number of counts
} avg_stand_test_t;

#pragma pack(1) 
typedef struct eepromData_s
{
  uint16_t      DEN_STAND     ;
  uint16_t      MOIST_STAND   ;
  constants_t   Constants     ;
  avg_stand_test_t  stand_test    ;

  uint16_t     M_CNT_AVG      ;           // 2BYTES average moisture count 
  uint32_t     D_CNT_AVG      ;           // 4BYTES average density count   
  uint16_t     M_CNT_STD      ;           // 2BYTES statisical moisture count 
  uint16_t     D_CNT_STD      ;           // 2BYTES statisical density count  

  // Other Storage
  uint16_t     COUNT_TIME     ;          // 1BYTES test time for counting pulses 
  float        D_OFFSET       ;          
  float        K_VALUE        ;                    
  float        T_OFFSET       ;                 
  float        MARSHALL       ;          
  float        PROCTOR        ;         
  uint16_t     TEST_DEPTH     ;          // 2BYTES Test depth for conducting density measurements(long int)

  ///////////////////////STORAGE FOR RECALL///////////////////////////////////////
  uint16_t    LAST_TEST_DEPTH     ;      
  date_time_t LAST_TEST_TIME      ;      

  /////////////Special calibration and Nomograph constants///////////////////////
  DOUBLE_FLOAT  SPECIALCAL_CR     ;    // special cal constant for CR value
  uint16_t    SPECIALCAL_DEPTH    ;    // depth for special cal constant
  uint16_t    MAT_THICK           ;    // Material thickness for Nomograph mode
  float       BOTTOM_DENS         ;    // Bottom Density for Nomograph mode

  uint16_t    OFF_MODE            ;      // Monitors when gauge is turned off or shuts down after 1 hr.
  struct offsets_struct    OFFSET_SETTINGS     ;     // stores offset settings for auto shutdown
  struct flag_struct       FLAG_SETTINGS       ;     // stores flag settings for auto shutdown
  struct control_struct    CONTROL_SETTINGS    ;     // stores Control settings for auto shutdown
  struct features_struct   FEATURE_SETTINGS    ;     
  uint32_t    SPECIALCAL_DEN      ;     // 4BYTES
  uint16_t    ENTER_DEN_LATER     ;     // 1BYTES      // set when special cal density is to be entered later
  uint16_t    TEMP2               ;     // 2BYTES      // unused
  float       KK_VALUE            ;     // 4BYTES      // store calculated KK value in memory for Nomograph
  uint16_t     MANUAL_STORE       ;         // 1 BYTES
  uint16_t     GP_DISPLAY         ;         // 1 BYTES saves the index of the last displayed screen in general purpose test
  uint16_t     DENSE_CAL_STD      ;         // 2 BYTES saves standard count at time of calibration
  uint16_t     MOIST_CAL_STD      ;         // 2 BYTES saves standard count at time of calibration
//  uint32_t     CAL_COUNTER        ;         // 4 BYTES saves day(offset from 12/31 of the previous year) 
  uint16_t     st_dense_avg1      ;         // 2 BYTES
  uint16_t     st_moist_avg1      ;         // 2 BYTES
  uint16_t     st_dense_avg       ;         // 2 BYTES
  uint16_t     st_moist_avg       ;         // 2 BYTES
  uint32_t     st_dense_ratio1    ;         // 4 BYTES
  uint32_t     st_moist_ratio1    ;         // 4 BYTES  
  uint32_t     st_dense_ratio     ;         // 4 BYTES
  uint32_t     st_moist_ratio     ;         // 4 BYTES
  uint16_t     dr_dense_avg       ;         // 2 BYTES
  uint16_t     dr_moist_avg       ;         // 2 BYTES  
  uint32_t     dr_dense_per       ;         // 4 BYTES
  uint32_t     dr_moist_per       ;         // 4 BYTES
  uint16_t     st_pass_fail       ;         // 1 BYTES set if first stat test of self diag test fails
  uint32_t     d_temp             ;         // 4 BYTES gauge temp before drift test
  uint32_t     d_volts            ;         // 4 BYTES high voltage 
  date_time_t  diag_test_date     ;         // 4 BYTES time/date stamp
  uint16_t     TEMP_1  ;                    // 2 BYTES Not Used
  float        NOMOGRAPH_CORRECTION ;       // 4 bytes Correction when top layer < 1.5"
  uint8_t      LAST_READING_TYPE  ;         // 1 byte  
  char         active_project_name[PROJ_NAME_LENGTH];
  GPSDATA      LAST_GPS_READING   ;
  float        SOIL_GRAVITY       ;         // 4 bytes
  uint8        depth_strip_type   ;         // 1 byte 
  float        safe_volts         ;         // 4 bytes  the voltage at SAFE position
  float        ds_slope           ;         // 4 bytes  the slope in volts/inch for the depth strip
  uint8        gauge_type         ;         // 1 byte for InstroTek X2 or Troxler 3440, 255 types
  uint8        temp[6]            ;         // future use
  uint8_t      intialized;
  uint32_t     SERIAL_NUM_HI      ;         // If serial number is greater thant 65535
  uint32_t      GaugeType;                  // which gauge is it?
  uint8_t      SHUT_DOWN_TIME_HOURS  ;
  uint8        temp_2[35]         ;         // future use - make all changes here

 } EEPROM_DATA_t; ;

  
extern EEPROM_DATA_t  eepromData;  

void EEpromWriteArray(uint8 *array, uint32 len,uint32 eepromOffset) ;
uint8 ReadEepromData(void);
//************************* Macros ***************************************

#define NV_AVG_STD_TEST    eepromData.stand_test
#define NV_CONSTANTS(M)    eepromData.Constants.M

// Gets the data from RAM copy
#define NV_RAM_MEMBER_RD(M)     eepromData.M

// update both RAM and EEPROM copies  
#define NV_MEMBER_STORE(M,V)  do{\
                                 eepromData.M = V;\
                                 SavePartialEepromData((uint8*)&eepromData.M, sizeof(eepromData.M), offsetof( EEPROM_DATA_t,M)) ;\
								                }while(0)    
   
#define READ_EEPROM(M) 		EEpromReadArray((uint8*)&eepromData.M,sizeof(eepromData.M),offsetof(EEPROM_DATA_t,M))

#define NV_EE_RD(M,D)     do {\
							              	READ_EEPROM(M);\
              								*D = NV_RAM_MEMBER_RD(M);\
              							  }while(0)	


#endif 


