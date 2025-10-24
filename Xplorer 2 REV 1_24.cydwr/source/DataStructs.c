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

//////////////////////////////////////////////////////////////////////////////

/*---------------------------------------------------------------------------*/
/*---------------------------[  Revision History  ]--------------------------*/ 
/*---------------------------------------------------------------------------*/ 
/*
 *  when?       who?    what?
 *  ----------- ------- ------------------------------------------------------ 
 *   
 *
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/ 
/*----------------------------[  Include Files  ]----------------------------*/ 
/*---------------------------------------------------------------------------*/
#include "project.h"
#include "Globals.h"
#include "DataStructs.h"
#include "prompts.h"
#include "ProjectData.h"
#include "SDCard.h"
#include "StoreFunctions.h"
#include <stddef.h> /* for offsetof */ 

/*---------------------------------------------------------------------------*/
/*-----------------------[  Global Functions  ]-----------------------*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/ 
/*------------------------------[  Macros ]----------------------------------*/ 
/*---------------------------------------------------------------------------*/


EEPROM_DATA_t  eepromData; 




/*******************************************************************************
* Function Name: EEpromWrite Array
********************************************************************************
* Summary: Write an array of data to the EEprom at the given offset, uses byte write - very inefficient
*          if error occurs data is lost
* Parameters:  uint8 *Array to write, number of bytes to write, offset into eeprom
* Return: none
*******************************************************************************/
void EEpromWriteArray(uint8 *array, uint32 len,uint32 eepromOffset)
{
	uint32 i,indx;
	uint16 addr = eepromOffset;
   uint8 row[CYDEV_EEPROM_ROW_SIZE];
	uint32 r,b;
   reg8 *RegPointer = (reg8*)CYDEV_EE_BASE;

  r = addr / CYDEV_EEPROM_ROW_SIZE; // start row
  b = addr % CYDEV_EEPROM_ROW_SIZE; // start byte
 
  if( ( len + b ) > CYDEV_EEPROM_ROW_SIZE ) 
  { 
    // data requires more than 1 row
    if(b != 0) 
    { // starting in middle of first row
     RegPointer = (reg8*)(CYDEV_EE_BASE + r * 16);
     for(i = 0; i < CYDEV_EEPROM_ROW_SIZE; i++) { row[i] = RegPointer[i]; } // read first row
     memcpy(&row[b],array,CYDEV_EEPROM_ROW_SIZE - b); // finish first row
     Global_ID();
     EEPROM_Write(row,r++); // write row to eeprom
     Global_IE();
     indx = (CYDEV_EEPROM_ROW_SIZE - b);
    }
    else
    { 
      indx = 0; 
    }
    
    while(indx < len) 
    {
     if( ( len - indx ) < CYDEV_EEPROM_ROW_SIZE) 
     { // partial row starting at beginning
        RegPointer = (reg8*)(CYDEV_EE_BASE + r * 16);
        for(i = len - indx; i < CYDEV_EEPROM_ROW_SIZE; i++) { row[i] = RegPointer[i]; } // read in rest of row
        memcpy(row,&array[indx],len - indx); 
        Global_ID();
        EEPROM_Write(row,r++);  
        Global_IE();
        indx = len; // done;
     }
     else 
     { // enough for an entire row
        Global_ID();
        b = EEPROM_Write(&array[indx],r++);  
        Global_IE();
        indx += CYDEV_EEPROM_ROW_SIZE;
     }
    }
   }
   else 
   { // data fits on 1 row
      RegPointer = (reg8*)(CYDEV_EE_BASE + r * 16);
      for (i = 0; i < CYDEV_EEPROM_ROW_SIZE; i++) 
      {
        row[i] = RegPointer[i]; 
      } // read first row
      memcpy(&row[b],array,len);
      Global_ID();
      EEPROM_Write(row,r);                  // write row to eeprom
      Global_IE();
   }

}
/*******************************************************************************
* Function Name: EEpromRead Array
********************************************************************************
* Summary: reads len bytes into given array
*          
* Parameters:  uint8* array, len of bytes to read, offset into eeprom
* Return: none
*******************************************************************************/
void EEpromReadArray(uint8 *array, uint32 len,uint32 eepromOffset)
{
	uint32 i;
	reg8 *RegPointer = (reg8*)CYDEV_EE_BASE;
   RegPointer +=  eepromOffset;

	for(i = 0;i < len; i++)
	{
		array[i] = *RegPointer++;
	}

}


