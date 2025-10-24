
 /******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  Depth.c
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
 *----------------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------------*/
/*-------------------------[   Include Files   ]------------------------------*/
/*----------------------------------------------------------------------------*/
 
#include "project.h"
#include "Globals.h"
#include "DataStructs.h"
#include "prompts.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "Utilities.h"

uint8 getPlusDepthReading ( void );

/*----------------------------------------------------------------------------*/
/*----------------------------------[   Macro   ]-----------------------------*/
/*----------------------------------------------------------------------------*/
#define NUMBER_DEPTHS 13
#define delta_v_half 0.105
#define delta_v_half_3440 0.070
/*----------------------------------------------------------------------------*/
/*-------------------------[  locals              ]---------------------------*/
/*----------------------------------------------------------------------------*/
//                                                   SAFE    BS    2     3      4    5    6       7    8    9    10     11    12   
static const float volts_depth_3500[NUMBER_DEPTHS]   = { 3.11, 2.87, 2.64, 2.41, 2.18, 1.95, 1.72, 1.49, 1.26, 1.02,  .79,  .56,  .33 };
static const uint8_t sensor_depth[NUMBER_DEPTHS+1]   = {  0,      1,    2,    3,    4,    5,    6,    7,    8,   9,   10,   11,   12 , 13 }; 
static float volts_depth[NUMBER_DEPTHS];


/******************************************************************************
 *  Name:
 *  
 *  PARAMETERS: Type of Depth Strip
 *
 *  DESCRIPTION: Determines if the depth strip is 3500 or 3440 Type.
 *               If 3500, copies the pre-determined voltages into the volts_depth array                         
 *	             If 34440, calculates the voltage at each depth from the calibration values
 *  RETURNS: 
 *
 *****************************************************************************/ 

void initDepthVoltages ( void )
{
  float safe;
  float slope;
  uint8 n;
  
  // There are two different two strips - 3500 and 3440.
  if ( NV_RAM_MEMBER_RD(depth_strip_type ) == DEPTH_STRIP_3500 )
  {
     // copy the voltages
     for ( n=0; n < NUMBER_DEPTHS; n++ ) 
     {
      volts_depth[n] = volts_depth_3500[n];
     }
  }
  else
  {
    // calculate voltages from cal values
    slope = NV_RAM_MEMBER_RD ( ds_slope );  // get the slope volts/ inch
    safe = NV_RAM_MEMBER_RD (safe_volts );  // get the voltage at SAFE position;
    
    volts_depth[0] = safe;
    volts_depth[1] = safe - SAFE_BS_DISTANCE*slope;    // BS position
    volts_depth[2] = volts_depth[1] - 2*slope;    // BS position
    for ( n = 3; n < 13; n++ )
    {
      volts_depth[n] = volts_depth[n-1] - slope;
    } 
  }  
}

 /******************************************************************************
 *  Name:
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: 
 *	
 *  The values for the depth strip are   SAFE = 0  
 *                                   BSCATTER = 1 
 *                                         2" =  2                                         
 *                                         3" =  3
 *                                        
 *                                         12"=  12
 *	                                      
 *	
 *  RETURNS: 
 *
 *****************************************************************************/ 
void display_depth(BYTE function, BYTE depth_temp_inches)
{  
    if ( (depth_temp_inches == 0) && Features.auto_depth )
    {
      if((depth_temp_inches == 0) && (function == 0))
       { 
         _LCD_PRINT("Depth:SAFE "); 
       }
    }    
    else if( bit_test(valid_depth,depth_temp_inches))    
    {    
      if ( depth_temp_inches == 1)
      {
       _LCD_PRINT("Depth:BS   ");
      }
      else if ( depth_temp_inches == 13  )
      {
       _LCD_PRINT("Depth:None ");
      }
      else
      {
        if(Features.SI_units)       // in "kg/m3" mode
        { _LCD_PRINTF("Depth:%3umm", depth_temp_inches * 25);}
        else
        { _LCD_PRINTF("Depth:%2uin.", depth_temp_inches);  }        // in "PCF" mode   
       
      } 
    }
    else
    {
      //sprintf ( lcdstr,"%s", "Depth: No Depth" );
      //LCD_print ( lcdstr );
      _LCD_PRINT("Depth:None  ");  
    }
}
/******************************************************************************
 *  Name:
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: 
 *	
 *  The values for the depth strip are   SAFE = 0  
 *                                   BSCATTER = 1 
 *                                         2" =  2                                                                                 3" =  3
 *                                        
 *                                         12"=  12
 *	                                     
 *	
 *  RETURNS: 
 *
 *****************************************************************************/ 


