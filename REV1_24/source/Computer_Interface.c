/**************************   ***************************************************************** ***************************/
//
// Title:             Elite
// Company:     Instrotek
//
// Document No.:
// Issue:
// Description:  
//
// Filename:    Computer_Interface .c
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
#include "UARTS.h"
#include "LCD_drivers.h"


/************************************* EXTERNAL VARIABLE AND BUFFER DECLARATIONS  *************************************/

/************************************************  LOCAL DEFINITIONS  *************************************************/

/*****************************************  VARIABLE AND BUFFER DECLARATIONS  *****************************************/
                        
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
 #define COMM_DELAY()   delay_ms(100) 

/////COMPUTER INTERFACE FUNCTIONS ////////////////////////////////////////////////////////

void get_const_computer(void)  // receives constants from computer
{
  
  uint8_t i, depth, depth_temp = 0;
  DOUBLE_FLOAT  value;
  uint16_t std_value;
  date_time_t cal_date;
   
  char space[] = ",";
  char *A, *B, *C, *E, *F;
  char cal_values[50];
  
  UART2_Start();
  isrUART2_Enable();
  UART2_EnableTxInt();
    
  while(1)                            // prompt for backscatter constants
  {
    clearComputerRXBuffer();
    UART2_ClearTxBuffer();
    
    depth = BS;                        // B_SCAT_A location in memory
    
    COMM_DELAY(); 
    sprintf( lcdstr, "Enter the Backscatter values A, B, C and <enter>\n\r");
    UART2_PutString ( (const char *)lcdstr ) ; // Blocks until all data is placed in TX buffer
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
        
    gets_comp(cal_values);  // read values from computer
    
    // Parse the input for A,B,C values
    A = strtok(cal_values,space);
    B = strtok(0,space);
    C = strtok(0,space);
    
    // Save BS constants
    value = atof(A); 
    NV_MEMBER_STORE(Constants.DEPTHS[depth].A, value);
    
    value = atof(B); 
    NV_MEMBER_STORE(Constants.DEPTHS[depth].B, value);
    
    value = atof(C);
    NV_MEMBER_STORE(Constants.DEPTHS[depth].C, value);
     
    // get the value from EEPROM (not ram)
    NV_EE_RD ( Constants.DEPTHS[depth].A, &value); 
  
        
    sprintf ( lcdstr, "\n\rA = %.5Lf\n\r", value );
    UART2_PutString ( (const char *)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );   
    COMM_DELAY();    
      
    NV_EE_RD ( Constants.DEPTHS[depth].B, &value ); 
    sprintf ( lcdstr, "B = %.5Lf\n\r", value );
    UART2_PutString ( (const char *)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
        
    NV_EE_RD ( Constants.DEPTHS[depth].C, &value ); 
    sprintf ( lcdstr, "C = %.5Lf\n\r", value );
    UART2_PutString ( (const char *)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
    

    sprintf ( lcdstr, "Are these values correct? Y or N\n\n\r");
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );    
    COMM_DELAY();    
    
    if ( waitc_RX(&s_Computer_RX, 600000) == 'Y')
    {
        break;
    }   
  }
  
  i=1;
  for ( depth = DEPTH_2_INCH; depth < MAX_DEPTHS; depth++)  // prompt for remaining 12 depth constants
  {
    i++;
    clearComputerRXBuffer();
     
    sprintf ( lcdstr, "Enter Depth %d values A, B, C, or S(skip) and <enter>\n\r", i );
   
    UART2_PutString ( (const char *)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
    gets_comp(cal_values);
   
    if(toupper((int)cal_values[0]) == 'S')
    {
        depth += 12;
    }    
    else
    {
      depth_temp = depth;
      while(1)
      {
        // Parse the input for A,B,C values
        A = strtok(cal_values,space);
        B = strtok(0,space);
        C = strtok(0,space);
        
        // Save BS constants
        value = atof(A); 
        NV_MEMBER_STORE(Constants.DEPTHS[depth].A, value);
        
        value = atof(B); 
        NV_MEMBER_STORE(Constants.DEPTHS[depth].B, value);
        
        value = atof(C);
        NV_MEMBER_STORE(Constants.DEPTHS[depth].C, value);
         
        // get the value from EEPROM (not ram)
        NV_EE_RD ( Constants.DEPTHS[depth].A, &value ); 
                  
        sprintf ( lcdstr, "\n\rA = %.5Lf\n\r", value );
        UART2_PutString ( (const char*)lcdstr ) ; 
        while ( UART2_GetTxBufferSize() != 0 );  
        COMM_DELAY();    
       
        NV_EE_RD ( Constants.DEPTHS[depth].B, &value ); 
        
        sprintf ( lcdstr, "B = %.5Lf\n\r", value );
        UART2_PutString ( (const char *)lcdstr ) ; 
        while ( UART2_GetTxBufferSize() != 0 );   
        COMM_DELAY();    
            
        NV_EE_RD ( Constants.DEPTHS[depth].C,&value ); 
        sprintf ( lcdstr, "C = %.5Lf\n\r", value );
        UART2_PutString ( (const char *)lcdstr ) ; 
        while ( UART2_GetTxBufferSize() != 0 );   
        COMM_DELAY();    
        
        sprintf ( lcdstr, "Are these values correct? Y or N\n\n\r");
        UART2_PutString ( (const char *)lcdstr ) ; 
        while ( UART2_GetTxBufferSize() != 0 );  
        COMM_DELAY();    
        
        if ( waitc_RX(&s_Computer_RX, 600000)  == 'Y')
        {
            break;
        }    
        else
        {
          depth = depth_temp;
          clearComputerRXBuffer();
                  
          sprintf ( lcdstr, "Enter Depth %d values A, B, C and <enter>\n\r", i);
          UART2_PutString ( (const char *)lcdstr ) ; 
          while ( UART2_GetTxBufferSize() != 0 );  
          COMM_DELAY();    
        
          gets_comp(cal_values);
        }
      }
    }
  }
  
  while(1)
  {
    clearComputerRXBuffer();

    sprintf( lcdstr, "Enter Moisture values E, F  and <enter>\n\r");
    UART2_PutString ( (const char *)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
    
  
    gets_comp(cal_values);
    
    E = strtok(cal_values,space);   // E Value
    F = strtok(0,space);            // F Value
    
    value = atof(E); 
    NV_MEMBER_STORE(Constants.E_MOIST_CONST, value);    // Store E value
    
    value = atof(F); 
    NV_MEMBER_STORE(Constants.F_MOIST_CONST, value);  // Store F value
       
    // get the value from EEPROM (not ram)
    NV_EE_RD ( Constants.E_MOIST_CONST, &value ); 
    
    sprintf( lcdstr, "E = %.5Lf\n\r", value);
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
           
    // get the value from EEPROM (not ram)
    NV_EE_RD ( Constants.F_MOIST_CONST, &value ); 
    
    sprintf( lcdstr, "F = %.5Lf\n\r", value);
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
   
    sprintf( lcdstr, "Are these values correct? Y or N\n\n\r");
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
    
    if(waitc_RX(&s_Computer_RX, 600000) == 'Y')
    {
      break;
    }    
  }  
 
  // get standard counts
  while(1)
  {
    clearComputerRXBuffer();

    sprintf( lcdstr, "Enter Standard Counts Values DEN, MOIST and <enter>\n\r");
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
  
    gets_comp(cal_values);
    
    A = strtok(cal_values,space);   // DENSITY STD
    B = strtok(0,space);            // MOISTURE STD
    
    std_value = atoi(A); 
    NV_MEMBER_STORE( DENSE_CAL_STD, std_value);    // Store Density std
    
    std_value = atoi(B); 
    NV_MEMBER_STORE(MOIST_CAL_STD, std_value);  // Store Moisture Std.
    std_value = 0;   
    // get the value from EEPROM (not ram)
    NV_EE_RD ( DENSE_CAL_STD, &std_value ); 
    
    sprintf( lcdstr, "DEN = %u\n\r", std_value );
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
    std_value = 0;   
    // get the value from EEPROM (not ram)
    NV_EE_RD ( MOIST_CAL_STD, &std_value ); 
    
    sprintf( lcdstr, "MOIST = %u\n\r", std_value);
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
   
    sprintf( lcdstr, "Are these values correct? Y or N\n\n\r");
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
    
    if(waitc_RX(&s_Computer_RX, 600000) == 'Y')
    {
     break;
    }  
  }  
  
  // get cal date
  while(1)
  {
    clearComputerRXBuffer();
        
    sprintf( lcdstr, "Enter Cal. Year, Month, Day and <enter>\n\r");
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
    
  
    gets_comp(cal_values);
    
    A = strtok(cal_values,space);   // YEAR
    B = strtok(0,space);            // MONTH
    C = strtok(0,space);            // DAY
    
    // set the date and time in the date struct
    cal_date.iyear  = atoi(A); 
    cal_date.imonth = atoi(B);
    cal_date.iday   = atoi(C);
    cal_date.isecond = 0;
    cal_date.ihour   = 12;
    cal_date.iminute = 0; 
    
    //write the date/time to memory
    NV_MEMBER_STORE(Constants.CAL_DATE, cal_date);
    
    cal_date.iyear   = 0;
    cal_date.imonth  = 0;
    cal_date.iminute = 0;

    cal_date = NV_RAM_MEMBER_RD(Constants.CAL_DATE);
    
    sprintf( lcdstr, "Year = %u\n\r", cal_date.iyear );
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
 
    sprintf( lcdstr, "Month = %u\n\r", cal_date.imonth );
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
   
    sprintf( lcdstr, "Day = %u\n\r", cal_date.iday );
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
   
    sprintf( lcdstr, "Are these values correct? Y or N\n\n\r");
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
    
    
    if(waitc_RX(&s_Computer_RX, 600000) == 'Y')
    {
      break;
    }  
  }  
  // Send the 12 depths A,B,C values and the E and F values
  // read memory locations and send via serial port
  for ( depth = BS; depth < MAX_DEPTHS; depth++)      
  {
    
    NV_EE_RD ( Constants.DEPTHS[depth].A, &value );
       
    sprintf( lcdstr, " %.5Lf\n\r", value );
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
    
   
    NV_EE_RD ( Constants.DEPTHS[depth].B, &value );
       
    sprintf( lcdstr, " %.5Lf\n\r",value );
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
    
   
    NV_EE_RD ( Constants.DEPTHS[depth].C, &value );
       
    sprintf( lcdstr, " %.5Lf\n\r",value );
    UART2_PutString ( (const char *)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
       
  }
  
    // get the value from EEPROM (not ram)
    NV_EE_RD ( Constants.E_MOIST_CONST, &value ); 
    
    sprintf( lcdstr, "E = %.5Lf\n\r", value);
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
   
    // get the value from EEPROM (not ram)
    NV_EE_RD ( Constants.F_MOIST_CONST, &value ); 
    
    sprintf( lcdstr, "F = %.5Lf\n\r", value);
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
     
    // get the value from EEPROM (not ram)
    NV_EE_RD ( DENSE_CAL_STD, &std_value); 
    
    sprintf( lcdstr, "DEN = %u\n\r", std_value);
    UART2_PutString ( (const char*)lcdstr ) ; 
     while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
 
    // get the value from EEPROM (not ram)
    NV_EE_RD ( MOIST_CAL_STD, &std_value ); 
    
    sprintf( lcdstr, "MOIST = %u\n\r", std_value);
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
 
    cal_date = NV_RAM_MEMBER_RD(Constants.CAL_DATE);
    
    sprintf( lcdstr, "Year = %u\n\r", cal_date.iyear );
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
    
    sprintf( lcdstr, "Month = %u\n\r", cal_date.imonth );
    UART2_PutString ( (const char*)lcdstr ) ; 
     while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
     
    sprintf( lcdstr, "Day = %u\n\r", cal_date.iday );
    UART2_PutString ( (const char*)lcdstr ) ; 
    while ( UART2_GetTxBufferSize() != 0 );  
    COMM_DELAY();    
  
    update_valid_depths();    // update valid depths
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
  
void wait_for_serial(void)  // if computer is connected, wait for input from the computer
{  
  char command;  
  
  COM_SELECT_Write ( 0 ); 
  initUARTS();      // moved to main.c to support the Plus depth sensor comms.  
  
  UART2_Start();
  isrUART2_Enable();
  clearComputerRXBuffer();
  
  while ( IS_ACTICE_RS232_DETECTED()  )
  {
   UART2_PutString ( "Enter 'L'(load cal data), 'A'(access serial #) or 'E'(exit)\n\r" ) ; 
   while ( UART2_GetTxBufferSize() != 0 );  
   
   delay_ms(100);
    
    while ( ( getRXNumEntries ( &s_Computer_RX) == 0 ) && ( IS_ACTICE_RS232_DETECTED()  ) )
    {
      delay_us(10);
    }

    delay_ms (1);
    
    if ( getRXNumEntries(&s_Computer_RX) > 0 )  
    {
      
      command =  getcRXBuffer(&s_Computer_RX);
      if(command == 'L')                       // load constants Ai, Bi, Ci,...E, F
      {
          get_const_computer();        
      }    
      else if (command == 'A')               //   access serial #
      {
          serial_number(1);     
      }    
      else if (command == 'E')               //   exit            
      {
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
  
void gets_comp(char* s)  // reads a string from UART2 XMIT = PIN_F5, RCV = PIN_F4 
{
  #define MAX_RX_TIME_ERROR   0x1122
  uint16_t len = 0, c;
  do
  {
    c = waitc_RX ( &s_Computer_RX , 1000 );
    if ( c != MAX_RX_TIME_ERROR )
    {
     if(c>=' ' && c<='~')
     {
       s[len++]=c;
     }  
    }
  } while(c!=13);
  
  s[len]='\0';
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
  
void puts_printer ( char *s )  //print the string to attached printer
{
  uint8_t c = 0;  
  char data_byte ;
  CLEAR_BUFFER2;
  
  ok_2_print = TRUE;
  
  do
  {
    if ( ok_2_print )
    {
      data_byte  = *(s + c++);   
      UART2_PutChar( data_byte );  //send character to printer up to and including CR
      while ( UART2_GetTxBufferSize() != 0 )   
      {
        CyDelayUs ( 100 );
      }  
    }    
   else
   {
      delay_ms(10);
   }    
  
  }while ( data_byte != '\r' );      
 
 
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
  
void hold_buzzer(void)
{
  while ( BUZZER_Read() )
  {
    delay_us(10);
  }  
}