/*****************************************************************************
* Function Name: SavePartialEeprom Data ()
******************************************************************************
* Summary: save part of eeprom data
* Parameters: uint8* data, offset of data in eeprom data, num bytes to write
* Return: 	None
*****************************************************************************/
void SavePartialEepromData(uint8* array,int32 len,int32 offset)
{

  EEpromWriteArray(array,len,offset);

}
/*****************************************************************************
* Function Name: SaveEeprom Data ()
******************************************************************************
* Summary: save eeprom data
* Parameters: None
* Return: 	None
*****************************************************************************/
void SaveEepromData(void)
{
  EEpromWriteArray((uint8*)&eepromData,sizeof(EEPROM_DATA_t),0);

}
/*****************************************************************************
* Function Name: ReadEeprom Data ()
******************************************************************************
* Summary: read eeprom data
* Parameters: None
* Return: 	1 if loading defaults
*****************************************************************************/
uint8 ReadEepromData(void)
{

  EEpromReadArray((uint8*)&eepromData,sizeof(EEPROM_DATA_t),0);
  return 0;
}
/******************************************************************************
 *  Name: auto_initialization(void)
 *  PARAMETERS: NA
 *  DESCRIPTION: Sets Default EEPROM values
 *  RETURNS: NA 
 *****************************************************************************/ 
