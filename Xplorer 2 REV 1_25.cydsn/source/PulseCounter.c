/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
/* ========================================
 * Copyright InstroTek Inc., 2014
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF InstroTek Inc.
 * ========================================*/
#include "Project.h"
#include "Globals.h"
#include "Elite.h"


extern uint16 const countTime ;

volatile uint32 pulseCounts[2];
volatile BOOL cntDone = FALSE;

 
// After end of one shot pulse, read the remainder in the counters
// Set the global countdown flag equal to TRUE
CY_ISR(ISR_ONESHOT)
{   
  pulseCounts[PROBE_GM_COUNT] += Counter_GM_ReadCounter();
  pulseCounts[PROBE_HE3_COUNT] += Counter_HE3_ReadCounter();
  cntDone = TRUE;
  ONE_SHOT_TIMER_Stop();
}

// counter reloads at 200, so add 200 to the reading
CY_ISR ( ISR_GM )
{
  
  Counter_GM_ReadStatusRegister();
  pulseCounts[PROBE_GM_COUNT] += 200;
}


CY_ISR ( ISR_HE3 )
{
  
  Counter_HE3_ReadStatusRegister();
  pulseCounts[PROBE_HE3_COUNT] += 200;
}


/*******************************************************************************
* Function Name: 
********************************************************************************
* Summary:
*     
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void initOneShotTimer ( void )
{
	one_shot_timer_clock_Start(); // turn on the clock
	ONE_SHOT_TIMER_Start();       // Configure the timer
  isrOneShot_ClearPending();
  isrOneShot_StartEx(ISR_ONESHOT);
  ONE_SHOT_TIMER_Stop();

}


/*******************************************************************************
* Function Name: 
********************************************************************************
* Summary:
*     
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void setOneShotTimeMs ( uint32 one_shot_time_ms )
{
  	uint32 scale = (PULSETIMERCLK/1000);
    
    ONE_SHOT_TIMER_WritePeriod ( one_shot_time_ms * scale + 1);
}


/*******************************************************************************
* Function Name: 
********************************************************************************
* Summary:
*     
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void triggerOneShotPulse ( uint32 one_shot_time_ms )
{
	
  ONE_SHOT_TIMER_Stop();
  ONE_SHOT_TIMER_ClearFIFO();
  isrOneShot_ClearPending();
  setOneShotTimeMs ( one_shot_time_ms );

  ONE_SHOT_RESET_Write(1);			// Put ONE_SHOT in reset
  ONE_SHOT_RESET_Write(0);			// Take ONE_SHOT out of reset
  ONE_SHOT_TIMER_Start();  
}


/*******************************************************************************
* Function Name: 
********************************************************************************
* Summary:
*     
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void stop_ONE_SHOT_Early ( void )
{
  ONE_SHOT_TIMER_Stop();
  ONE_SHOT_TIMER_ClearFIFO();
  ONE_SHOT_TIMER_WriteCounter(0);
  ONE_SHOT_RESET_Write(1);			// Put ONE_SHOT in reset
  isrOneShot_ClearPending();
}



/*******************************************************************************
* Function Name: void moist CntStrt(uint8 timeSec)
********************************************************************************
* Summary: 		sets the timer period to match the given time, 
				resets and starts the timer as a one shot.
* Parameters:   int8 timeSec time to run test in seconds, warmup time in ms
* Return: 		none
*******************************************************************************/
void initPulseCntStrt( void )
{
    initOneShotTimer();
    Counter_GM_Start(); //Init();
    isr_GM_StartEx(ISR_GM);
    Counter_HE3_Start(); //Init();
    isr_HE3_StartEx(ISR_HE3);
    ONE_SHOT_TIMER_Start();
  
}


/*******************************************************************************
* Function Name: PulseCntStrt( uint16 timeSec )
********************************************************************************
* Summary: 		sets the timer period to match the given time, 
*     				resets and starts the timer as a one shot.
* Parameters:  timeSec time to run test in seconds
*
* Return: 		none
*******************************************************************************/
void PulseCntStrt( float timeSec )
{
	uint32 ms = (uint32)(timeSec * 1000.0); 

 	cntDone = FALSE;
  // clear counts to zero
  pulseCounts[PROBE_GM_COUNT] = 0;
  pulseCounts[PROBE_HE3_COUNT] = 0;
  
  // clear hardware to zero
  Counter_GM_WriteCounter(0);  
  Counter_HE3_WriteCounter(0);  
  
  pulseCounts[PROBE_GM_COUNT] += Counter_GM_ReadCounter();
  pulseCounts[PROBE_HE3_COUNT] += Counter_HE3_ReadCounter();
  
  triggerOneShotPulse ( ms );
    
}


uint8 checkCountDone ( void )
{
  return cntDone;

}

void resetPulseTimers ( void )
{
  Global_ID();   

  TIMER_1_WriteCounter ( 0 );
  isrTIMER_1_ClearPending();
  TIMER_1_Start();


  isrTIMER_1_Enable();
  Global_IE();                // turn on all enabled interrupts 

}

uint32 getGMPulseCounts ( void )
{
  return pulseCounts[PROBE_GM_COUNT];
  
}

uint32 getHEPulseCounts ( void )
{
  return pulseCounts[PROBE_HE3_COUNT];
  
}


/* [] END OF FILE */

/* [] END OF FILE */