void display_invalid_depth( BYTE depth_temp_inches)
{  
  CLEAR_DISP;
    
  if( !bit_test(valid_depth,depth_temp_inches))
  {
    LCD_PrintAtPosition("No Calibration at   ",LINE1);  
  }
  
  LCD_position(LINE2);
  if ( (depth_temp_inches == 0) && Features.auto_depth )
  {
   _LCD_PRINT("Depth:SAFE "); 
  }    
  else    
  {    
      if ( depth_temp_inches == 1)
      {
       _LCD_PRINT("Depth:BS   ");
      }
      else if ( depth_temp_inches >= 13 ) // 12 is the last height. Anything greater than 12 is not a valid depth
      {
        _LCD_PRINT("Depth:NOT MEASURED ");
      }
      else
      {
        if(Features.SI_units)       // in "kg/m3" mode
         { _LCD_PRINTF("Depth:%3umm ", depth_temp_inches * 25);}
        else
         { _LCD_PRINTF("Depth:%2uin. ", depth_temp_inches);  }        // in "PCF" mode   
      } 
    }
 
}
 /******************************************************************************
 *  Name:
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 

/////DEPTH and TIME FUNCTIONS///////////////////////////////////////////////////
void set_count_time(void)  // set duration of count (TIME button initiates)
{
  uint8_t value;      //15,30,60,240 sec.  
  enum buttons button;
                  
  value = cnt_time;  
 // select_test_or_spec_cal(Flags.in_spec_cal);  // display "Select Test" or "Select Spec. Cal" on LINE1
  
  CLEAR_DISP;  

  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("UP/DOWN TO CHANGE   "); 
  }
   else 
  {
    _LCD_PRINT ("Arriba/Abajo       ");       
  } 
   
  YES_to_Accept(LINE3);   
  ESC_to_Exit(LINE4);
 
  while(1)                                   // neither abort or enter key is pressed
  {            
    display_count_time(value,LINE1);  // display "Count Time: %u sec. " or "Count Time:  %u min. "      
    while(1)
    {
      button = getKey(TIME_DELAY_MAX);  
                    
      if((button == YES) || (button == ESC) || (button == UP) || (button == DOWN))
      {
        break;
      }    
    }       
    if(button == ESC)        //button is not enter, down or up
      break;
    else if(button == YES)                                // Is button enter?
    {      
      cnt_time = value;
      NV_MEMBER_STORE(COUNT_TIME, cnt_time);      
      break;
    }
    else if(button == DOWN)
    {        
      if(value == 15)
      {
        value = 240;
      }  
      else if(value == 30)
      {
        value = 15; 
      }  
      else if(value == 60)
      {
        value = 30;
      }  
      else if(value == 240)
      {
        value = 60;
      }  
      else 
      {
       value = 60;
      } 
    }
    else  //UP was pressed
    {        
      if(value == 15)
      {
        value = 30;
      }  
      else if(value == 30)
      {
        value = 60;
      }  
      else if(value == 60)
      {
        value = 240;
      }  
      else if(value == 240)
      {
        value = 15;
      }  
      else 
      {
       value = 60;
      }
    }    
  }     
}


 
 
 /******************************************************************************
 *  Name:
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: 
 *	
 *  The values for the depth strip are   SAFE = 0  
 *                                   BSCATTER = 1 
 *                                         2" =  2                                          
 *                                         3" =  3
 *                                         12"=  12
 *	                         
 *	
 *  RETURNS: 
 *
 *****************************************************************************/ 

