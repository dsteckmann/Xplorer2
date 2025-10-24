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
#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#include "Globals.h"
#include <FS.h>

  
#define PROJECT_NOT_VALID 0  
  
////////////////////////Memory locations for project storage////////////////////
#pragma pack(1)  
typedef struct station_data_s
{
  char       name[PROJ_NAME_LENGTH]  ;     // name of station                 2 BYTES 0
  uint16_t   depth          ;     // record of depth for count at station     2 BYTES 2
  uint32_t   density_count  ;     // density count at station                 4 BYTES 4
  uint16_t   moisture_count ;     // moisture count at station                2 BYTES 8
  float      density        ;     //                                          4 BYTES 10
  float      moisture       ;     //                                          4 BYTES 14
  uint16_t   density_stand  ;     // d_stand when station count was done      2 BYTES 18
  uint16_t   moisture_stand ;     // m_stand when station count was done      2 BYTES 20  
  float      PR             ;     // PR constant when station count was done  4 BYTES 22 
  float      MA             ;     // MA count when station count was done     4 BYTES 26
  float      MCR            ;     //                                          4 BYTES 30 
  float      DT             ;     //                                          4 BYTES 34   
  uint8_t    units          ;     //                                          1 BYTES 38
  uint8_t    offset_mask    ;     //                                          1 bytes 39
  date_time_t date          ;     //                                          7 bytes 40
  float      den_off        ;     //                                          4 bytes 47
  float      k_value        ;     //                                          4 bytes 51
  float      t_offset       ;     //                                          2 bytes 55
  float      kk_value       ;     //                                          4 bytes 57
  float      bottom_den     ;     //                                          4 bytes 61
  GPSDATA    gps_read       ;     //                                         19 bytes 65
  float      battery_voltage[2];
} station_data_t;                                                      


#pragma pack(1)  
typedef struct 
{
  uint16_t    station_auto_start     ;     // if auto numbering, start here
  uint8_t     station_auto           ;     // auto number enabled 
  uint16_t    station_number         ;     // total number of stations within project, will also tell station index
  station_data_t  station[MAX_STATIONS] ;    // station data
} project_data_t;


/*---------------------------------------------------------------------------*/
/*----------------------[  Global Function Prototypes  ]---------------------*/
/*---------------------------------------------------------------------------*/


uint16_t  getProjectNumber ( void );
void      clearStationNumber ( char* project  );
void      setProjectPointer ( uint16_t proj_ptr  );
uint8     writeStationName (  char* project, char * name, uint16_t index_station );
uint8_t   checkStationAutoNumber ( char* project );
uint16_t  getStationNumber ( char* project );
void      setStationAutoNumber ( char* project, char flag );
uint16    resetProjectStorage ( void );
int32     readStation (char* project, uint16_t index_station, station_data_t * station   );
uint16_t  incrementStationNumber ( char* project   );
void      writeStation ( char* project, uint16_t index_station, station_data_t * station_n );

uint16_t  clearProject  ( char * project_name  );
void      setActiveProjectEE ( char * proj );
void      getActiveProjectEE ( char * proj );
int32     getStationName (  char* project, char * name, uint16_t index_station );
void      writeAutoStationStart ( char* project,  uint16_t auto_start_num );
uint16    getAutoStationStart ( char* project  );

#endif 

/* [] END OF FILE */
