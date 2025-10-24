/**************************   ***************************************************************** ***************************/
//
// Title:       Elite
// Company:     Instrotek
//
// Document No.:
// Issue:
// Description:
//
// Filename:
// Author:
//
//
//
// History: date, version, personal initials with comments about any changes
//
/**********************************************************************************************************************/
/*********************************************  INCLUDE FILES  ***************************************************/
#include "project.h"
#include "Globals.h"
#include "DataStructs.h"
#include "StoreFunctions.h"
#include "Utilities.h"
#include "conversions.h"
#include "Alfat.h"
#include "prompts.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "ProjectData.h"
#include "SDcard.h"
#include "UARTS.h"
/************************************* EXTERNAL FUNCTION DECLARATIONS  *************************************/
extern float convertKgM3DensityToUnitDensity ( float value_in_kg, uint8_t units );
extern  uint8_t getCalibrationDepth ( uint8_t depth_inches );
extern uint32 getSerialNumber ( void );
/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/
project_info_t project_info;
/************************************************  LOCAL DEFINITIONS  *************************************************/
/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/
char final_str[512];
/******************************************************************************
 *  Name: updateProjectInfo ( void )
 *****************************************************************************/
 uint8 updateProjectInfo ( void )
 {
    char current_p_name[PROJ_NAME_LENGTH] ;
  //  FS_FILE *p_file = null;
      // read total number of projects stored on SD card
    if ( getProjectNumber() == 0 )
    {
      resetProjectStorage();
    }
    getActiveProjectEE ( current_p_name ); // get the current project name from EEPROM;
    // read total number of projects stored on SD card
    project_info.number_of_projects = getProjectNumber();
    if ( SD_CheckIfProjExists ( current_p_name ) != false )
    {
      if ( project_info.number_of_projects > 0 )
      {
        //read total number of stations within current project
        project_info.station_index     = getStationNumber ( current_p_name );
        // get project name
        strcpy( project_info.current_project,current_p_name );
      //set auto name option for current project
      Flags.auto_number_stations          = checkStationAutoNumber( current_p_name );    //set auto name option for current project
      project_info.station_auto_en = Flags.auto_number_stations;
      if ( project_info.station_auto_en == AUTO_NUMBER_ON )
      {
        project_info.station_start = getAutoStationStart ( current_p_name );
      }
     }
    }
    else
    {
     Features.auto_store_on = FALSE;
     return false;
    }
    return true;
 }
/******************************************************************************
 *
 *  Name: start_new_project ( void )
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *
 *
 *  RETURNS:
 *
 *****************************************************************************/
/////STORAGE FUNCTIONS//////////////////////////////////////////////////////////
void start_new_project ( void )  // leads user through setup for new project
{
  Bool auto_temp = 0, escape = 0;
  uint8_t  go_to_screen = 0, lcd_line ;
  FS_FILE *nfile;
  uint16_t station_start = 0;
  enum buttons button;
  //array that enter_name stores the name
  char project[PROJ_NAME_LENGTH]  = NULL_NAME_STRING;
//  char station_name[PROJ_NAME_LENGTH] = NULL_NAME_STRING;
  char num_temp[5] = "1";
  SD_Wake();
  // loop until break
  while(!escape)
  {
    CLEAR_DISP;
    switch ( go_to_screen )
    {
      case 0:
          enter_project_name_text ( project );  //TEXT// display "Enter Project \nName: %s",project LINE1,2
          YES_to_Accept(LINE3);              //TEXT// display "YES to Accept"
          ESC_to_Exit(LINE4);                //TEXT// display "ESC to Exit"
          lcd_line = (Features.language_f) ? LINE2+6 : LINE2+10;
          enter_name ( project,lcd_line) ;
          button = getLastKey();
          if(button == ESC)
          {
            escape = TRUE;
          }
          else if (button == YES)        // YES was pressed to accept entered project name
          {
            if ( project[0] != '\0' )   // make sure something was entered for the project name
            {
              if ( true  == SD_CheckIfProjExists ( project ) )
              {
               CLEAR_DISP;
               LCD_PrintAtPosition ("Project Exists!",LINE2);
               ESC_to_Exit(LINE4);                //TEXT// display "ESC to Exit"
               CyDelay ( 1500 );
               go_to_screen = 0;
              }
              else
              {
               // create file
               nfile = SD_CreateProjectSimpleFile ( project );
               // RESET STATION
               clearStationNumber ( project );
               setStationAutoNumber ( project, AUTO_NUMBER_ON ); // make station auto number
               if ( nfile == null )
               {
                escape = TRUE;
                continue;
               }
              else
               {
                go_to_screen = 1;        // if not, stay on enter name screen.
               }
              }
            }
          }
          break;
      case 1:
            project_name_text(project);  //TEXT// display "    Project Name\n    %s",project LINE2,3
            hold_buzzer();
            delay_ms(2000);
            go_to_screen = 6; // Get measurement type (Normal or Thin layer)
            break;
     case 6:
          // Measurement Type by default is Normal
          go_to_screen = 2;        // if not, stay on enter name screen.
          break;
     case 2:
            station_mode_text();    //TEXT// display "Station Name Mode\n1. Auto (sequential)\n2. Manual Entry" LINE1,2,3
            up_down_select_text(0); //TEXT// display "Select #, ESC to Exit" LINE4
            while(1)
            {
              button = getKey( TIME_DELAY_MAX );
              if(button == 1 || button == 2 || button == ESC)
              {
                break;
              }
            }
            if(button == ESC)
            {
              escape = TRUE;
            }
            else if ( button == 1 )
            {
              go_to_screen = 3;
            }
            else if(button == 2)
            {
              go_to_screen = 4;
            }
            break;
      case 3:
            // Auto Number Enabled
            station_start_text(num_temp);  //TEXT// display "Starting Station\nNumber: %s",num_temp
            YES_to_Accept(LINE3);          //TEXT// display "YES to Accpet"
            ESC_to_Exit(LINE4);            //TEXT// display "ESC to Exit"
            auto_temp = TRUE;
            station_start = (uint16)enter_number_std ( num_temp, LINE2 + 8, 4, 0 );
            button = getLastKey();
            if(button == ESC)
            {
              escape = TRUE;
            }
            else if(button == YES)
            {
              go_to_screen = 4;
            }
            break;
      case 4:
              if ( auto_temp )
              {
                station_increment_text ( station_start );  //TEXT// display "Station # %u\nIncrements by 1\nFor Each Reading",station_start
                //save first station name
                sprintf ( project_info.current_station_name,"%u",station_start );
                //write name to first station name location in project
                writeAutoStationStart ( project, station_start );
                station_start = getAutoStationStart ( project );
                hold_buzzer();
                delay_ms(2000);
              }
              else
              {
                station_name_text();  //TEXT// display "Enter Station Name\nBefore Each Reading"
                hold_buzzer();
                delay_ms(2000);
              }
              // RESET STATION
              clearStationNumber ( project );
              // AUTO NUMBER MODE
              if (auto_temp)
              {
                setStationAutoNumber ( project, AUTO_NUMBER_ON );
              }
             else
              {
                setStationAutoNumber ( project, AUTO_NUMBER_OFF );
              }
              //enable AUTO STORE
              if( !Features.auto_store_on )  //auto store is not enabled, ask if they want to enable it
              {
                button = enable_disable_features('O');
              }
              escape = TRUE;
               //set auto name option for current project
              Flags.auto_number_stations = checkStationAutoNumber( project );    //set auto name option for current project
              setActiveProjectEE ( project ); // Put the active project name into EEPROM
              break;
      case 5:
            project_limit_text();  //TEXT// display "Project Limit\nExceeded, Delete\nOne or More Project" LINE1,2,3
            hold_buzzer();
            delay_ms(2000);
            escape = TRUE;
            break;
    }
  }
}
/******************************************************************************
 *
 *  Name: getProjectIndex (  )
 *
 *  PARAMETERS:  Name for project selected
 *
 *  DESCRIPTION: Allows user to select a project for review.normal_data
 *
 *
 *  RETURNS: 00 if selected or error (17)
 *
 *****************************************************************************/