uint8_t get_depth_auto ( uint8_t position )  // reads depth when in auto depth mode
{
  
  float  value;
  uint8 n, depth = 13;
  float delta = delta_v_half;
  
  if ( NV_RAM_MEMBER_RD(depth_strip_type ) == DEPTH_STRIP_3440 )
  {
    delta = delta_v_half_3440;
  }
  
  // PLUS BB sends the data as char not voltage
  if ( NV_RAM_MEMBER_RD( gauge_type ) == GAUGE_3440_PLUS )
  {
    depth = getPlusDepthReading ();
    // display depth
    if(position != 0)
    {
      LCD_position(position);
      display_depth(0, sensor_depth[depth]);   
    }
  }
  else
  {
    // get the voltage
    value = readADCVolts(DEPTH_SENS_ADC_CHAN);
   	
   	//the values for the depth strip are SAFE = 0;  BSCATTER=1; 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 ,12 
  	
    for ( n = 0; n < NUMBER_DEPTHS; n++ )
    {
      if ( ( value > (( volts_depth[n] -  delta )))  && ( value < (( volts_depth[n] +  delta )) ) ) 
      {
        depth = n;
        break;    
      } 
    }
    
    // display depth
    if(position != 0)
    {
      LCD_position(position);
      display_depth(0, sensor_depth[depth]);   
    }
  } 
    
  return sensor_depth[depth];
  
}



 /******************************************************************************
 *  Name:       update_valid_depths(void) 
 *  
 *  PARAMETERS:
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 

void update_valid_depths(void)  // gets valid depths for gauge
{
  uint16_t i;
  DOUBLE_FLOAT data;

  valid_depth = 0;   

  for ( i=0; i < MAX_DEPTHS ; i++ )                                                   
  {    
    data = NV_CONSTANTS(DEPTHS[i].A); 
    
    if (data >= 1.0)    
    {
        valid_depth |= ( 1<< (i+1) ) ;    
    }    
  }  

}

 /******************************************************************************
 *  Name:
 *  
 *  PARAMETERS: tst_depth_g, the global variable that contains the depth setting.
 *
 *  DESCRIPTION: 
 *	
 *  The values for the depth strip are 
 *  SAFE = 0;  BSCATTER=1;  2, 3, 4, 5, 6, 7, 8, 9, 10, 11 ,12 None = 13
 *	
 *  RETURNS: 
 *
 *****************************************************************************/ 

void set_depth_manual( void )  // Manually set Depth it auto depth is disabled
{
  uint8_t i;
  uint16_t count = 0;
  enum buttons button;
  
  CLEAR_DISP;    
  
  LCD_position(LINE1);
  
  if ( Features.auto_depth && !Flags.in_spec_cal )
  { 
    disable_auto_depth();    // display "Disable Auto\nDepth?"
    
    Press_YES_or_NO (LINE4);  // display "Press YES ot NO"  
    
    while(1)
    {
      button = getKey(TIME_DELAY_MAX);  
     
      if ( (button == ESC) || (button == YES) || (button == NO) || (button == MENU) || (button == ENTER) )
      {
         break;
      }   
    }    
    
    if( (button != YES) && ( button != ENTER ) )
    {
     return;     
    }    
    
    Features.auto_depth = FALSE;
   
    if(tst_depth_g < 1)
    {
      tst_depth_g = 0;
    }    
  }
  CLEAR_DISP;
  // Check the range of the depth. 
  if  ( Flags.in_spec_cal )
  {
   count = (tst_depth_g < 14) ? tst_depth_g : 1;  
   if ( count == 0 )
   {
    count = 1;
   }
  }
  else
  {
   count = (tst_depth_g < 14) ? tst_depth_g : 0;  
  }
 // select_test_or_spec_cal(Flags.in_spec_cal);   // display "Select Test" or "Select Spec. Cal" on LINE1
  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("UP/DOWN TO CHANGE   "); 
  }
   else 
  {
    _LCD_PRINT ("Arriba/Abajo       ");       
  } 
    
 
  YES_to_Accept(LINE3);                   // display "YES to Accept"
  
  ESC_to_Exit(LINE4);                     // display "ESC to Exit"      
  
  while(1)
  {
    LCD_position(LINE1);
    
    // Display the depth on LINE2
    display_depth(1,count);
    
    while(1)
    {
      button = getKey(TIME_DELAY_MAX);  
      if((button == ESC) || (button == UP) || (button == DOWN) || (button == YES) || (button == MENU) || (button == ENTER))
      {
          break;
      }    
    }     
    
    if ( (button == ESC) || (button == MENU) )
    {
      return;
    }    
    else if ( (button == YES) || (button == ENTER) )
    {        
      // set the depth and leave
      tst_depth_g = count;        
      return;
    }
    else if ( button == UP )                     // button is "UP"
    {
      for(i = (count+1)%14;i < 14; i++)
      {
        if(bit_test(valid_depth,i))
        {
           break;            
        }    
      }
      count = i%14;
    }      
    else if(button == DOWN)                    // button is "Down"
    {
      
      for(i = (count+13)%14; i > 0; i--)
      {
        if(bit_test(valid_depth,i))
        {
           break;                        
        } 
      }
      count = i;
    }                                              
  }   
}

