/**********************************************************************************************************************
 Title:       Elite
 Company:     Instrotek
 Document No.:
 Issue:
 Description:  
 Filename:   Elite.c
 Author:      
 Processor:  PSOC 5LP   
 History: date, version, personal initials with comments about any changes
    7/29/2022 Rich Stacy
        Added some communication between the xp micro and the BLE module
            the Xp can query the module for its version. this version requirs BLE 1.0 or higer
            the module can query the Xp for its version
            the module can update the Xp Serial number and calibration constants
        added a check for the proper version
        fixed the project station at startup, it was not populated - first recall failed without it
        added 2 functions to center strings, the original did not work, TODO replace all the originals
        added a uint32 gaugetype to the EEPROM data, this permits more variations than the bit fields
        added a isRecall bool for the BLE module
        the SN,gaugeType, and version are sent to the ble module at the end of dataCC packet
        the projectName and the isRecal are sent to the ble module at the end of the BLEdata packet
        added a diag prompt to reset the BLE module, it also displays the BLE version
**********************************************************************************************************************/
/*********************************************  INCLUDE FILES  ***************************************************/
#include "elite.h"
#include "Globals.h"
#include "DataTypes.h"
#include "DataStructs.h"
#include "StoreFunctions.h"
#include "Utilities.h"
#include "interrupts.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "prompts.h"
#include "Alfat.h"
#include "Utilities.h"
#include "Batteries.h"
#include "UARTS.H"
#include "ProjectData.h"
/*-------------------------[   Global Functions   ]---------------------------*/
extern void pulseBuzzer ( void );
extern void print_menu ( void ); 
extern void initUARTPlus ( void );

extern uint8 read_I2C_RTC ( date_time_t* d_time );
/*----------------------------------------------------------------------------*/
/*-------------------------[   Global variables   ]------------------------------*/

#define debug_mode 0

uint16 BleVersionHi = 0;
uint16 BleVersionLo = 0;
uint32 enableFlags;
uint32 priority;
int8 i;
char buf[128];
/*-------------------------[   Local variables   ]------------------------------*/
char wrBuffer[80];      // these are actually global
char menu = UNUSED ;
char t[DISPLAY_LENGTH];
/*-------------------------[   Local function Headers   ]------------------------------*/
/* Add an explicit reference to the floating point printf library */
/* to allow the usage of floating point conversion specifiers. */
/* This is not linked in by default with the newlib-nano library. */
asm (".global _printf_float");
/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/
extern void initTimers ( void );
extern void enter_cal_const(void) ;
extern void initGPS ( void ) ;
/************************************************  LOCAL DEFINITIONS  *************************************************/
/************************************************************************/
//  Functions Name: main()
//  Description:  The main loop in the program
//  Parameters:
//  Returns:   
/***************************************************************************/

void GetLastStationName() 
{
    // project_info.station_index updated in updateProjectInfo(), called just before this function
    if (( project_info.station_index == 0 ) || (Features.auto_store_on == 0 ))
    {
      strcpy(project_info.current_station_name, "" );
    }
    else
    {
      getStationName(project_info.current_project,project_info.current_station_name,project_info.station_index - 1);
    }  
}

    
void CheckBleVersion()
{
    if(BleVersionHi == 0 && BleVersionLo == 0) 
    {
        RequestBleVersion();        // try again, versions 1.0 and above answer this request
        CyDelay(500);
        if(BleVersionHi == 0) 
        {
            CLEAR_DISP;
            CtrStrNC( "Incorrect BLE",   LINE1 );
            CtrStrNC( "Module Firmware", LINE2 );
            CtrStrNC( "The MyGauge App", LINE3 );
            CtrStrNC( "may not work!",   LINE4 );
            CyDelay(4000);
        }
    }
}

