

/**************************   ***************************************************************** ***************************/
//
// Title:       Elite Explorer
// Company:     Instrotek
//
// Document No.:
// Issue:
// Description:  Driveers for the EPSON RX 8035
//
// Filename:   
// Author:      
//
// Processor:  
// Clock:	    12MHz	
//
//
// History: date, version, personal initials with comments about any changes
//
/**********************************************************************************************************************/
#include "Globals.h"
#include <device.h>
#include "Keypad_functions.h"
#include "LCD_drivers.h"
#include "prompts.h"
#include "Utilities.h"
#include "DataStructs.h"
#include "Alfat.h"

/************************************************************************************/
#define RTC_ADDRESS 0x32
#define I2C_WRITE 0
#define I2C_READ  1

#define RTC_SEC   0<<4
#define RTC_MIN   1<<4
#define RTC_HOUR  2<<4
#define RTC_DAY   4<<4
#define RTC_MONTH 5<<4
#define RTC_YEAR  6<<4

extern ALFAT_DATE_TIME_UNION time_date;

/*******************************************************************************
* Function Name: I2C_MasterSendStart
********************************************************************************
* Summary:
*  Generates Start condition and sends slave address with read/write bit.
* Parameters:
*  slaveAddress:  7-bit slave address.
*  R_nW:          Zero, send write command, non-zero send read command.
* Return:
*  Status error - Zero means no errors.
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
* Global variables:
*  I2C_1_state - The global variable used to store a current state of
*                           the software FSM.
* Reentrant:
*  No.
*******************************************************************************/
uint8 I2C_MasterSendStart(uint8 slaveAddress, uint8 R_nW) {
    uint8 errStatus;
    uint8 count = 0;
    errStatus = I2C_1_MSTR_NOT_READY;
    if(I2C_1_SM_IDLE == I2C_1_state) /* If IDLE, check if bus is free */
    {
        /* If bus is free, generate Start condition */
        if(I2C_1_CHECK_BUS_FREE(I2C_1_MCSR_REG))
        {
            /* Disable interrupt for manual master operation */
            I2C_1_DisableInt();

            /* Set address and read/write flag */
            slaveAddress = (uint8) (slaveAddress << I2C_1_SLAVE_ADDR_SHIFT);
            if(0u != R_nW)
            {
                slaveAddress |= I2C_1_READ_FLAG;
                I2C_1_state = I2C_1_SM_MSTR_RD_ADDR;
            }
            else
            {
                I2C_1_state = I2C_1_SM_MSTR_WR_ADDR;
            }

            /* Hardware actions: write address and generate Start */
            I2C_1_DATA_REG = slaveAddress;
            I2C_1_GENERATE_START_MANUAL;

            /* Wait until address is transferred */
            while(I2C_1_WAIT_BYTE_COMPLETE(I2C_1_CSR_REG))
            {
              CyDelayUs(1);
              count++;
              if ( count > 100 )
              {
                break; 
               }
            }

            if(I2C_1_CHECK_ADDR_NAK(I2C_1_CSR_REG))
            {
                /* Address has been NACKed: reset FSM to IDLE */
                I2C_1_state = I2C_1_SM_IDLE;
                errStatus = I2C_1_MSTR_ERR_LB_NAK;
            }
            else
            {
                /* Start was sent without errors */
                errStatus = I2C_1_MSTR_NO_ERROR;
            }
        }
        else
        {
            errStatus = I2C_1_MSTR_BUS_BUSY;
        }
    }

    return(errStatus);
}
/*******************************************************************************
* Function Name: I2C_MasterSendRestart
********************************************************************************
* Summary:
*  Generates ReStart condition and sends slave address with read/write bit.
* Parameters:
*  slaveAddress:  7-bit slave address.
*  R_nW:          Zero, send write command, non-zero send read command.
* Return:
*  Status error - Zero means no errors.
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
* Global variables:
*  I2C_1_state - The global variable used to store a current state of
*                           the software FSM.
* Reentrant:
*  No.
*******************************************************************************/
uint8 I2C_MasterSendRestart(uint8 slaveAddress, uint8 R_nW) {
    uint8 errStatus;
    uint8 count = 0;

    errStatus = I2C_1_MSTR_NOT_READY;

    /* Check if START condition was generated */
    if(I2C_1_CHECK_MASTER_MODE(I2C_1_MCSR_REG))
    {
        /* Set address and read/write flag */
        slaveAddress = (uint8) (slaveAddress << I2C_1_SLAVE_ADDR_SHIFT);
        if(0u != R_nW)
        {
            slaveAddress |= I2C_1_READ_FLAG;
            I2C_1_state = I2C_1_SM_MSTR_RD_ADDR;
        }
        else
        {
            I2C_1_state = I2C_1_SM_MSTR_WR_ADDR;
        }

        /* Hardware actions: write address and generate ReStart */
        I2C_1_DATA_REG = slaveAddress;
        I2C_1_GENERATE_RESTART_MANUAL;
        count = 0;
        /* Wait until address has been transferred */
        while(I2C_1_WAIT_BYTE_COMPLETE(I2C_1_CSR_REG))
        {
         CyDelayUs(1);
         count++;
         if ( count > 200 )
         {
          break; 
         }
        }
  
        if(I2C_1_CHECK_ADDR_NAK(I2C_1_CSR_REG))
        {
            /* Address has been NACKed: reset FSM to IDLE */
            I2C_1_state = I2C_1_SM_IDLE;
            errStatus = I2C_1_MSTR_ERR_LB_NAK;
        }
        else
        {
            /* ReStart was sent without errors */
            errStatus = I2C_1_MSTR_NO_ERROR;
        }
    }

    return(errStatus);
}
/*******************************************************************************
* Function Name: I2C_1_MasterSendStop
********************************************************************************
*
* Summary:
*  Generates I2C Stop condition on bus. Function do nothing if Start or Restart
*  condition was failed before call this function.
*
* Parameters:
*  None.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  Stop generation is required to complete the transaction.
*  This function does not wait until a Stop condition is generated.
*
* Global variables:
*  I2C_1_state - The global variable used to store a current state of
*                           the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 I2C_MasterSendStop(void)  {
    uint8 errStatus;
    uint8 count = 0;
    errStatus = I2C_1_MSTR_NOT_READY;

    /* Check if master is active on bus */
    if(I2C_1_CHECK_MASTER_MODE(I2C_1_MCSR_REG))
    {
        I2C_1_GENERATE_STOP_MANUAL;
        I2C_1_state = I2C_1_SM_IDLE;

        /* Wait until stop has been generated */
        while(I2C_1_WAIT_STOP_COMPLETE(I2C_1_CSR_REG))
        {
         CyDelayUs(1);
         count++;
         if ( count > 100 )
         {
          break; 
         }
        }

        errStatus = I2C_1_MSTR_NO_ERROR;
    }

    return(errStatus);
}
/*******************************************************************************
* Function Name: I2C_MasterWriteByte
********************************************************************************
*
* Summary:
*  Sends one byte to a slave. A valid Start or ReStart condition must be
*  generated before this call this function. Function do nothing if Start or
*  Restart condition was failed before call this function.
*
* Parameters:
*  data:  The data byte to send to the slave.
*
* Return:
*  Status error - Zero means no errors.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  I2C_1_state - The global variable used to store a current state of
*                           the software FSM.
*
*******************************************************************************/
uint8 I2C_MasterWriteByte(uint8 theByte) {
    uint8 errStatus;
    uint8 count = 0;
    errStatus = I2C_1_MSTR_NOT_READY;

    /* Check if START condition was generated */
    if(I2C_1_CHECK_MASTER_MODE(I2C_1_MCSR_REG))
    {
        I2C_1_DATA_REG = theByte;   /* Write DATA register */
        I2C_1_TRANSMIT_DATA_MANUAL; /* Set transmit mode   */
        I2C_1_state = I2C_1_SM_MSTR_WR_DATA;

        /* Wait until data byte has been transmitted */
        while(I2C_1_WAIT_BYTE_COMPLETE(I2C_1_CSR_REG))
        {
         CyDelayUs(1);
         count++;
         if ( count > 100 )
         {
          break; 
         }
        }

        if(I2C_1_CHECK_DATA_ACK(I2C_1_CSR_REG))
        {
            I2C_1_state = I2C_1_SM_MSTR_HALT;
            errStatus = I2C_1_MSTR_NO_ERROR;
        }
        else
        {
            I2C_1_state = I2C_1_SM_MSTR_HALT;
            errStatus = I2C_1_MSTR_ERR_LB_NAK;
        }
    }

    return(errStatus);
}
/*******************************************************************************
* Function Name: I2C_MasterReadByte
********************************************************************************
*
* Summary:
*  Reads one byte from a slave and ACK or NACK the transfer. A valid Start or
*  ReStart condition must be generated before this call this function. Function
*  do nothing if Start or Restart condition was failed before call this
*  function.
*
* Parameters:
*  acknNack:  Zero, response with NACK, if non-zero response with ACK.
*
* Return:
*  Byte read from slave.
*
* Side Effects:
*  This function is entered without a "byte complete" bit set in the I2C_CSR
*  register. It does not exit until it is set.
*
* Global variables:
*  I2C_1_state - The global variable used to store a current
*                           state of the software FSM.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 I2C_MasterReadByte(uint8 acknNak) {
    uint8 theByte;
    uint8 count = 0;
    theByte = 0u;

    /* Check if START condition was generated */
    if(I2C_1_CHECK_MASTER_MODE(I2C_1_MCSR_REG))
    {
        /* When address phase needs to release bus and receive byte,
        * then decide ACK or NACK
        */
        if(I2C_1_SM_MSTR_RD_ADDR == I2C_1_state)
        {
            I2C_1_READY_TO_READ_MANUAL;
            I2C_1_state = I2C_1_SM_MSTR_RD_DATA;
        }

        /* Wait until data byte has been received */
        while(I2C_1_WAIT_BYTE_COMPLETE(I2C_1_CSR_REG))
        {
         CyDelayUs(1);
         count++;
         if ( count > 100 )
         {
          break; 
         }
         
        }

        if ( count > 100 )
        {
         theByte = 0xAA;
        }
        else
        {
         theByte = I2C_1_DATA_REG;
        }  

        /* Command ACK to receive next byte and continue transfer.
        *  Do nothing for NACK. The NACK will be generated by
        *  Stop or ReStart routine.
        */
        if(acknNak != 0u) /* Generate ACK */
        {
            I2C_1_ACK_AND_RECEIVE_MANUAL;
        }
        else              /* Do nothing for the follwong NACK */
        {
            I2C_1_state = I2C_1_SM_MSTR_HALT;
        }
    }

    return(theByte);
}
/*******************************************************************************
* Function Name: writePointerValue
********************************************************************************
* Summary: Write a 8 bit register value to the EPSON RX-8035 RTC
*  
*
* Parameters: pointer to the register
*  None
*
* Return: the 8 bit register value
*  None
*
*******************************************************************************/
uint8 writePointerValue ( uint8 ptr_rtc, uint8 value ) {
 uint8 i2c_error;
 uint8 intState;
 intState = CyEnterCriticalSection(); 

  // write address and pointer 
  i2c_error = I2C_MasterSendStart( RTC_ADDRESS, I2C_1_WRITE_XFER_MODE);  
    
  if ( i2c_error == I2C_1_MSTR_NO_ERROR )
  {
    i2c_error = I2C_MasterWriteByte ( ptr_rtc );            // write the pointer address to Device ID ( 0x1004 )
  }  
    
  if ( i2c_error == I2C_1_MSTR_NO_ERROR )
  {
    i2c_error = I2C_MasterWriteByte( value );   //read byte, send ACK MSB
      
  }  
  i2c_error =I2C_MasterSendStop(); // send stop, since we want to read from device
  CyDelayUs ( 100 );
  
  CyExitCriticalSection(intState);
  return i2c_error;
   

}
/*******************************************************************************
* Function Name: readPointerValue
********************************************************************************
* Summary: Read a 7, 8 bit register values from the RX-8035
*  
*
* Parameters: pointer to the register
*  None
*
* Return: the 16 bit register value
*  None
*
*******************************************************************************/
uint8 readPointerValue ( uint8* rtc_data ) {

 uint8 i2c_error, n;
 uint8 intState;
 intState = CyEnterCriticalSection(); 
  
  // write address and pointer 
  i2c_error = I2C_MasterSendStart( RTC_ADDRESS, I2C_WRITE);
    
  if ( i2c_error == I2C_1_MSTR_NO_ERROR )
  {
    i2c_error = I2C_MasterWriteByte ( 0 );      // write the pointer address 
  }  
  
  // read byte from device 
  if ( i2c_error == I2C_1_MSTR_NO_ERROR )
  {
    i2c_error = I2C_MasterSendRestart( RTC_ADDRESS, I2C_READ);    // set for read mode
  }  
    
  if ( i2c_error == 0 )
  {
    for ( n = 0; n < 6; n++ )
    { 
      rtc_data[n] = I2C_MasterReadByte( I2C_1_ACK_DATA );   //read byte, send ACK MSB
    }  
    rtc_data[6] = I2C_MasterReadByte( I2C_1_NAK_DATA );   //read byte, send ACK LSB
     
  }  
  i2c_error = I2C_1_MasterSendStop();   // send stop, since we want to read from device
  CyDelayUs ( 100 );
  
  CyExitCriticalSection(intState);
  return i2c_error; 
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
uint32_t decode_date ( date_time_t date ) {
  
  float counter;
  
  counter  =  ( (float)date.imonth * 30.42 ) + (float)date.iday  + ( (float)date.iyear * 365.25 );  
                   
 
  return( (uint32_t) counter );  
}
/*******************************************************************************
* Function Name: bcdToDecimal
********************************************************************************
* Summary: Convert a BCD number to its decimal equivalent
*  
*
* Parameters: 8 bit BCD value
*  None
*
* Return: the 8 bit decimal
*  None
*
*******************************************************************************/
uint8 bcdToDecimal ( uint8 value ) {
 uint8 valuem = (((value & 0xF0)/16) * 10) + ( value & 0x0F );
 return valuem; 
}
/******************************************************************************
 *
 *  Name: read_I2C_RTC
 *
 *  PARAMETERS: pointer to date/time struct
 *
 *  DESCRIPTION: read the RX-8035 RTC and return the time/date
 *               
 *            
 *  RETURNS: error and data in pointer 
 *
 *****************************************************************************/ 
void read_RTC ( date_time_t* d_time ) {  //reads time and date from RTC
  uint8 error;
  uint8 rtc[16];
  // get the time and date into a 32 bit union
  error = readPointerValue ( rtc );
  if ( error == 0 )
  {
    d_time->ihour = (bcdToDecimal (rtc[2]& 0x3F)) ; // mask off 24/12 flag
    d_time->iyear = bcdToDecimal (rtc[6]) + 2000;
    d_time->imonth =  bcdToDecimal (rtc[5]);
    d_time->iday = bcdToDecimal (rtc[4]);
    d_time->iminute =  bcdToDecimal(rtc[1]);
    d_time->isecond = bcdToDecimal (rtc[0]);
  }
  else
    {
    d_time->ihour = 12;
    d_time->iyear = 00;
    d_time->imonth =  1;
    d_time->iday = 1;
    d_time->iminute =  0;
    d_time->isecond = 0;
  }
} 
/******************************************************************************
 *
 *  Name:  ( date_time_t date )
 *
 *  PARAMETERS: date/time struct and dest. address of RAM
 *
 *  DESCRIPTION: Takes the integer values from the struct and makes a string
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
void getTimeDateStr ( date_time_t date, char * d_t_str ) {

 char AM_PM[] = "AM";
 
 if ( date.ihour > 12 )
 {
    date.ihour -= 12;
    AM_PM[0] = 'P';    
 }
 else if ( date.ihour == 12 )   // Noon
 {
   AM_PM[0] = 'P';    
 }
 else if ( date.ihour == 0 )   // Midnight
 {
   date.ihour = 12;
 }
  
 sprintf( d_t_str,"%02u/%02u/%04u %02u:%02u %s", date.imonth, date.iday,date.iyear, 
                                    date.ihour,date.iminute, AM_PM );
 
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
void  convertRTCtoAlfatTime ( date_time_t date ) {
 if ( date.iyear > 1979 ) {
    time_date.dateTimeBits.Years =  date.iyear - 1980;
 }
 else {
   time_date.dateTimeBits.Years = 0;
 }    
 time_date.dateTimeBits.Day     = date.iday;
 time_date.dateTimeBits.Month   = date.imonth;
 time_date.dateTimeBits.Hour    = date.ihour;
 time_date.dateTimeBits.Minute  = date.iminute; 
 time_date.dateTimeBits.Second2 = date.isecond/2;
}
/******************************************************************************
 *
 *  Name: printTimeDate ( date_time_t date )
 *
 *  PARAMETERS: date/time struct
 *
 *  DESCRIPTION: Prints the MONTH:DAY:YEAR HOUR:MINUTE to LCD
 *               
 *            
 *  RETURNS: NA 
 *
 *****************************************************************************/ 
void printTimeDate ( date_time_t date )
{
 char lcd[21];
 
 getTimeDateStr ( date, lcd );
      
 LCD_print ( lcd );
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

Bool date_check(uint8_t month_temp, uint8_t day_temp)  // verifies that the entered date is valid
{
  Bool date_correct = 1;


  if ( ( ( (month_temp == 1 ) || (month_temp == 3) || (month_temp == 5) || (month_temp == 7) || (month_temp == 8) 
          || (month_temp == 10) || (month_temp == 12)) && day_temp > 31) )
  {
    date_correct = 0;                                                               //not feb, apr, jun, sep, or nov and date not valid  
  }  
  else if ( ( (month_temp == 4) || (month_temp == 6) || (month_temp == 9) || (month_temp == 11)) && (day_temp > 30))
  {
    date_correct = 0;                                                               //feb, apr, jun, sep, or nov and date not valid  
  }
  //february and date not valid
  else if ( (month_temp == 2) && (day_temp > 29 ) )
  {
    date_correct = 0;
  }
  else if ( month_temp > 12)                       
  {
    date_correct = 0;
  }
  
  if(!date_correct)
  {
    enter_valid_text(0);  //TEXT// display "  Please Enter"\n   Valid Date" LINE2,3    
    return(0);
  }  
return(1);  
}

Bool time_check(uint8_t hr)   // verifies that the entered date is valid
{      
   if(hr > 12)
   {
     enter_valid_text(1); //TEXT// display "    Please Enter\n     Valie Time" LINE2,3
     return (0);
   }
   return (1);
}
uint8 DecimalToBCD ( uint8 value )
{
 uint8 valuem = value/10 ;
 
 valuem *= 16; 
 valuem += (value % 10 ); 
  
 return valuem; 
}
void RTC_WriteTime ( date_time_t* d_time )  //write time and date from RTC
{
  uint8 error;
  uint8 sec, min, hr, day, month;
  uint16 yr;
  uint8 lsb_yr;
  
  // Put info into BCD format needed for RTC
  sec = DecimalToBCD (d_time->isecond);
  min = DecimalToBCD (d_time->iminute);
  hr =  DecimalToBCD ( d_time->ihour );
  hr |= 0x80;                              // 24 hour mode
  day = DecimalToBCD ( d_time->iday );
  month = DecimalToBCD ( d_time->imonth );
  
//  if ( d_time->Year < 2000 ) // date time sent from PC cannot be < 2022
//  {
//    yr = 0x2000;
//  }
//  else
    if ( d_time->iyear > 2099 ) { yr = 0x2100; }
    else { yr = 0x2000; }
    lsb_yr = d_time->iyear - 2000;
    yr += DecimalToBCD ( lsb_yr );
  
  error = writePointerValue (0x00, sec);    //sec
  if(error == 0) {error = writePointerValue(0x10, min); }
  if(error == 0) {error = writePointerValue(0x20, hr); }  
  if(error == 0) {error = writePointerValue(0x40, day); } 
  if(error == 0) {error = writePointerValue(0x50, month); }
  if(error == 0) {error = writePointerValue(0x60, yr); }
}
void enterTimeRTCI2C (void)  // leads user through process to set the time and date
{
  Bool date_set = 0, menu_exit = 0, escape = 0;
  uint8_t i, j, msb_yr=0, lsb_yr=0, msb_month=0, lsb_month=1, msb_day=0, lsb_day=1,  msb_hr=0, lsb_hr=0, msb_min=0,
      lsb_min, month_temp=2, day_temp=1, date_reg = 0, time_reg = 0;  
  uint16_t min, hr, day, month, yr;
  
  enum buttons button;
  
/////////////////////////Enter the date///////////////////////////////////////////
  CURSOR_OFF;
  CLEAR_DISP;
  // Display the Calibration Date
 
  LCD_position (LINE1);
  printTimeDate ( date_time_g ) ;
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
   return;
  }  

  // Get a valid access code before changing time
  if ( 0 == enter_access_code() )
  {
   return;
  } 
  
  while(!escape)
  {
    CLEAR_DISP;
    LCD_position(LINE1);
    _LCD_PRINT ( "Enter Current");
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
            date_set = 1;            
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
  }
////////////////////////////Enter the time////////////////////////////////////
  escape = FALSE;
  while(!escape && date_set)
  {
    while(!escape)
    {
      enter_time_text();    //TEXT// display "Enter Present\nTime: hh:mm" LINE1,2      
      YES_to_Accept(LINE3); //TEXT// display "YES to Accept"    
      ESC_to_Exit(LINE4);   //TEXT// display "ESC to Exit"    
      
      i = 0;
      time_reg = 0;
      j = 1;
      
      while(1)                                        //loop for entering each cnt cycle digit
      {         
        if(j==5) CURSOR_OFF;
        else CURSOR_ON;
        LCD_position(LINE2 + (i+6));                // position cursor and flashing  8
        while(1)
        {
          button = getKey(TIME_DELAY_MAX);
          if((button <= 9) || (button == YES) || (button == CE) || (button == ESC))
          { break ; }
        }        
        if(button <= 9)                          // digit was entered   
        {
          if(j == 1)               // first digit must be 1 or 0
          {
            if(button < 2)
            {
              _LCD_PRINTF ( "%c",key_char);
              msb_hr = button;
              j++;
              i++;
              BIT_SET(time_reg,0);
            }
            else if(button > 1)
            {
            _LCD_PRINTF ( "0%c",key_char);
            msb_hr = 0;
            lsb_hr = button;
            j += 2;
            i += 3;
            BIT_SET(time_reg,0);
            BIT_SET(time_reg,1);
            }           
          }
          else if(j == 2)
          { 
            if((msb_hr == 0) && (button > 0))        // second digit must be 1 - 9
            {
              _LCD_PRINTF ( "%c",key_char);
              lsb_hr = button;
              j++;
              i = i+2;
            }
            else if((msb_hr == 1)  && (button < 3))  // second digit must be 0 - 2
            {
              _LCD_PRINTF ( "%c",key_char);
              lsb_hr = button;
              j++;
              i = i+2;
            }
            BIT_SET(time_reg,1);
          }  
          else if((j == 3) && (button < 6))          // third digit must be 0 - 5
          {
            _LCD_PRINTF ( "%c",key_char);
            msb_min = button; 
            j++;
            i++;
            BIT_SET(time_reg,2);
          }
          else if(j == 4)                               // fourth digit
          {
            _LCD_PRINTF ( "%c",key_char);
            lsb_min = button;
            j++;
            i++; 
            BIT_SET(time_reg,3);
          }
        }
        else if((button == YES) && (time_reg == 15))          // Yes key was pressed and all spaces filled
        {
          CURSOR_OFF;
          if(time_check(msb_hr*10 + lsb_hr) == 1)                   // 1 = entered valid time
          {
            escape = TRUE;            
          }    
          break;
        }
        else if(button == CE)                   // NO/CE key was pressed, clear last digit entered   
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
            else if (j == 5)
            {
              --i;
              --j;
            } 
            LCD_position(LINE2 + (i + 6)); 
          }       
        }
        else                                                   // MENU or STOP was pressed, exit to MENU
        {
          escape = TRUE;
          CURSOR_OFF;
          menu_exit = TRUE;
          break;
        }        
      }  // end of loop for entering each  digit      
    }
    if(menu_exit == FALSE)
    {
      am_pm_text();  //TEXT// display "Select\n1. AM\n2. PM" LINE1,2,3      
      while(1)
      {
        button = getKey(TIME_DELAY_MAX);
       
        if((button == 1) || (button == 2) || (button == ESC))
        {
          break;
        }    
      }             
      if((button == 1) || (button == 2))
      {
        if(button == 2)
        {        
          if((lsb_hr <= 7) && (msb_hr == 0))
          { 
            lsb_hr += 2;
            msb_hr = 1;
          }
          else if((lsb_hr >= 8) && (msb_hr == 0))
          { 
            lsb_hr -= 8;
            msb_hr = 2;
          }
          else if((msb_hr == 1) && (lsb_hr < 2))      //10 or 11PM 
          {
            lsb_hr += 2;
            msb_hr = 2;
          }
        } 
        else                                     //AM time update
        {         
          if((lsb_hr == 2) && (msb_hr == 1))
          { 
            lsb_hr = 0;
            msb_hr = 0;
          }     
        }    
  
        // Put info into BCD format needed for RTC
        min = (msb_min<<4) + lsb_min;
        hr =  ((msb_hr<<4) + lsb_hr) | 0x80;    // 24 hour mode
        day = (msb_day<<4) + lsb_day;
        month = (msb_month<<4) + lsb_month;
        yr =  (msb_yr<<4) + lsb_yr;     
         
        uint8 error; 
        error =  writePointerValue ( 0x00, 0 );    //sec
        
        if ( error == 0 )
        {
          error =  writePointerValue ( 0x10, min );  // minute
        }
        
        if ( error == 0 )
        {        
          error =  writePointerValue ( 0x20, hr );  // hour
        }  
        if ( error == 0 )
        {
          error =  writePointerValue  ( 0x40, day );  // day
        }  
        
        if ( error == 0 )
        {
          error =  writePointerValue  ( 0x50, month );  // month
        }  
        
        if ( error == 0 )
        {
         error =  writePointerValue  ( 0x60, yr );  // year
        }
             
        if ( error == 0 )
        {
        day_counter_g = decode_date( date_time_g ) - decode_date ( NV_RAM_MEMBER_RD (Constants.CAL_DATE) );  
        } 
        
        if ( error == 0 )
        {       
          break;
        }  
        else
        {
         clock_set_error_text();
         break; 
        }
      }  
      else if (button == ESC)   //MENU or STOP was pressed, exit to MENU
      {
         escape = TRUE;           
      }   
    }
  }
}


/* [] END OF FILE */