uint16_t getProjectIndex ( char * project )
{
  uint16_t i = 0, display_index = 1, project_count;
  enum buttons button;
  for ( i = 0; i < PROJ_NAME_LENGTH; i++ )
  {
    project[i] = '\0';
  }
  SDstart(); // enable power the SD card
  project_count = getProjectNumber() ;
  CURSOR_OFF;
  if ( project_count == 0 )
  {
    no_stored_projects_text();  //TEXT// display "    No Projects\n      Stored" LINE2,3
    hold_buzzer();
    SDstop(null );
    delay_ms(1000);
    return ( 17);
  }
   //TEXT// display "   Select Project\n    From List" LINE2,3
  select_from_list_text(0);
  hold_buzzer();
  delay_ms(1000);
  while(1)
  {
  LCD_position(LINE1);
  _LCD_PRINT ("                    ");
  LCD_position(LINE1);
  // Get project name
  SD_FindFile  (  display_index, "\\Project\\", project, false );
  sprintf ( lcdstr, "%u. %s",display_index, project );
  LCD_print (lcdstr);
  up_down_ENTER_select_text();
  while(1)
  {
    button = getKey ( TIME_DELAY_MAX );
    if((button <= 11) || (button == ESC))
    {
      break;
    }
  }
  if ( button == ESC )
  {
    SDstop(null );
    return (17);
  }
  if ( button == UP )
  {
    --display_index;
    if ( display_index == 0 )
    {
      display_index = project_count ;
    }
  }
  else if(button == DOWN)
  {
    if(++display_index > project_count)
    {
      display_index = 1;
    }
  }
  else if(button == ENTER )
  {
   // Project XXXX Selected
   CLEAR_DISP;
   LCD_position(LINE1);
   sprintf ( lcdstr, "%s ", project );
   LCD_print (lcdstr);
   LCD_position(LINE2);
   LCD_print ( "    Selected");
   CyDelay ( 2000 );
   SDstop(null );
   return 0; // file selected
  }
 }// end while(1)
  SDstop(null );
 return (17);
}
/******************************************************************************
 *
 *  Name: getStation (  )
 *
 *  PARAMETERS: Project
 *
 *  DESCRIPTION: Allows user to select a station for review.normal_data
 *
 *
 *  RETURNS: station index or error (17)
 *
 *****************************************************************************/
uint32_t getStation ( char * project )
{
  uint16 station_count;
  uint16_t display_index;
  char station[PROJ_NAME_LENGTH] = NULL_NAME_STRING;
  enum buttons button;
  FS_FILE* nfile = null;
  if ( SD_CheckIfProjExists ( project ) == false )
  {
    return 17;
  }
  // open the file for read,write
  nfile = SDProjOpen ( project );
  if ( nfile == null )
  {
    return 17;
  }
  //read number of stations from eeprom
  station_count = getStationNumber ( project );
  if ( station_count == 0 )
  {
    no_data_stored_text();  //TEXT// display "   No Data Stored"  LINE2
    hold_buzzer();
    delay_ms(1000);
    return(17);
  }
  display_index = 0;
  select_from_list_text(1); //TEXT// display "   Select Station\n     From List"  LINE2,3
  hold_buzzer();
  delay_ms ( 1000 );
  while(1)
  {
    LCD_position(LINE1);
    _LCD_PRINT ("                    ");
    LCD_position(LINE1);
    // Get station name and display it
    getStationName (  project, station , display_index );
    sprintf (lcdstr, "%s", station );
    LCD_print ( lcdstr );
    up_down_ENTER_select_text();
    while(1)
    {
      button = getKey(TIME_DELAY_MAX);
      if((button <= 11) || (button == ESC) || (button == ENTER ) )
      {
        break;
      }
    }
    if(button == ESC)
    {
      return(17);
    }
    if(button == UP)
    {
      if(display_index == 0)
      {
        display_index = station_count-1;
      }
      else
      {
        display_index--;
      }
    }
    else if(button == DOWN)
    {
      if( display_index >= ( station_count - 1 ))
      {
        display_index = 0;
      }
      else
      {
       display_index++;
      }
    }
   else if(button == ENTER )
   {
     // Display the station name ans return the station number
     CLEAR_DISP;
     _LCD_PRINTF ( "Station : %s", station );
     delay_ms(1500);
     return ( display_index );
   }
 }
}
/******************************************************************************
 *
 *  Name: select_stored_project ( uint8_t function )
 *
 *  PARAMETERS:
 *
 *  DESCRIPTION:
 *  //select proj. (0=resume,1=delete,2=display)
 *
 *  RETURNS: selected station number of selected project, and proj name, or 17 is esc
 *
 *****************************************************************************/
