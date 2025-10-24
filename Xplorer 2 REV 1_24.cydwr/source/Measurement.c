/**********************************************************************************************************************/
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
#include "Utilities.h"
#include "StoreFunctions.h"
#include "ProjectData.h"
#include "conversions.h"
#include "prompts.h"
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "Batteries.h"
#include "Tests.h"
#include <math.h>
#include "SDcard.h"
#include "BlueTooth.h"
#include "Measurement.h"

/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/
uint8_t measureThinLayer(void) ;
/************************************************  LOCAL DEFINITIONS  *************************************************/
#define MAX_FLOAT_VALUE 10000.0
#define MIN_FLOAT_VALUE .001
// 15 minute wait time when 50ms delay is used
#define WAIT_TIME_BEFORE_BREAK  ( 11380 )
#define NO_DEMO_DEBUG  1
/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/

/******************************************************************************
 *
 *  Name:  getCalibrationDepth ( uint8_t depth_inches )
 *
 *  PARAMETERS: The depth in inches of the source rod
 *
 *  DESCRIPTION: The depth in inches and the calibration constant index
 *                for that depth are not the same. So get the correct cal depth
 *               index
 *
 *  RETURNS: Calibration Depth Index
 *
 *****************************************************************************/
 uint8_t getCalibrationDepth ( uint8_t depth_inches ) {
   switch ( depth_inches )
   {
    case ( 0 ) : return 0;      // this is safe position
                 break;
    case ( 1 ) : return BS;      // this is  BS
                 break;
    case ( 2 ) : return DEPTH_2_INCH;
                 break;
    case ( 3 ) : return DEPTH_3_INCH;
                 break;
    case ( 4 ) : return DEPTH_4_INCH;
                 break;
    case ( 5 ) : return DEPTH_5_INCH;
                 break;
    case ( 6 ) : return DEPTH_6_INCH;
                 break;
    case ( 7 ) : return DEPTH_7_INCH;
                 break;
    case ( 8 ) : return DEPTH_8_INCH;
                 break;
    case ( 9 ) : return DEPTH_9_INCH;
                 break;
    case ( 10 ) : return DEPTH_10_INCH;
                 break;
    case ( 11 ) : return DEPTH_11_INCH;
                  break;
    case ( 12 ) : return DEPTH_12_INCH;
                  break;
   default:     return 0;
                break;
 }
}
/******************************************************************************
 *
 *  Name: void measureMoistureDensity(void)
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION: Cal consts are in GCC
 *
 *
 *  RETURNS: NA
 *
 *****************************************************************************/