void auto_initialization(void)  // initializes system settings to default values
{
  uint8_t i;
    
  initializing();
  eepromData.intialized   = 0xAA;	      // The eeprom is now intialized
  eepromData.OFF_MODE     =    0;       
  eepromData.TEST_DEPTH   =    0;       // Initialize depth to 0 inches
  eepromData.COUNT_TIME   =   60;       // Initialize test time to 1 minute
  eepromData.DEN_STAND    = DEFAULT_DENSITY_STD;              // initialize density standard count   
  eepromData.MOIST_STAND  = DEFAULT_MOISTURE_STD;              // initialize moisture standard count 
  eepromData.PROCTOR      = 100 /  KG_PCF_CONV; // 100 PCF    
  eepromData.MARSHALL     = 140 /  KG_PCF_CONV; // 140 PCF
  eepromData.SOIL_GRAVITY = 2700.0 ;              // KG/M3    
  eepromData.D_OFFSET     = 0;
  eepromData.K_VALUE      = 0;
  eepromData.T_OFFSET     = 0;
  eepromData.Constants.CAL_DATE.iday     = 1;
  eepromData.Constants.CAL_DATE.imonth   = 1;
  eepromData.Constants.CAL_DATE.iyear    = 2000;
  eepromData.Constants.CAL_DATE.ihour    = 1;
  eepromData.Constants.CAL_DATE.iminute  = 1;
  eepromData.Constants.CAL_DATE.isecond  = 1;
  eepromData.DENSE_CAL_STD = 3600;
  eepromData.MOIST_CAL_STD = 1000;
  eepromData.SHUT_DOWN_TIME_HOURS = 1;
  
  NV_CONSTANTS(SERIAL_NUM)   = 0;                         // Initialize serial number to 00000
  eepromData.SERIAL_NUM_HI   = 0;                         // Initialize serial number 2 to 00000
  
   // initialize all calibration const to 0.00
  for ( i = 0; i < MAX_DEPTHS; i++ )
  { 
    NV_CONSTANTS(DEPTHS[i].A) = 1.0;
    NV_CONSTANTS(DEPTHS[i].B) = 0.0;
    NV_CONSTANTS(DEPTHS[i].C) = 0.0;
  }  
  
  NV_CONSTANTS(E_MOIST_CONST ) = 0.0;
  NV_CONSTANTS(F_MOIST_CONST ) = 0.0;
    
  NV_CONSTANTS(DEPTHS[BS].A) = 1;
  eepromData.Constants.SPECIALCAL_B   = 0;                 // initialize special calibrations consts. to "0.00"
  eepromData.SPECIALCAL_CR  = 0.0;
  eepromData.SPECIALCAL_DEPTH = 0;
  eepromData.MAT_THICK      = 0;                	// initialize Material Thickness for Nomograph to "0.00"
  eepromData.BOTTOM_DENS    = 0;                  // initialize bottom density for void nomograph to "0.00"
  eepromData.OFF_MODE       = 0;                  // update status of shutdown, 1 = auto shutdown
  eepromData.depth_strip_type = DEPTH_STRIP_3500 ; // Default to Instrotek
  eepromData.safe_volts       = 2.0;
  eepromData.ds_slope         = .142;
  eepromData.gauge_type       = GAUGE_3500;        // InstroTek Gauge until changed
  
  //OFFSETS = 0;
	Offsets.den_offset_pos    = 0;
	Offsets.moist_offset_pos  = 0;
	Offsets.tren_offset_pos   = 0;
	eepromData.OFFSET_SETTINGS = Offsets;
											
  
  Features.auto_depth      =  1; 
	Features.avg_std_mode    =  1;    // Averaging standard counts is the default. The 
	Features.SI_units        =  0; 
	Features.auto_store_on   =  0; 
	Features.sound_on        =  1; 
	Features.kg_gcc_units    =  0; 
	Features.auto_scroll     =  0; 
	Features.mc_mode         =  EXPLORER_2_MODE;  // Elite Mode
  Features.dummy_mode      =  FALSE;            // Dummy Mode is disabled
  Features.chi_sq_mode     =  1;   // Chi test disabled
  Features.language_f      =  1;   // English
  Features.gps_on          =  0;   // Disable GPS
  eepromData.FEATURE_SETTINGS = Features;
  
  
  //FLAGS = 0;
  Flags.button_pressed        = 0; // Flag is high when "key" is pressed
  Flags.stat_flag             = 0; // set during stat test   
  Flags.stand_flag            = 0; // set during stand test
  Flags.in_spec_cal           = 0; // set during special cal routine
  Flags.drift_flag            = 0; // set during drift test
  Flags.auto_number_stations  = 0; // set when stations are to be automatically numbered
  Flags.bat_volt              = 0; // when FALSE, battery is automatically monitored by timer1_isr()
  Flags.diag                  = 0;
  eepromData.FLAG_SETTINGS = Flags;
    
  Controls.buzz_enable           = 0; // set to enable buzzer when sound_on is enabled
  Controls.shut_dwn              = 0; // set when "auto" shut off is enabled, some tests temporarily turn this off.
  Controls.LCD_light             = 0; // enables LCD backlight if set
  Controls.bat_sel               = 0; // selects NICAD or ALK (via scaler board connector only).  EBB pins should not be used.
  Controls.reset_count           = 0; // resets count when enter is pressed during test
  Controls.update_time           = 0;
  eepromData.CONTROL_SETTINGS    = Controls;
  
  eepromData.ENTER_DEN_LATER    = 0 ;   
  NV_AVG_STD_TEST.dense_count_1 = DEFAULT_DENSITY_STD ;
  NV_AVG_STD_TEST.dense_count_2 = DEFAULT_DENSITY_STD ;
  NV_AVG_STD_TEST.dense_count_3 = DEFAULT_DENSITY_STD ;  
  NV_AVG_STD_TEST.moist_count_1 = DEFAULT_MOISTURE_STD ;
  NV_AVG_STD_TEST.moist_count_2 = DEFAULT_MOISTURE_STD ;
  NV_AVG_STD_TEST.moist_count_3 = DEFAULT_MOISTURE_STD ;
  NV_AVG_STD_TEST.std_number    = 0 ;   
  NV_AVG_STD_TEST.std_index     = 0 ; 
  NV_AVG_STD_TEST.std_counts    = 0 ;
  
  SDstart();
  resetProjectStorage();
  updateProjectInfo();
  SDstop(null);
  
  eepromData.GP_DISPLAY      = 1;
  gp_disp = 1;
  
  update_valid_depths();
  
  cnt_time = eepromData.COUNT_TIME;
  
  // The RAM copy of NV storage data is stored in EEPROM
  SaveEepromData( );
  
}


/*****************************************************************************
* Function Name: initEepromData ()
******************************************************************************
* Summary: Store initValues into the EEPROM
*			Check intialized value in EEPROM. If not AA, set the eeprom values	
* Parameters: None
* Return: 	
*****************************************************************************/
void getNVFromEEProm (void )
{
	uint8_t check_eeprom;
	
    // Turn on EERPOM and Leave on
    EEPROM_Start();
    if ( sizeof(EEPROM_DATA_t) > EEPROM_EEPROM_SIZE )
    {
     // dispscrn(s_EEPROMDATAError);
     delay_ms(5000);
   }
  
  // read the eeprom into RAM
  ReadEepromData();
  
	check_eeprom = NV_RAM_MEMBER_RD (intialized);
	if ( check_eeprom != 0xAA )
	{
    eeprom_msg(); // Give message instead of reseting EEPROM.
	}	
	
}

 


