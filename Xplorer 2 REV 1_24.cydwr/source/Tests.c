


/**************************   ***************************************************************** ***************************/
//
// Title:       Explorer II
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
// Clock:	    
//
//
// History: date, version, personal initials with comments about any changes
//
/**********************************************************************************************************************/

/*********************************************  INCLUDE FILES  ***************************************************/

#include <project.h>
#include "Globals.h"
#include "DataStructs.h"
#include "Utilities.h"
#include "StoreFunctions.h"
#include "conversions.h"
#include "Alfat.h"
#include "prompts.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "Batteries.h"
#include "uarts.h"
#include <math.h>


/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/
extern void PulseCntStrt( float timeSec );
extern void resetPulseTimers ( void );
extern void stop_ONE_SHOT_Early ( void );
extern uint32 getHEPulseCounts ( void );
extern uint32 getGMPulseCounts ( void );

/************************************************  LOCAL DEFINITIONS  *************************************************/
#define DENSITY_PASS_PERCENT    .01
#define MOISTURE_PASS_PERCENT   .02

#define PRESCALE_7_5  2                   // prescale is 3.75s
#define PRESCALE_15   4                   // prescale is 3.75s
#define PRESCALE_30   PRESCALE_15 * 2
#define PRESCALE_60   PRESCALE_15 * 4
#define PRESCALE_240  PRESCALE_15 * 16

#define SMART_MC_CHI_COUNTS 32

#define STD_COUNT_DELAY 450