uint8_t measureMoistureDensity(void) 
{ // leads user through a general count, (START button initiates)
    Bool auto_scroll_advance = 0;    //Bool prompt_for_start = 0;
    int8_t LCD_offset = 13, lcd_line, hj, x;
    int16_t loop_cnt = 0, spec_cal_depth = 14;
    float  Ad,Bd,Cd,E,F, Mg, Mt, cr, per_MA, mcr, pr, ma,dry_dens, moist_percent;
    float  KK, dt, bottom_density,temp_cnt, pr_temp, density, moisture;
    enum buttons button;
    uint16_t    d_stand, m_stand, wait_time, moisture_cnt = 0;
    date_time_t date_time;
    uint32_t    density_cnt = 0;
    station_data_t station_d;
    char   temp_str[80], buff[21];
    int str_equal;
    static float soil_air_voids,soil_sg;
    
    cnt_time =  NV_RAM_MEMBER_RD( COUNT_TIME ); // Get the count time
#if AUTO_TEST
    cnt_time = 1;
#endif

    if ( Features.SI_units == FALSE )
    { 
      LCD_offset = 15; 
    }
    else
    {
        if ( Features.kg_gcc_units == GCC_UNITS )
        { 
          LCD_offset = 15;
        }
        else
       { 
        LCD_offset = 13;
       } 
    }
    Global_IE();
    if(Spec_flags.spec_cal_flag) 
    { 
      spec_cal_depth = NV_RAM_MEMBER_RD(SPECIALCAL_DEPTH);
    }
    if ( Spec_flags.recall_flag )
    { // Recall the depth
        depth_setting    = NV_RAM_MEMBER_RD(LAST_TEST_DEPTH);
        if ( depth_setting > MAX_DEPTHS )
        { 
          return 0;
         }    // leave function if depths are invalid
        date_time = NV_RAM_MEMBER_RD(LAST_TEST_TIME);
    }
    else
    {
        if(Features.auto_depth)
        { 
          tst_depth_g = get_depth_auto( 0 ); 
        } // set depth if in auto mode, update depth
        else 
        {
            set_depth_manual(); // updates tst_depth_g
            if ( ESC == getLastKey() )
            { 
              return 0; 
            }
        }
        depth_setting = tst_depth_g; // the values for the depth strip are SAFE = 0;  BSCATTER=1; 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 ,12
       
        if ( (depth_setting == 0) || (depth_setting >= 13) || !(bit_test(valid_depth, depth_setting )) ) 
        { // auto store is on and a valid project is not selected
            delay_ms(500);
            update_valid_depths();  // repeat getting depth
            if(Features.auto_depth)
            { 
              tst_depth_g = get_depth_auto( 0 ); 
            } // update depth, set depth if in auto mode
            else 
            { 
              set_depth_manual();
            } // updates tst_depth_g
            depth_setting = tst_depth_g;
            if ( (depth_setting == 0) || (depth_setting >= 13) || !(bit_test(valid_depth, depth_setting )) )
            {
              // Depth failed for second time so display message
              display_invalid_depth(depth_setting);
              delay_ms(1500);
              return 0;
            }
        } 
        if ( Spec_flags.nomograph_flag && (depth_setting != 1) ) 
        { // Must be in BS if thin layer mode is enabled
            not_in_bs_text();                 // TEXT: "THIN LAYER NOT IN BS POSITION
            delay_ms(1000);
            return 0;
        }
  	} // end of getting depth
    if ( Features.auto_store_on && (Spec_flags.recall_flag == 0))
    { // Automatic Project storage on
        if ( sdOpened == OFF )
        { 
          SDstart(); 
        }
        updateProjectInfo();
        str_equal= strcmp ((const char*)& project_info.current_project,  "none_selected" );
        if ( ( project_info.number_of_projects == 0 ) || (str_equal == 0 ) )
        { //no project has been created and count is started... auto name starting at 1
            start_new_project();
            if( getLastKey() == ESC) 
            {
                if(sdOpened == ON) 
                { 
                  SDstop(null);
                }
                return 0;
            }
        }
        if ( project_info.station_index >= MAX_STATIONS ) 
        { // Check to see if all station positions are full
            max_stations_text( project_info.current_project ); // display "Max # of Stations\nFor %s Has\nBeen Exceeded.\nStart New Project",current_project
            delay_ms(1500);
            if (  sdOpened == ON  ) 
            { 
              SDstop(null); 
            }
            return 0;
        }
        if ( SD_CheckIfProjExists ( project_info.current_project ) == FALSE ) 
        { // auto store is on and a valid project is not selected
            no_project_selected();  //display "No Project Has Been\nSelected. Please\nCreate or Select\nProject."
            delay_ms(1500);
            if (  sdOpened == ON  ) 
            { 
              SDstop(null);
            }
            return 0;
        }
       if ( !Flags.auto_number_stations )
       { // auto name is off
            enter_station_name_text();  //TEXT// display "Enter Station\nName:" LINE1,2
            YES_to_Accept(LINE3);              //TEXT// display "YES to Accept"
            ESC_to_Exit(LINE4);                //TEXT// display "ESC to Exit"
            lcd_line = (Features.language_f) ? LINE2+6 : LINE2+10; // project_info.current_station_name[1] = 0;
            enter_name ( project_info.current_station_name, lcd_line ); //write entered name of station
            if ( getLastKey() == ESC) 
            { //prompt_for_start = TRUE;
                if (  sdOpened == ON  ) 
                { 
                  SDstop(null); 
                }
                return 0;
            }
        }
        else { // auto name is on //read last station and increment
            uint16 auto_number_temp;
            auto_number_temp = project_info.station_index + project_info.station_start ;  // equals index of last station
            itoa(auto_number_temp, project_info.current_station_name, 10);
        }
    } // end project setup
   	read_RTC( &date_time_g );  //read time at beginning of count
    if ( Spec_flags.recall_flag == FALSE)
    { 
      date_time = date_time_g;
    }
    Ad = get_constant('a', depth_setting ); // Calibration constants are stored in GCC units.
    station_d.offset_mask     = 0;
    if( Spec_flags.spec_cal_flag && (spec_cal_depth == depth_setting))
    {  //  check for spec calibration mode
        Bd = NV_RAM_MEMBER_RD (Constants.SPECIALCAL_B);
        station_d.offset_mask  |= SPECIAL_CAL_BIT;
    }
    else
    {
      Bd = get_constant('b', depth_setting ); 
    }
    Cd = get_constant('c', depth_setting );
    E = NV_RAM_MEMBER_RD(Constants.E_MOIST_CONST);
    F = NV_RAM_MEMBER_RD(Constants.F_MOIST_CONST) ;
    if ( Features.dummy_mode == TRUE )
    { 
      NV_MEMBER_STORE(MOIST_STAND, 900); NV_MEMBER_STORE(DEN_STAND, 3200);
    }
    m_stand = NV_RAM_MEMBER_RD (MOIST_STAND);
    d_stand = NV_RAM_MEMBER_RD (DEN_STAND);
    pr = NV_RAM_MEMBER_RD(PROCTOR);
    ma = NV_RAM_MEMBER_RD(MARSHALL);
    Controls.reset_count = TRUE;
    while(1) {   ////////////////////////////////////////////////////////////////////////////////
        if( bit_test(valid_depth, depth_setting ) && (d_stand > 0) && (m_stand > 0) ) { // valid data in EEPROM
            if(!Spec_flags.recall_flag) {
                while ( Controls.reset_count ) {
                    CLEAR_DISP;
                    measurePulses(LINE4, cnt_time, &moisture_cnt, &density_cnt, depth_setting );           //take measurements
                    if (getLastKey() == ESC || getLastKey() == ENTER ) { break; }
                    NV_MEMBER_STORE(LAST_TEST_TIME,date_time_g); // Recall will need the time, type and depth of the reading.
                    NV_MEMBER_STORE(LAST_TEST_DEPTH, depth_setting );
                    if ( Features.dummy_mode == TRUE ) {
                        moisture_cnt = 500;
                        density_cnt = 1000;
                        NV_MEMBER_STORE(M_CNT_AVG, moisture_cnt);
                        NV_MEMBER_STORE(D_CNT_AVG, density_cnt);
                    }
                }
            }             //else { }// nothing else gets filled in? Date/time,project,station
            moisture_cnt = NV_RAM_MEMBER_RD ( M_CNT_AVG );
            density_cnt  = NV_RAM_MEMBER_RD ( D_CNT_AVG );
            if ( getLastKey() == ESC || getLastKey() == ENTER ) { break; }
            // WD = 1/Bd * LN ( Ad / (CR + Cd ) ) - M/20 , where Ad, Bd, Cd are the density cal constants
            // M  = ( MCR - E) / F, where E, F are the moisture cal constants.
            if(Offsets.tren_offset_pos) { //Moisture Calculations, add the trenchoffset to the moisture count before getting the  moisture count ratio
                float trench = NV_RAM_MEMBER_RD(T_OFFSET);
                mcr = ((float)moisture_cnt + trench )/(float)m_stand;
            }
            else { // moisture count ratio is the current moisture count divided by the moisture standard count
                mcr = (float)moisture_cnt / (float)m_stand;
            }
            checkFloatLimits ( & mcr );
            moisture = ((mcr - E) / F) ; // moisture calculation. Result is supposed to be in GCC. //if ( Features.d ummy_mode == TRUE ) { moisture = .25; }
            checkFloatLimits ( & moisture );
            cr = (float)density_cnt / (float)d_stand; //Density Calculations. Result is supposed to be in GCC. get the density count ration
            checkFloatLimits ( & cr );
            density = 1/Bd * log ( Ad / (cr + Cd ) ) ; // density calculation without offset
            density = density - ( moisture/20); // density without offset
            if ( Features.dummy_mode == TRUE ) {
                CLEAR_DISP;  //                density = 2.0;
                LCD_PrintAtPosition ( "DEMO MODE", LINE3 ); //LCD_position( LINE1); _LCD_PRINT ("DEMO MODE" );
                for ( hj = 0; hj < 10; hj++ ) {
                    output_high(BUZZER); delay_ms ( 100 );
                    output_low(BUZZER); delay_ms ( 100 );
                }
            }
            checkFloatLimits ( & density );
            density /= GCC_TO_KG;  // density in KGM3 // All offsets are stored in Kg/M3 units. So, change GCC units to KG/M3
            moisture /= GCC_TO_KG; // density in KGM3
            if ( Offsets.den_offset_pos ) { // density calculation with density offset. Offset should be stored in kg
                density +=  NV_RAM_MEMBER_RD(D_OFFSET);
            }
            dry_dens = density - moisture; // %Moisture and Dry Density Calculations without K offset DD calculation without K offset
            moist_percent = (moisture / dry_dens) * 100; //Moist percent calculation without K offset
            if ( Offsets.moist_offset_pos ) 
            { // %Moisture and Dry Density Calculations with K offset This requires a new Bm value found from K
                // K = 1000 x ( %Mtrue - %Mgauge ) / ( %Mgauge + 100 )
                // %Mtrue = ( K x (%Mgauge + 100 ) / 1000 ) + %Mgauge
                // %M = (moisture * 100) / ( density - moisture )
                // moisture = (%M * density)/(100 + %M)
                Mg = moist_percent;
                Mt = (( NV_RAM_MEMBER_RD(K_VALUE) * ( Mg + 100 ) ) / 1000.0) + Mg;
                moisture = (Mt * density)/(100 + Mt); // recalulate moisture using new %Mt
                dry_dens = density - moisture;                               // DD calculation
                moist_percent = (moisture / dry_dens) * 100.0;   //
            }
            pr = NV_RAM_MEMBER_RD(PROCTOR);
            pr_temp = (dry_dens /NV_RAM_MEMBER_RD(PROCTOR)) * 100;  //calculate %PR
            checkFloatLimits ( & pr );  // check to see if values are valid numbers
            checkFloatLimits ( & ma );
            checkFloatLimits ( & dry_dens );
            checkFloatLimits ( & moist_percent );
            checkFloatLimits ( & pr_temp );
            soil_sg = NV_RAM_MEMBER_RD ( SOIL_GRAVITY ); // get value in KGM3
            soil_air_voids = 100 * ( 1 - (dry_dens/soil_sg) - moisture/1000 ); // dry dens and moisture are in KG/M3, so convert to GCC.
            LCD_light_timer(15);                // 15 seconds
            if ( Controls.LCD_light )
            { 
              KEY_B_LIGHT_ENABLE();
            }  // turn on backlight at end of test if enabled
            if ( !Spec_flags.self_test  ) { // If not a self test
                if ( Features.sound_on ) 
                { 
                  output_high(BUZZER); 
                } //beep at end of test, use delay to store station data in secret
                station_d.depth           = depth_setting; // Put the measurement data into a struct for auto or manual storage
                station_d.density_count   = density_cnt;
                station_d.moisture_count  = moisture_cnt;
                station_d.density         = density;
                station_d.moisture        = moisture;
                station_d.density_stand   = d_stand;
                station_d.moisture_stand  = m_stand;
                station_d.PR              = pr;
                station_d.MA              = ma;
                station_d.MCR             = mcr;
                station_d.date            = date_time_g;
                station_d.battery_voltage[0] =  readBatteryVoltage(NICAD) ;
                station_d.battery_voltage[1] =  readBatteryVoltage(ALK) ;
                x = sizeof(GPSDATA); // need to store the GPS reading for recall and project storage
                if(!Spec_flags.recall_flag) 
                { // Get GPS data If GPS is enabled and a fix is found, get the GPS reading
                    clearGPSData(); // Get rid of existing GPS readings
                    if (  Features.gps_on == TRUE ) 
                    {
                        if ( gps_fix == '1' )
                        {
                            Global_ID();
                            parseGPSString();
                            Global_IE();
                        }
                        else 
                        {
                            CyDelay( 1100 ); // The GPS data comes in at 1PPS
                            if ( gps_fix == '1' )
                            {
                                Global_ID();
                                parseGPSString();
                                Global_IE();
                            }
                        }
                    }
                    memcpy ( &station_d.gps_read, &gdata , x );
                    memcpy ( &eepromData.LAST_GPS_READING,  &gdata , x );
                    SavePartialEepromData((uint8*)&eepromData.LAST_GPS_READING, x, offsetof( EEPROM_DATA_t,LAST_GPS_READING)) ;
                }
                else 
                { // recall last GPS reading
                    memcpy(station_d.name,project_info.current_station_name,PROJ_NAME_LENGTH);
                    memcpy ( &station_d.gps_read, &eepromData.LAST_GPS_READING , x );
                }
                station_d.kk_value        = 0.0; // clear all offset values.
                station_d.bottom_den      = 0.0;
                station_d.den_off         = 0.0;
                station_d.k_value         = 0.0;
                station_d.t_offset        = 0;
                if ( Offsets.den_offset_pos )
                { // set the offset mask for the station         /*/*/          station_d.offset_mask = 0;
                    station_d.offset_mask     |= DENSITY_OFFSET_BIT;
                    station_d.den_off = NV_RAM_MEMBER_RD(D_OFFSET);
                }
                if ( Offsets.moist_offset_pos ) 
                {
                    station_d.offset_mask     |= MOISTURE_OFFSET_BIT;
                    station_d.k_value  = NV_RAM_MEMBER_RD ( K_VALUE );
                }
                if ( Offsets.tren_offset_pos ) 
                {
                    station_d.offset_mask     |= TRENCH_OFFSET_BIT;
                    station_d.t_offset        = NV_RAM_MEMBER_RD ( T_OFFSET );
                }
                if ( Features.SI_units == FALSE )
                { station_d.units           = PCF;
                }
                else if ( Features.kg_gcc_units == GCC_UNITS ) 
                { station_d.units           = GM_CC; 
                }
                else 
                { 
                  station_d.units          = KG_M3; 
                }
                if ( Spec_flags.nomograph_flag && (depth_setting == 1) )
                {  // Thin Layer Mode only valid in BS// SAFE = 0;  BSCATTER=1; AC = 13; 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 ,12
                    KK = NV_RAM_MEMBER_RD(KK_VALUE);        // read value from eeprom
                    bottom_density = NV_RAM_MEMBER_RD(BOTTOM_DENS);
                    dt = ( (density) - (KK * bottom_density)) / (1 - KK); // dt is the overlay density
                    station_d.DT = dt; // Save the result in kg/m3 units as is required
                    station_d.kk_value = NV_RAM_MEMBER_RD ( KK_VALUE );
                    station_d.offset_mask |= NOMOGRAPH_OFFSET_BIT;
                    station_d.bottom_den = NV_RAM_MEMBER_RD ( BOTTOM_DENS );
                }
                else
                { 
                  station_d.DT = 0; 
                }
                if (( Features.auto_store_on ) && ( Spec_flags.recall_flag == FALSE ))
                { // store the data
                    strcpy ( station_d.name, project_info.current_station_name );
                    writeStation ( project_info.current_project, project_info.station_index, &station_d );
                    incrementStationNumber ( project_info.current_project );            //increment number of stations within project
                    project_info.station_index     = getStationNumber ( project_info.current_project );
                }
                SendBLEDataCC ();
                station_d.den_off = NV_RAM_MEMBER_RD(D_OFFSET);
                station_d.k_value  = NV_RAM_MEMBER_RD ( K_VALUE );
                station_d.t_offset        =  NV_RAM_MEMBER_RD ( T_OFFSET );
                station_d.kk_value = NV_RAM_MEMBER_RD ( KK_VALUE );
                station_d.bottom_den = NV_RAM_MEMBER_RD ( BOTTOM_DENS );
                station_d.DT = ( (station_d.density) - (station_d.kk_value * station_d.bottom_den)) / (1 - station_d.kk_value);
               
                if ((eepromData.gauge_type  == GAUGE_3440  ) ||(eepromData.gauge_type  == GAUGE_3440_PLUS  ))
                { 
                  station_d.offset_mask |= TR_GAUGE_BIT;
                }           // If 3440 set gauge type bit
               
                SendBLEData ( &station_d, (bool)Spec_flags.recall_flag );
                CyDelay ( 800 );  // added longer delay for beeper (2-14-2022)
                output_low(BUZZER);
            }
            checkFloatLimits ( & dt );
            wait_time = 0;  // Display data // loop in this routine for 15 minutes
            while(1) 
            {                      //display data until button is pressed to exit
             CLEAR_DISP; // All density values are in KG/M3 at this point
             switch(gp_disp)
             {
                case 1:
                  LCD_position (LINE1);
                  if(Offsets.tren_offset_pos)
                  {
                   // add the trench offset to the moisture count
                  temp_cnt = (float)(moisture_cnt) + NV_RAM_MEMBER_RD(T_OFFSET) ;
                  sprintf(temp_str,"%u",(uint16_t)temp_cnt);
                  }
                  else
                  {
                    sprintf(temp_str,"%u",moisture_cnt);
                  }
                  count_text(0);  //TEXT// display "M Count"
                  print_string_backward(temp_str,LINE1+LCD_offset);
                  LCD_position (LINE2);
                  sprintf(temp_str,"%lu",(unsigned long)density_cnt);
                  count_text(1);  //TEXT// display "D Count";
                  print_string_backward(temp_str,LINE2+LCD_offset);
                  if ( mcr > 10 || mcr < -10 )
                  {
                    mcr = 0.0;
                  }
                  sprintf(temp_str,"MCR: %.2f", mcr);
                  LCD_PrintAtPosition ( temp_str, LINE3 );
                  if ( cr > 10 || cr < -10  )
                  {
                    cr = 0.0;
                  }
                  sprintf(temp_str,"DCR: %.2f", cr );
                  LCD_PrintAtPosition ( temp_str, LINE3 + 10);
                  break;
            case 2:
                  LCD_position (LINE1);
                  count_text(8);  //TEXT// display " Moist:"
                  displayValueWithUnitsBW ( moisture, LINE1 + 19,temp_str );
                  LCD_position(LINE2);
                  count_text(10);  //TEXT// display "    DD:"
                  displayValueWithUnitsBW ( dry_dens, LINE2 + 19, temp_str );
                  if(Features.language_f)
                  {
                    sprintf(temp_str,"%%MOIS:%.1f %%PR:%.1f",(double)moist_percent,(double)pr_temp);
                  }
                  else
                  {
                    sprintf(temp_str,"%%HUME:%.1f %%PR:%.1f",(double)moist_percent,(double)pr_temp);
                  }
                  LCD_PrintAtPosition( temp_str, LINE3 );
                  break;
            case 3:
                  LCD_position(LINE1);
                  // Display density or top density
                  // Depth must be in BS for nomograph mode
                  if(Spec_flags.nomograph_flag && (depth_setting == 1))
                  {
                    count_text(4);  //TEXT// display "    DT:"
                    //_LCD_PRINT ("    DT:");
                    displayValueWithUnitsBW ( dt, LINE1 + 19, temp_str );
                  }
                  else
                  {
                    count_text(5);  //TEXT// display "    WD:"
                    displayValueWithUnitsBW ( density, LINE1 + 19,  temp_str );
                  }
                  // density and MARSHALL are in KG/M3
                  if(!Spec_flags.nomograph_flag)
                  {
                    per_MA = (density / NV_RAM_MEMBER_RD(MARSHALL)) * 100;
                  }
                  else
                  {
                    per_MA = (dt/NV_RAM_MEMBER_RD(MARSHALL))*100;
                  }
                  checkFloatLimits ( & per_MA );
                  LCD_position (LINE2);
                  sprintf(temp_str,"%.1f",(double)per_MA);
                  count_text(6);  //TEXT// display "   %%MA:"
                  print_string_backward(temp_str,LINE2+LCD_offset);
                  LCD_position (LINE3);
                  sprintf(temp_str,"%.1f",100.0 - (double)per_MA);
                  count_text(7);  //TEXT// display ""Voids:"
                  print_string_backward(temp_str,LINE3+LCD_offset);
                  break;
            case 4:
                  snprintf( buff,20,"LAT: %9.6f",station_d.gps_read.latitude); //gdata.latitude);
                  LCD_PrintAtPosition ( buff, LINE1 );
                  snprintf( buff,20,"LNG:%9.6f", station_d.gps_read.longitude); //gdata.longitude);
                  LCD_PrintAtPosition ( buff, LINE2 );
                  snprintf( buff,20,"ALT:%u SAT:%u ",station_d.gps_read.altitude,station_d.gps_read.sats); // gdata.altitude, gdata.sats);
                  LCD_PrintAtPosition ( buff, LINE3 );
                  break;
            case 0:
                  LCD_position(LINE1);
                  printTimeDate ( date_time ) ;
                  LCD_position (LINE2);
                  display_depth(1,depth_setting);
                  LCD_position (LINE3);
                  if ( Features.soil_air_voids_on == 1 )
                  {
                    count_text(19);  //TEXT// display "%SAV:"
                    sprintf(temp_str,"%.1f",soil_air_voids);
                    _LCD_PRINTF ( "%s",temp_str);
                  }
                  else
                  {
                   _LCD_PRINT("          ");
                  }
                  break;
          }                // end of "Switch"
          LCD_position(LINE4);
          if(Features.language_f)
          {
           _LCD_PRINT("    PRESS UP/DOWN   ");
          }
          else
          {
            _LCD_PRINT ("Arriba/Abajo       ");
          }
          auto_scroll_advance = FALSE;
          if(Features.auto_scroll)
          {
            loop_cnt = 0;
            while(1)
            {
             button = getKey(50);
              if ( ++wait_time > WAIT_TIME_BEFORE_BREAK )
              {
                button = ESC ;
              }
              if((button == ESC) || (button == UP) || (button == DOWN) || (++loop_cnt >= 100) || (button == STORE) || (button == ENTER) || (button == PRINT))
              {
                wait_time = 0;
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
              #if AUTO_TEST
                button = getKey(50);
                if(button != ESC)
                {
                  button = ENTER;
                }
              #else
                    button = getKey(50);
              #endif
              if ( ++wait_time > WAIT_TIME_BEFORE_BREAK )
              {
                button = ESC ;
              }
              if((button == ESC) || (button == UP) || (button == DOWN) || (button == STORE) || (button == ENTER) )
              {
                wait_time = 0;
                break;
              }
            }
          }
          if((button == DOWN) || (auto_scroll_advance))    // down button
          {
            if (  Features.gps_on == FALSE )
            {
              gp_disp = (gp_disp + 1)%4;
            }
            else
            {
               gp_disp = (gp_disp + 1)%5;
            }
          }
          else if(button == UP)                      // up button
          {
            if (  Features.gps_on == FALSE )
            {
              gp_disp = ((gp_disp + 4)-1)%4;
            }
            else
            {
             gp_disp = ((gp_disp + 5)-1)%5;
            }
          }
          else if ( button == STORE )
          {
            if ( Spec_flags.recall_flag )
            {
            no_data_to_store_text( );
            delay_ms ( 1500 );
            }
            else
            {
             storeStationData ( project_info.current_project, station_d  );
            }
          }
          else if( (button==ESC) || (button==ENTER) )
          {
            break;
          }
        }
      } // end of if(a >= 0.00001 && d_standard > 0)
      else {
        if ( d_stand <= 0)
        {
          invalid_den_std_text();
          delay_ms(2000);
        }
        if ( m_stand <= 0 )
        {
          invalid_mois_std_text();
          delay_ms(2000);
        }
        if ( !bit_test(valid_depth, depth_setting ) )
        {
          no_valid_depth_selected ( );
          delay_ms(1500);
        }
        hold_buzzer();
        delay_ms(2000);
        while(Flags.button_pressed)
        {
          delay_ms(10);
        }
        button = DFLT;
      }
      break;
    }
    NV_MEMBER_STORE(GP_DISPLAY,gp_disp);
    return 1;
}
/******************************************************************************
 *
 *  Name: placeGaugeinBS (void)
 *
 *  PARAMETERS: NA
 *
 *  DESCRIPTION:
 *
 *
 *  RETURNS: NA
 *
 *****************************************************************************/
enum buttons placeGaugeinBS (void)
{
  enum buttons button;
   if ( AUTO_TEST )
   {
    return ENTER;
   }
 do
 {
    // Place Gauge
    // in BS Position
    // Press Enter to Start
    place_in_bs_text();
    // get ENTER or ESC key
    do
    {
      button = getKey(TIME_DELAY_MAX);
    }
    while ( (button != ESC) && (button != ENTER ) );
    if ( button == ESC )
    {
      return ESC;
    }
    // read depth if in auto mode
    if( Features.auto_depth)
    {
      // turn on depth power before entering routine
    //  DS_POWER_ON()  ;
      // let power stabilize
      delay_ms ( 30 );
      tst_depth_g = get_depth_auto( 0 );   // update depth
      // Display Error message is not in BS
      if ( tst_depth_g != 1 )
      {
        depth_not_in_bs_text();
        delay_ms ( 1000 );
      }
    }
    else
    {
     tst_depth_g = 1;  // Assume depth is in BS position
    }
 } while ( tst_depth_g != 1 ) ;
  return ENTER ;
}