int main() { //initialization and main loop
  //uint32_t size_proj;
  uint8_t last_power_down, auto_depth_timer = 0, refresh = 0;
  uint16_t  clock_timer = 0;
  enum buttons button;
  static uint8_t b_counter = 25;
  date_time_t calibration_date;
  GET_BITFIELD_ADDRESSES(); // assign addresses of unions of bitfield structs
  initSystem();             // starts the interrupts, prints the version
  GPS_EN_REG_Write ( 1 );
  COM_SELECT_Write ( 0 );
  
  Global_IE();
  last_power_down = NV_RAM_MEMBER_RD( OFF_MODE );      //check to see if auto shutdown or operator shutdown
  Features        =  NV_RAM_MEMBER_RD(FEATURE_SETTINGS);
  if ( Features.gps_on == TRUE )
  { 
    initGPS ();
  }
  
  // retrieve last display position
  gp_disp  =  NV_RAM_MEMBER_RD( GP_DISPLAY );
  splashScreen = TRUE;              // do the alkaline notification on bootup.
  SPEC_FLAG = 0;                    // auto turn off is disabled
 
  if ( last_power_down == MANUAL ) 
  {
    OFFSETS = FLAGS = 0;            // reinitialize all settings to "0" if operator shut down
    CONTROL = 0x02;                 // initialize control register, all 0 except shut_dwn  
  }
  else
  {                            // reinitialize settings to previous settings if auto shutdown
    Flags     = NV_RAM_MEMBER_RD( FLAG_SETTINGS  );      
    Flags.bat_volt = 0; //FALSE;
    Flags.drift_flag = FALSE;
    Flags.stat_flag = 0;
    Flags.stand_flag = 0;
    Flags.in_spec_cal = 0;
    Controls  = NV_RAM_MEMBER_RD( CONTROL_SETTINGS);
    Offsets   = NV_RAM_MEMBER_RD( OFFSET_SETTINGS );    
    Controls.LCD_light = 0;
  }
  Controls.shut_dwn = TRUE;
  cnt_time =  NV_RAM_MEMBER_RD( COUNT_TIME );   // check to see if memory is initialized
  KEYSCAN_DISABLE();                            // Keyscan disabled
 
  if ( RS232_CABLE_DETECT_Read() == 1)
  {        // check to see if serial cable is connected
    serial_port_text();                         //TEXT// display "    Serial Port\n      Activated"     
    wait_for_serial();
    UART2_Stop();
    isrUART2_Stop();
    
  }  
 
  // If this is a PLUS gauge, then enable comms with the PLUS baseboard
  if ( eepromData.gauge_type  == GAUGE_3440_PLUS  )
  {
   initUARTPlus();
   //Features.auto_depth = 1;
  }
  
  tst_depth_g = 0;
  calibration_date =  NV_RAM_MEMBER_RD  (Constants.CAL_DATE ); //read date of last calibration date from EEPROM 
  updateProjectInfo ( );
  GetLastStationName();
  CLEAR_DISP;
  KEYSCAN_ENABLE();                     // This enables the keypad task
  RequestBleVersion();                  // determines which packets the BLE module is expecting
  if ( last_power_down == MANUAL )
  {    // conduct a self test if there was a manual shutdown
    selftest ( );    
    read_RTC( &date_time_g );
    day_counter_g = decode_date( date_time_g ) - decode_date ( NV_RAM_MEMBER_RD (Constants.CAL_DATE) ); // gets days since last calibration
    if ( day_counter_g >= (365 -30) ) 
    {
        CLEAR_DISP;
        calibration_due ( );  //display calibration due text
        LCD_position (LINE3);
        printTimeDate( calibration_date );
        ESC_to_Exit (LINE4+4);  // display "ESC tp EXIT"  
        button = getKey(TIME_DELAY_MAX);  
    }
  }
  CheckBleVersion(); // try again, display error on fail
  Controls.buzz_enable = TRUE;
 
  update_valid_depths(); 
  // clock timer is set to read date immediatley
  clock_timer = 100;
  pulseBuzzer();
  
  checkAlkalines();
  
  //InitBleCharacteristics(); // sends BLE this gauges SN, allowing it to run and advertise

  while(1)  //main loop
  {    

   hold_buzzer();
    

	 if ( Features.auto_depth )
   {
    tst_depth_g = get_depth_auto(0);  
   }   

   LCD_position(LINE1);
   ready(cnt_time, 1);  //display <READY> Time

   LCD_position(LINE2);
    
   display_depth ( 0, tst_depth_g );  //display Depth:      
   display_offset_on ( Offsets );     // display OFFSET
   LCD_light_timer(5);
    
   if ( Controls.LCD_light )
   {
    KEY_B_LIGHT_ENABLE(); 
	 }

    refresh = 0;
    
    while(!refresh)
    {
      // enable the shut down timer
      // enable automatic timeout and shut down feature  
      g_turn_off_cnts = SIXTY_MINUTES * NV_RAM_MEMBER_RD (SHUT_DOWN_TIME_HOURS);       
      shutdown_timer = 0;    
      Spec_flags.auto_turn_off = TRUE;  

      global_special_key_flag = FALSE;
      
      while (  global_special_key_flag == FALSE )
      {  
        auto_depth_timer++;
        if(Features.auto_depth && (auto_depth_timer >= 18)) 
        {
          // display auto depth every 1 second
          tst_depth_g = get_depth_auto(LINE2);               
          auto_depth_timer = 0;             
        }
       
        // read clock every 10 seconds
        if ( ++clock_timer >= 100 )
        {
            read_RTC( &date_time_g ); 
            LCD_position(LINE3);
            printTimeDate ( date_time_g );
            clock_timer = 0;
         }
         
        
         // check for GPS satellite FIX
         if ( Features.gps_on == TRUE )
         {
          if( gps_fix == '0' )
          {
             LCD_position( LINE1);  
            _LCD_PRINT ( "N");    
      		} 
          else if ( gps_fix == '1' )
      	  {
            LCD_position( LINE1);  
            _LCD_PRINT ( "F");    
          }
          else
          {
            LCD_position( LINE1);  
            _LCD_PRINT ( "E");    
          }
         }
        
         // check for charging mode
         if(IS_CHARGER_DETECTED() )                          
         {
           LCD_position( LINE1 + 18);  
           shutdown_timer = 0;          
           _LCD_PRINT ( "C ");    
    		   LCD_PrintAtPosition( "Press START to Test", LINE4 ); 
         } 
          else
    	  {
        	// read battery every 5 s
        	if ( b_counter >= 50 )
        	{
          	static uint8_t flip = 0;
          	
          	// If running off of NICADS, 
          	if ( ( Controls.bat_sel == NICAD )  && ( checkNICADVoltageWarning () == TRUE ) )
          	{
          	 
              if ( flip == 0 )
              {
                LCD_PrintAtPosition( "  RECHARGE BATTERY  ", LINE4 );
              }
              else
              {
                LCD_PrintAtPosition( "Press START to Test", LINE4 );
              }  
      		  }
      	    else
      		  {
        		 LCD_PrintAtPosition( "Press START to Test", LINE4 );
        	
      		  }
     
      		  
      		  if ( Controls.bat_sel == ALK )  
          	{	
      		   if ( 0 == flip )
      		   { 
        		   LCD_PrintAtPosition( "ALK", LINE1 + 17);
        		   LCD_PrintAtPosition( "  RECHARGE BATTERY  ", LINE4 );
        		  
        		 }
        		 else
        		 {
        		   LCD_PrintAtPosition( "   ", LINE1 + 17);
        		   LCD_PrintAtPosition( "Press START to Test", LINE4 );
        		 }  
      		  }
      		  else
      		  {
      		      LCD_PrintAtPosition( "   ", LINE1 + 17);
      		  }
      		  
      		  flip ^= 0xFF;
      		  b_counter = 0;
      		
      		}       		  
    		  else
    		  {
    		    b_counter++;
    		  }
      		  
    	 	}
       CyDelay(100);
       Controls.buzz_enable = TRUE;       
       scan_keys();
      }   // loop for a key press       
      
      shutdown_timer = 0;    
      // enable the shut down timer
      // enable automatic timeout and shut down feature  
      g_turn_off_cnts = (uint32_t)SIXTY_MINUTES * (uint32_t)5; // non sleep mode is 66ms       
      shutdown_timer = 0;    
      Spec_flags.auto_turn_off = TRUE;  
     
      //button was pressed, determine what to do
      LCD_light_timer(30); 
      if(Controls.LCD_light)
	    {
        KEY_B_LIGHT_ENABLE(); 
	    }

      button = getLastKey();
      do
      {
          switch(button)
          {
            case PRINT:
                      print_menu(); 
                      refresh = TRUE;
                      break;      
            case PROJ:
                      project_menu(); 
                      refresh = TRUE;
                      break;      
            case MENU:
                      main_menu();   
                      refresh = TRUE;
                      break;
            case STAND:
                      stand_test();
                      refresh = TRUE;
                      break;
            case TIME:               
                      set_count_time();
                      refresh = TRUE;
                      break;
            case MAPR:
                      ma_pr();
                      refresh = TRUE;
                      break;
            case STORE:        
                      no_data_to_store_text( );
                      delay_ms ( 1500 );
                      refresh = TRUE;
                      break;
            case ENTER:
                   
                    #if AUTO_TEST // Auto test allows for automatic repeat and store of the readings
                     { uint8_t n;
                       for ( n=0; n < 100; n++ )
                       { 
                         Spec_flags.recall_flag = FALSE;
                         measureMoistureDensity(); 
                         if ( getLastKey() == ESC )
                         {
                          refresh = TRUE;
                          Flags.button_pressed = TRUE;
                          break;
                         }  
                       }
                     } 
                     if ( getLastKey() == ESC )
                     {
                      refresh = TRUE;
                      Flags.button_pressed = TRUE;
                      break;
                     }  
                     #else
                      while ( getLastKey() == ENTER)   //start another count if enter is pressed
                      { 
                         Spec_flags.recall_flag = FALSE;
                         if ( measureMoistureDensity( ) == 0 )
                         {
                           break;
                         }
                      }  
                     #endif 
                      refresh = TRUE;
                      Flags.button_pressed = TRUE;
                      break;
            
            default:  
                      delay_ms(10);
                      break;          
      
        }
      button = getLastKey();
    }while ( button == TIME || button == STAND || button == MAPR || button == MENU || button == PROJ || button == PRINT );
  	// set to read clock immediately
    clock_timer = 100;
    
  }  // end of main loop  
 }

}                  // end of Main