/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/
 int32_t stat_dense_avg1;
 
 int32_t stat_dense_avg;
 
 int32_t stat_moist_avg1;
 
 int32_t stat_moist_avg; 
 
 int32_t drift_dense_avg; 
 
 int32_t drift_moist_avg;

               
 uint32_t stat_dense[20];
 
 uint16_t stat_moist[20];
 
 uint32_t drift_dense[20];
 uint16_t drift_moist[20];

 fp32_t stat_dense_ratio1;
 fp32_t stat_dense_ratio;
 fp32_t stat_moist_ratio1;
 fp32_t stat_moist_ratio;
 
 uint8_t stat_pass_fail; 
 
 float drift_dense_per;
 float drift_moist_per;


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
 void storeDriftResultsToUSB ( Bool display_error )
 {
  char name_string[30];
  char temp_str[60];
  FILE_PARAMETERS fp;  
  
  AlfatStart();   
  
  isrTIMER_1_Disable();
  
  // If the drive can be mounted, data will be stored.
  
  if ( initialize_USB( display_error ) )
  {
    CLEAR_DISP;
    
    LCD_PrintAtPosition( "Writing Drift Result", LINE2);
    
    // make the file name string
    nullString(name_string, sizeof(name_string) );
    strcat( name_string, "DRIFT_RESULTS" );
    
    // open a file
    USB_open_file  ( name_string, &fp );  
    
    nullString(temp_str, sizeof(temp_str));
    sprintf( temp_str,"D%% Drift = %.2f %c\r\n", (double)drift_dense_per, (drift_dense_per <=0.5) ? 'P' : 'F');
    AlfatWriteStr(&fp,temp_str);
    //USB_append_file ( temp_str, strlen(temp_str) );
   
    nullString(temp_str, sizeof(temp_str));
    sprintf( temp_str,"Dens. Avg = %lu\r\n",(uint32)drift_dense_avg);
    AlfatWriteStr(&fp,temp_str);
    //USB_append_file ( temp_str, strlen(temp_str) );
    
    nullString(temp_str, sizeof(temp_str));
    sprintf ( temp_str,"M%% Drift = %.2f %c\r\n",(double)drift_moist_per, (drift_moist_per <= 1.0) ? 'P' : 'F');
    AlfatWriteStr(&fp,temp_str);
   
    nullString(temp_str, sizeof(temp_str));
    sprintf ( temp_str,"Moist.Avg = %lu\r\n",(uint32)drift_moist_avg);
    AlfatWriteStr(&fp,temp_str);
    
    nullString(temp_str, sizeof(temp_str));
    sprintf ( temp_str,"#1  D %lu M %u\r\n",(uint32) drift_dense[0],drift_moist[0]);
    AlfatWriteStr(&fp,temp_str);
    
    nullString(temp_str, sizeof(temp_str));
    sprintf ( temp_str,"#2  D %lu M %u\r\n", (uint32)drift_dense[1],drift_moist[1]);
    AlfatWriteStr(&fp,temp_str);
    
    nullString(temp_str, sizeof(temp_str));
    sprintf ( temp_str,"#3  D %lu M %u\r\n",(uint32) drift_dense[2],drift_moist[2]);
    AlfatWriteStr(&fp,temp_str);
   
    nullString(temp_str, sizeof(temp_str));
    sprintf ( temp_str,"#4  D %lu M %u\r\n", (uint32)drift_dense[3],drift_moist[3]);
    AlfatWriteStr(&fp,temp_str);
    
    nullString(temp_str, sizeof(temp_str));
    sprintf ( temp_str,"#5  D %lu M %u\r\n", (uint32)drift_dense[4],drift_moist[4]);
    AlfatWriteStr(&fp,temp_str);
   
    // close file     
     AlfatFlushData(fp.fileHandle);
     AlfatCloseFile(fp.fileHandle);  
  }   
  
  AlfatStop();   
  isrTIMER_1_Enable();
 
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
 void storeStdCountsToUSB ( Bool display_error )
 {
  char date_string[30];
  char name_string[30];
  char temp_str[60];
  
  uint8_t i, j, number_of_counts, index;
  uint16_t DSC,MSC;
  date_time_t TSC;
  FILE_PARAMETERS fp;  
  
  // Get the number of STD counts stored in memory
  index = NV_RAM_MEMBER_RD(stand_test.std_index); // index is always 1-30
  index -= 1;  //match index to array element number
  number_of_counts = NV_RAM_MEMBER_RD(stand_test.std_counts);   
 
  if(number_of_counts != 0)
  {        
    
    AlfatStart();   
    
     isrTIMER_1_Disable();
    
    // If the drive can be mounted, data will be stored.
    
    if ( initialize_USB( display_error ) )
    {
      CLEAR_DISP;
      
      LCD_PrintAtPosition( "Writing STD Counts", LINE2);
      
      // make the file name string
      nullString(name_string, sizeof(name_string) );
      strcat( name_string, "STD_COUNTS" );
      
      // open a file
      USB_open_file  ( name_string , &fp);  
    
      i = index;
      j = 0;
      
      nullString(temp_str, sizeof(temp_str));
      sprintf( temp_str, "Density\tMoisture\tDate\t\r\n" );
      AlfatWriteStr(&fp,temp_str);
     do   //read all valid counts from memory
     {
      DSC= NV_RAM_MEMBER_RD(stand_test.dense_count_x[i] );  //each count is 2 bytes long
      MSC = NV_RAM_MEMBER_RD(stand_test.moist_count_x[i] );  //each count is 2 bytes long    
      TSC = NV_RAM_MEMBER_RD(stand_test.date_count_x[i]  );   //each date is 4 bytes long
     
      getTimeDateStr ( TSC, date_string );     
      nullString(temp_str, sizeof(temp_str));
      sprintf( temp_str, "%u\t%u\t%s\r\n", DSC,MSC,date_string );
      AlfatWriteStr(&fp,temp_str);
      
      i = (i!=0) ? (i-1) : 29;
      j++;
    
     } while((i!=index) && (j<number_of_counts));  //index is array element with most recent data      
   
    
    // close file     
     AlfatFlushData(fp.fileHandle);
     AlfatCloseFile(fp.fileHandle);    
    }   
   AlfatStop();   
   
    isrTIMER_1_Enable();
 
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
 void storeStatTestResultsToUSB ( Bool display_error )
 {
  char date_string[30];
  char name_string[30];
  char temp_str[30];
  FILE_PARAMETERS fp;  
  
  uint8_t n;
  

  
  AlfatStart();   
  
  // If the drive can be mounted, stat test data will be stored.
  if ( initialize_USB( display_error ) )
  {
     CLEAR_DISP;
    
     LCD_PrintAtPosition( "Writing Stat Results", LINE2);
    // make the file name string
    nullString(name_string, sizeof(name_string) );
    strcat( name_string, "StatResults_" );
    
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
    USB_open_file  ( name_string, &fp );  
  
    // make header
    // get the current time, and turn into a string
    read_RTC( &date_time_g );
    getTimeDateStr ( date_time_g, date_string );
    sprintf( temp_str, "%s\t\r\n", date_string );
    AlfatWriteStr(&fp,temp_str);
  
    nullString(temp_str, sizeof(temp_str));
    sprintf( temp_str, "Density Counts\t\r\n" );
    AlfatWriteStr(&fp,temp_str);
  
    for ( n = 0; n < 20; n++ )
    {
      
      LCD_position(LINE3);
      _LCD_PRINTF ( "Writing DC %4u    ", n);
      nullString(temp_str, sizeof(temp_str));
      sprintf( temp_str, "%ld\t\r\n", (uint32)stat_dense[n] );
      AlfatWriteStr(&fp,temp_str);
    }  

    nullString(temp_str, sizeof(temp_str));
    sprintf( temp_str, "Density Average\t %lu\t\r\n", (unsigned long)stat_dense_avg );
    AlfatWriteStr(&fp,temp_str);
    
    nullString(temp_str, sizeof(temp_str));
    sprintf( temp_str, "Density Ratio\t %.2f\t\r\n", stat_dense_ratio );
    AlfatWriteStr(&fp,temp_str);



    nullString(temp_str, sizeof(temp_str));
    sprintf( temp_str, "\t\r\n Moisture Counts\t\r\n" );
    AlfatWriteStr(&fp,temp_str);

    for ( n = 0; n < 20; n++ )
    {
      LCD_position(LINE3);
      _LCD_PRINTF ( "Writing MC %4u    ", n);
      nullString(temp_str, sizeof(temp_str));
      sprintf( temp_str, "%u\t\r\n", stat_moist[n] );
      AlfatWriteStr(&fp,temp_str);
    }
  
    nullString(temp_str, sizeof(temp_str));
    sprintf( temp_str, "Moisture Average\t %u\t\r\n", (uint16_t)stat_moist_avg );
    AlfatWriteStr(&fp,temp_str);
    
    nullString(temp_str, sizeof(temp_str));
    sprintf( temp_str, "Moisture Ratio\t %.2f\t\r\n", stat_moist_ratio );
    AlfatWriteStr(&fp,temp_str);
    // close file     
     AlfatFlushData(fp.fileHandle);
     AlfatCloseFile(fp.fileHandle);   
  }   
 AlfatStop();   
 
  ADC_DelSig_Start();
  ADC_SAR_Start();
  UART_GPS_Start();

  isrTIMER_1_Enable();
 }
 
/******************************************************************************
 *
 *  Name: measurePulses ( )
 *
 *  PARAMETERS: time is secs
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: 
 *
 *****************************************************************************/ 

void measurePulses ( uint8_t line, uint8_t time1, uint16_t * moisture_count, uint32_t * density_count,uint8 depth )  // acquires moisture and density count during tests 
{
  uint16_t  i = 0;
  uint8_t  LCD_line;  //line3+11   
  uint32_t density_temp, moisture_temp,div_by; 

  uint8_t batt_flag;
  
  // Save the state of the auto turn off flag
  BOOL temp_auto_turn_off = Spec_flags.auto_turn_off;
  
  LCD_line = line + 11;
  
   
  enum buttons button = DFLT;

  
  Controls.reset_count = FALSE;                            // clear skip over code bit
  
  if ( Flags.bat_volt == 1 )
  {
    batt_flag = 1;
  }
  else
  {
    batt_flag = 0;
  }  
  
  Flags.bat_volt = 1;   // Don't do background battery monitoring
  
  if(!Spec_flags.self_test)
  {
   GM_TUBE_ENABLE();                    // enable 2nd GM tube
  }   
  
  delay_ms(10);
 
  // check for gauge temperature on the Xplorer2. 3440 does not have temperature sensor.
  if ((Features.dummy_mode == FALSE) && ((eepromData.gauge_type == GAUGE_3500 ) || (eepromData.gauge_type == GAUGE_3440_PLUS) ))
  {
    check_temp(0);                                     // check internal temperature of gauge
  }
  
  Spec_flags.auto_turn_off = FALSE;                  // disable auto_turn_off during count
  
  // Set the prescale value
  switch ( time1 )
  {
     case 5:
     case 15: div_by = PRESCALE_15;
              break;
     case 30: div_by = PRESCALE_30;
              break;
     case 60: div_by = PRESCALE_60;
              break;
     case 240:div_by = PRESCALE_240;
              break;
     default: div_by = PRESCALE_60;
             break;
  } 
  
  
  // Check to see if in self test mode  
  // Display all the optional flags and current settings  
  if ( !Spec_flags.self_test )
  {
    LCD_position(LINE1);
   
    if(Flags.stand_flag)
    {
      
      std_text();                                     //TEXT// display "Standard Count"      
    }    
    else if(Flags.stat_flag)
    {
        stat_text();                                    //TEXT// display "Stat Test"     
    }    
    else if(Flags.drift_flag)
    {
        drift_text();                                   //TEXT// display "Drift Test"      
    }    
    else if( Spec_flags.spec_cal_flag && (NV_RAM_MEMBER_RD(SPECIALCAL_DEPTH) == depth_setting))
    {
      
      if( Features.auto_store_on  )
      {
       LCD_position(LINE1);
       current_project_text( project_info.current_project );  
       LCD_position(LINE2);
       display_station_name(project_info.current_station_name);         //TEXT// display "Station: %s",current_station               
      }
      CyDelay(1500);
      LCD_position(LINE1);
      spec_text();
      //TEXT// display "Special Calibration"      
    }  
    else if (Spec_flags.nomograph_flag )
    {       
      if( Features.auto_store_on  )
      {
       LCD_position(LINE1);
       current_project_text( project_info.current_project );  
       LCD_position(LINE2);
       display_station_name(project_info.current_station_name);         //TEXT// display "Station: %s",current_station               
      }
      CyDelay(1500);
      LCD_position(LINE1);
      nomograph_text();                                //TEXT// display " Nomograph"      
    }
    else if ( Spec_flags.trench_offset_flag )
    {
    trench_text();
    
    }  
    else //general purpose test
    {
     
      
      if( (Features.auto_store_on) && (Flags.in_spec_cal == FALSE ) )
      {
       LCD_position(LINE1);
       current_project_text( project_info.current_project );  
       LCD_position(LINE2);
       display_station_name(project_info.current_station_name);         //TEXT// display "Station: %s",current_station               
      }
      else   // Display Time 
      {
       LCD_position(LINE1);
       read_RTC( &date_time_g ); 
       printTimeDate ( date_time_g );
      }
      
     
    }
    if ( Flags.in_spec_cal == FALSE )
    {
      LCD_position(LINE3);
      display_depth(1,tst_depth_g);                     //TEXT// display "Depth: %u",tst_depth_g     
    }
    display_time(time1,line );
    
    //TEXT// display "Time: %3u sec. " on LCD_LINE   
  
    if ( Offsets.den_offset_pos || Offsets.moist_offset_pos || Offsets.tren_offset_pos )
      { 
        display_offset( Offsets );                               //TEXT// display "Offset:"  on LINE4      
      }
      

  } // end of display settings 

  // don't wait for key release in self-test mode
  if ( time1 != 5 || depth != 0 )
  {
   wait_for_key_release();       
  }

  
  
  
  if ( !Features.language_f )      // if in spanish mode
  {
    LCD_line +=2;       // LINE3+8
  }    

  Controls.update_time = FALSE;
  LCD_timer = 0;  
 
  resetPulseTimers ( );
  PulseCntStrt( time1 );
  
  i = 0;
  while ( checkCountDone() == FALSE )
  {       
    uint32 timer;
    CyDelay ( 250 );
    i++;      

      if( !Spec_flags.self_test )
      {
        if ( i==40 )                  //turn off backlight after 10 seconds if it's on
      	{
          KEY_B_LIGHT_DISABLE ();
  	    };
  	
        button = getLastKey();
        if ( (!Flags.stat_flag && (button == ENTER) ) || (button == ESC) )          
        {
          if(button == ENTER)               // ENTER was pressed, reset count
          {
            Controls.reset_count = TRUE;    // set skip over code bit         
          }    
          break;
        }
        if ( checkCountDone() == FALSE ) 
        {
          // The one shot counts at 1000HZ.
          timer = ONE_SHOT_TIMER_ReadCounter (); 
          timer /= PULSETIMERCLK;
          LCD_position (LCD_line);
          _LCD_PRINTF ( "%3u", time1 - (uint8_t)timer);
        }  
       }
   }                               
                                   

  if ( checkCountDone() == TRUE )  //count completed 
  {
    density_temp  = getGMPulseCounts (); 
    moisture_temp = getHEPulseCounts (); 

    
    *density_count   = (uint32_t)(density_temp/div_by);
    *moisture_count  = (uint16_t)(moisture_temp/div_by);   
    
    if ( !Spec_flags.self_test )
    {
     NV_MEMBER_STORE(M_CNT_AVG, *moisture_count);
     NV_MEMBER_STORE(D_CNT_AVG, *density_count);
    }   
  }
  
  if ( button != ENTER )               // either test ended or, esc was pressed to exit
  {           
    stop_ONE_SHOT_Early();
    
  }  
 
  if ( batt_flag == 1 )
  {
    Flags.bat_volt = 1;
  }
  else
  {
    Flags.bat_volt = 0;
  }
  
  Spec_flags.auto_turn_off = temp_auto_turn_off;
  shutdown_timer = 0;    
}


/******************************************************************************
 *
 *  Name: clearPulseTimer
 *
 *  PARAMETERS: Reset, then start the pulse timer, pulse counters
 *                
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: 
 *
 *****************************************************************************/

void clearAndStartPulseTimer ( void )
{

  Global_ID();   
  
  LCD_timer = 0;  
      
 
  TIMER_1_WriteCounter ( 0 );
  isrTIMER_1_ClearPending();
  TIMER_1_Start();


  Global_IE();                // turn on all enabled interrupts 

  
  Controls.update_time = FALSE;
 
}

/******************************************************************************
 *
 *  Name: measurePulses ( )
 *
 *  PARAMETERS: time is secs
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: 
 *
 *****************************************************************************/ 

void measurePulsesStandardCount ( uint8_t line, uint32_t * moisture_count, uint32_t * density_count )  // acquires moisture and density count during tests 
{
  uint8_t  n, LCD_line, sec;  //line3+11   
  uint8_t batt_flag;
  uint32 timer, i;
  float elapsed_time;
  // Save the state of the auto turn off flag
  BOOL temp_auto_turn_off = Spec_flags.auto_turn_off;
  enum buttons button = DFLT;

  // Where to display time
  LCD_line = line + 11;
  
  // don't check battery state
  if ( Flags.bat_volt == 1 )
  {
    batt_flag = 1;
  }
  else
  {
    batt_flag = 0;
  }  
  Flags.bat_volt = 1;   // Don't do background battery monitoring
  
  
  delay_ms(10);
 
  Spec_flags.auto_turn_off = FALSE;                  // disable auto_turn_off during count
  
  // When key is released, stop running the keyscanner
  wait_for_key_release();  
    
  LCD_PrintAtPositionCentered ( "Standard Count",LINE2+10 );
 //TEXT// display "Time: %3u sec. " on LCD_LINE   
  
  display_time(240,line );
  
  // get the start of the test
  timer = msTimer;   
  for ( n = 0; n < SMART_MC_CHI_COUNTS; n++ ) // take 32, 7.5 second counts
  {  
  
   elapsed_time = (n * 15)/2;
  
   LCD_position (LCD_line);
   _LCD_PRINTF ( "%3u ", (uint8)(240.0 - elapsed_time));
 
  resetPulseTimers ( );
  PulseCntStrt( 7.5 ) ;
  
  i = msTimer + 1000 ;// add one second to time
  sec = 0;
  while ( checkCountDone() == FALSE )
  {       
     CyDelay ( 50 );
     
     if ( timer + 10000 >  msTimer )
     {
       KEY_B_LIGHT_DISABLE();
		 }; 
    
     // Keyscan is disabled, so check keys here
     button = getLastKey();
     if ( button == ESC )          
     {
       break;
     }
    
     // update every 1 second
     if ( i < msTimer ) 
     {
      LCD_position (LCD_line);
      _LCD_PRINTF ( "%3u", 240 - (uint8) (elapsed_time + ++sec));
      i = msTimer + 1000; // add one second to time
    } // end of LCD update and keyscan
      
   } // 7.5 sec count ended
   if ( button == ESC )          
   {
    stop_ONE_SHOT_Early(); 
    break;
   }// exit for() loop
   
    // read in the counts
    density_count[n]  = getGMPulseCounts()/PRESCALE_7_5;            
    moisture_count[n] = getHEPulseCounts()/PRESCALE_7_5;            
 } // finished 32 counts                                
  
    
 if ( batt_flag == 1 )
 {
   Flags.bat_volt = 1;
 }
 else
 {
   Flags.bat_volt = 0;
 }
  
 Spec_flags.auto_turn_off = temp_auto_turn_off;
 shutdown_timer = 0;    
}


#define STAND_TEST 0
/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: 
 *
 *****************************************************************************/ 

/////TEST FUNCTIONS/////////////////////////////////////////////////////////////
void stand_test(void)  // leads user through standard count procedure (STD button initiates)
{
  int8_t tests = 0, tests_tot = 0, index,n;
  uint16_t moisture_count[4],moisture_cnt ;
  uint32_t density_cnt,density_count[4],density[SMART_MC_CHI_COUNTS],moisture[SMART_MC_CHI_COUNTS],moisture_mean,density_mean;
  int32_t date_int;
  
  float percent_diff_dense, percent_diff_moist, avg_std_moist, avg_std_dense, d_stand_cal;
  char moist_pass_fail[10] , dense_pass_fail[10];
  
  enum buttons button;
  uint16_t d_stand;
  uint16_t m_stand;
  float temp, moist_sd,density_sd ;

  static uint8 dummy_tests = 0;

  sprintf( moist_pass_fail,"PASS" );
  sprintf( dense_pass_fail,"PASS" );
 
  Flags.stand_flag = TRUE;                           // set "Stand" flag for display format during measuring
  Global_IE();

  Controls.shut_dwn = FALSE;                         // disable shut down feature when NO is pressed 

  d_stand = NV_RAM_MEMBER_RD(DEN_STAND);             //read the last stored density stand from eeprom 

  m_stand = NV_RAM_MEMBER_RD(MOIST_STAND);           //read the last stored moisture stand from eeprom   

  CLEAR_DISP;
  LCD_position(LINE1);
  if(d_stand!=1)
  {
    _LCD_PRINTF ( "DS = %u",d_stand);  
  }  
  else 
  {
    _LCD_PRINT ( "DS = 0");
  }  
  
  LCD_position(LINE2);
  _LCD_PRINTF ( "MS = %u",m_stand);  //0x25 is '%' symbol 
  new_std_count();         // display "New STD Count?" on LINE3
  Press_YES_or_NO(LINE4);  // display "Press YES or NO"  
  
  while(1)
  {
    button = getKey ( TIME_DELAY_MAX );
    
    if((button == NO) || (button == ESC))                                // NO or ESC was pressed, exit to ready screen
    {
      break;
    }  
    else if(button == YES)
    {
      
      std_count_press_start();  // display "  Press START for\n  Standard Count" 
      while(1)
      {
        button = getKey ( TIME_DELAY_MAX );
        if(button == ESC || button == ENTER)
        {
          break;
        }  
      }
      
      // Exit to ready screen if not at SAFE position (2" for strata )
      if(Features.auto_depth)                // set depth if in auto mode
      {
       tst_depth_g = get_depth_auto( 0 );   // update depth     
  
       if ( tst_depth_g != 0 )              // test for SAFE position
       {
        not_in_safe_text();                 // TEXT: "NOT IN SAFE POSITION
        delay_ms(1000);
        break;
       }
          
      }
      
      button = getLastKey();
      if(button == ESC)                                 // ESC was pressed, exit to ready screen
      {
        break;
      }  
      else if  (button == ENTER) 
      {
        if  (  ( NO_ERROR == alfat_error ) )
        {
          AlfatStart();   
          read_RTC ( &date_time_g ); //get time and date, date_unformatted now contains coded date value
          AlfatStop();   
        }  
        
        Controls.shut_dwn = TRUE;                         // enable shut down feature when NO is pressed
        if(Controls.LCD_light)                            // ENTER was pressed, continue with test
		    {
          KEY_B_LIGHT_DISABLE()  ;               // turn off keyboard backlight    
		    };


        CLEAR_DISP;
        measurePulsesStandardCount(LINE4, &moisture[0], &density[0]);  // 4 minute test, 240 sec
       
       if ( button == ESC )          
       {
        break;
       }
        moisture_mean = 0;
        density_mean  = 0;
        // get the average of the counts
        for ( n =0; n < SMART_MC_CHI_COUNTS; n++ )
        {
         moisture_mean += moisture[n];
         density_mean += density[n];
        } 
        moisture_mean /= SMART_MC_CHI_COUNTS;    // unprescaled count
        density_mean  /= SMART_MC_CHI_COUNTS;    // unprescaled count

        
        // get the SD of the counts
        moist_sd = density_sd = 0;
        for ( n =0; n < SMART_MC_CHI_COUNTS; n++ )
        {
          temp = (float)(moisture_mean) - (float)moisture[n];
          moist_sd += (temp*temp);
          temp = (float)(density_mean) - (float)density[n];
          density_sd += (temp*temp);
        } 
        moist_sd /= (SMART_MC_CHI_COUNTS - 1);
        density_sd /= (SMART_MC_CHI_COUNTS - 1);

        moist_sd = sqrtf(moist_sd);
        density_sd = sqrtf(density_sd);


        density_cnt  =  density_mean ;
        moisture_cnt =  moisture_mean ;
 

        if ( Features.dummy_mode == TRUE )
        { 
         moisture_cnt = 1048 + (++dummy_tests * 5 );
         density_cnt = 3939 + (++dummy_tests * 5 ) ;
        }


        
        button = getLastKey();
        
        if(button == ESC)                                 // ESC was pressed, exit to main screen
        {
          break;
        }  // break out of while loop
     
         //beep at end of test, use delay to store station data in secret
          if ( Features.sound_on )
          {
            { 
             uint8_t i;
             for( i = 0; i<5; i++ )
             {
              output_high(BUZZER);  
              delay_ms(500);
              output_low(BUZZER);  
              delay_ms(500);
             }
            }
          } 

        //COUNT COMPLETED WITHOUT INTERRUPTION//    
        Controls.shut_dwn = FALSE;                        // disable shut down feature when NO is pressed
        if(Features.avg_std_mode == TRUE)                 // this mode will compare the new standards to a rolling average of the last four valid
        {                                                 // measurements and display the % error.
          density_count[0] = NV_RAM_MEMBER_RD(stand_test.dense_count_1);
          density_count[1] = NV_RAM_MEMBER_RD(stand_test.dense_count_2);
          density_count[2] = NV_RAM_MEMBER_RD(stand_test.dense_count_3);
          
          moisture_count[0] = NV_RAM_MEMBER_RD(stand_test.moist_count_1);
          moisture_count[1] = NV_RAM_MEMBER_RD(stand_test.moist_count_2);
          moisture_count[2] = NV_RAM_MEMBER_RD(stand_test.moist_count_3);
          
          tests = NV_RAM_MEMBER_RD(stand_test.std_number);             //get number of tests completed
          
          if (tests == 2 )
          {
           // average the last two "stored" readings
            avg_std_dense = ((float)density_count[0] + (float)d_stand)/2.0;
            avg_std_moist = ((float)moisture_count[0] + (float)m_stand)/2.0;
          }
          else if (tests == 3 )
          {
           // average the last three "stored" readings
            avg_std_dense = ((float)density_count[0] + (float)density_count[1] + (float)d_stand)/3.0;
            avg_std_moist = ((float)moisture_count[0] + (float)moisture_count[1] + (float)m_stand)/3.0;
          }
          else if ( tests == 4 )
          {
            // average the last four "stored" readings
            avg_std_dense = ((float)density_count[0] + (float)density_count[1] + (float)density_count[2] + (float)d_stand)/4.0;
            avg_std_moist = ((float)moisture_count[0] + (float)moisture_count[1] + (float)moisture_count[2] + (float)m_stand)/4.0;
          }
          else // test = 1 or 0
          {
            avg_std_dense = (float)density_cnt;
            avg_std_moist = (float)moisture_cnt;
          }
      
        }
        else // Not in averaging mode
        {
          d_stand_cal   =  (float)NV_RAM_MEMBER_RD(DENSE_CAL_STD); //read calibrated standard counts from memory
          avg_std_moist =  (float)NV_RAM_MEMBER_RD(MOIST_CAL_STD);
               
          // gets days since last calibration
  
          date_int = decode_date( date_time_g ) - decode_date ( NV_RAM_MEMBER_RD (Constants.CAL_DATE) );  
  
          temp = (-0.693/(30.0 * 365.25)) * (float)date_int;         
          temp = pow ( 2.71828183, temp );
          avg_std_dense = d_stand_cal * temp;
        }          
        percent_diff_dense = ((float)density_cnt - avg_std_dense) / avg_std_dense;       //calculate % difference in density cnt from rolling avg.
        if(fabsf(percent_diff_dense) > DENSITY_PASS_PERCENT)
        {
          sprintf ( dense_pass_fail, "FAIL" );
        }  
        percent_diff_moist = ((float)moisture_cnt - avg_std_moist) / avg_std_moist;      //calculate % difference in moist cnt from rolling avg.
        if(fabsf(percent_diff_moist) > MOISTURE_PASS_PERCENT)
        {
          sprintf(moist_pass_fail, "FAIL");
        }  
        CLEAR_DISP;

        LCD_position(LINE1);        
        sprintf ( lcdstr, "DS=%ld %.1f%c %s",(uint32)density_cnt,(double)(percent_diff_dense*100.0),0x25,dense_pass_fail);   
        LCD_print ( lcdstr );
         
        LCD_position(LINE2);
        sprintf ( lcdstr,"MS=%u %.1f%c %s",moisture_cnt,(double)(percent_diff_moist*100.0),0x25,moist_pass_fail);  //0x25 is '%' symbol 
        LCD_print ( lcdstr );
        
        use_new_std_count();     // display "Use New STD Count?"
        Press_YES_or_NO(LINE4);  // display "Press YES or NO"         
        while(1)
        {
         button = getKey ( TIME_DELAY_MAX );
         
          if((button == NO) || (button == YES) || (button == ESC))
          {
            break;
          }  
        }           
      
        
        if((button == NO) || (button == ESC))
        {
          break;
        }  
        else if(button == YES)
        {
          index = NV_RAM_MEMBER_RD(stand_test.std_index); //read std index position
          index %= 30;          
          
          //save dense std to buffer
          NV_MEMBER_STORE(stand_test.dense_count_x[index],  (uint16_t)density_cnt) ;
                    
          // save moist std to buffer
          NV_MEMBER_STORE(stand_test.moist_count_x[index],  moisture_cnt) ;
          
          //save std date to buffer
          NV_MEMBER_STORE(stand_test.date_count_x[index],  date_time_g ) ;
          
          
          NV_MEMBER_STORE(stand_test.std_index,++index);
          
          tests_tot = NV_RAM_MEMBER_RD(stand_test.std_counts);
          
          if(++tests_tot <= 30)    // write number of tests up to 30
          {
             NV_MEMBER_STORE(stand_test.std_counts,tests_tot);
          }  
          
          // store the last 4 minute count as the standard count
          NV_MEMBER_STORE(DEN_STAND, (uint16_t)density_cnt);
          NV_MEMBER_STORE(MOIST_STAND, moisture_cnt);
          
          if(Features.avg_std_mode)
          {
            NV_MEMBER_STORE(stand_test.dense_count_3, density_count[1] );       //shift values and write to eeprom
            NV_MEMBER_STORE(stand_test.dense_count_2, density_count[0]);       //shift values and write to eeprom
            NV_MEMBER_STORE(stand_test.dense_count_1, d_stand);             
            
            NV_MEMBER_STORE(stand_test.moist_count_3,moisture_count[1]);
            NV_MEMBER_STORE(stand_test.moist_count_2,moisture_count[0]);
            NV_MEMBER_STORE(stand_test.moist_count_1,m_stand);             
            
            if(++tests < 5)
            {
             NV_MEMBER_STORE(stand_test.std_number,tests);         // store number of tests in eeprom until 4th test   
            }  
          }
          break;
        }
      }        
    }   
  }  
  Flags.stand_flag = FALSE;
  Controls.shut_dwn = TRUE; 
}


/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Take 20, 60 sec readings.
 *               AVG = (SUM of 20)/20
 *               Theoretical_SD = sqrt(AVG)
 *               SD = SQRT ( SUM[reading(i) - avg]^2  / 19 )
 *               Ratio = SD/Theo
 *               
 *            
 *               Ideal Ratio = 1. With Prescale, ideal ratio = Sqrt(ps)/ps
 * 
 * 
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

uint8_t stat_test(void)  // leads user through static test procedure
{  
  Bool     auto_scroll_advance = 0;
  int8_t   display_set = 0,  pass_fail = 0, i;
  int32_t  mc_sub  = 0,     dc_sub = 0, delta = 0,loop_cnt ;
  int32_t  dc_sum = 0,      mc_sum = 0;  
  float    dc_std = 0,     dc_theo = 0, mc_std = 0, mc_theo = 0;
  char pass = 'P', fail = 'F';
  enum buttons button;

  uint16_t moisture_cnt = 0;
  uint32_t density_cnt = 0;
  
  float test_ratio_upper, test_ratio_lower;

  // Save the state of hte auto turn off flag
  BOOL temp_auto_turn_off = Spec_flags.auto_turn_off;

  Spec_flags.auto_turn_off = FALSE;                  // disable auto_turn_off during count

   test_ratio_upper = RATIO_UPPER; 
   test_ratio_lower = RATIO_LOWER;
  
  while(1)
  {
    if(!Flags.diag)
    {
      press_start(0);  // display "  Press START for\n  20m. STAT TEST" on LINE2 and LINE3      
      while(1)
      {
       button = getKey(TIME_DELAY_MAX);
        
        if((button == ENTER) || (button == ESC) || (button == MENU))
        {
          break;
        }  
      }    
      
      if(button != ENTER)                        // ESC was pressed, exit to menu
      {
        break;  
      }  
    }    
   
    Flags.stat_flag = TRUE;                               // set stat test flag for LCD display during pulse counting      
   
    if(Controls.LCD_light)
  	{
      KEY_B_LIGHT_DISABLE()    ;                        // turn off keyboard backlight    
  	};

    reading();  // display "Reading # " on LINE2
    for(i=0; i<20; i++)                             // loop for taking (20) 1 minute counts
    {      
      LCD_position(LINE2+10);
      _LCD_PRINTF ( "%d",(i+1));


      if ( Features.dummy_mode == TRUE )
      {
        measurePulses (LINE3, 1, &moisture_cnt, &density_cnt, tst_depth_g );
        moisture_cnt = 500;
        density_cnt = 1000;
      }
            
      else      
      {
        measurePulses (LINE3, 60, &moisture_cnt, &density_cnt, tst_depth_g );
      }  
            
      if( getLastKey() == ESC)                             // ESC was pressed, exit to menu
      {
         break;
      }   
      stat_dense[i] = density_cnt;
      stat_moist[i] = moisture_cnt;
      dc_sum += (int32_t)density_cnt;  
      mc_sum += (int32_t)moisture_cnt; 
    }
    
    if(Controls.LCD_light)    
    {
      LCD_timeout = 70;  //7 seconds    
    }
    
    if ( getLastKey() == ESC )                             // ESC was pressed, exit to menu
    { 
      break;     
    }  
    
    stat_dense_avg = dc_sum / 20;
    stat_moist_avg = mc_sum / 20;
    dc_theo = sqrt((float)stat_dense_avg );
    mc_theo = sqrt((float)stat_moist_avg );
        
    for(i=0; i<20; i++)
    {        
      delta = (int32_t)stat_dense[i] - stat_dense_avg;  //+-
      dc_sub += (delta * delta); //+
      delta = (int32_t)stat_moist[i] - stat_moist_avg; //+-
      mc_sub += (delta * delta); //+
    }
    dc_std = sqrt((float)dc_sub / 19.0); //+
    mc_std = sqrt((float)mc_sub / 19.0); //+
    stat_dense_ratio = dc_std / (dc_theo ); //+
    stat_moist_ratio = mc_std / (mc_theo);  //+
    
    NV_MEMBER_STORE(D_CNT_STD, (uint16_t)stat_dense_avg);            //store values for L% calc in drift test
    NV_MEMBER_STORE(M_CNT_STD, (uint16_t)stat_moist_avg);
	
  	NV_MEMBER_STORE(st_dense_avg1,(uint16_t)stat_dense_avg);
	  NV_MEMBER_STORE(st_moist_avg1,(uint16_t)stat_moist_avg);
	
////routine for diagnostic self test...store counts,avg,ratio for review, determine pass/fail    
    if(Flags.diag)
    {
      if(!test_number)  //first stat test
      {        
        //write results to EEPROM
        NV_MEMBER_STORE(st_dense_avg1,stat_dense_avg);
        NV_MEMBER_STORE(st_moist_avg1,stat_moist_avg);
        NV_MEMBER_STORE(st_dense_ratio1,stat_dense_ratio);
        NV_MEMBER_STORE(st_moist_ratio1,stat_moist_ratio);       
        stat_dense_avg1 = stat_dense_avg;
        stat_moist_avg1 = stat_moist_avg;
        stat_dense_ratio1 = stat_dense_ratio;
        stat_moist_ratio1 = stat_moist_ratio;
      }       
      else  //write results of 2nd test to EEPROM
      {
       NV_MEMBER_STORE(st_dense_avg,stat_dense_avg);
       NV_MEMBER_STORE(st_moist_avg,stat_moist_avg);
       NV_MEMBER_STORE(st_dense_ratio,stat_dense_ratio);
       NV_MEMBER_STORE(st_moist_ratio,stat_moist_ratio);
      }      
      

      if(stat_dense_ratio < test_ratio_lower || stat_dense_ratio > test_ratio_upper || stat_moist_ratio < test_ratio_lower || stat_moist_ratio > test_ratio_upper)
      {
        pass_fail = 1;        
      }  


      break;
    }
////End of self diagnostic test portion/////////////////////////////////////////    
//DISPLAY DATA//DISPLAY DATA//DISPLAY DATA//DISPLAY DATA//DISPLAY DATA//DISPLAY DATA//      
   
    // If a USB drive is installed, the drive will be mounted and stat test data will be stored.
    //storeStatTestResultsToUSB(FALSE);
    
    
    while(1)                                   //remains in display mode until button is pressed
    {
      CLEAR_DISP;
      if(display_set==0)
      {
        LCD_position(LINE1);
        stat_drift_count_text(4); //TEXT// display "Dense R = "
        checkFloatLimits ( & stat_dense_ratio );
        
        if ( (stat_dense_ratio >= test_ratio_lower) && (stat_dense_ratio <= test_ratio_upper ) )
        {
          sprintf ( lcdstr,"%4.2f P   ",stat_dense_ratio );
        }
        else
        {  
          sprintf ( lcdstr,"%4.2f F   ",stat_dense_ratio );
        }  
                
        LCD_print ( lcdstr );
        
               
        LCD_position(LINE2);
        stat_drift_count_text(1); //TEXT// display "Dense.Avg = "
        _LCD_PRINTF ( "%lu", (uint32)stat_dense_avg);
        
        LCD_position(LINE3);
        stat_drift_count_text(5); //TEXT// display "Moist R = "
        sprintf ( lcdstr,"%4.2f %c", (double)stat_moist_ratio,(stat_moist_ratio >= test_ratio_lower && stat_moist_ratio <= test_ratio_upper) ? pass : fail);
        LCD_print ( lcdstr );
        
        LCD_position(LINE4);
        stat_drift_count_text(3); //TEXT// display "Moist.Avg = "
        _LCD_PRINTF ( "%lu", (uint32)stat_moist_avg);

      }
      else
      {
        i = display_set*4 - 4;
        LCD_position(LINE1);
        sprintf ( lcdstr,"#%u D %lu M %u", i+1, (uint32)stat_dense[i], stat_moist[i] );
        LCD_print ( lcdstr );
        i++;
        
        LCD_position(LINE2);
        sprintf ( lcdstr,"#%u D %lu M %u", i+1,(uint32)stat_dense[i],stat_moist[i]);       
        LCD_print ( lcdstr );
        i++;
        
        LCD_position(LINE3);
        sprintf ( lcdstr,"#%u D %lu M %u", i+1,(uint32)stat_dense[i],stat_moist[i]);
        LCD_print ( lcdstr );
        i++;
        
        LCD_position(LINE4);
        sprintf ( lcdstr,"#%u D %lu M %u", i+1,(uint32)stat_dense[i],stat_moist[i]);
        LCD_print ( lcdstr );
      }             
      auto_scroll_advance = FALSE;     
      if(Features.auto_scroll)
      {
        loop_cnt = 0;
                
        while( 1 )
        {
          button = getKey ( 10 );
                    
          if((button == ESC) || (button == UP) || (button == DOWN) || (button == STORE) || (++loop_cnt >= 500))
          {
            break;
          }  
        
        }
        
        if(loop_cnt >= 500)
        {
          auto_scroll_advance = TRUE;
        }  
        else
        {
          auto_scroll_advance = FALSE;
        }  
      }
      else
      {
        while(1)
        {
          button = getKey( TIME_DELAY_MAX );
          if((button == ESC) || (button == UP) || (button == DOWN) || (button == STORE) )
          {
            break;
          }  
        }        
      }
      if((button == DOWN) || auto_scroll_advance)                         
      {
        display_set = (display_set + 1)%6;
      }  
      else if(button == UP)                     
      {
        display_set = ((display_set + 6)-1)%6; 
      }  
      else if(button == STORE) 
      {
        storeStatTestResultsToUSB ( TRUE );
      }
      else
      {
        break;
      }  
    } 
    
    break;
  
  } // end while(1)    
  Flags.stat_flag = FALSE; 
  
  // re-enable auto turn off if it was on
  Spec_flags.auto_turn_off = temp_auto_turn_off;
  shutdown_timer = 0;
  
  return(pass_fail);
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

void drift_test(void)  // leads user through drift test procedure
{
  Bool auto_scroll_advance = 0;
  int8_t display_set = 1, i;
  int32_t dc_sum = 0, mc_sum = 0, dc_statavg, mc_statavg, loop_cnt;
  int32_t delta_1 = 0, delta = 0;
  char  pass = 'P', fail = 'F'; 
  enum buttons button;
  uint16_t moisture_cnt = 0;
  uint32_t density_cnt = 0;
 
 
  // Save the state of the auto turn off flag
  BOOL temp_auto_turn_off = Spec_flags.auto_turn_off;
  Spec_flags.auto_turn_off = FALSE;                  // disable auto_turn_off during count
 
  
  while(1)
  {    
    if ( !Flags.diag )
    {
      press_start(1);  // display "  Press START for\n  20m. DRIFT TEST" on LINE2 and LINE3      
      while(1)
      {
        button = getKey(TIME_DELAY_MAX);
        
        if(button == ENTER || button == ESC || button == MENU)
        {
          break;
        }  
          
      }    
      if(button != ENTER)                                 // ESC was pressed, exit to menu
      { 
        break;
      }  
    }
    Flags.drift_flag = TRUE;
    if(Controls.LCD_light)
  	{
     KEY_B_LIGHT_DISABLE(); // turn off keyboard backlight    
  	};

   
    // Get the 5, 4 minute readings
    reading();  // display "Reading # " on LINE2
    for(i=0; i<5; i++)
    {      
      LCD_position (LINE2+10);
      _LCD_PRINTF ( "%d ",(i+1) );
    
      measurePulses( LINE3, 240, &moisture_cnt, &density_cnt ,0);                           //240
      
      button = getLastKey();
      
      if(button == ESC)
      {
        Flags.drift_flag = FALSE;
        break;
      }  
     
      drift_dense[i] = density_cnt;
      dc_sum += density_cnt;
      drift_moist[i] = moisture_cnt;
      mc_sum += moisture_cnt;
      delay_ms(10);
    }
    
    Flags.drift_flag = FALSE;
    
    if(Controls.LCD_light)
    {
      LCD_timeout = 70;                                   // 7 seconds      
    }  
         
    if(button == ESC)                                 // ESC was pressed, exit to menu
    {
      break;
    }      
    
    drift_dense_avg = dc_sum / 5;
    drift_moist_avg = mc_sum / 5;
    dc_statavg = NV_RAM_MEMBER_RD(D_CNT_STD);            // retreive stat values from EEPROM
    mc_statavg = NV_RAM_MEMBER_RD(M_CNT_STD);            
    delta = dc_statavg - drift_dense_avg;
    delta_1 = (dc_statavg + drift_dense_avg) / 2;                         // new code
    
    if ( delta_1 != 0 )
    { 
      drift_dense_per = fabsf (100.0 * ( (float)delta / (float)delta_1 ));         // new code
    }
    else
    { 
      drift_dense_per = 9999;
    }    
      
    
    delta = mc_statavg - drift_moist_avg;
    delta_1 = (mc_statavg + drift_moist_avg) / 2;                         // new code
    
    if ( delta_1 != 0 )
    {
      drift_moist_per = fabsf(((float)delta / (float)delta_1) *100.0);         // new code  
    }
    else
    {
      drift_moist_per = 9999;
    }  
      
////routine for diagnostic self test...store counts,%drift for review, determine pass/fail
    
	//********************************* copies the below statement from if(diag)
	  NV_MEMBER_STORE( dr_dense_avg,drift_dense_avg );
  	NV_MEMBER_STORE ( dr_moist_avg,drift_moist_avg );
	
 	  if ( Flags.diag )
    { //write results to EEPROM     
      NV_MEMBER_STORE(dr_dense_avg,drift_dense_avg);
      NV_MEMBER_STORE(dr_moist_avg,drift_moist_avg);
      NV_MEMBER_STORE(dr_dense_per,drift_dense_per);
      NV_MEMBER_STORE(dr_moist_per,drift_moist_per);
      break;
    }        
//DISPLAY DATA//DISPLAY DATA//DISPLAY DATA//DISPLAY DATA//DISPLAY DATA//DISPLAY DATA//   
    while(1)
    {  
      CLEAR_DISP;
      switch(display_set)
      {        
        case 1:///////////////////////////////////////////////////////////////
        LCD_position (LINE1);
        stat_drift_count_text(0); //TEXT// display "D% Drift = "
        
        sprintf( lcdstr,"%.2f %c", (double)drift_dense_per, (drift_dense_per <=0.5) ? pass : fail);
        LCD_print (lcdstr);
        
        LCD_position (LINE2);
        stat_drift_count_text(1); //TEXT// display "Dense.Avg = "
        _LCD_PRINTF ( "%lu",(uint32)drift_dense_avg);   
            
        LCD_position (LINE3);
        stat_drift_count_text(2); //TEXT// display "M%% Drift = "
        sprintf ( lcdstr,"%.2f %c",(double)drift_moist_per, (drift_moist_per <= 1.0) ? pass : fail);
        LCD_print ( lcdstr );
        
        LCD_position (LINE4);
        stat_drift_count_text(3); //TEXT// display "Moist.Avg = "
        _LCD_PRINTF ( "%lu",(uint32)drift_moist_avg);
       
        break;           
////////////////////////////////////////////////////////////////////////////////
        case 2:///////////////////////////////////////////////////////////////
        LCD_position (LINE1);
        sprintf ( lcdstr,"#1  D %lu M %u",(uint32) drift_dense[0],drift_moist[0]);
        LCD_print ( lcdstr );
       
        LCD_position (LINE2);
        sprintf ( lcdstr,"#2  D %lu M %u",(uint32) drift_dense[1],drift_moist[1]);
        LCD_print ( lcdstr );
       
        LCD_position (LINE3);
        sprintf ( lcdstr,"#3  D %lu M %u", (uint32)drift_dense[2],drift_moist[2]);
        LCD_print ( lcdstr );
       
        LCD_position (LINE4);
        sprintf ( lcdstr,"#4  D %lu M %u",(uint32) drift_dense[3],drift_moist[3]);
        LCD_print ( lcdstr );
        break;
////////////////////////////////////////////////////////////////////////////////            
        case 0:///////////////////////////////////////////////////////////////
        LCD_position (LINE1);
        sprintf ( lcdstr,"#5  D %lu M %u",(uint32) drift_dense[4],drift_moist[4]);
         LCD_print ( lcdstr );
        break;
      } 
      
      
      auto_scroll_advance = FALSE;      
      if(Features.auto_scroll)
      {
        loop_cnt = 0;
        while(1)
        {
          wait_for_key_release();          
          button = getKey (50);
          
          if ( (button == STORE) || (button == ESC) || (button == UP) || (button == DOWN) || (++loop_cnt >= 100) )
          { 
            break; 
          }
        }               
        
        if ( loop_cnt >= 100 )
        {
          auto_scroll_advance = TRUE;
        }  
        else
        {
          auto_scroll_advance = FALSE;
        }  
      }
      else
      {
        while(1)
        {
          button = getKey(TIME_DELAY_MAX);
          
          if((button == ESC) || (button == UP) || (button == DOWN) || (button == STORE) )
          {
            break;
          }  
        }        
      }
      if ( button == STORE )
      {
       storeDriftResultsToUSB ( TRUE );
      }
      else if((button == DOWN) || auto_scroll_advance)   // down button
      {
        display_set = (display_set + 1)%3;          
      }  
      else if(button == UP)                        // up button
      {
        display_set = ((display_set + 3)-1)%3;                
      }  
      else  //button == ESC exit function
      {
        break;
      }  
    }
    break;
}  
// re-enable auto turn off if it was on
 Spec_flags.auto_turn_off = temp_auto_turn_off;
 shutdown_timer = 0;
  

Flags.drift_flag = FALSE; 
Controls.shut_dwn = TRUE;



}

/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: 
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void selftest(void)  // tests high voltage,keypad,tubes and temp
{
  int8_t test_failed = 0x00, cont = 0;  //bit 0 = keys, bit 1 = he3, bit 2 = temperature
 

  uint16_t moisture_cnt =0;
  uint32_t density_cnt = 0;
  //enum buttons button;
  uint16 column;
  
  Global_ID();
  output_high(ROW_2);
  while(++cont < 50)
  {
    delay_ms(10);
    // check if key pressed
    column  =    Status_KEY_COLUMN_0_Read ();
    column |=  ( Status_KEY_COLUMN_1_Read  () << 8 );
    column &= 0x0FFF;
    if  ( bit_test ( column, 3 ) == 0 )
    {
      output_low(ROW_2);
      Global_IE();
      return;
    }
  }
  output_low(ROW_2);
  Global_IE();
  CLEAR_DISP;
  Spec_flags.self_test = TRUE;                      // set self test flag
    
    self_test_text(1);  // display "    Keypad Test\n    "In Progress"  on LINE2 and LINE3    
   // wait_for_key_release();
    delay_ms(1000);
    global_special_key_flag = FALSE; 
    scan_keys();     
    // self test keypad for shorted keys
    if(global_special_key_flag == TRUE ) // if key is shorted, fail self test
    { 
       test_failed |= 1<<0;    
    }  
    
    self_test_text(2);  // display "     Tube Test\n    In Progress" on LINE2 and LINE3    

    GM_TUBE_ENABLE();           // enable 2nd GM tube
  
    // run count for 5 seconds
    measurePulses ( LINE3, 5, &moisture_cnt, &density_cnt, 0);                      
      
   
   // if 3500 X2, test temperature.
    if ((eepromData.gauge_type == GAUGE_3500 ) || (eepromData.gauge_type == GAUGE_3440_PLUS))
    {
      if(!check_temp(0))  // temperature reading bounds are (-35C,75C)
       {
          BIT_SET(test_failed,2);
       }   
    }
    
  if( test_failed > 0 )  // at least one test failed
  {    
      CLEAR_DISP;      
      LCD_position(LINE1);
      self_test_pf_text(0,bit_test(test_failed,0));  //TEXT// display "Keypad: PASSED/FAILED"
           
      // if 3500 X2, test temperature. No sensor in 3440.
      if ((eepromData.gauge_type == GAUGE_3500 ) || (eepromData.gauge_type == GAUGE_3440_PLUS) )
      {
        LCD_position(LINE2);
        self_test_pf_text(8,bit_test(test_failed,2));  //TEMP
      }  
      CyDelay ( 5000 );
     
  }
  else
  {
    CLEAR_DISP;
    self_test_text(3);  // display "    Self Test\n      Passed" on LINE2 and LINE3
    delay_ms(1500); 
  }   
  
  GM_TUBE_ENABLE();         // enable 2nd GM tube
  Spec_flags.self_test = FALSE;                      // clear self test flag

}


/******************************************************************************
 *
 *  Name: 
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION: 
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 

void extended_drift_test ( void )
{
  uint16_t loop_cnt, c ;
  enum buttons button;
  
  //array that enter_name stores the name
  char test_name[] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
  char number_ptr[] = {0,0,0,0,0};

  char date_time_str[35];
  uint16_t moisture_cnt = 0;
  uint32_t density_cnt = 0;
  char temp_str[100] = "\0";
  float temp_calc, voltage;
  // Save the state of the auto turn off flag
  BOOL temp_auto_turn_off = Spec_flags.auto_turn_off;
  
  FILE_PARAMETERS fp;  
 
  Spec_flags.auto_turn_off = FALSE;                  // disable auto_turn_off during count
 
  Features.auto_store_on = FALSE;
  
  CLEAR_DISP;
  LCD_PrintAtPosition("EXTENDED COUNT TEST",LINE1);  
  LCD_PrintAtPosition("COUNTS:            ",LINE2);  

  Enter_to_Accept(LINE3);        //TEXT// display "Enter to Accept"
     
  ESC_to_Exit(LINE4);          //TEXT// display "ESC to Exit"    
        
  sprintf(number_ptr,"%d",0);
        
  loop_cnt = (uint16_t)enterNumber( number_ptr,LINE2 + 12, 4 );  // prompt user to enter number of counts
  button = getLastKey();      
  if(button == ESC)
  {
    Spec_flags.auto_turn_off = temp_auto_turn_off;
    shutdown_timer = 0;    
    return;          
  }  

  for(;;)
  {
    CLEAR_DISP;
    LCD_PrintAtPosition("Enter Test Name",LINE1);  
    YES_to_Accept(LINE3);              //TEXT// display "YES to Accept"
    ESC_to_Exit(LINE4);                //TEXT// display "ESC to Exit" 
    enter_name ( test_name,LINE2) ;  
    
    button = getLastKey();
            
    if(button == ESC)
    {
      Spec_flags.auto_turn_off = temp_auto_turn_off;
      shutdown_timer = 0;    
      return;          
    }  
    else if ( button == YES )        // YES was pressed to accept entered project name
    {  
      if ( test_name[0] != '\0' )   // make sure something was entered for the project name
      {
       break;       
      }  
    }
  }      

 CLEAR_DISP;
 LCD_PrintAtPosition ("Drift Test Name",LINE1);
 LCD_PrintAtPosition(test_name,LINE2);
 delay_ms(1000);
 
 CLEAR_DISP;

 AlfatStart();   
 read_RTC( &date_time_g );  //get date


 USB_text(0); // display "  Insert External\n Drive in USB Port\n     Press ENTER" on LINE1, LINE2 and LINE4 
 while(1)
 {
   button = getKey ( TIME_DELAY_MAX );
    
   if((button == ENTER) || (button == ESC))
   {
     break;
   }  
      
 }      
 if(button == ESC)
 {
   Spec_flags.auto_turn_off = temp_auto_turn_off;
   shutdown_timer = 0;    
   return;
 }  
 
 CLEAR_DISP;
 LCD_PrintAtPosition("CREATING NEW FILE",LINE1);
 
 // Create a file on USB by this name
 if(!check_for_USB())
 {       
   USB_text(2);  // display "   No USB Device\n    Detected." on LINE2 and LINE3
   delay_ms(2000);    
   return;
 }   
 else if(!initialize_USB(TRUE))  //try to initialize USB.  if it cant initialize, escape
 {
   Spec_flags.auto_turn_off = temp_auto_turn_off;
   shutdown_timer = 0;    
   return; 
 }

 USB_open_file  ( test_name, &fp );  

 sprintf(temp_str,"MOSTURE\t DENSITY\t NICD\t ALKALINE\t TEMPERATURE\t TIME\r\n");
 AlfatWriteStr(&fp,temp_str);  //write string to USB
 AlfatFlushData(fp.fileHandle);
  
 cnt_time =  NV_RAM_MEMBER_RD( COUNT_TIME );
 for ( c=0; c<loop_cnt; c++ )
 {
   float nimh_volts,alk_volts;
   
   // Display the count number
   CLEAR_DISP;
   LCD_PrintAtPosition("EXTENDED DRIFT TEST",LINE1);
   LCD_PrintAtPositionCentered("ESC to EXIT",LINE4+10);
   sprintf(temp_str,"COUNT %d OF %d  ",c+1,loop_cnt);
   LCD_PrintAtPosition(temp_str,LINE2);
   
    
   // Take the count
    measurePulses( LINE3,cnt_time, &moisture_cnt, &density_cnt, tst_depth_g ); 
    
   // leave function early if esc pressed
   button = getLastKey();
   if ( button == ESC )
   {
     Spec_flags.auto_turn_off = temp_auto_turn_off;
     shutdown_timer = 0;    
     return;
   }  
   
   // Get the time at the end of the count
   

   read_RTC( &date_time_g );  //get date
   getTimeDateStr ( date_time_g, date_time_str );
 
   Global_ID();

   nimh_volts = readADCVolts(NICAD_ADC_CHAN) ;   
   nimh_volts /= .177;
    
   alk_volts = readADCVolts(ALKALINE_ADC_CHAN) ;
   alk_volts /= .177;

   voltage = readADCVolts( TEMPERATURE_ADC_CHAN );
    // °C = (Vout - 500mV) / 10 mv/°C
    temp_calc = (voltage - .5) * 100;

   Global_IE();
   delay_ms( 200 );

   sprintf(temp_str,"%d\t %ld\t %.2f\t %.2f\t %.0f\t %s\t\r\n",moisture_cnt,(uint32)density_cnt,nimh_volts,alk_volts,temp_calc,date_time_str);
   AlfatWriteStr(&fp,temp_str);    //write string to USB
   AlfatFlushData(fp.fileHandle);


 }// end for loop
 AlfatCloseFile( fp.fileHandle ); 
Spec_flags.auto_turn_off = temp_auto_turn_off;
shutdown_timer = 0;    
}

