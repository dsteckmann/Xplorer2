/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  Globals.c
 *  Originating Author:  Chris Churavy
 *       Creation Date:  10/21/2010 
 *
 ******************************************************************************/ 
 
 /*--------------------------------------------------------------------------*/
/*---------------------------[  Revision History  ]--------------------------*/
/*---------------------------------------------------------------------------*/
/*
 *  when?       who?    what?
 *  ----------- ------- ------------------------------------------------------
 *  10/21/2010  CC      Original
 *
 *----------------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------------*/
/*-------------------------[   Include Files   ]------------------------------*/
/*----------------------------------------------------------------------------*/

#include <string.h>
#include "DataTypes.h"
#include <project.h>

/* Some Key Global Variables */
uint8   KeyActive;                       // Key active, 0 if none pending
uint8   KeyBuffer;                       // Keypad character, 0 if none valid


#define MAX_FLOAT_VALUE  10000.0
#define MIN_FLOAT_VALUE .001
/*----------------------------------------------------------------------------*/
/*----------------------[   Global Variables   ]------------------------------*/
/*----------------------------------------------------------------------------*/

//enum eADC { HV, TEMPERATURE, DEPTH_SENSOR , BATTERY };
//enum eADC eADC_CHANNEL;

// time and date structure for information from RTC
typedef struct date_time_s
{
  uint8_t   iday;
  uint8_t   imonth;
  uint16_t  iyear;
  uint8_t   ihour;
  uint8_t   iminute;
  uint8_t   isecond;

} date_time_t;


// bitfield structure definitions

struct flag_struct
{
   uint16_t button_pressed        : 1; // Flag is high when "key" is pressed
   uint16_t stat_flag             : 1; // set during stat test   
   uint16_t stand_flag            : 1; // set during stand test
   uint16_t in_spec_cal           : 1; // set during special cal routine
   uint16_t drift_flag            : 1; // set during drift test
   uint16_t auto_number_stations  : 1; // set when stations are to be automatically numbered
   uint16_t bat_volt              : 1; // when FALSE, battery is automatically monitored by timer1_isr()
   uint16_t diag                  : 1; // set during diagnostic self test  
   uint16_t usb_stopped           : 1; // set when usb in stop mode
   uint16_t Abort                 : 1; // Abort test when NO button held for 3 secs.
} Flags;

// the bitfield values can also be accessed as an uint8_t
union 
{
  uint16_t * flag_value;
  struct flag_struct * flag_bitfield ;
}  Flag_U;


struct control_struct
{
   uint16_t buzz_enable           : 1; // set to enable buzzer when sound_on is enabled
   uint16_t shut_dwn              : 1; // set when "auto" shut off is enabled, some tests temporarily turn this off.
   uint16_t LCD_light             : 1; // enables LCD backlight if set
   uint16_t bat_sel               : 1; // selects NICAD or ALK (via scaler board connector only).  EBB pins should not be used.
   uint16_t reset_count           : 1; // resets count when enter is pressed during test
   uint16_t update_time           : 1; // controls when the display will update countdown clock
} Controls;


union 
{
  uint16_t * controls_value;
  struct control_struct * controls_bitfield ;
}  Controls_U;



struct features_struct
{
	uint16_t auto_depth            : 1; // flag for auto depth setting, set high when enabled
	uint16_t avg_std_mode          : 1; // flag for average standard mode, set when enabled
	uint16_t SI_units              : 1; // set when units are in kg/m3
	uint16_t auto_store_on         : 1; // set when configured in auto store mode
	uint16_t sound_on              : 1; // set to enable buzzer 
	uint16_t kg_gcc_units          : 1; // Cleared when kg_m3 enable. Set when GCC enabled
	uint16_t auto_scroll           : 1; // automatically scrolls data if set
	uint16_t language_f            : 1; // Select English "bit 1" or Spanish "bit 0"
	uint16_t mc_mode               : 2; // 0: Elite Mode, 1: Smart MC Mode
  uint16_t dummy_mode            : 1; // 0: Normal, 1:DUMMY
  uint16_t chi_sq_mode           : 1; // 0: Chi squared test enabled, 1 chi test disabled
} Features;

