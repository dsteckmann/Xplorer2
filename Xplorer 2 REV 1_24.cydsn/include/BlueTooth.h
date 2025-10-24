/* ========================================
 * Copyright InstroTek Inc., 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF InstroTek Inc..
 * ========================================*/

#ifndef BLUETOOTHEADER
#define BLUETOOTHEADER
   
 #include <project.h>
 #include <stdbool.h>
 #include "ProjectData.h"
  
 #define BLUETOOTH_ON    0
 #define BLUETOOTH_OFF   1
 #define BTPINLEN        12
 #define MAXDATALEN      255
 #define BTNAMELEN       17
 #define BTGOTCMD 			1
 #define BTSETPIN 			2
 #define BTAOK	 			3
 #define BTMENU				4
 #define BTGETPIN			5
 #define BTGETNAME       6
   
#define BT_SERIAL_NUMER                       1
#define BT_NO_SEND_LCD                       '2'
#define BT_SEND_LCD                          '3'
#define BT_DISCONNECT                         2

   
 int8 BluetoothEnable();
 void BT_Disable(bool verbal);
 void doBluetooth();
 void sendPcStr(char *str);
 void sendBlueResults( station_data_t station_d ) ;
 
#endif

/* [] END OF FILE */
