
/******************************************************************************
 *                                                                            
 *  InstroTek, Inc. 2010                  
 *  5908 Triangle Dr.
 *  Raleigh,NC 27617 
 *  www.instrotek.com  (919) 875-8371                                         
 *                                                                               
 *           File Name:  menus.c
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
#include "StoreFunctions.h"
#include "Keypad_functions.h"
#include "prompts.h"
#include "Utilities.h"
#include "Tests.h"
#include "SDcard.h"

extern void standCountMode(void);


void enterTimeRTCI2C (void);
void gps_test();

/*----------------------------------------------------------------------------*/
/*----------------------[   Global Variables   ]------------------------------*/
/*----------------------------------------------------------------------------*/


/////MENU FUNCTIONS/////////////////////////////////////////////////////////////
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

void main_menu(void)  // controls the main menu, (MENU button initiates)
{
 
//#if(GAUGETYPE==1)
 uint8_t menu_track = 1, menu_n = 10, selection;       

 enum buttons button;
 
 in_menu = TRUE;

    while(1)                    // only exit menu when ESC is pressed
    { 
      main_menu_display ( menu_track );
      in_menu = FALSE;
      while ( 1 )
      {
        button = getKey( TIME_DELAY_MAX );   
        
        if ( (button <= 11) || (button == ESC) )
        {  
          break; 
        }
      }          
     
      if ( button == ESC )             // ESC button pressed, exit to ready screen
      {
          break;  
      }    
      else if ( button == UP )                 
      {
          menu_track = ((menu_track + menu_n) - 1)%menu_n;     
      }    
      else if(button == DOWN)                
      {
          menu_track = (menu_track + 1)%menu_n;                  
      }    
      else if ( button <= 9)                // selection was made
      { 
        selection = button;           
        if(selection <= 2)                  // first button was a 1, wait for second button for 1 sec.
        {
          button = getKey (1000);
          
          if( button <= 9 )
          {
            selection = selection*10 + button;
          }
        }
      
        switch(selection)
        {
          case  1:  Spec_flags.recall_flag = TRUE;                               
                    measureMoistureDensity();  
                    Spec_flags.recall_flag = FALSE;     //from the most recent general purpose test                         
                    break;
          
          case  2:  offset();                         break;  // set moist, dense, or trench offsets
          case  3:  enable_disable_features('S');     break;  // auto_scroll
          case  4:  enable_disable_features('L');     break;  // LCD_backlight
          case  5:  stat_test();                      break;  // run stat test
          case  6:  drift_test();                     break;  // run drift test
          case  7:  enable_disable_features('G');     break;  // GPS
          case  8:  auto_depth_enable_or_calibrate(); break;  // auto depth       
          case  9:  review_std_counts();              break;  // review last 30 standard counts
          case 10:  select_language();                break;  // select between English and Spanish        
          case 11:  set_units();                      break;  // select English or SI units
          case 12:  standCountMode();                 break;  // avg_std_mode or decay mode
          case 13:  serial_number(0);                 break;  // view, reset serial number
          case 14:  enterTimeRTCI2C();              break;  // enter clock time
          case 15:  enable_disable_features('B');     break;  // Buzzer
          case 16:  special_cal();                    break;  // setup for special cal        
          case 17:  nomograph();                      break;  // setup for nomograph
          case 18:  enter_cal_const();                break;  // manually enter calibration constants from gauge   
          case 19:  soilvoids_menu();                 break;  // go to diagnostic menu
          case 20:  diag_menu();                      break;  // go to diagnostic menu
          
          case 21:  select_mode();                    break;  // Smart MC Mode
        }
     }
      
     if(button==ESC)
     {
       break;
     }    
            
     in_menu = TRUE;
    } // end while loop     

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
 *  RETURNS: 
 *
 *****************************************************************************/ 


void project_menu(void)  // controls the project menu (PROJECT button initiates)
{
  uint8_t menu_track = 1, menu_n = 3, selection;  
  enum buttons button;
  char proj[PROJ_NAME_LENGTH]; 
  in_menu = TRUE;
 
  if ( sdOpened == OFF )
  {
    SDstart();
  }
  
  while(1)                    // only exit menu when ESC is pressed
  { 
    project_menu_display(menu_track);
    in_menu = FALSE;
    while(1)
    {
      button = getKey( TIME_DELAY_MAX );
      if ( button <= 11 || button == ESC )
      {  
        break;
      }  
    }      
    if(button == UP)                 
    {
        menu_track = ((menu_track + menu_n) - 1)%menu_n;     
    }    
    else if(button == DOWN)                
    {
      menu_track = (menu_track + 1)%menu_n;                
    }  
    else if ( button <= 9 )                // selection was made
    { 
      selection = button;           
  
      if ( sdOpened == OFF )
      {
        SDstart();
      }
      switch(selection)
      {
  
        case 1:
              // Auto Store
              enable_disable_features('O');
              break;
        case 2:
              start_new_project();
              break;
        case 3:
              select_stored_project(0,proj);
              break;
        case 4:
              review_data();
              break;
        case 5:
              delete_projects();     
              break;
        default:
              break;   
      }
      if(button==ESC)
        break;
      in_menu = TRUE;
    }
    else if(button == ESC)             // ESC button pressed, exit to ready screen
      break;
  } 
  
  if (  sdOpened == ON  )
  {
    SDstop(null);
  }
}




/******************************************************************************
 *
 *  Name: print_menu
 *
 *  PARAMETERS: 
 *
 *  DESCRIPTION:  
 *               
 *            
 *  RETURNS: 
 *
 *****************************************************************************/ 


void print_menu(void)  // controls the project menu (PROJECT button initiates)
{
  enum buttons button, selection;
 
  if ( sdOpened == OFF )
  {
    SDstart();
  }
  
  while(1)                    // only exit menu when ESC is pressed
  { 
    print_menu_display();
    in_menu = FALSE;
    while(1)
    {
      button = getKey( TIME_DELAY_MAX );
      if ( button <= 11 || button == ESC )
      {  
        break;
      }  
    }      
   
    if ( button <= 2 )                // selection was made
    { 
      selection = button;           
  
      if ( sdOpened == OFF )
      {
        SDstart();
      }
      switch(selection)
      {
        case 1:
              write_data_to_USB();
              break;
        
        case 2:
              write_data_to_printer();
              break;
           default:
              break;   
      }
      if(button==ESC)
        break;
      in_menu = TRUE;
    }
    else if(button == ESC)             // ESC button pressed, exit to ready screen
      break;
  } 
  
  if (  sdOpened == ON  )
  {
    SDstop(null);
  }
}
/******************************************************************************
 *  Name: 
 *  PARAMETERS: 
 *  DESCRIPTION:  
 *  RETURNS: 
 *****************************************************************************/ 
void diag_menu(void)
{
  uint8_t menu_track = 1, menu_n = 8, selection;    
  enum buttons button;
  
  in_menu = TRUE;
  
  while(1)                    // only exit menu when ESC is pressed
  { 
    diag_menu_display(menu_track);
    in_menu = FALSE;
    while(1)
    {
      button = getKey( TIME_DELAY_MAX );
      if((button <= 11) || (button == ESC) || (button == MENU))
      {
          break;
      }    
    }          
    if((button == ESC) || (button == MENU))             // ESC->ready screen, MENU->main menu
    {
        break;  
    }    
    else if(button == UP)                 
    {
        menu_track = ((menu_track + menu_n) - 1)%menu_n;     
    }    
    else if(button == DOWN)                
    {
        menu_track = (menu_track + 1)%menu_n;                  
    }    
    else if(button <= 9)                // selection was made
    { 
      selection = button;    
      if(selection == 1)                  // first button was a 1, wait for second button for 1 sec.
      {
        button = getKey (1000);
        
        if( button <= 9 )
        {
          selection = selection*10 + button;
        }
      }
      switch(selection)
      {
        case  1:  batt_volt();  
                  break;  // monitor battery voltage 
        case  2: extended_drift_test(); 
                  break;
        case  3:  
                  if ((eepromData.gauge_type == GAUGE_3500 ) || (eepromData.gauge_type == GAUGE_3440_PLUS))
                  {
                    check_temp(1); 
                  }  
                  break;  // monitor gauge temperature
        case  4:  memory_reset();  
                  break;  // reset memory to default settings               
        case  5:  check_depth(); 
                  break;
        case  6:  key_pad_test();
                  break;
        case  7:  USB_store_test();
                  break;          
        case  8:  raw_count_test();
                  break;
        case  9:  shut_down_test();
                  break;  
        case  10: light_test();
                  break;              
        case  11: gps_test();
                  break;      
        case  12: FirmwareMenu();
                  break;      
        case  13: sd_card_menu();
                  break;
        case  14: reset_ble_menu();
                  break;
        case  15: idle_shutdown();
                  break;

        
       default: break;
      }
      if(button==ESC)
      {
          break;
      }    
      in_menu = TRUE;
    }      
  }  
}
