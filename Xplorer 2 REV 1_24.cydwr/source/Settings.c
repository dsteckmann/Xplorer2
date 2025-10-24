/**************************   ***************************************************************** ***************************/
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
// Processor:   Microchip PIC24HJ256GP206    
// Clock:	    12MHz	
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
#include "Alfat.h"

/************************************* EXTERNAL FUNCTION DECLARATIONS  *************************************/
extern float convertDensityToPCF ( float value_in_operational_units);
extern  void calibrateDepthSensor ( void ) ;
extern  int isNaN ( uint8_t  f[]) ;
extern void disableGPS ( void ) ;
extern void initGPS ( void ) ;

/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/

/************************************************  LOCAL DEFINITIONS  *************************************************/

/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/

struct ALFAT_DATE 
{
  uint32_t  second2   : 5  ;  
  uint32_t  minute    : 6  ;
  uint32_t  hour      : 5  ;
  uint32_t  day       : 5  ;
  uint32_t  month     : 4  ;
  uint32_t  year      : 7  ;

}  s_alfat_date;





 /******************************************************************************
 *  Name:     void displayValueWithUnits ( float value_in_kg, uint8_t position )
 *  
 *  PARAMETERS: A value in kg/m3, the LCD display position, string that is displayed
 *
 *  DESCRIPTION: Convert the kg/m3 value to the units that are operational
 *               Display the new values to the appropriate decimal place
 *               at the LCD position, and then add the unit tag (kg,pcf,gcc)
 *
 *  RETURNS: density in operational units, density displayed
 *
 *****************************************************************************/ 
 
float displayValueWithUnits ( float value_in_kg, uint8_t position, char * display)
{
  float value;
  
  LCD_position ( position );
  
  if ( Features.SI_units == FALSE )
  {
     value = value_in_kg *  KG_PCF_CONV;
     sprintf ( display, "%.1f PCF", value );
  }
  else 
  {
    if ( Features.kg_gcc_units == GCC_UNITS )
    {
      value = value_in_kg / 1000;
      sprintf(display,"%.3f GCC", value );
    }
    else
    {
      value = value_in_kg;
      sprintf(display,"%.0f KG/M3", value );
    }
  }
 
  LCD_print ( display );
  
  return value;
  
  
}
 
  /******************************************************************************
 *  Name:     void displayValueWithUnitsBW ( float value_in_kg, uint8_t position )
 *  
 *  PARAMETERS: A value in kg/m3, the LCD display position, string that is displayed
 *
 *  DESCRIPTION: Convert the kg/m3 value to the units that are operational
 *               Display the new values to the appropriate decimal place
 *               at the LCD position, and then add the unit tag (kg,pcf,gcc)
 *
 *  RETURNS: density in operational units, density displayed
 *
 *****************************************************************************/ 
 
float displayValueWithUnitsBW ( float value_in_kg, uint8_t position, char * display)
{
  float value;
  
  
  if ( Features.SI_units == FALSE )
  {
     value = value_in_kg *  KG_PCF_CONV;
     sprintf ( display, "%.1f PCF", value );
  }
  else 
  {
    if ( Features.kg_gcc_units == GCC_UNITS )
    {
      value = value_in_kg / 1000;
      sprintf(display,"%.3f GCC", value );
    }
    else
    {
      value = value_in_kg;
      sprintf(display,"%.0f KG/M3", value );
    }
  }
 
  print_string_backward( display, position);    
  return value;
  
  
}
 
 /******************************************************************************
 *  Name:    convertDensityToKgM3 ( float value_in_operational_units)
 *  
 *  PARAMETERS: A density value in kg/m3,pcf, or gcc ( depends on current units setting )
 *
 *  DESCRIPTION: Converts the unit settings density to kg/m3
 *
 *  RETURNS: density in kg/m3
 *
 *****************************************************************************/ 
 
float convertDensityToKgM3 ( float value_in_operational_units)
{
  
  float value_in_kg;
  
  if ( Features.SI_units == FALSE )
  {
     // convert PCF to KG/M3
     value_in_kg = value_in_operational_units /  KG_PCF_CONV;
  }
  else 
  {
    if ( Features.kg_gcc_units == GCC_UNITS )
    {
     // convert GCC to KG/M3
      value_in_kg = value_in_operational_units * 1000;
    }
    else
    {
      // convert KG/M3 to KG/M3
      value_in_kg = value_in_operational_units;
    }
  }
  return value_in_kg;

}

 /******************************************************************************
 *  Name:    convertDensityToPCF ( float value_in_operational_units)
 *  
 *  PARAMETERS: A density value in kg/m3,pcf, or gcc ( depends on current units setting )
 *
 *  DESCRIPTION: Converts the unit settings density to PCF
 *
 *  RETURNS: density in PCF
 *
 *****************************************************************************/ 
 
float convertDensityToPCF ( float value_in_operational_units)
{
  
  float value_in_pcf;
  
  if ( Features.SI_units == FALSE )
  {
     // value is alreasy in PCF
     value_in_pcf = value_in_operational_units ;
  }
  else 
  {
    if ( Features.kg_gcc_units == GCC_UNITS )
    {
     // convert GCC to PCF
     value_in_pcf = (value_in_operational_units * 1000) * KG_PCF_CONV;
    }
    else
    {
      // convert KG/M3 to PCF
      value_in_pcf = value_in_operational_units * KG_PCF_CONV ;
    }
  }
  return value_in_pcf;

}

/******************************************************************************
 *  Name:    convertKgM3DensityToUnitDensity ( float value_in_kg )
 *  
 *  PARAMETERS: A density value in kg/m3
 *
 *  DESCRIPTION: Converts the unit settings density to kg/m3,pcf, gcc
 *               depending on syytems units setting
 *            
 *  RETURNS: density in system units
 *
 *****************************************************************************/ 
 
float convertKgM3DensityToUnitDensity ( float value_in_kg, uint8_t units)
{
  
  float value;
  
  if ( units == PCF )
  {
     value = value_in_kg *  KG_PCF_CONV;
  }
  else if ( units == GM_CC )
  {
   value = value_in_kg / 1000;
  }
  else
  {
    value = value_in_kg;
  }
  
  
  return value;  // density in kg,pcf, or gcc

}
 
 /******************************************************************************
 *  Name:     enable_disable_features()
 *  
 *  PARAMETERS: None
 *
 *  DESCRIPTION: enable/disable LCD backlight, auto depth, or avg std mode
 *
 *  RETURNS: key
 *
 *****************************************************************************/ 
 

enum buttons enable_disable_features(char c)  
{  
 // BYTE enable = TRUE;
  char temp_str[20];
  enum buttons button;
 // uint16_t access_number;
  //char number_ptr[11] = "\0\0\0\0\0\0";  // enter a code here when ready to test
  
  //unsigned int16 access_number;
  //char num[5] = "    "; 
  
 
if ((Controls.LCD_light && (c == 'L')) || (Features.auto_scroll && (c == 'S')) || (Features.auto_depth && (c == 'D')) || (Features.avg_std_mode && (c == 'A')) 
   || (Features.auto_store_on && (c == 'O')) || (Features.sound_on && (c == 'B')) || (Features.chi_sq_mode == 0 && (c == 'Q')) || (Features.gps_on == 1 && (c == 'G')) )  //the feature in question is enabled
 {
  //  enable=FALSE;
    if(Features.language_f)
    {
      sprintf(temp_str,"Disable");
    }   
    else
    { 
      sprintf(temp_str,"Inhabilite");
    }  
  }
  else
  {
    if(Features.language_f)
    {
      sprintf(temp_str,"Enable");
    }    
    else
    {
      sprintf(temp_str,"Permita");
    }  
  }
  
  enable_disable_text(temp_str,c);  //TEXT// display Enable/Disable screen for selected feature  
  Press_YES_or_NO(LINE4);
  
  while(1)
  {
   button = getKey(TIME_DELAY_MAX);
   
   if ( (button == ESC) || (button == YES) || (button == NO) || (button == MENU) )
   {
     break;
   }     
  }    
  
  if(button == YES)
  {
    if(c == 'L')
    {        
      Controls.LCD_light ^= 1;                    // toggle LCD backlight flag
      if(Controls.LCD_light)
	    {
        KEY_B_LIGHT_ENABLE();
	    }
      else
	    {
        KEY_B_LIGHT_DISABLE();
	    }
   
      
    }
    else if(c=='S')
    {
      Features.auto_scroll ^= 1;                   // toggle scroll feature
      

    }    
    else if(c=='D')
    {
      Features.auto_depth ^= 1;                    // toggle auto depth feature
      if((tst_depth_g <= 1) && !Features.auto_depth)   // if backscatter, adjust tst_depth_g variable
      {
        tst_depth_g = 0;
      }  
      

    }
    else if(c=='A')
    {
      Features.avg_std_mode ^= 1;                  // toggle average standard mode
    }  
    else if(c=='O')
    {
      Features.auto_store_on ^= 1;
    }   
    else if(c=='B')
    {
      Features.sound_on ^= 1;

    } 
    else if(c=='G')
    {
      Features.gps_on ^= 1;
    }
  
    
   
  }  
  