uint16_t select_stored_project ( uint8_t function, char* prj_name )
{
  uint16_t  project_count, selection, display_index, station_count ;
  char station[PROJ_NAME_LENGTH] = NULL_NAME_STRING;
  enum buttons button;
  project_count = getProjectNumber() ;
  CURSOR_OFF;
  if ( project_count == 0 )
  {
    no_stored_projects_text();  //TEXT// display "    No Projects\n      Stored" LINE2,3
    hold_buzzer();
    SDstop(null);
    delay_ms(1000);
    return ( 17);
  }
  //TEXT// display "   Select Project\n    From List" LINE2,3
  select_from_list_text(0);
  hold_buzzer();
  delay_ms(1000);
  display_index = 1;
  while(1)
  {
      LCD_position(LINE1);
      _LCD_PRINT ("                    ");
      LCD_position(LINE1);
      // Get project name
      SD_FindFile (  display_index, "\\Project\\", prj_name, false );
      sprintf ( lcdstr, "%s", prj_name );
      station_count = getStationNumber ( prj_name );
      SD_FindFile (  display_index, "\\Project\\", prj_name, false );
      sprintf ( lcdstr, "%s", prj_name );
      station_count = getStationNumber ( prj_name );
      LCD_print (lcdstr);
      up_down_ENTER_select_text();
      while(1)
      {
        button = getKey ( TIME_DELAY_MAX );
        if ( (button == ESC) || (button == ENTER) || ( button == UP ) || ( button == DOWN ) )
        {
          break;
        }
      }
      if ( button == ESC )
      {
        return (17);
      }
      if ( button == UP )
      {
        display_index --;
        if ( display_index == 0 )
        {
          display_index = project_count;
        }
      }
      else if(button == DOWN)
      {
        if(++display_index >  project_count )
        {
          display_index = 1;
        }
      }
      else if ( button == ENTER )
      {
////////////////////////////////////////////////////////////////////////////////
        if ( function == 0)  //resume old project
        {
          hold_buzzer ( );
          delay_ms ( 1000 );
          if( !Features.auto_store_on && (function == 0) )  //auto store is not enabled, ask if they want to enable it
          {
            button = enable_disable_features('O');
          }
          setActiveProjectEE ( prj_name ); // Put the active project name into EEPROM
          return(0);
        }
        //////////////////////////////////////////////////////////////////////////
        else if ( function == 1 ) //delete selected project or select project for USB write
        {
          return ( true );
        }
        /////////////////////////////////////////////////////////////////////////////
        else if ( function == 2 )  // select project for display
        {          //read number of stations
          station_count = getStationNumber ( prj_name );
          if ( station_count == 0 )
          {
            no_data_stored_text();  //TEXT// display "   No Data Stored"  LINE2
            hold_buzzer();
            delay_ms(1000);
            return(17);
          }
          display_index = 0;
          select_from_list_text(1); //TEXT// display "   Select Station\n     From List"  LINE2,3
          hold_buzzer();
          delay_ms ( 1000 );
          while(1)
          {
            LCD_position(LINE1);
            _LCD_PRINT ("                    ");
            LCD_position(LINE1);
            // Get station name and display it
            getStationName ( prj_name, station , display_index++ );
            sprintf (lcdstr, "%s", station );
            LCD_print ( lcdstr );
            up_down_ENTER_select_text();
            while(1)
            {
              button = getKey ( TIME_DELAY_MAX );
              if ( (button == ESC) || (button == ENTER) || ( button == UP ) || ( button == DOWN ) )
              {
                break;
              }
            }
            if ( button == ESC )
            {
               return(17);
            }
            if(button == UP)
            {
              if(display_index == 0)
              {
                display_index = station_count-1;
              }
            }
            else if(button == DOWN)
            {
              if(++display_index >= station_count)
              {
                display_index = 0;
              }
            }
           else if ( button == ENTER ) // maximum of 50 stations
           {
             selection = display_index;
             // Display the station number
             CLEAR_DISP;
             getStationName ( prj_name, station , selection );
             _LCD_PRINTF ( "Station : %s", station );
             delay_ms(500);
             //return offset into EEPROM of the selected station
             // station index starts at 0 in EEPROM, so decrement by 1
             return( selection );  // pass station number
           }
         }
        }
      }
    }
 return(0);
}
/******************************************************************************
 *  Name:
 *  PARAMETERS:
 *  DESCRIPTION:
 *****************************************************************************/
