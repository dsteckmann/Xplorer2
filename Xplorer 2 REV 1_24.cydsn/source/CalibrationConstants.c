/**********************************************************************************************************************/
//
// Title:        Elite
// Company:     Instrotek
//
// Document No.:
// Issue:
// Description:  
//
// Filename:   Settings.c
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
#include "project.h"
#include "Globals.h"
#include "DataStructs.h"
#include "conversions.h"
#include "prompts.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "Tests.h"
#include "Utilities.h"
#include "UARTS.h"



/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/

/************************************************  LOCAL DEFINITIONS  *************************************************/

#define GCC_PCF		 62.42796

/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/

void enter_calibration_time(void);
void enter_cal_stand (void);


 // Looks to see if the EEPROM data is all ff
 int isNaN ( uint8_t  f[]) 
 {
    if ( (f[0] == 255 ) && ( f[1] == 255) && ( f[2] == 255 ) && (f[3] == 255 ) ) 
    {
      return 1;
    }
    
    return 0;
      
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
 
void enter_cal_const(void)  // Manually Enter Calibration Constants
{
  uint8_t display_set = 1, selection;
  uint16_t key_timer;
  DOUBLE_FLOAT a_temp, b_temp, c_temp;

  char number_ptr[11] = "\0";
  enum buttons button;
  
  in_menu = TRUE;
  if ( enter_access_code() )
  {
    while(1)
    {
      CLEAR_DISP;
      while(1)
      {               
        cal_menu_text(display_set);  //TEXT// display calibration menu
        if (in_menu)
        {
         up_down_select_text(1);
        }    
        in_menu = FALSE;      
      
        while(1)
        {
          button = getKey( TIME_DELAY_MAX );
         
          if(button == ESC || button <= 11)
          {
            break;
          }  
        }            
       
        // ESC pressed, exit to menu       
        if(button == ESC)                             
        {  
          break;
        }  
        else if(button == UP)                 
        {
            display_set = ((display_set + 8) - 1)%8;     // There are 8 menus to scroll through
        }    
        else if(button == DOWN)                
        {
            display_set = (display_set + 1)%8;                  
        }    
        else if(button <= 9)                          // selection was made
        { 
          selection = button;           
          if(selection == 1)                          // first button was a 1, wait for second button for 1 sec.
          {
            wait_for_key_release();
            
            key_timer = 0; 
            while(key_timer++ < 250)
            {
              delay_ms(4);
              
              if ( Flags.button_pressed && ( getLastKey() <= 9) )
              {
                button = getLastKey();
                selection = selection*10 + button;
                break;
              } 
            }
          }  
          in_menu = TRUE;
          break;
        } 
      } // end of while(1)      
      
      if(button == ESC)
      {
          break;
      }    
      
      if ((selection > 0 ) && (selection < 13) )                          // some depth was selected
      {        
        cal_depth_text(selection);  //TEXT// display "   Depth # %d   ",selection on LINE1        
        
    		LCD_position(LINE1 + 17);
        LCD_print ( "GCC" );
    	
        selection = selection - 1;
                  
        LCD_position(LINE2);
        _LCD_PRINTF ( "A =   %+7.5f", (float)NV_CONSTANTS(DEPTHS[selection].A));
           
    		LCD_position(LINE3);  
        _LCD_PRINTF ( "B =   %+7.5f", (float)NV_CONSTANTS(DEPTHS[selection].B));
    	                       
        LCD_position(LINE4);
        _LCD_PRINTF ( "C =   %+7.5f", (double)NV_CONSTANTS(DEPTHS[selection].C));
         
        sprintf ( number_ptr, "%+7.5f", (double)NV_CONSTANTS(DEPTHS[selection].A )); 
        a_temp = enterNumber (number_ptr,LINE2 + 6, 10);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
            //return;         
            continue;
        }    
        
    		sprintf ( number_ptr, "%+7.5f", (double)NV_CONSTANTS(DEPTHS[selection].B )); 
    	
    		b_temp = enterNumber(number_ptr,LINE3 + 6, 10);    // *number_ptr1 will hold the entered number

        button = getLastKey();
        if(button == ESC)                                   //ESC was pressed, exit to menu
        {
          //return;              
          continue;
        }  
        
        sprintf ( number_ptr, "%+7.5f", (double)NV_CONSTANTS(DEPTHS[selection].C) ); 
        c_temp = enterNumber( number_ptr,LINE4 + 6, 10);    // *number_ptr1 will hold the entered number
        
        button = getLastKey();
        if ( button == ESC )                                   //ESC was pressed, exit to menu
        {
          //return;        
          continue;
        }   
        
        // if enter is pressed the third time, the values will be saved
        // Only write to EEPROm if values have changed
        if (NV_CONSTANTS(DEPTHS[selection].A != a_temp ) )
        {
          NV_MEMBER_STORE ( Constants.DEPTHS[selection].A, a_temp );
        }
        
        if ( NV_CONSTANTS(DEPTHS[selection].B != b_temp ))
        {
          NV_MEMBER_STORE ( Constants.DEPTHS[selection].B, b_temp );
        }  
         
        if (NV_CONSTANTS(DEPTHS[selection].C != c_temp ))
        { 
          NV_MEMBER_STORE ( Constants.DEPTHS[selection].C, c_temp );
        }  
        
      }
      
      else if(selection == 0)
      {       
     
        moisture_const_text();  //TEXT// display " MOISTURE CONST " LINE1        
    		LCD_position(LINE1 + 17);
  		  LCD_print ( "GCC" );
 
        LCD_position(LINE2);
	      _LCD_PRINTF ( "E =   %+7.5f", (double)NV_CONSTANTS(E_MOIST_CONST )); 
         LCD_position(LINE3);
        _LCD_PRINTF ( "F =   %+7.5f", (double)NV_CONSTANTS(F_MOIST_CONST ));
        YES_to_Accept(LINE4);  
      
        sprintf ( number_ptr, "%+7.5f", (double)NV_CONSTANTS(E_MOIST_CONST )); 
   
    		a_temp = enterNumber (number_ptr, LINE2 + 6, 10 );    // *number_ptr1 will hold the entered number
  
        button = getLastKey();
        if (button == ESC)                             // ESC was pressed, exit to menu      
        {
           // return;       
            continue;
        }  

	     sprintf ( number_ptr, "%+7.5f", (double)NV_CONSTANTS(F_MOIST_CONST ));  // PCF
 	     b_temp = enterNumber ( number_ptr, LINE3 + 6, 10);     // *number_ptr1 will hold the entered number
       button = getLastKey();     
        if(button == ESC)                             // ESC was pressed, exit to menu
        {
            //return;
            continue;
        }   
      
        // if enter is pressed the second time, the values will be saved
        if ( NV_CONSTANTS(E_MOIST_CONST ) != a_temp ) 
        {
          NV_MEMBER_STORE ( Constants.E_MOIST_CONST, a_temp );
        }  
        
        if ( NV_CONSTANTS(F_MOIST_CONST ) != b_temp )
        { 
          NV_MEMBER_STORE ( Constants.F_MOIST_CONST, b_temp );
        }  
      
      }
      else if ( selection == 13)
      {
        CLEAR_DISP;
        // Display the Calibration Date
        calibration_date_txt();
        LCD_position (LINE1+9);
        printTimeDate (  NV_RAM_MEMBER_RD (Constants.CAL_DATE) );
        change_val_text();       //TEXT// display "Change Value?" LINE2     
        Press_YES_or_NO (LINE3);  // display "Press YES or NO"
        ESC_to_Exit(LINE4);
        

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
         continue;
        }  
        
        // Enter the calibration date and write to EEPROM
        enter_calibration_time();
        hold_buzzer();
          
        delay_ms(2000);        
      }  
      else if ( selection == 14 )
      {
        enter_cal_stand();
        
      } 

         
    }  // end of while(1)
  }  // end of if(acces_number == ACCESS_CODE)  
  else if( getLastKey() == ESC)
  {
      return;
  }    
  else
  {
    hold_buzzer();
      
    delay_ms(2000);
  }  
  update_valid_depths();                              // sets corresponding bit in auto_depth for each valid depth;
}


 /******************************************************************************
 *  Name: enter_calibration_time
 *  
 *  PARAMETERS:None
 *
 *  DESCRIPTION: Enter the month, day, year of the calibration.
 *               Stores date in EEPROM
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 
 
void enter_calibration_time(void)  // leads user through process to set the time and date
{
  Bool  escape = 0;
  uint8_t i, j, msb_yr, lsb_yr, msb_month, lsb_month, msb_day, lsb_day, month_temp, day_temp, date_reg = 0;  
    
  date_time_t date ;  
  
  enum buttons button;
  
/////////////////////////Enter the date///////////////////////////////////////////
  CURSOR_OFF;
  while(!escape)
  {
    CLEAR_DISP;
    LCD_position(LINE1);
    _LCD_PRINT ( "Enter Calibration");
    LCD_position(LINE2);  
    _LCD_PRINT ( "Date: MM/DD/20YY");       
    YES_to_Accept(LINE3);  
    ESC_to_Exit(LINE4);
 
    i = 0;  //initialize markers
    j = 1;
    while(1)                                    //loop for entering each cnt cycle digit
    {
      if(j == 7)
      {
         CURSOR_OFF;
      }   
      else 
      {
        CURSOR_ON;
      }  
      LCD_position(LINE2 + (i+6));                // position cursor and flashing
      while(1)
      {
        button = getKey(TIME_DELAY_MAX);
        if((button <= 9) || (button == ESC) || (button == YES) || (button == CE))
        { break ; }
      }      
      if(button <= 9)                             // digit was entered   
      {   
        switch(j)
        {
          case 1:
          if(button < 2)                          //number must be 0 or 1
          {
            _LCD_PRINTF ( "%c",key_char); 
            msb_month = button;   
            BIT_SET(date_reg, 0);    
            i++;
            j++;
          }
          else if(button > 1)                     //in case number is > 1
          {
            _LCD_PRINTF ( "0%c",key_char);
            msb_month = 0;
            lsb_month = button;
            BIT_SET(date_reg, 0);
            BIT_SET(date_reg, 1);
            i = 3;   // i += 3
            j = 3;   // j += 2
          }
          break;
          
          case 2:                      
          if((msb_month == 1) && (button < 3))        //number must be 0 - 2
          {
            _LCD_PRINTF ( "%c",key_char);
            lsb_month = button;
            BIT_SET(date_reg, 1);
            i = 3;  // i += 2
            j = 3;  // j++;
          }
          else if((msb_month == 0) && (button > 0))   //number must be 0 - 9
          {
            _LCD_PRINTF ( "%c",key_char);
            lsb_month = button;
            BIT_SET(date_reg, 1);
            i = 3;  // i += 2;
            j = 3;  // j++;
          }
          break;
                    
          case 3:
          month_temp = msb_month*10 + lsb_month;
          if((month_temp != 2) && (button < 4))        //if not February, number from 0 - 3
          {
            _LCD_PRINTF ( "%c",key_char);
            msb_day = button;
            BIT_SET(date_reg, 2);
            i = 4;
            j = 4;
          }
          else if((month_temp == 2) && (button < 3))  //if February, number from 0 - 2
          {
            _LCD_PRINTF ( "%c",key_char);
            msb_day = button;
            BIT_SET(date_reg, 2);
            i = 4;
            j = 4;
          }
          else if(((month_temp != 2) && (button > 3)) || ((month_temp == 2) && (button > 2)))
          {
            _LCD_PRINTF ( "0%c",key_char);
            msb_day = 0;
            lsb_day = button;
            BIT_SET(date_reg, 2);
            BIT_SET(date_reg, 3);
            i = 8;
            j = 5;
          }          
          break;
          
          case 4:
          if((msb_day != 0) && (msb_day < 3))           // day must be greater than 00
          {
            _LCD_PRINTF ( "%c",key_char);
            lsb_day = button;
            BIT_SET(date_reg, 3);
            i = 8;
            j = 5;
          }
          else if((msb_day > 2) && (button == 1) && (month_temp != 4) && (month_temp != 6) && (month_temp != 9) && (month_temp != 11))
          {                                                             //take into account 30 day months
            _LCD_PRINTF ( "%c",key_char);
            lsb_day = button;
            BIT_SET(date_reg, 3);
            i = 8;
            j = 5;
          }
          else if((msb_day > 2) && (button == 0))                    //take into account 31 day months
          {
            _LCD_PRINTF ( "%c",key_char);
            lsb_day = button;
            BIT_SET(date_reg, 3);
            i = 8;
            j = 5;
          }
          else if((msb_day == 0) && (button > 0))         // day cannot be 00
          {
            _LCD_PRINTF ( "%c",key_char);
            lsb_day = button;
            BIT_SET(date_reg, 3);
            i = 8;
            j = 5;
          }
          break;
           
          case 5:
          _LCD_PRINTF ( "%c",key_char);
          msb_yr = button;
          BIT_SET(date_reg,4);
          i = 9;
          j = 6;
          break;
          
          case 6:
          _LCD_PRINTF ( "%c",key_char);
          lsb_yr = button;
          BIT_SET(date_reg,5);
          i = 10; 
          j = 7;
          break;
        }
      }
      else if(button == YES)                    // Yes key was pressed
      { 
        if(date_reg == 63)                            // All spaces filled
        {
          CURSOR_OFF;
          month_temp = msb_month*10 + lsb_month;
          day_temp = msb_day*10 + lsb_day;
          if(date_check(month_temp,day_temp)==1)
          {             
            escape = TRUE;
            CURSOR_OFF;
                  
            break;
          }
          break;
        }
      }
      else if(button == CE)                                       // NO/CE key was pressed, clear last digit entered   
      {
        if(j != 1)                               
        { 
          if(j == 2)
          {
            --i;
            --j;
          } 
          else if (j == 3)
          {
            i -= 2;
            --j;
          }
          else if (j == 4)
          {
            --i;
            --j;
          } 
          else if (j == 5 )
          {
            i -= 4;
            --j;
          }
          else if (j == 6)
          { 
            --j;
            --i;
          }  
          else if (j == 7)
          { 
            --j;
            --i;
          } 
          LCD_position(LINE2 + (i+6));
        }
      }
      else  //button == ESC                                                   
      {
        escape = TRUE;
        CURSOR_OFF;
        break;
      }          
  }  // end of loop for entering each  digit
  
    if (button != ESC)                      
    {
      date.iday   = (msb_day *10) + lsb_day;
      date.imonth = (msb_month *10) + lsb_month;
      date.iyear    = (msb_yr *10) + lsb_yr + 2000;      
      date.iminute = 0;  
      date.ihour  = 12;
      date.isecond = 0;
      //write the date/time to memory
      NV_MEMBER_STORE(Constants.CAL_DATE, date);
    }
    break;
 } // end while(1) loop 
  
  if (button == ESC)   //MENU or STOP was pressed, exit to MENU
  {
     escape = TRUE;           
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
void enter_cal_stand (void)
{
  uint16_t stand;
  char number_ptr[11] = "\0\0\0\0\0\0";  // enter a code here when ready to test
  
  enter_cal_den_std_text();    //TEXT// display "Enter Density Std,Count: " LINE1 and LINE2
  YES_to_Accept(LINE3);        //TEXT// display "YES to Accept"  
  ESC_to_Exit(LINE4);          //TEXT// display "ESC to Exit"      
  
 
  NV_EE_RD( DENSE_CAL_STD, &stand );
  
  if ( stand > 65535 )
  {
   stand = 9999 ;
  }
 
  sprintf( number_ptr, "%u", stand );
  stand = (uint16)enter_number(number_ptr,LINE2+10,5,0); 

  
  if( getLastKey() != ESC)
  {
    NV_MEMBER_STORE ( DENSE_CAL_STD, stand);
  }  
     
  enter_cal_mois_std_text();    //TEXT// display "Enter Density Std,Count: " LINE1 and LINE2
  YES_to_Accept(LINE3);        //TEXT// display "YES to Accept"  
  ESC_to_Exit(LINE4);          //TEXT// display "ESC to Exit"      
  
  NV_EE_RD( MOIST_CAL_STD, &stand );

  if ( stand > 65000 )
  {
   stand = 9999 ;
  }
  
  sprintf( number_ptr, "%u", stand );
  
  stand = (uint16)enter_number(number_ptr,LINE2+10,5,0);   
  
  if( getLastKey() != ESC)
  {
    NV_MEMBER_STORE ( MOIST_CAL_STD, stand);
  }   
  
  
 
}