union 
{
  uint16_t * features_value;
  struct features_struct * features_bitfield ;
}  Features_U;



struct offsets_struct
{
   uint16_t den_offset_pos        : 1; // Density offset flag, set when "ON"
   uint16_t moist_offset_pos      : 1; // Moisture offset flag, set when "ON"
   uint16_t tren_offset_pos       : 1; // Trench offset flag, set when "ON"
} Offsets;

union 
{
  uint16_t * offsets_value;
  struct offsets_struct * offsets_bitfield ;
}  Offsets_U;



struct spec_flag_struct
{
   uint16_t spec_cal_flag         : 1;
   uint16_t nomograph_flag        : 1;
   uint16_t auto_turn_off         : 1; // set when enabled to time out and turn off automatically                                              
   uint16_t recall_flag           : 1; // set when recall feature is activated.
   uint16_t stored_data           : 1; // set when previous stored data is to be used in Spec cal.
   uint16_t gauge_derived         : 1; // set when SC values are gauge derived.
   uint16_t self_test             : 1; // self test flag
   uint16_t RDA_CR                : 1; // '\r' has been read into uart1 buffer
   uint16_t trench_offset_flag    : 1; // set when doing a trench offset count
} Spec_flags;


union 
{
  uint16_t * spec_flags_value;
  struct spec_flag_struct * spec_flags_bitfield ;
}  Spec_flags_U;



/*********************************************************************/
 date_time_t date_time_g;
 
volatile uint32 msTimer;

 uint8_t usb_start = 0;
 uint8_t global_special_key_flag;
 uint8_t tst_depth_g;
 uint8_t cnt_time;
 uint16_t  g_stop_time_secs;
 uint8_t ok_2_print;
 uint8_t gp_disp; 

 uint8_t test_number;
 uint8_t in_menu;
 uint8_t next_in2; 
 uint8_t next_out2;

 uint16_t special_depth=0, temp=0;				

 uint8_t	splashScreen=TRUE, nicadVoltageFlag=TRUE, alkVoltageFlag=TRUE;
           
 uint32_t shutdown_timer;
 uint32_t g_turn_off_cnts;
 uint16_t valid_depth;

 uint16_t total_seconds;
 uint16_t depth_setting;

 uint32_t date_unformatted; 
 uint32_t day_counter_g;
 
 fp32_t calib_depth;

 fp32_t volts_calc;

 uint16_t alfat_errors;
 uint8_t alfat_error;

char s1[4] = "!00";

BYTE buffer2[32];
 
char lcdstr[80];

char gps_fix = FALSE;

uint8 PCB_REV;

uint8 charger_state = 0;

/*---------------------------------------------------------------------------*/
/*------------------------------[  Global Macros  ]--------------------------*/
/*---------------------------------------------------------------------------*/

void append_char(char* s, char c)    
{  
  uint8_t len = strlen(s);
  s[len] = c;
  s[len+1] = '\0';
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
void nullString ( char * string, uint16_t length )
{

  
  while ( length-- )
  {
    string[length] = 0;
  }

}




/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/

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


/******************************************************************************
 *
 *  Name: checkFloatLimits
 *
 *  PARAMETERS: address of a value to be checked
 *
 *  DESCRIPTION: If the floating point values are over the expected limits for the 
 *               moisture and density calculations, set them to zero 
 *               
 *               Limit scope to this module.
 *  RETURNS: changes the passed value if limits not met
 *
 *****************************************************************************/ 
 void checkFloatLimits ( float * value_to_check )
 {
    if ( Features.dummy_mode == TRUE ) return;
    if (  *value_to_check > MAX_FLOAT_VALUE  )
    {
      *value_to_check = MAX_FLOAT_VALUE;
    } 
    else if ( *value_to_check < MIN_FLOAT_VALUE )
    {
      *value_to_check = MIN_FLOAT_VALUE;
    }
    else if ( !(*value_to_check == *value_to_check) )
    {
      *value_to_check = 99999.0;
    }
 
 }
 
 