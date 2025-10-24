
#ifndef TESTS_H
#define TESTS_H
#include <Globals.h>

void storeDriftResultsToUSB ( Bool display_error );
void stand_test(void);
void general_purpose_test(void) ;
uint8_t stat_test(void) ;
void drift_test(void) ;
extern void selfTest(void);
void diag_self_test(void);
void diag_self_test_print(uint8_t destination);
void measurePulses ( uint8_t line, uint8_t time1, uint16_t * moisture_count, uint32_t * density_count,uint8 depth ) ;
void storeStdCountsToUSB ( Bool display_error );
void extended_drift_test ( void );

#endif