  // Display if feature is enabled or disabled
  if(c == 'L')
  {       
        
    CLEAR_DISP;
    if ( Controls.LCD_light == TRUE )
    {
     LCD_PrintAtPositionCentered("Light Enabled",LINE2+10); 
    }
    else
    {
     LCD_PrintAtPositionCentered("Light Disabled",LINE2+10); 
    }
    
  }
  else if(c=='S')
  {
    
    CLEAR_DISP;
    if ( Features.auto_scroll == TRUE )
    {
     LCD_PrintAtPosition("Auto Scroll Enabled",LINE2);
    }
    else
    {
     LCD_PrintAtPosition("Auto Scroll Disabled",LINE2);
    }
  }    
  else if(c=='D')
  {
    
  }
  else if(c=='A')
  {
    
  }  
  else if(c=='O')
  {

    CLEAR_DISP;
    if ( Features.auto_store_on == TRUE )
    {
     LCD_PrintAtPosition( "Auto Store Enabled  ",LINE2);
    }
    else
    {
     LCD_PrintAtPosition("Auto Store Disabled ",LINE2);
    } 
  }   
  else if(c=='B')
  {
    
    CLEAR_DISP;
    if ( Features.sound_on == TRUE )
    {
     LCD_PrintAtPositionCentered("BUZZER Enabled",LINE2+10);
    }
    else
    {
     LCD_PrintAtPositionCentered("Buzzer Disabled",LINE2+10);
    }
  }
   else if(c=='G')
  {
    
    CLEAR_DISP;
    if ( Features.gps_on  == 1 )
    {
     LCD_PrintAtPositionCentered("GPS Enabled",LINE2+10);
      CyDelay ( 100 );
      initGPS ();
    }
    else
    {
     LCD_PrintAtPositionCentered("GPS Disabled",LINE2+10);
     disableGPS();
    }

  }  
 // save struct Features to eeprom
 NV_MEMBER_STORE( FEATURE_SETTINGS, Features );
         