void delete_projects(void)  // allows user to delete one or all project(s)
{
  Bool escape = 0;
  uint8_t go_to_screen = 0;
  char selected_project_name [ PROJ_NAME_LENGTH] ;
  enum buttons button;
  uint8 esc_key;
 // char project [] = "TEMP";
  //unsigned int32 selected_project;
 // char temp_str[PROJ_NAME_LENGTH] = NULL_NAME_STRING;
  SD_Wake();
  updateProjectInfo();
  if ( project_info.number_of_projects == 0 )
  {
   no_stored_projects_text();  //TEXT// display "    No Projects\n     Stored" LINE2,3
   hold_buzzer();
    delay_ms(1500);
    return;
  }
  while ( !escape )
  {
    CLEAR_DISP;
    switch ( go_to_screen )
    {
      case 0:
            delete_data_text();     //TEXT// display "Delete Data\n1. Delete All Data\n2. Delete One Proj." LINE1,2,3
            up_down_select_text(0); //TEXT// display "Select #, ESC Exit"
            while(1)
            {
              button = getKey ( TIME_DELAY_MAX );
              if((button == 1) || (button == 2) || (button == ESC))
              {
                break;
              }
            }
            if(button == ESC)
            {
              escape = TRUE;
            }
            else if ( button == 1 )
            {
              go_to_screen = 1;  //go to delete all data screen set
            }
            else
            {
              go_to_screen = 4;  //go to select_stored_project(delete)
            }
            break;
      case 1: // ASk to Delete all data from project storage
            enter_to_delete_text();  //TEXT// display "  Press ENTER to\n  Delete All Data"  LINE2,3
            while(1)
            {
              button = getKey ( TIME_DELAY_MAX );
              if((button == ESC) || (button == ENTER))
              {
                break;
              }
            }
            if(button == ESC)
            {
              escape = TRUE;
            }
            else
            {
              go_to_screen = 2;
            }
            break;
      case 2: // Ask to Delete All Data
            erase_data_text();  //TEXT// display "Erase All Data\nFrom Memory?\nYES to Continue"
            ESC_to_Exit(LINE4); //TEXT// display "ESC to Exit"
            while(1)
            {
              button = getKey ( TIME_DELAY_MAX );
              if((button == YES) || (button == ESC))
              {
                break;
              }
            }
            if ( button == ESC )
            {
              escape = TRUE;
            }
            else
            {
              go_to_screen = 3;
            }
            break;
      case 3: // Delete Data after asking twice
              if ( resetProjectStorage ( ) == 0 )
              {
                all_data_erased_text();  //TEXT// display "      All Data\n       Erased" LINE2,3
                hold_buzzer();
              }
              updateProjectInfo();
              Features.auto_store_on = FALSE;
              NV_MEMBER_STORE( FEATURE_SETTINGS, Features ); // makes user reselect a new proj.
              delay_ms(2000);
              escape = TRUE;
              break;
      case 4: //select single project to delete
              esc_key = select_stored_project( 1, selected_project_name );  //arg 1 returns index of selected project
              if ( esc_key == 17 )  //esc was pressed, 17 is not a valid vector
              {
                return;
              }
              delete_project_text(selected_project_name);  //TEXT// display "Press ENTER to\nDelete Data In\nProject %s",temp_str LINE1,2,3
              ESC_to_Exit(LINE4);             //TEXT// display "ESC to Exit"
              while(1)
              {
                button = getKey ( TIME_DELAY_MAX );
                if((button == ESC) || (button == ENTER))
                {
                  break;
                }
              }
              if(button == ESC)
              {
                escape = TRUE;
              }
              else
              {
                go_to_screen = 5;   // go delete the project
              }
              break;
      case 5: // Ask again to delete the project
            erase_project_data_text();  //TEXT// display "Erase Project Data?\nYES to Continue" LINE1,3
            ESC_to_Exit(LINE4);
            while(1)
            {
              button = getKey ( TIME_DELAY_MAX );
              if((button == YES) || (button == ESC))
              {
                break;
              }
            }
            if(button == ESC)
            {
              escape = TRUE;
            }
            else
            {
              clearProject ( selected_project_name );
              updateProjectInfo();
              Features.auto_store_on = FALSE;
              escape = TRUE;
              NV_MEMBER_STORE( FEATURE_SETTINGS, Features );// makes user reselect a new proj.
            }
            break;
    }
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
void review_data(void)  // computes and displays stored data
{
  Bool auto_scroll_advance = 0; //, escape = 0;
  uint8_t  display_set = 1, LCD_offset = 15;
  uint16_t loop_cnt;
  uint16_t index;
  int32_t location;
  char proj [PROJ_NAME_LENGTH];
  float per_MA, dry_dense_rev, moist_percent_rev, unit_density;
  char younit[7] = " PCF";
  char temp_str[20];
  station_data_t review;
  enum buttons button;
  SD_Wake();
  // Ask the user to select a project
  index = select_stored_project ( 0, proj );
  if ( index == 17)  // no project selected
  {
     return;
  }
  while ( 1 )
  {
    // get a station from the project
    location = getStation ( proj );
    if ( location == 17)  // no project selected
    {
     return;
    }
    // This takes a pointer to a station stuct in EEPROM and copies it into RAM
    readStation ( proj, location, &review );
    dry_dense_rev = review.density - review.moisture;
    moist_percent_rev = ( review.moisture / dry_dense_rev ) * 100;
    if ( review.units == KG_M3 )
    {
      sprintf(younit," kg/m3");
      LCD_offset = 13;
    } else if ( review.units == PCF)
    {
      sprintf(younit," PCF");
      LCD_offset = 15;
    } else
    {
      sprintf(younit," GCC");
      LCD_offset = 15;
    }
////////////////////////Display data//////////////////////////////////////////////////////
    while(1)                     //display data until button is pressed to exit
    {
      CLEAR_DISP;
      switch(display_set)
      {
        case 1:
              LCD_position (LINE1);
              sprintf(temp_str,"%u", review.moisture_count);
              count_text(0);  //TEXT// display "M Count"
              print_string_backward(temp_str,LINE1+LCD_offset);
              LCD_position (LINE2);
              sprintf(temp_str,"%lu",(unsigned long) review.density_count);
              count_text(1);  //TEXT// display "D Count";
              print_string_backward(temp_str,LINE2+LCD_offset);
              LCD_position (LINE3);
              sprintf(temp_str,"%.2f",(float)review.MCR );
              count_text(2);  //TEXT// display "    MCR:"
              print_string_backward(temp_str,LINE3+LCD_offset);
              LCD_position (LINE4);
              sprintf(temp_str,"%.2f", (float)review.density_count / (float)review.density_stand);
              count_text(3);  //TEXT// display "    DCR:"
              print_string_backward(temp_str,LINE4+LCD_offset);
              break;
        case 2:
              if(review.DT != 0)
              {
                unit_density = convertKgM3DensityToUnitDensity ( review.DT, review.units );
                if ( review.units != GM_CC )
                {
                  sprintf(temp_str,"%3.1f", unit_density);
                }
                else
                {
                  sprintf(temp_str,"%.3f", unit_density);
                }
                strcat(temp_str,younit);
                count_text(4);  //TEXT// display "    DT:"
                print_string_backward(temp_str,LINE1+19);
                if ( review.MA != 0.0 )
                {
                  per_MA = (review.DT/review.MA)*100;
                }
                else
                {
                  per_MA = 0.0;
                }
              }
              else
              {
                unit_density = convertKgM3DensityToUnitDensity ( review.density, review.units );
                if ( review.units != GM_CC )
                {
                  sprintf(temp_str,"%3.1f", unit_density);
                }
                else
                {
                  sprintf(temp_str,"%.3f", unit_density);
                }
                strcat(temp_str,younit);
                count_text(5);  //TEXT// display "    WD:"
                print_string_backward(temp_str,LINE1+19);
                if ( review.MA != 0.0 )
                {
                  per_MA = (review.density/review.MA)*100;
                }
                else
                {
                  per_MA = 0.0;
                }
              }
              checkFloatLimits( & per_MA );
              LCD_position (LINE2);
              sprintf(temp_str,"%.1f",(float)per_MA);
              count_text(6);  //TEXT// display "   %%MA:"
              print_string_backward(temp_str,LINE2+LCD_offset);
              LCD_position (LINE3);
              sprintf(temp_str,"%.1f",100-(float)per_MA);
              count_text(7);  //TEXT// display ""Voids:"
              print_string_backward(temp_str,LINE3+LCD_offset);
              break;
        case 3:
              LCD_position (LINE1);
              unit_density = convertKgM3DensityToUnitDensity ( review.moisture, review.units );
              if ( review.units != GM_CC )
              {
                sprintf(temp_str,"%3.1f", unit_density);
              }
              else
              {
                sprintf(temp_str,"%.3f", unit_density);
              }
              strcat(temp_str,younit);
              count_text(8);  //TEXT// display " Moist:"
              print_string_backward(temp_str,LINE1+19);
              LCD_position (LINE2);
              sprintf(temp_str,"%.1f",(float)moist_percent_rev);
              count_text(9);  //TEXT// display "%Moist:"
              print_string_backward(temp_str,LINE2+LCD_offset);
              LCD_position(LINE3);
              unit_density = convertKgM3DensityToUnitDensity ( dry_dense_rev, review.units );
              if ( review.units != GM_CC )
              {
                sprintf(temp_str,"%3.1f", unit_density);
              }
              else
              {
                sprintf(temp_str,"%.3f", unit_density);
              }
              strcat(temp_str,younit);
              count_text(10);  //TEXT// display "    DD:"
              print_string_backward(temp_str,LINE3+19);
              LCD_position (LINE4);
              sprintf(temp_str,"%.1f",(float)dry_dense_rev/(float)review.PR * 100);
              count_text(11);  //TEXT// display "   %PR:"
              print_string_backward(temp_str,LINE4+LCD_offset);
              break;
              
        case 4:
                  snprintf( temp_str,20,"LAT: %9.6f",review.gps_read.latitude); //gdata.latitude);
                  LCD_PrintAtPosition ( temp_str, LINE1 );
                  snprintf( temp_str,20,"LNG:%9.6f", review.gps_read.longitude); //gdata.longitude);
                  LCD_PrintAtPosition ( temp_str, LINE2 );
                  snprintf( temp_str,20,"ALT:%u SAT:%u ",review.gps_read.altitude,review.gps_read.sats); // gdata.altitude, gdata.sats);
                  LCD_PrintAtPosition ( temp_str, LINE3 );
                  break;              
        case 0:
              LCD_position(LINE1);
              printTimeDate(review.date );
              LCD_position(LINE2);
              display_depth(1, review.depth);
              break;
        }
       auto_scroll_advance = FALSE;
       if(Features.auto_scroll)
       {
         loop_cnt = 0;
         while(1)
         {
           button = getKey ( 50 );
            if((button == ESC) || (button == UP) || (button == DOWN) || (loop_cnt >= 100) || (button == STORE))
            {
              break;
            }
         }
         if(loop_cnt >= 100)
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
           button = getKey ( TIME_DELAY_MAX );
            if((button == ESC) || (button == UP) || (button == DOWN) || (button == STORE))
            {
              break;
            }
          }
         }
         if((button == DOWN) || auto_scroll_advance)                           // down button
         {
           display_set = (display_set + 1)%5;
         }
         else if(button == UP)                      // up button
         {
           display_set = ((display_set + 4)-1)%5;
         }
         else  //button == ESC exit function
         {
           break;
         }
      }
    //  escape = TRUE;
      //break;
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
Bool USB_write_file (  char * project , FILE_PARAMETERS * file)  // writes project info at vector to file on USB
{
  uint8_t depth_rev,  units_rev, i;
  uint16_t station_count, m_stand_rev, m_count_rev;
  uint32_t d_count_rev, d_stand_rev;
  float moist_rev, PR_rev, MA_rev, MCR_rev, DT_rev, per_MA, dry_dense_rev, moist_percent_rev;
  float  moisture_offset_rev, density_offset_rev, bottom_den_rev,kk_value_rev;
  float  trench_offset_rev;
  float A, B, C, M_A, M_B;
  float dense_rev = 0.0;
  float temp_value, unit_density;
  station_data_t review;
  uint32_t serial_number;
  char younit[8] = "\0";
  char HT[3] = "\t";
  char name_rev[PROJ_NAME_LENGTH];
  char temp_val[100];
  char date_time_str[50];
  char temp_str[201];
  Bool pass = TRUE;
  isrTIMER_1_Disable();
    // Store the Header
    nullString( temp_str, sizeof(temp_str) );
    sprintf(temp_str,"Date\tSerial Number\tStation\tDepth\tWD or DT\t%%MA\tMA\t%%Voids\tM Count\tD Count\tMCR\tDCR\tMoist\t%%Moist\tDD\t%%PR\tPR\tDensity Std Cnt\tMoist Std Cnt\t");
    AlfatWriteStr( file,temp_str);
    sprintf(temp_str,"Const A\tConst B\tConst C\tConst E\tConst F\tDensity Offset\tMoisture Offset(K)\tTrenchOffset\t Nomograph  Offset\tBottom Density\tLAT\tLNG\tALT\r\n");
    AlfatWriteStr( file,temp_str);   //write string to USB
    // Get the number of stations to store
    station_count = getStationNumber( project );
    // Get the serial number
    serial_number = getSerialNumber ();
    // store each station
    for( i=0; i < station_count; i++ )
    {
      if ( readStation ( project, i, & review ) == -1 )
      {
       break;
      }
      //read station name into local variable
      if ( strlen(review.name) < PROJ_NAME_LENGTH  )
      {
       strcpy ( name_rev, review.name );
      }
      depth_rev   =   review.depth;
      d_count_rev =   review.density_count;
      m_count_rev =   review.moisture_count;
      dense_rev   =   review.density;
      moist_rev   =   review.moisture;
      d_stand_rev =   review.density_stand;
      m_stand_rev =   review.moisture_stand;
      PR_rev      =   review.PR;
      MA_rev      =   review.MA;
      MCR_rev     =   review.MCR;
      DT_rev      =   review.DT;
      units_rev   =   review.units;
      // Get the various offset values
      if ( review.offset_mask & DENSITY_OFFSET_BIT )
      {
        density_offset_rev =  review.den_off;
      }
      else
      {
        density_offset_rev = 0.0;
      }
      // Moisture offset K value
      if ( review.offset_mask & MOISTURE_OFFSET_BIT )
      {
        moisture_offset_rev =  review.k_value;
      }
      else
      {
        moisture_offset_rev = 0.0;
      }
      // Trench offset
      if ( review.offset_mask & TRENCH_OFFSET_BIT )
      {
        trench_offset_rev =  review.t_offset;
      }
      else
      {
        trench_offset_rev = 0.0;
      }
      // Nomograph offset
      if ( review.offset_mask & NOMOGRAPH_OFFSET_BIT )
      {
        kk_value_rev   = review.kk_value;
        bottom_den_rev = review.bottom_den;
      }
      else
      {
        kk_value_rev   = 0.0;
        bottom_den_rev = 0.0;
      }
      // get the time/date and make a string
      getTimeDateStr ( review.date, date_time_str );
      A = NV_CONSTANTS(DEPTHS[getCalibrationDepth(depth_rev)].A);
      // If special cal B value was used for this reading, make B the special value
      if ( review.offset_mask & SPECIAL_CAL_BIT )
      {
        B = NV_RAM_MEMBER_RD (Constants.SPECIALCAL_B);
      }
      else
      {
        B = NV_CONSTANTS(DEPTHS[getCalibrationDepth(depth_rev)].B);
      }
      C = NV_CONSTANTS(DEPTHS[getCalibrationDepth(depth_rev)].C);
      M_A = NV_CONSTANTS(E_MOIST_CONST);
      M_B = NV_CONSTANTS(F_MOIST_CONST);
      dry_dense_rev = dense_rev - moist_rev;
      moist_percent_rev = moist_rev/dry_dense_rev * 100;
      if (units_rev == KG_M3 )  // SI units, printf kg/m3
      {
        sprintf(younit," kg/m3\t");
      }
      else if ( units_rev == PCF )
      {
        sprintf(younit," PCF\t");
      }
      else
      {
        sprintf(younit," GCC\t");
      }
      // Date
      nullString( temp_str, sizeof(temp_str) );
      strcpy ( temp_str,date_time_str );
      strcat ( temp_str,HT );
      // Serial Number
      sprintf( temp_val,"%lu",(unsigned long)serial_number );
      strcat(temp_str,temp_val);
      strcat(temp_str,HT);
      // STATION
      strcat(temp_str,name_rev);
      strcat(temp_str,HT);
       // DEPTH
      if(depth_rev == 1)
      {
        sprintf(temp_val,"BSCATTER");
      }
      else if ( depth_rev == 13 )
      {
       sprintf(temp_val,"AC");
      }
      else
      {
        if ( units_rev == KG_M3  ||  units_rev == GM_CC )      // in "kg/m3" mode
        {
         sprintf(temp_val,"%u mm.", depth_rev * 25);
        }
        else
        {
          sprintf(temp_val,"%u in.", depth_rev);          // in "PCF" mode
        }
      }
      strcat(temp_str,temp_val);
      strcat(temp_str,HT);
      if(DT_rev != 0)
      {
        per_MA = (DT_rev/MA_rev)*100;
        // change to proper units
        unit_density = convertKgM3DensityToUnitDensity ( DT_rev, units_rev );
        if ( units_rev != GM_CC )
        {
         sprintf(temp_val,"%3.1f", unit_density);  // fixed error here in v 5.16
        }
        else
        {
          sprintf(temp_val,"%.3f", unit_density);
        }
      }
      else
      {
        per_MA = (dense_rev / MA_rev) * 100;
        checkFloatLimits ( & dense_rev );
        unit_density = convertKgM3DensityToUnitDensity ( dense_rev, units_rev );
        if ( units_rev != GM_CC )
        {
         sprintf(temp_val,"%3.1f", unit_density);
        }
        else
        {
          sprintf(temp_val,"%.3f", unit_density);
        }
      }
      // DENSITY
      strcat ( temp_str, temp_val );
      strcat ( temp_str, younit );
      checkFloatLimits ( & per_MA );
      // %MA
      sprintf(temp_val,"%.2f\t", per_MA);
      strcat(temp_str,temp_val);
      // MA
      checkFloatLimits ( & MA_rev );
      unit_density = convertKgM3DensityToUnitDensity ( MA_rev, units_rev );
      if ( units_rev != GM_CC )
      {
        sprintf(temp_val,"%3.1f", unit_density);
      }
      else
      {
        sprintf(temp_val,"%.3f", unit_density);
      }
      strcat ( temp_str, temp_val );
      strcat ( temp_str, younit );
      // %VOIDS
      sprintf(temp_val,"%.2f\t",100 - per_MA);
      strcat(temp_str,temp_val);
      // MOISTURE COUNT
      sprintf(temp_val,"%u\t",m_count_rev);
      strcat(temp_str,temp_val);
      // DENSITY COUNT
      sprintf(temp_val,"%u\t",(unsigned int)d_count_rev);
      strcat(temp_str,temp_val);
      // MCR
      sprintf(temp_val,"%.4f\t",MCR_rev);
      strcat(temp_str,temp_val);
      // DCR
      sprintf(temp_val,"%.4f\t", (float)d_count_rev / (float)d_stand_rev);
      strcat(temp_str,temp_val);
      // MOISTURE
       checkFloatLimits ( & moist_rev );
      // change from kg/m3 to proper displayed units
      unit_density = convertKgM3DensityToUnitDensity ( moist_rev, units_rev );
      if ( units_rev != GM_CC )
      {
        sprintf(temp_val,"%3.1f", unit_density);
      }
      else
      {
        sprintf(temp_val,"%.3f", unit_density);
      }
      strcat(temp_str,temp_val);
      strcat(temp_str,younit);
      // % MOISTURE
      checkFloatLimits ( & moist_percent_rev );
      sprintf(temp_val,"%.2f\t", moist_percent_rev);
      strcat(temp_str,temp_val);
      // DRY DENSITY
      checkFloatLimits ( & dry_dense_rev );
      // change from kg/m3 to proper displayed units
      unit_density = convertKgM3DensityToUnitDensity (dry_dense_rev, units_rev );
      if ( units_rev != GM_CC )
      {
       sprintf(temp_val,"%.1f", unit_density);
      }
      else
      {
          sprintf(temp_val,"%.3f", unit_density);
      }
      strcat(temp_str,temp_val);
      strcat(temp_str,younit);
      // % PROCTOR
      temp_value = (dry_dense_rev/PR_rev) * 100.0;
      checkFloatLimits ( & temp_value );
      sprintf(temp_val,"%.2f\t", temp_value );
  		strcat(temp_str,temp_val);
      // store the row of data
      AlfatWriteStr( file,temp_str);   //write string to USB
      // Proctor
      checkFloatLimits ( & PR_rev );
      // change from kg/m3 to proper displayed units
      unit_density = convertKgM3DensityToUnitDensity (PR_rev, units_rev );
      if ( units_rev != GM_CC )
      {
       sprintf(temp_str,"%3.1f", unit_density);
      }
      else
      {
        sprintf(temp_str,"%.3f", unit_density);
      }
      strcat ( temp_str, younit );
      // DENSITY STD COUNT
      sprintf(temp_val,"%.0f\t", (float)d_stand_rev );
  		strcat(temp_str,temp_val);
      // MOISTURE STD COUNT
      sprintf(temp_val,"%.0f\t", (float)m_stand_rev );
  		strcat(temp_str,temp_val);
      // DENSITY A CONST
      sprintf(temp_val,"%.5f\t",(float)A);
  		strcat(temp_str,temp_val);
      // DENSITY B CONST
      sprintf(temp_val,"%.5f\t",(float)B);
  		strcat(temp_str,temp_val);
      // DENSITY C CONST
      sprintf(temp_val,"%.5f\t",(float)C);
      strcat(temp_str,temp_val);
      // MOISTURE A CONST
      sprintf(temp_val,"%.5f\t",(float)M_A);
  		strcat(temp_str,temp_val);
      // MOISTURE B CONST
      sprintf(temp_val,"%.5f\t",(float)M_B);
  		strcat(temp_str,temp_val);
      // store the row of data
       AlfatWriteStr( file,temp_str);   //write string to USB
      // send the offsets
      // Density Offset to proper units
      unit_density = convertKgM3DensityToUnitDensity ( density_offset_rev, units_rev );
      if ( units_rev != GM_CC )
      {
       sprintf(temp_str,"%3.1f", unit_density);
      }
      else
      {
        sprintf(temp_str,"%.3f", unit_density);
      }
      strcat ( temp_str, younit );
      // Moisture Offset K value
      sprintf(temp_val, "%.3f\t", moisture_offset_rev);
      strcat ( temp_str, temp_val );
      // Trench Offset
      sprintf(temp_val, "%.0f\t", trench_offset_rev);
      strcat ( temp_str, temp_val );
      // Nomograph offset
      sprintf ( temp_val, "%.3f\t", kk_value_rev );
      strcat  ( temp_str, temp_val );
      // bottom density
      // Density Offset to proper units
      unit_density = convertKgM3DensityToUnitDensity ( bottom_den_rev, units_rev );
      if ( units_rev != GM_CC )
      {
       sprintf(temp_val,"%3.1f", unit_density);
      }
      else
      {
        sprintf(temp_val,"%.3f", unit_density);
      }
      strcat ( temp_str, temp_val );
      strcat ( temp_str, younit );
      // store the row of data
      AlfatWriteStr( file,temp_str);   //write string to USB
      // Write GPS Data
      sprintf( temp_str,"%9.6f\t",  review.gps_read.latitude);
      AlfatWriteStr( file,temp_str);   //write string to USB
      sprintf( temp_str,"%9.6f\t",  review.gps_read.longitude );
      AlfatWriteStr( file,temp_str);   //write string to USB
      sprintf( temp_str,"%u.00",  review.gps_read.altitude );
      strcat ( temp_str, "\r\n");
      // store the row of data
      AlfatWriteStr( file,temp_str);   //write string to USB
    }
 isrTIMER_1_Enable();
  return pass;
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
void write_data_to_USB ( void )  // leads user though process to write project(s) to USB
{
  Bool escape = 0, scope =0;
  uint16_t go_to_screen = 0, i = 0, j;
  uint16_t location = 0;
  char name_temp[PROJ_NAME_LENGTH];
  char proj[PROJ_NAME_LENGTH];
  enum buttons button;
  Bool pass = TRUE;
  FILE_PARAMETERS  file;
  if ( alfat_errors > 0 )
  {
    date_usb_error_text();  // if alfat errors put up message
    getKey(TIME_DELAY_MAX);
    CLEAR_DISP;
    return;
  }
  updateProjectInfo();
  if ( project_info.number_of_projects == 0 )
  {
    no_stored_projects_text();  //TEXT// display "    No Projects\n      Stored" LINE2,3
    hold_buzzer();
    delay_ms(2000);
    return;
  }
 // AlfatStart();    // wake up the ALFAT device to full power
  AlfatStart();
  while(!escape)
  {
    CLEAR_DISP;
    switch(go_to_screen)
    {
      case 0:
            write_USB_text();  //TEXT// display " Write Data to USB\n1. Write All Data\n2. Write One Project" LINE1,2,3
            ESC_to_Exit(LINE4);
            while(1)
            {
             button = getKey ( TIME_DELAY_MAX );
              if((button == 1) || (button == 2) || (button == ESC))
              {
                break;
              }
            }
            if(button == ESC)
            {
              escape = TRUE;
            }
            else if(button == 1)
            {
              scope = 1;
              go_to_screen = 1;
            }
            else //button == 2
            {
              scope = 0;
              go_to_screen = 2;
            }
            break;
      case 1:
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
              escape = TRUE;
            }
            else //ENTER was pressed
            {
              go_to_screen = 3;
            }
            break;
      case 2:
            location = select_stored_project (1, proj);   //get memory location of project to write to USB
            if(location == 17)  // no project selected
            {
              go_to_screen = 0;
              break;
            }
            go_to_screen = 1;
            break;
      case 3:
              j = 0;
              while ( !check_for_USB() && j < 5 )
              {
               USB_text(2);  // display " No USB Device "
               delay_ms ( 1000 );
               j++;
              };
              if ( j >= 5 )
              {
                escape = TRUE;
                break;
              }
              USB_text(1);  // display "    Writing Data\n    To USB Drive" on LINE2 and LINE3
              hold_buzzer();
              // Mount the USB drive
              if ( !initialize_USB( TRUE ) )  //try to initialize USB.  if it cant initialize, escape
              {
                escape = TRUE;
                break;
              }
              if(scope == 1)  //write all data to USB
              {
                for( i=1; i <= project_info.number_of_projects; i++ )
                {
                  SD_FindFile (  i, "\\Project\\", proj, false );
                  strcpy ( name_temp, proj );
                  LCD_PrintBlanksAtPosition ( 20, LINE4 );
                  LCD_PrintAtPositionCentered ( name_temp , LINE4 + 10 );
                  // send project
                  pass = USB_open_file ( name_temp, & file);
                  if ( pass == TRUE )
                  {
                     pass = USB_write_file(name_temp, &file);   //write project data to the USB drive
                     if ( pass == TRUE )
                     {
                      // close file
                      AlfatFlushData( file.fileHandle );
                      AlfatCloseFile( file.fileHandle );
                     }
                 }
                 // If wrting to the USB fails, break out of the for loop
                 if ( pass == FALSE )
                 {
                  break;
                 }
                }
              }
              else
              {
                FILE_PARAMETERS fp;
                //read project name to variable
                pass = USB_open_file  ( proj, &fp );
                if ( pass == TRUE )
                {
                  pass = USB_write_file ( proj, &fp );   //write project data to the USB drive
                  if ( pass == TRUE )
                  {
                   // close file
                   AlfatFlushData(fp.fileHandle);
                   AlfatCloseFile(fp.fileHandle);   //close file
                  }
                }
              }
             if ( pass == FALSE )
             {
              escape = TRUE;
             }
             delay_ms(1000);
             go_to_screen = 4;
             break;
      case 4:
              USB_text(3);  // display "   Data Download\n     Complete" on LINE2 and LINE3
              delay_ms(2000);
              escape = TRUE;
              break;
     }
  }
  // Put ALFAT in reduced power mode
  //AlfatStop();
  AlfatStop();
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
void print_data (  char * project )  // writes project info to file on USB
{
  uint8_t depth_rev, units_rev, i, data_set = 0;
  uint16_t station_count, d_stand_rev, m_count_rev;
  //m_stand_rev,
  uint32_t  d_count_rev;
  float dense_rev, moist_rev, PR_rev, MA_rev, MCR_rev, DT_rev, per_MA, dry_dense_rev, moist_percent_rev;
  float unit_density;
  char name_rev[PROJ_NAME_LENGTH];
  char younit[8] = "\0\0\0\0\0\0\0";
  UART2_Start();
  isrUART2_Enable();
  UART2_EnableTxInt();
  UART2_ClearTxBuffer();
  clearComputerRXBuffer();
  station_data_t review;
  SDstart();
  KEYSCAN_DISABLE();
  isrTIMER_1_Disable();
    nullString( final_str, sizeof(final_str) );
    sprintf (final_str,"Project Name: %s\r",project);
    puts_printer ( final_str );
    delay_ms (500);
    station_count = getStationNumber( project );
    for ( i=0; i < station_count; i++ )
    {
     readStation ( project, i, &review );
      //read station name into local variable
      if ( strlen(review.name) < PROJ_NAME_LENGTH  )
      {
        strcpy ( name_rev, review.name );
      }
      depth_rev   =   review.depth;
      d_count_rev =   review.density_count;
      m_count_rev =   review.moisture_count;
      dense_rev   =   review.density;
      moist_rev   =   review.moisture;
      d_stand_rev =   review.density_stand;
      //m_stand_rev =   review.moisture_stand;
      PR_rev      =   review.PR;
      MA_rev      =   review.MA;
      MCR_rev     =   review.MCR;
      DT_rev      =   review.DT;
      units_rev   =   review.units;
      dry_dense_rev = dense_rev - moist_rev;
      moist_percent_rev = moist_rev/dry_dense_rev * 100;
      checkFloatLimits ( & dry_dense_rev );
      checkFloatLimits ( & moist_percent_rev );
      if ( units_rev == KG_M3 )  // SI units, printf kg/m3
      {
        sprintf(younit," kg/m3\r");
      }
      else if ( units_rev == PCF )
      {
        sprintf(younit," PCF\r");
      }
      else
      {
       sprintf(younit," GCC\r");
      }
      while ( data_set < 17 )
      {
        switch (data_set)
        {
          case 0:
                sprintf(final_str,"Station Name: %s\r",name_rev);
                break;
          case 1:
                getTimeDateStr ( review.date, final_str );
                strcat ( final_str, "\r" );
                break;
          case 2:
                if(depth_rev == 0)
                {
                   sprintf(final_str,"DEPTH: BSCATTER\r");
                }
                else
                {
                  if(units_rev)                                         // in "kg/m3" mode
                  {
                    sprintf(final_str,"DEPTH: %u mm\r", depth_rev * 25);
                  }
                  else
                  {
                    sprintf(final_str,"DEPTH: %u in.\r", depth_rev);          // in "PCF" mode
                  }
                }
                break;
          case 3:
                if(DT_rev != 0)
                {
                  per_MA = (DT_rev/MA_rev)*100;
                  checkFloatLimits ( & per_MA );
                  // change from kg/m3 to proper displayed units
                  unit_density = convertKgM3DensityToUnitDensity (DT_rev, units_rev );
                  if ( units_rev != GM_CC )
                  {
                  sprintf(final_str,"DT: %3.1f",(float)unit_density);
                  }
                  else
                  {
                  sprintf(final_str,"DT: %.3f",(float)unit_density);
                  }
                  strcat(final_str,younit);
                }
                else
                {
                  per_MA = (dense_rev / MA_rev) * 100;
                  checkFloatLimits ( & per_MA );
                  // change from kg/m3 to proper displayed units
                  unit_density = convertKgM3DensityToUnitDensity (dense_rev, units_rev );
                  if ( units_rev != GM_CC )
                  {
                    sprintf(final_str,"WD: %3.1f",(float)unit_density);
                  }
                  else
                  {
                    sprintf(final_str,"WD: %.3f",(float)unit_density);
                  }
                  strcat(final_str,younit);
                }
                break;
          case 4:
                sprintf(final_str,"%%MAX: %2.1f\r",(float)per_MA);
                break;
          case 5:
                sprintf(final_str,"%%Voids: %3.1f\r",100-(float)per_MA);
                break;
          case 6:
                sprintf(final_str,"M Count: %u\r",m_count_rev);
                break;
          case 7:
               sprintf(final_str,"D Count: %lu\r",(uint32)d_count_rev);
               break;
          case 8:
                sprintf(final_str,"MCR: %2.2f\r",(float)MCR_rev);
                break;
          case 9:
                sprintf(final_str,"DCR: %2.2f\r", (float)d_count_rev / (float)d_stand_rev);
                break;
          case 10:
                // change from kg/m3 to proper displayed units
                unit_density = convertKgM3DensityToUnitDensity (moist_rev, units_rev );
                if ( units_rev != GM_CC )
                {
                  sprintf(final_str,"Moist: %2.1f",(float)unit_density);
                }
                else
                {
                  sprintf(final_str,"Moist: %.3f",(float)unit_density);
                }
                strcat(final_str,younit);
                break;
          case 11:
                sprintf(final_str,"%%Moist: %2.1f\r",(float)moist_percent_rev);
                break;
          case 12:
                unit_density = convertKgM3DensityToUnitDensity (dry_dense_rev, units_rev );
                if ( units_rev != GM_CC )
                {
                  sprintf(final_str,"DD: %3.1f",(float)unit_density);
                }
                else
                {
                  sprintf(final_str,"DD: %.3f",(float)unit_density);
                }
                
                strcat(final_str,younit);
                break;
          case 13:
                sprintf(final_str,"%%PR: %2.1f\r",(float)dry_dense_rev/(float)PR_rev * 100);
                   break;
         case 14:
               snprintf( final_str,20,"LAT:%9.6f\r", review.gps_read.latitude);
               break;
         case 15:
              snprintf( final_str,20,"LNG:%9.6f\r", review.gps_read.longitude);
              break;
         case 16:
              snprintf( final_str,20,"ALT:%u\r", (UINT16)review.gps_read.altitude);
              data_set = 17;
              break;
        }
        puts_printer ( final_str );
        data_set++;
      }
      data_set = 0; //reset data_set
      puts_printer ( "\r" );
    }
  isrTIMER_1_Enable();
  AlfatRxtInt_Enable() ;
  isrUART2_Disable() ;
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
void write_data_to_printer(void)  // leads user though process to write project(s) to Rs232
{
  Bool escape = 0, scope;
  uint8_t go_to_screen = 0,  i = 0;
  uint16_t location = 0;
  enum buttons button;
  char proj_name [PROJ_NAME_LENGTH];
  project_info.number_of_projects = getProjectNumber ();
  if ( getProjectNumber()  == 0)
  {
    no_stored_projects_text();  //TEXT// display "    No Projects\n      Stored"  LINE2,3
    hold_buzzer();
    delay_ms(1000);
    return;
  }
  while(!escape)
  {
    CLEAR_DISP;
    switch(go_to_screen)
    {
      case 0:
            print_data_text();  //TEXT// display "Print Data\n1. Print All Data\n2. Print One Project"  LINE1,2,3
            ESC_to_Exit(LINE4); //TEXT// display "ESC to Exit"
            while(1)
            {
              button = getKey ( TIME_DELAY_MAX );
              if(button == 1 || button == 2 || button == ESC)
              {
                break;
              }
            }
            if ( button == ESC )
            {
              escape = TRUE;
            }
            else if(button == 1)
            {
              scope = 1;
              go_to_screen = 1;
            }
            else //button == 2
            {
              scope = 0;
              go_to_screen = 2;
            }
            break;
      case 1:
            printer_text(2);  // display "  Connect Printer\n     To Gauge\n    Press ENTER" on LINE1,LINE2 and LINE4
            hold_buzzer();
            while(1)
            {
             button = getKey ( TIME_DELAY_MAX );
             if(button == ENTER || button == ESC)
             {
               break;
             }
            }
            if(button == ESC)
            {
              escape = TRUE;
            }
            else //ENTER was pressed
            {
              go_to_screen = 3;
            }
            break;
      case 2:
            location = select_stored_project(  1, proj_name );   //get memory location of project to write to USB
            if(location == 17)  // no project selected
            {
              go_to_screen = 0;
              break;
            }
            go_to_screen = 1;
            break;
      case 3:
            printer_text(0);  // display "   Printing Data\n    Please Wait"  on LINE2 and LINE3
            hold_buzzer();
            if ( !IS_ACTICE_RS232_DETECTED() )
            {
              printer_text(1);  // display "   No Printer\n    Detected!" on LINE2 and LINE3
              delay_ms(2000);
              escape = TRUE;
              break;
            }
            if(scope == 1)  //print all data
            {
              for ( i = 1 ; i <= project_info.number_of_projects;  i++)
              {
                // Get project name
                SD_FindFile ( i, "\\Project\\", proj_name, false );
                print_data ( proj_name );
               }
            }
            else
            {
              print_data ( proj_name );
            }
            delay_ms(1000);
            go_to_screen = 4;
            break;
      case 4:
            print_complete_text();  //TEXT// display "    Print Data\n      Complete" LINE2,3
            delay_ms(1000);
            escape = TRUE;
            break;
    }
  }
}
/******************************************************************************
 *
 *  Name: storeStationData ( station_data_t station_d )
 *
 *  PARAMETERS: station struct.
 *
 *
 *  DESCRIPTION: Takes station data and stores it in the current project and station
 *
 *  RETURNS:
 *
 *****************************************************************************/
void storeStationData ( char * project, station_data_t station  )
{
  uint8_t lcd_line;
  FS_FILE *  SDfile = null;
  uint16 station_num;
  strcpy ( station.name, "              ");
  SD_Wake();
  // Open the project file.
  SDfile = SDProjOpen ( project );
  if ( SDfile == null )
  {
    SDstop ( null );
    return ;
  }
  // get the current station number from the open project file
  station_num = getStationNumber ( project );
  // Check to see if all station positions are full
  if ( station_num >= MAX_STATIONS )
  {
   // display "Max # of Stations\nFor %s Has\nBeen Exceeded.\nStart New Project",current_project
   max_stations_text( project );
   SDstop ( SDfile );       // stop SD card and close the opened file
   delay_ms(1500);
   return ;
  }
  CLEAR_DISP;
  LCD_position(LINE2);
  current_project_text( project );
  delay_ms(1000);
  enter_station_name_text();  //TEXT// display "Enter Station\nName:"  LINE1,2
  YES_to_Accept(LINE3);       //TEXT// display "YES to Accept"
  ESC_to_Exit(LINE4);         //TEXT// display "ESC to Exit"
  lcd_line = (Features.language_f) ? LINE2+6 : LINE2+10;
  //write entered name of station
  enter_name ( station.name, lcd_line );
  if ( getLastKey() == ESC )
  {
    SDstop ( SDfile );       // stop SD card and close the opened file
    return;
  }
  writeStation ( project, station_num, &station );
  //increment number of stations within project
  incrementStationNumber ( project );
  project_info.station_index++ ;
}