 // enable shut down feature when NO is pressed                              
 Controls.shut_dwn = TRUE;                               
 delay_ms(1000);
 return button;
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
 

void auto_depth_enable_or_calibrate(void)
{  
  enum buttons button;
  float volts_safe,volts_bs,slope;
  char temp_str[21];
  auto_depth_settings(); //TEXT// display "Auto Depth Settings\n1. Enable/Disable\n
  ESC_to_Exit(LINE4);
       
  while(1)
  {
    button = getKey(TIME_DELAY_MAX);
    
    if((button == 1) || (button == 2 ) || (button == ESC) || (button == MENU))
      break;
  }
  
  if((button == ESC) || (button == MENU))
  {
    return ;
  }    
  else if(button == 1)
  {
    enable_disable_features('D');
    CLEAR_DISP;
    if (  Features.auto_depth == FALSE )
    {
      LCD_PrintAtPosition("Auto Depth Disabled ",LINE2);
    }
    else
    {
      LCD_PrintAtPosition("Auto Depth Enabled  ",LINE2);
    } 
    delay_ms(1000); 
  }
  else if ((button == 2) && ( NV_RAM_MEMBER_RD( gauge_type ) != GAUGE_3440_PLUS ))
  {
   CLEAR_DISP;
   depth_strip_type(); // 1) Xplorer , 2) 3400
   while(1)
   {
    button = getKey(TIME_DELAY_MAX);
    
    if((button == 1) || (button == 2 ) || (button == ESC) || (button == MENU))
      break;
   }
  
    if((button == ESC) || (button == MENU))
    {
      return ;
    }    
    else if(button == 1)
    {
     CLEAR_DISP;
     LCD_PrintAtPosition("Xplorer DS Enabled",LINE2); 
     NV_MEMBER_STORE (depth_strip_type , DEPTH_STRIP_3500 );
     CyDelay(1500);
    }
    else if ( button == 2 )
    {
     CLEAR_DISP;
     LCD_PrintAtPosition("3440 DS Enabled",LINE2); 
     NV_MEMBER_STORE (depth_strip_type , DEPTH_STRIP_3440 );
     CyDelay(1500);
     CLEAR_DISP;

     // Init but disconnect the depth mux.
     AMux_DEPTH_Init();
     AMux_DEPTH_Select(0); // connect the current source to the 3440 depth strip.
     IDAC8_DEPTH_Start();  // turn on the current source for the depth strip.
     
     // get depth strip voltage at SAFE
     LCD_PrintAtPosition ("   Put Rod in SAFE  ",LINE1);  
     LCD_PrintAtPosition ("   Position, and    ",LINE2);  
     LCD_PrintAtPosition ("   PRESS  <ENTER>   ",LINE3);   
     while(1)
     {
      button = getKey(TIME_DELAY_MAX);
      
      if((button == ESC) || (button == ENTER))
        break;
     }
    
      if( button == ESC )
      {
        return ;
      }    
      CyDelay(1000);
      volts_safe = readADCVolts(DEPTH_SENS_ADC_CHAN);
      
      CLEAR_DISP;
      // get the voltage at BS position
      LCD_PrintAtPosition("   Put Rod in BS    ",LINE1);  
      LCD_PrintAtPosition("   Position, and    ",LINE2);  
      LCD_PrintAtPosition("   PRESS  <ENTER>   ",LINE3);  
      while(1)
      {
        button = getKey(TIME_DELAY_MAX);

        if((button == ESC) || (button == ENTER))
        break;
      }

      if( button == ESC )
      {
      return ;
      }    
      
      CyDelay(1000);
      // get the voltage at BS
      volts_bs = readADCVolts(DEPTH_SENS_ADC_CHAN);
      slope = volts_safe - volts_bs;
      slope /= SAFE_BS_DISTANCE ; // volts/inch calculation 
      
      NV_MEMBER_STORE (safe_volts , volts_safe );
      NV_MEMBER_STORE (ds_slope , slope );
      
      CLEAR_DISP;
      // display the voltage at SAFE position
      // get the value from EEPROM (not ram)
      NV_EE_RD ( safe_volts, &slope ); 
      sprintf( temp_str,"SAFE VOLTS:%1.2f", slope );
      LCD_PrintAtPosition(temp_str,LINE1);  
      
      
      // display the slope
      NV_EE_RD ( ds_slope, &slope ); 
      sprintf( temp_str,"Slope: %1.4f", slope );
      LCD_PrintAtPosition(temp_str,LINE2); 
      LCD_PrintAtPosition("<ENTER> to Exit",LINE4);
      
      initDepthVoltages(); // calculate the volt at each depth
      
      while(1)
      {
       button = getKey(TIME_DELAY_MAX);
      
       if((button == ESC) || (button == ENTER))
        break;
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
 *  RETURNS: 
 *
 *****************************************************************************/ 
 

void standCountMode(void)
{  
  enum buttons button;

  stand_mode_settings_text(); //TEXT// display "Std Count Settings\n1. Average Mode n2.Decay Mode " on LINE1,2,3
  ESC_to_Exit(LINE4);
       
  while(1)
  {
    button = getKey(TIME_DELAY_MAX);
    
    if( (button == 1) || (button == 2) ||  (button == ESC) || (button == MENU) )
      break;
  }
  
  if((button == ESC) || (button == MENU))
  {
    return ;
  } 
     
  else if(button == 1)
  {
    Features.avg_std_mode = TRUE;
    avg_stand_mode_enabled_text();
  }  
  else if (button == 2)
  {   
   decay_stand_mode_enabled_text();
   Features.avg_std_mode = FALSE;  
  }


  // Put the data into EEPROM
  NV_MEMBER_STORE (FEATURE_SETTINGS, Features);     // write kg or GCC as active units and save to memory          
  delay_ms ( 1000 );

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
 
void set_units(void)  // Set/Store Gauge Units as SI or English
{  
  enum buttons button;

  set_units_text();  //TEXT// display "1. PCF\n2. kg/m3" on LINE1 and LINE2
  up_down_select_text(0);  //TEXT// display "Select #, ESC Exit" LINE4();  
    
  while(1)
  {
    button = getKey(TIME_DELAY_MAX);
    
    if((button == 1) || (button == 2) || (button == 3) || (button == ESC) || (button == MENU))
    {
      break;
    }  
  }   
  
  if ( button == 1 )  // "Enter" was pressed, selection was made
  {  
    // PCF selected
 
    Features.SI_units = FALSE;    
    NV_MEMBER_STORE(FEATURE_SETTINGS,Features);     // write PCF as active units and save to memory          
    CLEAR_DISP;
    LCD_PrintAtPositionCentered("PCF",LINE2+10);
    delay_ms(2000);
  }
  else if ( button == 2 )
  {   
    // KG/M3
    Features.SI_units = TRUE;    
    Features.kg_gcc_units = KG_UNITS;
    NV_MEMBER_STORE (FEATURE_SETTINGS, Features);     // write kg or GCC as active units and save to memory          
    CLEAR_DISP;
    LCD_PrintAtPositionCentered("kg/m3",LINE2+10);
    delay_ms(2000);
    
  }   
  else if ( button == 3 )
  {
    // GCC
    Features.SI_units = TRUE;    
    Features.kg_gcc_units = GCC_UNITS;
    NV_MEMBER_STORE (FEATURE_SETTINGS, Features);     // write kg or GCC as active units and save to memory          
    CLEAR_DISP;
    LCD_PrintAtPositionCentered("GCC",LINE2+10);
    delay_ms(2000);
  }
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


void select_mode(void)
{
  uint8_t menu_track = 1, menu_n = 3;    
  enum buttons button;
  
  in_menu = TRUE;
  
  while(1)                    // only exit menu when ESC is pressed
  { 
    mode_menu_display(menu_track);
    in_menu = FALSE;
    while(1)
    {
      button = getKey( TIME_DELAY_MAX );
      if((button <= 5) || (button == ESC) || (button == UP) || (button == DOWN) )
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
    else if(button <= 5)                // selection was made
    { 
        if(button == 2)  // DUMMY Selected
        {
          Features.dummy_mode = TRUE;
          CLEAR_DISP;
          LCD_PrintAtPositionCentered("DEMO  Mode          ",LINE2+10);
          NV_MEMBER_STORE(FEATURE_SETTINGS,Features);  //save settings to EEPROM
        }     
        else if(button == 1)  // DUMMY Disable
        {
          Features.dummy_mode = FALSE;
          CLEAR_DISP;
          LCD_PrintAtPositionCentered("Normal Mode          ",LINE2+10);
          NV_MEMBER_STORE(FEATURE_SETTINGS,Features);  //save settings to EEPROM
        }
        else if(button == 3)  
        {
          eepromData.gauge_type       = GAUGE_3500;        // InstroTek Gauge 
          CLEAR_DISP;
          LCD_PrintAtPositionCentered("InstroTek X2        ",LINE2+10);
          NV_MEMBER_STORE (gauge_type,GAUGE_3500);
        }   
        else if(button == 4)  
        {
          eepromData.gauge_type       = GAUGE_3440;        // 3440 Gauge 
          CLEAR_DISP;
          LCD_PrintAtPositionCentered("Troxler 3440        ",LINE2+10);
          NV_MEMBER_STORE (gauge_type,GAUGE_3440);
        }   
        else if(button == 5)  
        {
          eepromData.gauge_type       = GAUGE_3440_PLUS;        // 3440 Gauge 
          CLEAR_DISP;
          LCD_PrintAtPositionCentered("Troxler 3440 PLUS   ",LINE2+10);
          NV_MEMBER_STORE (gauge_type,GAUGE_3440_PLUS);
        }  
     
     delay_ms ( 1000 );
     break;  
   }
     
    in_menu = TRUE;
  } // loop      
   
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
 
void select_language(void)
{
  enum buttons button;
  
  select_language_text();  //TEXT// display "1. English\n2. Spanish"  LINE1,2

  up_down_select_text(0);  //TEXT// display "Select #, ESC Exit"

  while(1)
  {
   button = getKey ( TIME_DELAY_MAX );
    if((button == 1) || (button == 2) || (button == ESC) || (button == MENU))
    {
      break;
    }    
  }
  if(button > 2)  //ESC or MENU pressed
  {
    return;
  }    
  else if(button == 1)  //english selected
  {
    Features.language_f = TRUE;
    CLEAR_DISP;
    LCD_PrintAtPositionCentered("ENGLISH",LINE2+10);
  }    
  else                  //spanish selected
  {
    if(enter_access_code())                   // confirm access code
    {
      Features.language_f = FALSE;  
      CLEAR_DISP;
      LCD_PrintAtPositionCentered("Espanol",LINE2+10);
    }
    else
    {
      return;
    }  
  }    
  NV_MEMBER_STORE(FEATURE_SETTINGS,Features);  //save settings to EEPROM
  delay_ms ( 1000 );
  
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
 
void offset(void)  // leads user through process to enable/disable or enter offsets
{          
  char number_ptr[20] = "\0";
  char temp_str[20];
  enum buttons button;
  float num_temp;
  uint16_t mois_cnt;
  uint32_t density_cnt;
  Bool enabled = 0;
  Controls.shut_dwn = TRUE;               // enable shut down feature when NO is pressed  
 
  while(1)                              // select either Dens, Moist or Trench offset
  {           
    offsets_text();  //TEXT// display "1. Density\n2. Moisture\n3. Trench" on LINE1,2,3
    up_down_select_text(0);  //TEXT// display "Select #, ESC Exit" on LINE4
    Controls.shut_dwn = TRUE;                                    // enable shut down feature when NO is pressed  
   
    while(1)
    {
     button = getKey(TIME_DELAY_MAX);
     
      if((button == 1) || (button == 2) || (button == 3) || (button == ESC) || (button == MENU))
      {
         break;
      }   
    }   
     
    if ( button == ESC )  // ESC pressed, exit to menu
    {
      break;
    }  
    
    Controls.shut_dwn = FALSE;
    CLEAR_DISP;
    
    // Density offset selected
    if ( (button == 1) || (button == 2) || (button == 3) )   
    {    
      if(button == 1)
      {
        if(Features.language_f)
        {
            sprintf(temp_str,"Density");
        }    
        else
        {
           sprintf(temp_str,"Densid.");
        }   
        enabled = (Offsets.den_offset_pos) ? 1 : 0;
      }
      else if(button == 2)
      {
        if(Features.language_f)
        {
           sprintf(temp_str,"Moist(K)"); 
        }   
        else
        {
          sprintf(temp_str,"Hume(K)");
        }    
        enabled = (Offsets.moist_offset_pos) ? 1 : 0;
      }
      else if(button == 3)
      {
        if(Features.language_f)
        {
          sprintf(temp_str,"Trench"); 
        }    
        else
        {
            sprintf(temp_str,"Zanja");
        }    
        enabled = (Offsets.tren_offset_pos) ? 1: 0;
      }
      
      offset_on_off_text(temp_str,enabled);  //TEXT// display "%s Offset ON\nWant to Disable?",temp_str or "%s Offset OFF\,Want to Enable?",temp_str LINE1,2
      Press_YES_or_NO(LINE4);  //TEXT// display "Press YES or NO" on LINE4         
      
      while(1)
      {
        button = getKey(TIME_DELAY_MAX);
        if((button == YES) || (button == NO) || (button == ESC) || (button == MENU))
        {
         break;
        }    
            
            
      } 
      if((button == NO) || (button == YES))
      {
        CLEAR_DISP;
        if(((button == YES)  && enabled) || ((button == NO) && !enabled))  // Disable density offset
        {
          if(temp_str[0]=='D')
          {
            Offsets.den_offset_pos = FALSE;                     // clear density offset bit to "OFF"
          }
          else if((temp_str[0]=='M') || (temp_str[0]=='H'))
          {
            Offsets.moist_offset_pos = FALSE;
          }
          else if((temp_str[0]=='T') || (temp_str[0]=='Z'))
          {
            Offsets.tren_offset_pos = FALSE;
          }
          offset_enable_disable_text(temp_str,0);     //TEXT// display "   %s Offset\n      Disabled",temp_str on LINE2,3            
          delay_ms(1200);                                   // show enabled/disabled screen for 1.2 seconds  
        }   
        else if(((button == YES) && !enabled) || ((button == NO) && enabled))  // enable density offset
        {         
          if ( (temp_str[0]=='D') || (temp_str[0]=='M') || (temp_str[0]=='H') )
          {
            enter_offset_text(temp_str);  //TEXT// display "Enter %s\nOffset:",temp_str  on LINE1 and LINE2         
            Enter_to_Accept(LINE3);       //TEXT// display "Enter to Accept"
            ESC_to_Exit(LINE4);           //TEXT// display "ESC to Exit"
            number_ptr[0] = '\0';
          }
          // Get Density, Moisture, or Trench Offset
          if(temp_str[0]=='D')
          {  
            // Get the Density offset
            
            // This is in kg_m3 units
            num_temp = NV_RAM_MEMBER_RD(D_OFFSET);  // Get the density offset from NV memory
            
            // Convert the density value to the proper units and display
            // returns the value in proper units (kg,pcf,gcc)
            num_temp = displayValueWithUnits ( num_temp, LINE2 + 9, number_ptr );
            
            // enter density offset
            num_temp = enterNumber ( number_ptr, LINE2 + 9, 6 );
            
          
          }
          else if((temp_str[0]=='M') || (temp_str[0]=='H'))
          {
            //Get the moisture offset
           
            num_temp = NV_RAM_MEMBER_RD(K_VALUE);  // Get the K value from NV memory
            sprintf(number_ptr,"%.2f", num_temp);   //write K value to string and convert for display
            
            num_temp = enterNumber( number_ptr,LINE2+9,5 );       
          
          }
          else if((temp_str[0]=='T') || (temp_str[0]=='Z'))
          {
            // Get the Trench offset
             num_temp = (float)NV_RAM_MEMBER_RD(T_OFFSET);
            
            //write tren_offset to string and convert for display
            CLEAR_DISP;
            LCD_position ( LINE1 );
            _LCD_PRINTF("Trench Offset:%.0f", num_temp);
             get_new_offset_text();      //TEXT// display "GET NEW OFFSET?" LINE2     
            Press_YES_or_NO(LINE3);  //TEXT// display "Press YES or NO"
            ESC_to_Exit(LINE4);      //TEXT// display "ESC to Exit"
      
            while(1)
            {
              button = getKey(TIME_DELAY_MAX);
              if((button == ESC) || (button == YES) || (button == NO))
               {
                 Offsets.tren_offset_pos = TRUE; // Just enable Trench offset
                 break;
               }
            }     
            if(button != YES)   // NO or ESC was pressed, exit to menu
            {
              break;
            }
            else
            {
                           
              // Take a 1 minute reading in Standard block
              while ( 1 )
              {
                trench_count_press_start();  // display "  Press START for\n  Standard Count" 
                
                while(1)
                {
                  button = getKey ( TIME_DELAY_MAX );
                  if(button == ESC || button == ENTER)
                  {
                    break;
                  }  
                }
               if(button == ESC)                    // ESC was pressed, exit 
               {
                break;
               }  
              // Exit to ready screen if not at SAFE position (2" for strata )
              if(Features.auto_depth == 1 )                // set depth if in auto mode
              {
                 tst_depth_g = get_depth_auto( 0 );   // update depth     
                 if ( tst_depth_g != 0 )              // test for SAFE position
                 {
                  not_in_safe_text();                 // TEXT: "NOT IN SAFE POSITION
                  delay_ms(1000);
                 }
                 else
                 {
                  break; // exit while loop
                 }
              }   
      				else
      				{
      				  break; // In manual depth so continue test
      				}            
              } // end while waiting for SAFE position
              if(button == ESC)                                 // ESC was pressed, exit to ready screen
              {
                break;
              }  
              else if(button == ENTER)
              {
               // Take count
               CLEAR_DISP;
               Spec_flags.trench_offset_flag = TRUE;
               measurePulses ( LINE4, 60, &mois_cnt, &density_cnt, tst_depth_g );
               Spec_flags.trench_offset_flag = FALSE;
               if( getLastKey() == ESC )
               {
                break;
               }
               // Trench offset is mois_cnt - moisture standard count 
               num_temp = (float)NV_RAM_MEMBER_RD (MOIST_STAND) - (float)mois_cnt;
               if ( num_temp > 0.0 )
               {
                num_temp = 0.0;  // Trench count should also be greater than regular count
               }
               //write tren_offset to string and convert for display
                CLEAR_DISP;
                LCD_position ( LINE1 );
                _LCD_PRINTF("Trench Offset:%.0f", num_temp);
                use_val_text();       //TEXT// display "Use Value?" LINE2     
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
              } 
            }          
          }        
          if( (getLastKey() == ENTER)  || ((button == YES) && ((temp_str[0]=='T') || (temp_str[0]=='Z') )) )                         // a number was entered and accepted
          {
            if(temp_str[0]=='D')
            {
              // update the flag bit and NV memory
               
              // Convert to KG/M3 before storing the density
              num_temp = convertDensityToKgM3 ( num_temp );
              NV_MEMBER_STORE(D_OFFSET, num_temp);
              Offsets.den_offset_pos = TRUE;                     
            
              // Convert the density value to the proper units and display
              // returns the value in proper units (kg,pcf,gcc)
              CLEAR_DISP;
              displayValueWithUnits ( num_temp, LINE2+3, number_ptr );
              delay_ms(1000);
            }
            else if((temp_str[0]=='M') || (temp_str[0]=='H'))
            {
              // store K value. It should be unitless
              NV_MEMBER_STORE (K_VALUE, num_temp);
              Offsets.moist_offset_pos = TRUE;
              CLEAR_DISP;
              LCD_position(LINE2);
              _LCD_PRINTF("   %.2f", num_temp);
              delay_ms(1000);
            }
            else if((temp_str[0]=='T') || (temp_str[0]=='Z'))
            {
              // store Trench Offset
              NV_MEMBER_STORE (T_OFFSET, num_temp);
              Offsets.tren_offset_pos = TRUE;
              
              CLEAR_DISP;
              LCD_position(LINE2);
              _LCD_PRINTF("   %.0f", num_temp);
              delay_ms(1000);
            } 
            offset_enable_disable_text(temp_str,1);  //TEXT// display "   %s Offset\n      Enabled",temp_str on LINE2,3
            delay_ms(1200);                                   // show enabled/disabled screen for 1.2 seconds  
          }
          else
          {  
            break;
          }
        }
      }
      else
      {
        break;
      }    
    }
    else   // get out of offset funtion, back to menu or main screen
     {
        break;   
     }   
  } // end while(1) loop
  Controls.shut_dwn = TRUE;                                    // enable shut down feature when NO is pressed  
} // end of function

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
void soilvoids_menu () 
{
  enum buttons button;
  
  int enabled = 0;
  
  if ( Features.soil_air_voids_on == 1 )
  {
    enabled = 1;
  }
    
  
  SAV_enable_disable_text(enabled);
 
  // % Soil Air Voids OFF
  // Want to Enable?
  
  // % Soil Air Voids ON
  // Want to Disable?
  
  while(1)
  {
    button = getKey(TIME_DELAY_MAX);
    if((button == YES) || (button == NO) || (button == ESC) )
    {
     break;
    }    
  } 
  
  if((button == NO) || (button == YES))
  {
    CLEAR_DISP;
    if(((button == YES)  && ( enabled == 1)) || ((button == NO) && (enabled==0 ) ))  // Disable %soil air voids mode
    {
     Features.soil_air_voids_on = 0;
     if(Features.language_f)
     {
      LCD_position(LINE1);
      _LCD_PRINT("%%Soil Air Voids ");
      LCD_position(LINE2);
      _LCD_PRINT("   Disabled");  
     } 
     else
     {
      LCD_position(LINE1);
      _LCD_PRINT("%%Tiearra Aire Nulo  ");  // Soil air voids
      LCD_position(LINE2);
      _LCD_PRINT("   Deshabilitado");       // Disabled
     }
    }
    else if(((button == NO)  && ( enabled == 1)) || ((button == YES) && (enabled==0 ) ))  // Enable %soil air voids mode
    {
     Features.soil_air_voids_on = 1;
     if(Features.language_f)
     {
      LCD_position(LINE1);
      _LCD_PRINT("%%Soil Air Voids ");
      LCD_position(LINE2);
      _LCD_PRINT("   Enabled");  
     } 
     else 
     {
       LCD_position(LINE1);
      _LCD_PRINT("%%Tiearra Aire Nulo  ");
      LCD_position(LINE2);
      _LCD_PRINT("   Habilitado"); 
     }
    }
  } 
  // save struct Features to eeprom
  NV_MEMBER_STORE( FEATURE_SETTINGS, Features );
  CyDelay ( 1500 );
   
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
Bool enter_access_code(void)
{
  uint16_t access_number;
  char number_ptr[11] = "\0\0\0\0\0\0";  // enter a code here when ready to test
  
  enter_access_number_text();  //TEXT// display "Enter Access\,Number: " LINE1 and LINE2
  YES_to_Accept(LINE3);        //TEXT// display "YES to Accept"  
  ESC_to_Exit(LINE4);          //TEXT// display "ESC to Exit"      
  
  access_number = enter_number_std(number_ptr,LINE2+8,4,0);  // prompt user to enter access number 
  
  if( getLastKey() ==ESC)
  {
     return(0);
  }   
  else if(access_number==ACCESS_CODE)
  {
     return(1);
  }   
  else
  {
     access_number_incorrect_text();  //TEXT// display "   "Access Code\n    Incorrect" LINE2 and LINE3
  }   
  return(0);  
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
 
void memory_reset(void)  // Clears all Calibration Constants from Memory
{        
  enum buttons button;
  
  if(enter_access_code())
  {  
    Controls.shut_dwn = FALSE;                                // disable shutdown if no is pressed
    memory_reset_text();     //TEXT// display "Reset Memory\nto Default? on LINE1,2
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
    if(button == YES)
    {      
      auto_initialization();
      delay_ms(250);      
    }  
    Controls.shut_dwn = TRUE;                              // enable shut down feature when NO is pressed
  }
  else if( getLastKey() ==ESC)
  {
    return;
  }   
  else
  {
    hold_buzzer();
    delay_ms(2000);
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
 
void special_cal(void)  // Acquire/Enter Special Calibration Constants
{
  uint8_t count = 1, loopcnt = 0;  
  int32_t total_dens_cnts = 0;
  int32_t dens_cnt_avg = 0;
  float spec_depth, spec_den=0,spec_den_gcc = 0, spec_a, spec_c, spec_cal_b_value, CR_value; 
  char number_ptr[20] = NULL_NAME_STRING;
  char temp_str[20];
  enum buttons button;
  uint16_t moisture_cnt = 0;
  uint32_t density_cnt = 0;
 
  Flags.in_spec_cal = TRUE;  
  
  if(Spec_flags.nomograph_flag)
  {
    CLEAR_DISP;
    LCD_PrintAtPosition ("Disable Nomograph  ",LINE2);
    delay_ms(1000);
  }  
  else
  {  
    while(1)
    {
      Controls.shut_dwn = FALSE;  
      enable_disable_speccal_text(Spec_flags.spec_cal_flag);  //TEXT// display "Disable Special\nCalibration?" or Enable Special\nCalibration?" LINE1,2    
      Press_YES_or_NO(LINE3);                      //TEXT// display "Press YES or NO"   
      ESC_to_Exit(LINE4);                          //TEXT// display "ESC to Exit" 
      
      // Wait for YES or NO
      while(1)
      {
        button = getKey ( TIME_DELAY_MAX );
       
        if((button == ESC) || (button == YES) || (button == NO) || (button == MENU))
        {
          break;
        }  
      }    
      
          
      if((button == ESC) || (button == MENU))   // ESC-> exit to ready, MENU -> exit to menu
      {
         break;    
      }
  
      // disable the special cal flag and exit
      if(((button == YES) && Spec_flags.spec_cal_flag) || ((button == NO) && !Spec_flags.spec_cal_flag))   // disable special calibration
      {     
        Spec_flags.spec_cal_flag = FALSE;
        break;
      }    
      
      // The special cal is enabled
      // Get the stored special b value
      
      spec_cal_b_value = NV_RAM_MEMBER_RD(Constants.SPECIALCAL_B);    
    
        view_last_speccal_text();  //TEXT// display "View Last Saved\nSpec Cal. Data?" LINE1,2      
        Press_YES_or_NO(LINE3);    //TEXT// display "Press YES or NO"
        ESC_to_Exit(LINE4);        //TEXT// display "ESC to Exit"
          
        while(1)
        {
         button = getKey (TIME_DELAY_MAX );
          
          if((button == ESC) || (button == YES) || (button == NO) || (button == MENU))
          {
            break;
          }   
        }    
        
        if((button == ESC) || (button == MENU))
        {
          break;
        }
          
        // get special b value
        if(Spec_flags.gauge_derived)
        {
          spec_den = NV_RAM_MEMBER_RD(SPECIALCAL_DEN);   // store in kg/m3       
        }      
          // 0:BS, 1:2", 2:3"
        spec_depth = NV_RAM_MEMBER_RD(SPECIALCAL_DEPTH); 
        
        if(button == YES)
        {
          // view the special b value
          // 0:BS, 1:2", 2:3"
          CLEAR_DISP;   // DISPLAY SAVED CAL VALUES
          LCD_position(LINE1);
          display_depth ( 1, spec_depth ) ;
        
        
          if ( Spec_flags.gauge_derived )
          {
            LCD_position(LINE2);
            LCD_print ( "Dens =" );
            
            displayValueWithUnits ( spec_den, LINE2 + 7, temp_str );
                    
            LCD_position(LINE3);
            _LCD_PRINTF ( "B = %.5f", spec_cal_b_value);
          }
          else
          {
            LCD_position(LINE2);
            _LCD_PRINTF ( "B = %.5f", spec_cal_b_value);
          }
          vals_ok_text();  //TEXT// display "Vals. OK? YES/NO" on LINE4
           
          while(1)
          {
            button = getKey(TIME_DELAY_MAX);
            if((button == ESC) || (button == YES) || (button == NO) || (button == MENU))
            {
              break;
            }  
          }    
          if(button == YES) // the values are OK
          {
            Spec_flags.spec_cal_flag = TRUE;
            break;  // values are OK, exit function
          }
          else if((button == ESC) || (button == MENU))
          {
            break;
          }  
         
        } // end of view last saved sp. cal
       
      // User wants to change the values  
      use_gauge_text();        //TEXT// display "Use Gauge to Derive\nB Value?" LINE1,2    
      Press_YES_or_NO(LINE3);  //TEXT// display "Press YES or NO"
      ESC_to_Exit(LINE4);      //TEXT// display "ESC to Exit" 
          
      while(1)
      {
        button = getKey(TIME_DELAY_MAX);
        
        if((button == ESC) || (button == YES) || (button == NO) || (button == MENU))
        {
         break;
        }   
      }    
      
      if ( (button == ESC) || (button == MENU) )
      {
       break;    
      }   
      else if ( button == NO )      // NOT Gauge derived
      {
        // update tst_depth_g
        set_depth_manual();         // enter depth value         
        if(button == ESC)
        {
         break;    
        } 
        enter_value_text ( 0 );     //TEXT// display "Enter Value for\nB =" LINE1,2      
        Enter_to_Accept ( LINE3 );  //TEXT// display "ENTER to Accept 
        ESC_to_Exit ( LINE4 );      //TEXT// display "ESC to Exit" 
                        
        sprintf ( number_ptr, "%.5f", 0.0 );
        spec_cal_b_value = enterNumber ( number_ptr, LINE2+4, 8);
        
        if(button == ESC)        
        {
         break;            
        }

        NV_MEMBER_STORE(Constants.SPECIALCAL_B, spec_cal_b_value);    // store spec cal B value in memory
                
        NV_MEMBER_STORE(SPECIALCAL_DEPTH,tst_depth_g);      // store depth 
        
        if(spec_cal_b_value != 0)
        {
          Spec_flags.spec_cal_flag = TRUE;       // set special calibration flag
        }
       
        Spec_flags.gauge_derived = FALSE;        //  clear when SC values are not gauge derived.       
        break;  // values accepted, exit function      
      }            
      else      // use gauge derived values
      {
        if(NV_RAM_MEMBER_RD(ENTER_DEN_LATER) == 1)    // prompt for using stored dens cnts
        {
          use_stored_count_text();  //TEXT// display "Use Stored Count\nData?" LINE1,2        
          Press_YES_or_NO(LINE3);   //TEXT// display "Press YES or NO'
          ESC_to_Exit(LINE4);       //TEXT// display "ESC to Exit"    
              
          while(1)
          {
            button = getKey(TIME_DELAY_MAX);
            if((button == ESC) || (button == YES) || (button == NO) || (button == MENU))
            {
              break;
            }  
          }    
          if((button == ESC) || (button == MENU))
          {
            break;       
          }  
          else if(button == YES)   // use stored dens cnts
          { 
            enter_value_text(1);     //TEXT// display "Enter Value For\n Density =" LINE1,2         
            Enter_to_Accept(LINE3);  //TEXT// display "Enter to Accept" 
            ESC_to_Exit(LINE4);      //TEXT// display "ESC to Exit"
    
            displayValueWithUnits ( 0.0, LINE2+9, number_ptr ); 
            spec_den = enterNumber ( number_ptr, LINE2 + 9, 6 );
            spec_den = convertDensityToKgM3 ( spec_den );  // value in kg/m3
            spec_den /= 1000; // convert to GCC
            if ( getLastKey() == ESC )
            {
              break;          
            }
            
            CR_value = NV_RAM_MEMBER_RD ( SPECIALCAL_CR );
  
            spec_depth = NV_RAM_MEMBER_RD(SPECIALCAL_DEPTH);  // retrieve depth to get corresponding constants
                     
            spec_a = get_constant('a',spec_depth);
            spec_c = get_constant('c',spec_depth);
            spec_cal_b_value =  log( spec_a / (CR_value + spec_c)) /  spec_den ;  // b value in GCC
  
            
            CLEAR_DISP;
            LCD_position (LINE1);        
            _LCD_PRINTF ( "B = %.5f", spec_cal_b_value);  // display B value
            activate_speccal_text();  //TEXT// display "Activate Special\nCalibration?" LINE2,3          
            Press_YES_or_NO(LINE4);   //TEXT// display "Press YES or NO"
           
            while(1)
            {
              button = getKey(TIME_DELAY_MAX);
              if((button == ESC) || (button == YES) || (button == NO) || (button == MENU))
              {
                break;
              }  
            }    
            if(button == YES)
            {
              Spec_flags.spec_cal_flag = TRUE;      // set special calibration flag
              NV_MEMBER_STORE(ENTER_DEN_LATER,0);
              
              NV_MEMBER_STORE(Constants.SPECIALCAL_B, spec_cal_b_value);  // save B value to EEPROM 
                
              NV_MEMBER_STORE(SPECIALCAL_DEN,spec_den);                  // save density value to EEPROM  
            }
            else if(button == NO)
            {
              Spec_flags.spec_cal_flag = FALSE;           
            }  
            break;          
          } 
        }
  //USER WANTS TO GET VALUES FROM NEW COUNTS//   
        
        enter_number_counts_text();  //TEXT// display "Enter Number\nof Counts = " LINE1,2      
        
        Enter_to_Accept(LINE3);        //TEXT// display "Enter to Accept"
        
        ESC_to_Exit(LINE4);          //TEXT// display "ESC to Exit"    
        
        sprintf(number_ptr,"%d",0);
        
        loopcnt = enterNumber( number_ptr,LINE2 + 11, 3 );  // prompt user to enter number of counts
        
        if(button == ESC)
        {
          break;          
        }  
              
 
        set_depth_manual();
        if(button == ESC)
        {
          break;
        }  
             
        set_count_time();
        
        if(button == ESC)
        {
          break;
        }   
        count = 0;
        while ( loopcnt >= 1 )
        {
          //auto_depth_timer = 99;  //reset counter for getting auto depth        
          start_speccal_text();  //TEXT// display "# of Counts:\nDepth:\nPress Start to\nBegin Count"        
          LCD_position(LINE1+13);
          _LCD_PRINTF ( "%u",loopcnt);
          LCD_position(LINE2);
          display_depth(1,tst_depth_g);  
          
          while(1)
          {
            button = getKey(1000);
           
            if ( (button == ESC) || (button == ENTER) )
            {
               break;
            }   
          }
       
          if(button == ESC)
          {
            break;
          }  
             
          LCD_position(LINE3);
          _LCD_PRINT ( "                    ");
          LCD_position(LINE4);
          _LCD_PRINT ( "                    ");
          measurePulses(LINE4, cnt_time, &moisture_cnt, &density_cnt,tst_depth_g);    // take measurement 
          
          if(button == ESC)
          {
            break;
          }  
          
          total_dens_cnts += density_cnt;       
          speccal_results_text(loopcnt,density_cnt);  //TEXT// display "#5d Density\,Count: %u\nYES to Continue",loopcnt,density_cnt        
          ESC_to_Exit(LINE4);  //TEXT// display "ESC to Exit"
   
          loopcnt--;
          count++;
          while(1)
          {
            button = getKey(TIME_DELAY_MAX);
            if((button == YES) || (button == ESC) || (button == MENU))
            {
              break;        
            }  
          }    
          if(button != YES)
          {
            break;                  
          }  
        }
        if((button == ESC) || (button == MENU))
        {
          break;
        }  
        
        dens_cnt_avg = total_dens_cnts / count;
        CR_value = (float)dens_cnt_avg / (float)NV_RAM_MEMBER_RD(DEN_STAND); // calculate spec CR value
        
        NV_MEMBER_STORE(SPECIALCAL_CR, CR_value);
  
        NV_MEMBER_STORE(SPECIALCAL_DEPTH,tst_depth_g);
        NV_MEMBER_STORE(Constants.SPECIALCAL_B,0);  //reset b value until density is entered
  
        enter_value_text(2);    //TEXT// display "Enter Density\nValue Now?" LINE1,2      
        Press_YES_or_NO(LINE3); //TEXT// display "Press YES or NO"
        ESC_to_Exit(LINE4);     //TEXT// display "ESC to Exit"
        
        while(1)
        {
         button = getKey(TIME_DELAY_MAX);
          if((button == ESC) || (button == YES) || (button == NO) || (button == MENU))
          {
            break;
          }  
        }    
        if((button == ESC) || (button == MENU))
        {
          break;     
        }  
        else if(button == NO)
        {
          Spec_flags.spec_cal_flag = FALSE;
          NV_MEMBER_STORE(ENTER_DEN_LATER,1);                  // set flag for entering density later        
          break;
        }
        else  //yes, enter density now
        {
          enter_value_text(1);     //TEXT// display "Enter Value for\nDensity = " LINE1,2       
          Enter_to_Accept(LINE3);  //TEXT// display "Enter to Accept"        
          ESC_to_Exit(LINE4);      //TEXT// display "ESC to Exit"
   
          displayValueWithUnits ( 0.0, LINE2 + 9, number_ptr );
               
          spec_den = enterNumber ( number_ptr, LINE2+9, 6);
          
          if(button == ESC)
          {
             break;
          }   
         
          spec_den = convertDensityToKgM3 ( spec_den );
          spec_den_gcc = spec_den/1000;
          // Explorer II uses GCC for density constants.
          spec_depth = tst_depth_g;  
          spec_a = get_constant('a',spec_depth);
          spec_c = get_constant('c',spec_depth);
         	spec_cal_b_value = log( spec_a / (CR_value + spec_c)) / spec_den_gcc;  // units in GCC
          
          CLEAR_DISP;
          LCD_position (LINE1);        
          _LCD_PRINTF ( "B = %.5f", spec_cal_b_value );
         
          activate_speccal_text(); //TEXT// display "Activate Special\nCalibration?" LINE2,3      
          Press_YES_or_NO(LINE4);  //TEXT// display "Press YES or NO"
          
          while(1)
          {
            button = getKey(TIME_DELAY_MAX);
            
            if((button == ESC) || (button == YES) || (button == NO) || (button == MENU))
            {
               break;
            }   
          }           
          if(button == YES)
          {          
            Spec_flags.gauge_derived = TRUE;
            Spec_flags.spec_cal_flag = TRUE;                // set special calibration flag 
 
            NV_MEMBER_STORE(Constants.SPECIALCAL_B, spec_cal_b_value);  // in GCC units
            NV_MEMBER_STORE(SPECIALCAL_DEN,spec_den);       // store in kg/m3   units
            NV_MEMBER_STORE(ENTER_DEN_LATER,0);                 
          }
          else if(button == NO)
          {
            Spec_flags.spec_cal_flag = FALSE;
          }  
          break;
        }
      }
    }  
  }
  if(Spec_flags.spec_cal_flag)
  {
    speccal_enabled_text();  //TEXT// display " Special Calibration\n    Enabled" LINE2,3    
    delay_ms(1000);
  }
  Flags.in_spec_cal = FALSE;
  Controls.shut_dwn = TRUE;   // re-enable shut down 
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
 
void nomograph(void)  // allows user to enter  Nomograph values and enable/disable the feature
{
  // These were updated to match the 3500 in revision 1.14
  const float A1 = 1.174636, A2 = .728389, A3 = 0.117793;
  float   x, db;
  char number_ptr[20] = NULL_NAME_STRING;
  enum buttons button;
  float KK,correction;
  
  if(Spec_flags.spec_cal_flag)  // if special cal is enabled, exit
  {
    CLEAR_DISP;
    LCD_PrintAtPosition("Disable Special Cal ",LINE2);
    delay_ms(1000);
    return;
  }   
  while(1)
  {
    Controls.shut_dwn = FALSE;  
    enable_disable_nomograph_text(Spec_flags.nomograph_flag);  //TEXT// display "Enable Nomograph\nMode?" or "Disable  Nomograph\nMode?" LINE1,2    
    Press_YES_or_NO(LINE3);                          //TEXT// display "Press YES or NO" 
    ESC_to_Exit(LINE4);                              //TEXT// display "ESC to Exit"
         
    while(1)
    {
      button = getKey(TIME_DELAY_MAX);
      if((button == ESC) || (button == YES) || (button == NO) || (button == MENU))
      { break ; }
    }    
    
    if((button == ESC) || (button == MENU))   // ESC -> exit to ready, MENU -> exit to menu
    {
     break ;
    }    
    
    if(((button == YES) && Spec_flags.nomograph_flag) || ((button == NO) && !Spec_flags.nomograph_flag))   // disable special calibration
    {
      Spec_flags.nomograph_flag = FALSE;
      break;
    }
    // This is in mm
    x = NV_RAM_MEMBER_RD(MAT_THICK);
    
    if(x >= 20)            // Does  Nomograph const. exists in memory?
    {
      activate_saved_vals_text();  //TEXT// display "Activate Saved\nValues?" LINE1,2      
      Press_YES_or_NO(LINE3);      //TEXT// display "Press YES or NO"
      ESC_to_Exit(LINE4);          //TEXT// display "ESC to Exit"
 
      while(1)
      {
        button = getKey(TIME_DELAY_MAX);
        if((button == ESC) || (button == YES) || (button == NO) || (button == MENU))
        {
          break ;
        }
      }    
      
      if((button == ESC) || (button == MENU))
      { 
        break ;
      }      
      else if ( button == YES )  // use existing  Nomograph values from memory
      {
        // Density in kg/m3
        //Bot_Dens = NV_RAM_MEMBER_RD(BOTTOM_DENS);  
                
        nomograph_vals_text();  //TEXT// display "Mat Thick =\nBot Dens =\nAre Values OK?
        Press_YES_or_NO(LINE4);  //TEXT// display "Press YES or NO"        
        
        // display thickness
        if ( Features.SI_units == FALSE )
        {
          // Convert mm to inches
          sprintf ( number_ptr, "%.1f in", (x / 25.4) );
        }
        else 
        {
          // Display mm
          sprintf ( number_ptr, "%.0f mm", x );
        }
        // Thickness update
        LCD_PrintAtPosition ( number_ptr,LINE1+10);
     
        // display bottom density
        displayValueWithUnits ( NV_RAM_MEMBER_RD(BOTTOM_DENS), LINE2 + 10, number_ptr);
        
        // wait for ESC,YES,NO, or MENU to be pressed
        while(1)
        {
         button = getKey(TIME_DELAY_MAX);
          if((button == ESC) || (button == YES) || (button == NO) || (button == MENU))
          {
            break ;
          }
        }    
        
        if((button == ESC) || (button == MENU))
        { 
          break ;
        }
        else if(button == YES)  //YES was pressed
        {
          Spec_flags.nomograph_flag = TRUE;                                   // set  Nomograph flag                
          break;  // go to display  Nomograph enabled at end of function
        }
      }
    }
    x = 0;
    enter_mat_thickness_text(); //TEXT// display "Enter Material\nThickness:" LINE1,2    
    Enter_to_Accept(LINE3);     //TEXT// display "Enter to Accept"
    ESC_to_Exit(LINE4);         //TEXT// display "ESC to Exit"
 
    // display thickness
    if ( Features.SI_units == FALSE )
    {
     // Convert mm to inches
     sprintf ( number_ptr, "%.1f in", (x / 25.4) );
     x = enterNumber ( number_ptr, LINE2+11, 6 );  // prompt user to enter mat thick value in inches
     if ( x == 999999.0 )
     {
      button = ESC;
     } 
    
     x *= 25.4; // convert x to mm
    }
    else 
    {
     // Display mm
     sprintf ( number_ptr, "%.0f mm", x );
     x = enterNumber ( number_ptr, LINE2+11, 6 );  // prompt user to enter mat thick value in mm
     if ( x == 999999.0 )
     {
       button = ESC;
     } 
    }
    
    // Skip if ESC pressed
    if ( button == ESC )
    { 
      break ; 
    }
    
    // store thickness in mm's
    NV_MEMBER_STORE( MAT_THICK, x ); 
   
    // convert to inches for calculation
    x /= 25.4;  
    
    // Get effect of top layer    
    KK = (A1 * exp(-A2 * x)) - A3;         
    // value can go negative if x > 2.8inch
    if ( KK < 0.0 )
    {
      KK = 0;
    }  
    
    
    // Correction for layers < 1.5"
    if ( x <= 1.5 )
    {
      correction = (-5.44 * x) + 8.821;
    }
    else // correction is 0 if > 1.5"
    {
      correction = 0.0;
    }  
    // Same the correction value in NV memory as kg/m3
    correction /= KG_PCF_CONV;
    NV_MEMBER_STORE(NOMOGRAPH_CORRECTION,correction);
    
    db = 0;
    enter_bot_dense_text();  //TEXT// display "Enter Bottom\nDensity:" LINE1,2    
    Enter_to_Accept(LINE3);  //TEXT// display "Enter to Accept"
    ESC_to_Exit(LINE4);      //TEXT// display "ESC to Exit"
 
   
    // display bottom density
    displayValueWithUnits ( db, LINE2 + 10, number_ptr);   
 
    db = enterNumber ( number_ptr,LINE2+10, 6);    
    
    if(db == 999999.0)
    {
      Spec_flags.nomograph_flag = FALSE;
      break;     
    }
    
    NV_MEMBER_STORE(KK_VALUE,KK);
    
    // density was entered as kg,gcc, or pcf. Convert to kg/m3
    db = convertDensityToKgM3 ( db );
    
    NV_MEMBER_STORE(BOTTOM_DENS, db);  
   
    Spec_flags.nomograph_flag= TRUE;
    
    break;
  }
  if(Spec_flags.nomograph_flag)
  {
    nomograph_enabled_text();  //TEXT// display "    Nomograph\n     Enabled" LINE2,3   
    delay_ms(1000);    
  }
  Controls.shut_dwn = TRUE;   // re-enable shut down   
}

 /******************************************************************************
 *  Name:
 *  PARAMETERS:None
 *  DESCRIPTION: 
 *  RETURNS: 
 *****************************************************************************/ 
uint32 getSerialNumber ( void ) {
  // serial_number can only be up to 65535. If it needs to be bigger, than the serial number is stored into 
  // serial_number_2. If serial_number byte size changed, it would mess up the other eeprom data, so new
  // serial_number_2 uint32 is used.
  uint32 serial_number = (uint32)NV_RAM_MEMBER_RD(Constants.SERIAL_NUM);  // get serial number
  uint32 serial_number_2 = NV_RAM_MEMBER_RD (SERIAL_NUM_HI);
  if (( serial_number_2  <  99999999 ) &&  ( serial_number_2  > serial_number ) ) {
    serial_number = serial_number_2;
  }
   return serial_number;
}
 /******************************************************************************
 *  Name:
 *  PARAMETERS:None
 *  DESCRIPTION: 
 *  RETURNS: 
 *****************************************************************************/ 
void setSerialNumber ( uint32 serial_number )
  {
  // serial_number can only be up to 65535. If it needs to be bigger, than the serial number is stored into 
  // serial_number_2. If serial_number byte size changed, it would mess up the other eeprom data, so new
  // serial_number_2 uint32 is used.
    
  uint32 serial_number_2;
    
  if ( serial_number > 65535 ) // uint32 0-4,294,967,295
  {
    serial_number_2 = serial_number;
    serial_number = 0;
  }
  else
  {
    serial_number_2 = 0; // why not just store the lower one here also?
  }
  // store both the 16 and 32 bit serial numbers
  NV_MEMBER_STORE(Constants.SERIAL_NUM, serial_number);        
  NV_MEMBER_STORE(SERIAL_NUM_HI, serial_number_2);  
  
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
 
void serial_number ( Bool source )  //Enter or Edit Serial Number 0 is gauge, 1 is computer
{
  char num[9] = "        "; // 8 + \0
  uint16_t  timeout = 0, timer = 0;
  uint32 serial_number;
  enum buttons button;
  
  serial_number = getSerialNumber();        

  if(source)                                          // Access serial # from serial port
  {    
    while(1)
    {
      clearComputerRXBuffer();
     delay_ms(500);
      _UART2_PRINT ("Enter the 4 digit operator access code <Enter>\n\r");
      delay_ms(500);
      gets_comp(num);
      
      if(atol(num) == ACCESS_CODE)
      {
        _UART2_PRINT ( "Serial Number: ");
        delay_ms(500);
        _UART2_PRINTF ("%lu\n\r", serial_number);
        delay_ms(500);
        _UART2_PRINT ("Change the Serial #? Y or N\n\r");
        delay_ms(500);

        while( (getRXNumEntries(&s_Computer_RX) == 0 ) && (timeout < 200))  //wait 20 seconds for input
        {
          delay_us(10);
          if(++timer == 10000)
          {
            timer = 0;
            ++timeout;
          }
        }
        if (getRXNumEntries(&s_Computer_RX) > 0 )
        {
         if (waitc_RX(&s_Computer_RX, 600000) == 'Y')                   //change serial # via serial port interface
          {
            _UART2_PRINT ( "Enter the new 5 digit Serial #\n\r");
            delay_ms(500);
            gets_comp(num);
            _UART2_PRINTF ( "New Serial Number: %s\n\r", num);       
            delay_ms(500);     
            // needs to be 5 digit number
            serial_number = atol(num);
            setSerialNumber ( serial_number );
          }
          break;
        }
      }     // if(atol(num) == ACCESS_CODE)
    }       // while(true)
  }        //  if(!No_skip)
  else                                                     // Access serial # from keypad and LCD
  {     
    //////////////Confirm Access code then display serial number//////////////////////////////////////
    if(enter_access_code())                   // confirm access code
    {
      Controls.shut_dwn = FALSE;                             
      CLEAR_DISP;
      LCD_position (LINE1);
      _LCD_PRINT ( "Ser. Num:");
      LCD_position (LINE1 + 10);
      _LCD_PRINTF ( "%lu", serial_number);
      LCD_position (LINE2);
      _LCD_PRINT ( "Change Serial #?");
      Press_YES_or_NO(LINE3);
      ESC_to_Exit(LINE4);
       
      while(1)
      {
        button = getKey(TIME_DELAY_MAX);
        if((button == ESC) || (button == YES) || (button == NO))
        { break ; }
      }    
      Controls.shut_dwn = TRUE;
      if(button != YES)  // button other than YES was pressed, exit to menu
        return;     
     
      while ( 1 )
      {
        CLEAR_DISP;
        LCD_position(LINE1);
        _LCD_PRINT ( "Enter New Serial");
        LCD_position(LINE2);
        _LCD_PRINT ( "Number:");
        YES_to_Accept(LINE3);   
        ESC_to_Exit(LINE4); 
          
        sprintf(num,"        ");  //8 + \0
        
        serial_number = enter_number_std(num,LINE2+8,8,FALSE);   // prompt user to enter new serial number
           
        if(button == ESC)
          return;

        break;
      }
    setSerialNumber ( serial_number );
    ResetBleModule(true);
      
    }
    else if( getLastKey()==ESC)
    {
       return;
    }   
    else
    {
      hold_buzzer();
       
      delay_ms(2000);
    }     
  }    
  Controls.shut_dwn = TRUE;                                    // enable shut down feature when NO is pressed   
}
 
   

/******************************************************************************
 *  Name: ma_pr(void)
 *  
 *  PARAMETERS: Enter Marshal and Proctor Constants (MA_PR button initiates)
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 
 
void ma_pr(void)  // 
{ 
  float  num_temp;
  char number_ptr[20] =  NULL_NAME_STRING ;
  enum buttons button;
  
  Controls.shut_dwn = FALSE;
  prma_text();             //TEXT// display "1. Proctor\n2. Marshall" LINE1,2
  up_down_select_text(0);  //TEXT// display "Select #, ESC Exit" LINE4
  

  
  while(1)
  { 
    while(1)
    {
     button = getKey(TIME_DELAY_MAX);
     
     if((button == 1) || (button == 2) || (button == ESC) || (button == 3))
      {  
        break;
      }  
    }    
    if(button == ESC)
    {
      break;                                     
    }    
    if(button == 1)  //Proctor selected
    {     
      // This is in kg_m3 units
      num_temp = NV_RAM_MEMBER_RD(PROCTOR);          
     
      CLEAR_DISP;
      LCD_position(LINE1);
      _LCD_PRINT ( "PR:" );
      
      
      // Convert the density value to the proper units and display
      // returns the value in proper units (kg,pcf,gcc)
      num_temp = displayValueWithUnits ( num_temp, LINE1 + 4, number_ptr );
      
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
      enter_value_text(3);   //TEXT// display "Enter Value for\nProctor:" LINE1,2
      Enter_to_Accept(LINE3);  //TEXT// display "Enter to Accept"
      ESC_to_Exit(LINE4);    //TEXT// display "ESC to Exit"
 
      num_temp = enterNumber ( number_ptr, LINE2+9, 6 );
     
      // If esc hit, don't save a new ENTRY
      if ( getLastKey() == ESC )
      {
        break;      
      }  
      // Convert to KG/M3 before storing the density
      num_temp = convertDensityToKgM3 ( num_temp );
      NV_MEMBER_STORE(PROCTOR,num_temp);
     
      //Display the new PR:
      // This is in kg_m3 units
      num_temp = NV_RAM_MEMBER_RD(PROCTOR);          
     
      CLEAR_DISP;
      LCD_position(LINE1);
      _LCD_PRINT ( "PR:" );
            
      // Convert the density value to the proper units and display
      // returns the value in proper units (kg,pcf,gcc)
      displayValueWithUnits ( num_temp, LINE1 + 4, number_ptr );     
      delay_ms(1000);
     
      break;   
    }
     else if ( button == 2 )// Marshall selected
    {
       // This is in kg_m3 units
      num_temp = NV_RAM_MEMBER_RD(MARSHALL);     
      
      CLEAR_DISP;
      LCD_position(LINE1);
      _LCD_PRINT ( "MAX:");
      
      // Convert the density value to the proper units and display
      // returns the value in proper units (kg,pcf,gcc)
      num_temp = displayValueWithUnits ( num_temp, LINE1 + 4, number_ptr );
      
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
      
      enter_value_text(4);   //TEXT// display "Enter Value for\nMarshall:" LINE1,2
      Enter_to_Accept(LINE3);  //TEXT// display "YES to Accept" 
      ESC_to_Exit(LINE4);    //TEXT// display "ESC to Exit"
       
      num_temp = enterNumber ( number_ptr, LINE2 + 10, 6 );
      
     // If esc hit, don't save a new ENTRY
      if ( getLastKey() == ESC )
      {
        break;      
      }      
       // Convert to KG/M3 before storing the density
      num_temp = convertDensityToKgM3 ( num_temp );
      NV_MEMBER_STORE(MARSHALL,num_temp);
      
      CLEAR_DISP;
      LCD_position(LINE1);
      _LCD_PRINT ( "MAX:");
      
      // Convert the density value to the proper units and display
      // returns the value in proper units (kg,pcf,gcc)
      displayValueWithUnits ( num_temp, LINE1 + 4, number_ptr );
      delay_ms(1000);
      
      break;   
    }
    else if (( button == 3 )&& ( Features.soil_air_voids_on == 1 ))
    {
     break;
    }
  }
  
    if (( button == 3 ) && ( Features.soil_air_voids_on == 1 )) // Soil Specific Gravity
    {

      while ( 1 )
      {
       // This is in kg_m3 units
       num_temp = NV_RAM_MEMBER_RD(SOIL_GRAVITY);  
       if (  isNaN( (uint8_t *)(&num_temp))  || (  num_temp > 3500 ) || ( num_temp < 2000 ) ) 
       {
        num_temp = 2700 ;
        NV_MEMBER_STORE(SOIL_GRAVITY,num_temp);
       }
        CLEAR_DISP;
        LCD_position(LINE1);
        _LCD_PRINT ( "Soil SG:");
        
        // Convert the density value to the proper units and display
        // returns the value in proper units (kg,pcf,gcc)
        num_temp = displayValueWithUnits ( num_temp, LINE1 + 9, number_ptr );
        
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
          Controls.shut_dwn = TRUE;                      // enable shut down feature when NO is pressed      
          return; 
        }  
        
        enter_value_text(5);   //TEXT// display "Enter Value for\nSoil SG:" LINE1,2
        Enter_to_Accept(LINE3);  //TEXT// display "YES to Accept" 
        ESC_to_Exit(LINE4);    //TEXT// display "ESC to Exit"
         
        num_temp = enterNumber ( number_ptr, LINE2 + 10, 6 );
        
       // If esc hit, don't save a new ENTRY
        if ( getLastKey() == ESC )
        {
         Controls.shut_dwn = TRUE;                      // enable shut down feature when NO is pressed      
         return;     
        }      
         // Convert to KG/M3 before storing the density
        num_temp = convertDensityToKgM3 ( num_temp );
        if ((  num_temp > 3500 ) || ( num_temp < 2000 )) 
        {
         LCD_position(LINE1);
         _LCD_PRINT ( "Value out or Range!");
         delay_ms ( 2000 );
        } 
         else
         { 
           break;
         } 

      }    
      
      NV_MEMBER_STORE(SOIL_GRAVITY,num_temp);
      
      CLEAR_DISP;
      LCD_position(LINE1);
      _LCD_PRINT ( "Soil SG:");
      
      // Convert the density value to the proper units and display
      // returns the value in proper units (kg,pcf,gcc)
      displayValueWithUnits ( num_temp, LINE1 + 9, number_ptr );
      delay_ms(1000);

   }  
      
  Controls.shut_dwn = TRUE;                      // enable shut down feature when NO is pressed  
}


/******************************************************************************
 *  Name: ma_pr(void)
 *  
 *  PARAMETERS: Enter Marshal and Proctor Constants (MA_PR button initiates)
 *
 *  DESCRIPTION: 
 *
 *  RETURNS: 
 *
 *****************************************************************************/ 
 
void idle_shutdown(void)  // 
{ 
  float  num_temp;
  char number_ptr[20] =  NULL_NAME_STRING ;
  enum buttons button;
  Controls.shut_dwn = FALSE;
  
  while(1)
  { 
    // This is in kg_m3 units
    num_temp = NV_RAM_MEMBER_RD(SHUT_DOWN_TIME_HOURS);          
   
    CLEAR_DISP;
    LCD_position(LINE1);
    _LCD_PRINTF ("Shtdwn Time Hours:%2u",(uint8)num_temp );
       
    change_val_text();       //TEXT// display "Change Value?" LINE2     
    Press_YES_or_NO(LINE4);  //TEXT// display "Press YES or NO"

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
    enter_value_text(6);     //TEXT// display "Enter Value for\nSHT DWN(HOURS):" LINE1,2
    Enter_to_Accept(LINE3);  //TEXT// display "Enter to Accept"
    ESC_to_Exit(LINE4);      //TEXT// display "ESC to Exit"

    num_temp = enterNumber ( number_ptr, LINE2+17, 2 );
   
    if ( num_temp < 1 )
      num_temp = 1;
    
    // If esc hit, don't save a new ENTRY
    if ( getLastKey() == ESC )
    {
      break;      
    }  

    NV_MEMBER_STORE(SHUT_DOWN_TIME_HOURS,(uint8)num_temp);
   
    //Display the new shut dwon time:
    num_temp = NV_RAM_MEMBER_RD(SHUT_DOWN_TIME_HOURS);          
   
    CLEAR_DISP;
    LCD_position(LINE1);
    _LCD_PRINTF ( "SHT DWN TIME(HRS):%2u",(uint8)num_temp );
    delay_ms(1000);
    break;
  }  
      
  Controls.shut_dwn = TRUE;                      // enable shut down feature when NO is pressed  
}

