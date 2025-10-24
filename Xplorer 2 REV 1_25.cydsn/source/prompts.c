
#include "project.h"
#include "Globals.h"
#include "DataStructs.h"
#include "LCD_drivers.h"

void Press_YES_or_NO (BYTE position)
{ 
  LCD_position(position); 
  if(Features.language_f)
  {  _LCD_PRINT("Press YES or NO"); }
   else
  {  _LCD_PRINT("Presione SI o NO"); } 
}

void ESC_to_Exit(BYTE position)
{
  LCD_position(position);
  if(Features.language_f)
  {  _LCD_PRINT("ESC to Exit"); }
   else
  { _LCD_PRINT("ESC para Salir");}
}

void Enter_to_Accept(BYTE position)
{ 
  LCD_position(position);
  if(Features.language_f)
    _LCD_PRINT("ENTER to Accept");
   else
   _LCD_PRINT("SI para Aceptar");  

}


void YES_to_Accept(BYTE position)
{ 
  LCD_position(position);
  if(Features.language_f)
    _LCD_PRINT("YES to Accept");
   else
   _LCD_PRINT("SI para Aceptar");  

}

void up_down_select_text(int both)
{  
  if(both)
  {
    LCD_position(LINE3);
    if(Features.language_f)
     _LCD_PRINT("  UP/DOWN for Next  ");
     else 
     _LCD_PRINT("    Arriba/Abajo    ");       
  }
  LCD_position(LINE4);
  if(Features.language_f)
    _LCD_PRINT("Select #, ESC Exit  "); 
   else
   _LCD_PRINT("Sel #,ESC para Salir");  
   
}


void up_down_ENTER_select_text()
{  

  LCD_position(LINE2);
  if(Features.language_f)
    _LCD_PRINT(" <ENTER> to Select  "); 
  else
   _LCD_PRINT(" <ENTER> a Escoger  ");
  
  LCD_position(LINE3);
  if(Features.language_f)
    _LCD_PRINT("  UP/DOWN for Next  ");
  else 
    _LCD_PRINT("   Arriba/Abajo     ");       
  
  LCD_position(LINE4);
  if(Features.language_f)
    _LCD_PRINT("     <ESC> Exit     "); 
  else
    _LCD_PRINT("     <ESC> Salir    ");  
   
}

void display_version(void) { 
  char temp_str[20];
  struct features_struct  Feat        =  NV_RAM_MEMBER_RD(FEATURE_SETTINGS); 
  CLEAR_DISP;
  LCD_position(LINE1); 
  // if 3440 
  if ( GAUGE_3440 == NV_RAM_MEMBER_RD (gauge_type ) )
  {
     _LCD_PRINT("      3400     ");
  }  
  else if ( GAUGE_3500 == NV_RAM_MEMBER_RD (gauge_type ) )
  {
    _LCD_PRINT("      Xplorer 2");
  } 
  else if ( GAUGE_3440_PLUS == NV_RAM_MEMBER_RD (gauge_type ) )
  {
    _LCD_PRINT("    3400 PLUS  ");
  }
  
  sprintf(temp_str,"    Version %.2f",VERSION );
  LCD_PrintAtPosition(temp_str,LINE2);
  
  if (Feat.dummy_mode == TRUE )
  { 
    CtrStrNC("DEMO", LINE3); //LCD_PrintAtPosition("       DEMO         ",LINE3);
  }  
}

void calibration_due()
{   //213240  213128 saves 112 bytes, could free a lot of space if all are fixed
  if(Features.language_f)
  {
    CtrStrNC("Calibration Due!",LINE1);
    CtrStrNC("Last Calibration On", LINE2);
  }
  else
  {
    CtrStrNC("Require Calibracion",LINE1);
    CtrStrNC("Ultima cal. hecha en", LINE2);
  }
//  LCD_position(LINE1);
//  if(Features.language_f)
//  {
//    _LCD_PRINT("  Calibration Due!");
//    LCD_position(LINE2);  
//    _LCD_PRINT("Last Calibration On");
//  }
//  else
//  {    
//    _LCD_PRINT("Require Calibracion");  
//    LCD_position(LINE2);  
//     _LCD_PRINT("Ultima cal. hecha en");  
//  }
}

void eeprom_msg(void)
{ 
  CLEAR_DISP;    
  LCD_position (LINE2+9);
  _LCD_PRINT("EE");  
  CyDelay ( 1000 );
}


void initializing(void)
{ 
  CLEAR_DISP;    
  LCD_position (LINE2+5);
  _LCD_PRINT("Initializing");  // no spanish needed now.  Should only see on first programming
}

void main_menu_display(uint8_t menu_trk)   // display for the main menu
{  
  LCD_position(LINE1);  
  if(Features.language_f)
  {
    switch (menu_trk)
    {
      
      case 1: 
          _LCD_PRINT(" 1. Recall          ");
          LCD_position(LINE2);
          _LCD_PRINT(" 2. Offset          ");   
          break;    
      
      case 2: 
          _LCD_PRINT(" 3. Auto Scroll     ");   
          LCD_position(LINE2);
          _LCD_PRINT(" 4. Backlight       ");
          break;    
        
      case 3:    
          _LCD_PRINT(" 5. Stat Test       ");   
           LCD_position(LINE2);               
          _LCD_PRINT(" 6. Drift Test      ");  
          break;
  
      case 4:    
           _LCD_PRINT(" 7. GPS             ");
           LCD_position(LINE2);
           _LCD_PRINT(" 8. Auto Depth      ");           
           break; 
      
      case 5:    
          _LCD_PRINT(" 9. Review STD cnts.");
          LCD_position(LINE2); 
          _LCD_PRINT("10. Select Language ");            
          break;    
          
      case 6:   
          _LCD_PRINT("11. Set Units       ");
           LCD_position(LINE2);
          _LCD_PRINT("12. Standard Mode   ");      
          break;    
     
      case 7:   
          _LCD_PRINT("13. Serial Number   ");
          LCD_position(LINE2);      
          _LCD_PRINT("14. Date/Time       ");      
          break;   
     
      case 8:
          _LCD_PRINT("15. Buzzer/Alarm    ");
           LCD_position(LINE2);
          _LCD_PRINT("16. Special Cal.    ");
           break;    
     
      case 9:    
          _LCD_PRINT("17. Nomograph       ");
          LCD_position(LINE2);
          _LCD_PRINT("18. Cal. Constants  ");
           break;    
          
      case 0:    
         _LCD_PRINT("19. Soil Air Voids   ");         
         LCD_position(LINE2);          
         _LCD_PRINT("20. Diagnostic Test ");         
          break;      
          

          
   }
  }
   else
   {
     switch (menu_trk)
     {
        case 1: 
             _LCD_PRINT("1. Recordar         ");     //RECALL
             LCD_position(LINE2);
             _LCD_PRINT("2. Compensacion     ");  
             break;    
       
       case 2 :
             _LCD_PRINT("3. Voluta Auto      ");         // AUTO SCROLL        
             LCD_position(LINE2);
             _LCD_PRINT("4. Iluninacion LCD  ");         // LCD LIGHT 
             break;    
       case 3: 
          
            _LCD_PRINT("5. Prueba estadist. ");      // STAT TEST
            LCD_position(LINE2);
            _LCD_PRINT("6. Prueba desviacion");      // DRIFT TESTT   
            break;
        
      case 4:    
            _LCD_PRINT("7. GPS             ");        // GPS
            LCD_position(LINE2);
            _LCD_PRINT("8. Profundidad Auto ");        // AUTO DEPTH  
            break;    
      case 5:    
             _LCD_PRINT("9. Revisar STD Cnts");        // review standard counts 
            LCD_position(LINE2);
            _LCD_PRINT("10. Lengua           ");       // language   
            break;        
        case 6:   
            _LCD_PRINT("11. Unidades        ");         // SET UNITS
            LCD_position(LINE2);            
            _LCD_PRINT("12. Modo AVG del STD  ");        // AVG STANDARD COUNT MODE
            break;    
        case 7:   
            _LCD_PRINT("13. Numero de Serie  ");           // Serial Number
            LCD_position(LINE2);
            _LCD_PRINT("14. Fecha y Hora     ");        // DATE TIME 
            break;   
        case 8:
            _LCD_PRINT("15. Alarma En. / Ap.");         // BUZZER ON/OFF
            LCD_position(LINE2);
            _LCD_PRINT("16. Calib. Especial ");           // Special Calibration
            break;    
        case 9:    
            _LCD_PRINT("17.  Nomograph      ");       //  Nomograph
            LCD_position(LINE2);
            _LCD_PRINT("18. Const. de Calib.");       // Cal constants
            break; 
            
       case 0:    
            _LCD_PRINT("19. Tierra Aire Nulo");         
            LCD_position(LINE2);          
            _LCD_PRINT("20. Auto Diagnostico ");           // Diagnostics   
          
          break; 
         
       break;
         
     }
   }
  if(in_menu)
  {  up_down_select_text(1);  }
}

void project_menu_display(uint8_t menu_trk)   // display for the project menu
{
  if(Features.language_f)
  {
     switch (menu_trk)
    {
      case 1:
      LCD_position(LINE1);
      _LCD_PRINT("1. Auto Store       ");
      LCD_position(LINE2);
      _LCD_PRINT("2. Start New Project");
      break;
      
      case 2:
      LCD_position(LINE1);
      _LCD_PRINT("3. Sel. Stored Proj.");
      LCD_position(LINE2);
      _LCD_PRINT("4. Review Data      ");
      break;
      
      case 0:
      LCD_position(LINE1);
      _LCD_PRINT("5. Delete Data      ");
      LCD_position(LINE2);
      _LCD_PRINT("                    ");      
      break;   
     }
   }
    else
    {
      switch (menu_trk)
      {
        case 1:
        LCD_position(LINE1);
        _LCD_PRINT("1. Auto Memoria      ");
        LCD_position(LINE2);
        _LCD_PRINT("2. Nuevo Proyecto    ");
        break;
      
        case 2:
        LCD_position(LINE1);
        _LCD_PRINT("3. Proyect Memorizado");
        LCD_position(LINE2);
        _LCD_PRINT("4. Revisar la Info.  ");
        break;
       
        case 0 :
        LCD_position(LINE1);
        _LCD_PRINT("5. Borrar la Info.   ");       
        LCD_position(LINE2);
        _LCD_PRINT("                    ");       
      }
   }
  if(in_menu)
  {  up_down_select_text(1);  }
}

void print_menu_display( void )   // display for the print menu
{
  CLEAR_DISP; 
  if(Features.language_f)
  {
    LCD_position(LINE1);
    _LCD_PRINT("1. Send Data to USB ");  
    LCD_position(LINE2);
    _LCD_PRINT("2. Print Data       ");
     LCD_position(LINE4);
    _LCD_PRINT("Select #, ESC Exit  "); 

  }
  else
  {
    LCD_position(LINE1);
    _LCD_PRINT("1. Trans. Info al USB");     
     LCD_position(LINE2);
    _LCD_PRINT("2. Imprimir Info.    ");
     LCD_position(LINE4);
    _LCD_PRINT("Sel #,ESC para Salir"); 
  }
   
}




void mode_menu_display(unsigned int menu_trk)
{
  LCD_position(LINE1);    
  switch (menu_trk)
  {
    case 1: _LCD_PRINT("1. NORMAL           ");     
            LCD_position(LINE2);      
            _LCD_PRINT("2. DUMMY            ");
            break;
    case 2:    
            _LCD_PRINT("3. InstroTek        ");
            LCD_position(LINE2);      
            _LCD_PRINT("4. Troxler  3440    ");   
           break; 
    case 0:    
            _LCD_PRINT("5. Troxler 3440 Plus");
            LCD_position(LINE2);      
            _LCD_PRINT("                    ");   
            break;
            
    default:
            break;    
   }
 
  if(in_menu)
    up_down_select_text(1);  
}
// Strat Main Menu
void main_menu_strata_display(uint8_t menu_trk)   // display for the main menu
{  
  LCD_position(LINE1);  
  switch (menu_trk)
  {
    
    case 1: 
        _LCD_PRINT(" 1. Stat Test        ");
        LCD_position(LINE2);
        _LCD_PRINT(" 2. Drift Test       ");     
        break;    
    
    case 2: 
        _LCD_PRINT(" 3. Recall           ");
        LCD_position(LINE2);
        _LCD_PRINT(" 4. Offset           ");     
        break;    
    case 3:    
        _LCD_PRINT(" 5. Diagnostic Test ");            
        LCD_position(LINE2);      
        _LCD_PRINT(" 6. Review STD cnts.");
        break;
   case 4:    
        _LCD_PRINT(" 7. Standard Mode   ");
        LCD_position(LINE2);          
        _LCD_PRINT(" 8. Auto Scroll     ");    
        break;        
    case 5:   
        _LCD_PRINT(" 9. Set Units       ");
         LCD_position(LINE2);
        _LCD_PRINT("10. LCD Light       ");      
        break;    
    case 6:   
        _LCD_PRINT("11. Serial Number   ");
        LCD_position(LINE2);      
        _LCD_PRINT("12. Date/Time       ");      
        break;   
    case 0:
        _LCD_PRINT("13. Buzzer/Alarm    ");
         LCD_position(LINE2);
        _LCD_PRINT("14. Cal. Constants  ");
         break;    
  }  
 
  if( in_menu )
  {  
   up_down_select_text(1);  
  }

}

void diag_menu_display( uint8_t menu_trk)
{
  LCD_position(LINE1);    
  switch (menu_trk)
    {
      case 1:    
          _LCD_PRINT("1. Battery Voltage  ");     
          LCD_position(LINE2);      
          _LCD_PRINT("2. Extended Counts  ");
          break;    
      case 2:    
          _LCD_PRINT("3. Check Temperature");  
          LCD_position(LINE2);          
          _LCD_PRINT("4. Memory Reset    ");
          break;   
      case 3:
          LCD_position(LINE1);
         _LCD_PRINT("5. Depth Sensor Test");
         LCD_position(LINE2);
         _LCD_PRINT("6. KEY PAD Test     ");      
          break;           
      case 4:
          LCD_position(LINE1);
         _LCD_PRINT("7. USB Store Test   ");
         LCD_position(LINE2);
         _LCD_PRINT("8. Count Test       ");      
          break;                   
      case 5:
          LCD_position(LINE1);
         _LCD_PRINT("9.  Shut Down Test  ");
         LCD_position(LINE2);
         _LCD_PRINT("10. Light Test      ");      
          break;                   
      
      case 6:
          LCD_position(LINE1);
         _LCD_PRINT("11.  GPS Test       ");
         LCD_position(LINE2);
         _LCD_PRINT("12.  Update Firmware");      
          break;      
    
     case 7:
          LCD_position(LINE1);
         _LCD_PRINT("13.  SD Card Test   ");
         LCD_position(LINE2);
         _LCD_PRINT("14. Reset BLE Module");        
          break;    
        
      case 0:
          LCD_position(LINE1);
         _LCD_PRINT("15.Idle Shutdwn Time");
         LCD_position(LINE2);
         _LCD_PRINT("                    ");      
        break;      
        
      break;                  
  }
  if(in_menu)
    up_down_select_text(1);  
}




void sd_menu_display(uint8_t menu_trk)   // display for the project menu
{
  switch (menu_trk)
  {
    case 1:
    LCD_position(LINE1);
    _LCD_PRINT("1. SD Card Test     ");
    LCD_position(LINE2);
    _LCD_PRINT("2. Format SD Card   ");
    break;
    
    case 2:
    LCD_position(LINE1);
    _LCD_PRINT("3. Delete Proj. Dir.");
    LCD_position(LINE2);
    _LCD_PRINT("4. Create Proj. Dir.");
    break;
    
    case 0:
    LCD_position(LINE1);
    _LCD_PRINT("5. Add/Delete Files ");
    LCD_position(LINE2);
    _LCD_PRINT("                    ");
    break;
    
  }
   
  if (in_menu)
  {  up_down_select_text(1);  }
}


// *******************************************************************************************************************************


void ready(BYTE cnt_temp, int line)
{
  CLEAR_DISP;
  //LCD_position(LINE1);
  if(Features.language_f)
  {
     LCD_PrintAtPositionCentered("GAUGE READY",LINE1+10); 
  }
  else
  {
     LCD_PrintAtPositionCentered("PREPARADO",LINE1+10); 
  }
  
 /*LCD_position(LINE2); 
 
 if(cnt_temp <= 30)
  {
    if(Features.language_f)
     { _LCD_PRINTF("COUNT TIME: %u sec.",cnt_temp);}
    else
     { _LCD_PRINTF("COUNT TIME: %u seg.",cnt_temp); }
  }
  else
    { _LCD_PRINTF("COUNT TIME: %u min.",cnt_temp/60);  }
    
  */  
  LCD_position ( LINE4 );
  _LCD_PRINT ("Press START to Test "); 
}


void display_count_time(BYTE value_temp, uint8_t position )
{
  LCD_position  (position );
  if(Features.language_f)
    {
      _LCD_PRINT("Count Time: ");
    }
    else
    {
      _LCD_PRINT("Tiempo Cont ");
    }
      
  if(value_temp <= 30)
  {
    if(Features.language_f)
      {
        _LCD_PRINTF( "%u sec. ",value_temp );
      
      }
      else
      {
        _LCD_PRINTF( "%u seg.",value_temp ); 
      }  
  }
  else
  {
      _LCD_PRINTF(" %u min. ",value_temp/60); 
  }
}
void select_test_or_spec_cal(int inspec)
{  
  CLEAR_DISP;  
  LCD_position(LINE1);
  if(Features.language_f)
  {
    if(!inspec)
    {  _LCD_PRINT("Select Test");  }
    else 
     { _LCD_PRINT("Select Spec. Cal"); }
  }
    else
    {
      if(!inspec)
        _LCD_PRINT("Sel. la Prueba");  
      else 
        _LCD_PRINT("Sel. Calib. Especial");  
    }
}
void new_std_count()
{  
  LCD_position (LINE3);
  if(Features.language_f)
    { _LCD_PRINT("Take New Std Count? "); }
    else
    {  _LCD_PRINT("Nueva Cuenta Std?"); }
  
}

void press_start(BYTE which_test)
{
  CLEAR_DISP; 
  LCD_position (LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT ( "  Press START for   ");
    LCD_position (LINE3);
    if(which_test==0) //  stat test
    { 
      _LCD_PRINT("  20 one min counts ");  
      LCD_position ( LINE1 );
      _LCD_PRINT("      Stat Test     ");  
    }
    else if(which_test==1) //  drift test
    {
      _LCD_PRINT(" 5 four min counts  ");  
      LCD_position ( LINE1 );
      _LCD_PRINT("     Drift Test     ");  
    }
    else if (which_test==2)  // standard
    { 
       _LCD_PRINT("  Standard Count."); 
    }
    else if (which_test==3 )
    {
      _LCD_PRINT("  Count to Begin");  
    }
  }
    else
    {
      _LCD_PRINT(" Pres. Iniciar Para ");
      LCD_position (LINE3);
      if(which_test==0) //  stat test
      {  _LCD_PRINT("20m Prb estadistica"); }    
      else if(which_test==1)//   drift test
      {  _LCD_PRINT("20m Prb Desviacion"); }
      else if(which_test==2)//   standard
      { _LCD_PRINT(" Cuenta Standard"); }
      else if(which_test==3)
      {  _LCD_PRINT(" Cuenta a Iniciarse"); }
    }
}

void std_count_press_start(void)
{
  
  char temp_str[10];
  CLEAR_DISP; 
  
  
  if (  Features.SI_units ==  TRUE  )
  {
   sprintf (temp_str,"50mm");
  }
  else
  {
   sprintf(temp_str, "2in");
  }
  

  if(Features.language_f)
  {
    LCD_PrintAtPosition("Place Gauge on Poly ",LINE1);
    LCD_PrintAtPosition("Std. Block in SAFE  ",LINE2);
    LCD_PrintAtPositionCentered("Position",LINE3+10);
  	LCD_PrintAtPositionCentered("Press Start", LINE4+10 );
  }
  else
  {
    LCD_PrintAtPosition("Place Gauge on Poly ",LINE1);
    LCD_PrintAtPosition("Std. Block in SAFE  ",LINE2);
    LCD_PrintAtPositionCentered("Position",LINE3+10);
  	LCD_PrintAtPositionCentered("Press Start", LINE4+10 );
  }
    
}

// trench count
void trench_count_press_start(void)
{
  
  char temp_str[10];
  CLEAR_DISP; 
  
  
  if (  Features.SI_units ==  TRUE  )
  {
   sprintf (temp_str,"50mm");
  }
  else
  {
   sprintf(temp_str, "2in");
  }

 if(Features.language_f)
  {
    LCD_PrintAtPositionCentered("Place Gauge in",LINE1+10);
    LCD_PrintAtPosition("Trench on Std. Block",LINE2);
    LCD_PrintAtPositionCentered("in SAFE Position",LINE3+10);
  	LCD_PrintAtPositionCentered("Press Start", LINE4+10 );
  }
  else
  {
    LCD_PrintAtPositionCentered("Place Gauge in",LINE1+10);
    LCD_PrintAtPosition("Trench on Std. Block",LINE2);
    LCD_PrintAtPositionCentered("in SAFE Position",LINE3+10);
  	LCD_PrintAtPositionCentered("Press Start", LINE4+10 );
  }

}



void use_new_std_count()
{  
  LCD_position (LINE3);
  if(Features.language_f)
    { _LCD_PRINT("Use New STD Count?");   }
  else
    { _LCD_PRINT("Use Nueva Cuent Std?"); }
    
}

void current_project_text ( char *temp_str )
{
 _LCD_PRINTF("Project: %s",temp_str);
}

void max_stations_text(char *temp_str)
{  
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Max # of Stations");
    LCD_position(LINE2);
    _LCD_PRINTF("For %s has",temp_str);
    LCD_position(LINE3);
    _LCD_PRINT("Been Exceeded.");
    LCD_position(LINE4);
    _LCD_PRINT("Start New Project");  
  }
    else
    {
      _LCD_PRINT("Max # de Estaciones");  
      LCD_position(LINE2);
      _LCD_PRINTF("Para %s se ha",temp_str);
      LCD_position(LINE3);
      _LCD_PRINT("Execido. Inicie un");
      LCD_position(LINE4);
      _LCD_PRINT("Nuevo Proyecto.");   
    }
}
void no_project_selected ( void )
{ 
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  { 
    _LCD_PRINT("No Project Has Been");
    LCD_position(LINE2);
    _LCD_PRINT("Selected. Please");
    LCD_position(LINE3);
    _LCD_PRINT("Create or Select");
    LCD_position(LINE4);
    _LCD_PRINT("Project.");  
  }
    else
    { 
      _LCD_PRINT("Ningun Projecto se a");  
      LCD_position(LINE2);
      _LCD_PRINT("Seleccionado.       ");
      LCD_position(LINE3);
      _LCD_PRINT("Elabore o Seleccione");
      LCD_position(LINE4);
      _LCD_PRINT("un Projecto.");   
    }  
}

void no_valid_depth_selected ( void )
{ 
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  { 
    _LCD_PRINT(" No Valid Depth Has ");
    LCD_position(LINE2);
    _LCD_PRINT("   Been Selected.");
  }
    else
    { 
      _LCD_PRINT("Ningun Projecto se a");  
      LCD_position(LINE2);
      _LCD_PRINT("Seleccionado.       ");
      LCD_position(LINE3);
      _LCD_PRINT("Elabore o Seleccione");
      LCD_position(LINE4);
      _LCD_PRINT("un Projecto.");   
    }  
}


void disable_auto_depth()
{  
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Disable Auto");
    LCD_position(LINE2);
    _LCD_PRINT("Depth?");
  }
    else
    {
      _LCD_PRINT("Desconectar");  
      LCD_position(LINE2);
      _LCD_PRINT("Profundidad Auto.?");  
    }
}

void calibration_date_txt ( void )
{  
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Cal Date:           ");
    
  }
    else
    {
    _LCD_PRINT("Cal Date:           ");
    }
}

void invalid_std_or_const(void)
{
  CLEAR_DISP;
  LCD_position (LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("   Invalid STD or");
    LCD_position (LINE3);
    _LCD_PRINT("   Cal. Constants");
  }
    else
    {
      _LCD_PRINT("Conteo STD const. de");  
      LCD_position (LINE3);
      _LCD_PRINT("  Calib. Invalidas");  
    }
}

void invalid_den_std_text(void)
{
  CLEAR_DISP;
  LCD_position (LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("Invalid Density Std.");
  }
  else
  {
    _LCD_PRINT("Invalid Density Std.");
  }
}

void invalid_mois_std_text(void)
{
  CLEAR_DISP;
  LCD_position (LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("Invalid Moist. Std. ");
  }
  else
  {
    _LCD_PRINT("Invalid Moist. Std. ");
  }
}


void mat_thickness_error ( void )
{

  CLEAR_DISP;
  LCD_position (LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Thickness is between");
    LCD_position (LINE2);
    if ( Features.SI_units == FALSE )
    {
      _LCD_PRINT ("1 and 4 inches      ");
    }
    else
    {
     _LCD_PRINT ( "25 and 100 mm       ");
    }  
  }
  else
  {
       _LCD_PRINT("Thickness is between");
    LCD_position (LINE2);
    if ( Features.SI_units == FALSE )
    {
      _LCD_PRINT ("1 and 4 inches      ");
    }
    else
    {
     _LCD_PRINT ( "25 and 100 mm       ");
    } 
  }

}



void reading(void)
{
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
   { _LCD_PRINT("Reading # "); }
    else
      _LCD_PRINT("Lectura # ");
}

void self_test_text(BYTE which_test)
{  
  LCD_position(LINE2);
  if(Features.language_f)
  {
    if(which_test < 3)  
    {
      if(which_test==0)
      {      
        _LCD_PRINT(" High Voltage Test");  
        LCD_position(LINE3);
        _LCD_PRINT("    In Progress");
      }
      else if(which_test==1)
        _LCD_PRINT("    Keypad Test   ");
      else if(which_test==2)
        _LCD_PRINT("     Tube Test    ");    
    
    }
    else 
    {     
      _LCD_PRINT("     Self Test");
      LCD_position(LINE3);
      _LCD_PRINT("      Passed");
    }
  }
    else
    {
      if(which_test < 3)  
      {
        if(which_test==0)
        {      
          _LCD_PRINT("Prueba de Alto Volt.");
          LCD_position(LINE3);
          _LCD_PRINT("    En Progreso");
        }
        else if(which_test==1)
          _LCD_PRINT(" Prueba del Teclado");  
        else if(which_test==2)
          _LCD_PRINT("  Prueba de Tubos");    
    
      }
      else 
      {     
        _LCD_PRINT("    Auto Prueba");
        LCD_position(LINE3);
        _LCD_PRINT("      Aprobada");  
      }
    }  
}
void voltage_out_of_range(float volts_temp)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINTF("   Voltage = %3.0f",(double)volts_temp);
    LCD_position(LINE2);
    _LCD_PRINT("    Out of Range");
    LCD_position(LINE4);
    _LCD_PRINT("    Test Aborted!");
  }
    else 
    {
      _LCD_PRINTF("   Voltaje = %3.0f",volts_temp);  
      LCD_position(LINE2);
      _LCD_PRINT("   Fuera de Rango");
      LCD_position(LINE4);
      _LCD_PRINT("  Pruebo Abortada!"); 
    }
}

void diagnostics_text(BYTE which_prompt)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    if(which_prompt==1)
    {
      _LCD_PRINT("Place Gauge on");
      LCD_position(LINE2);
      _LCD_PRINT("Standard Block");
      LCD_position(LINE3);
      _LCD_PRINT("in SAFE Position");
      LCD_position(LINE4);
      _LCD_PRINT("and Press START");
    }
    else if(which_prompt==2)
    {         
      LCD_position(LINE2);
      _LCD_PRINT("    Please Wait");
      LCD_position(LINE3);
      _LCD_PRINT("  Time Left 60:00");   
    }     
  }
    else
    {
      if(which_prompt==1)
      {
        _LCD_PRINT("Coloque el Equipo en");  
        LCD_position(LINE2);
        _LCD_PRINT("el Bloque Standard");
        LCD_position(LINE3);
        _LCD_PRINT("en Posicion se Segur");
        LCD_position(LINE4);
        _LCD_PRINT("Presione Inciar");
      }
      else if(which_prompt==2)
      {   
        LCD_position(LINE2);
        _LCD_PRINT("  Por Favor Espere");  
        LCD_position(LINE3);
        _LCD_PRINT("     Tiempo 60:00");   
      }     
    }
}

void USB_text(BYTE which_prompt)
{
  CLEAR_DISP; 
  LCD_position(LINE2);
  if(Features.language_f)
  {
    if(which_prompt==1)
    {   
      _LCD_PRINT("    Writing Data");
      LCD_position(LINE3);
      _LCD_PRINT("    to USB Drive");
    }
    else if(which_prompt==2)
    {   
      _LCD_PRINT("   No USB Device");  
      LCD_position (LINE3);
      _LCD_PRINT("    Detected.");
    }
    else if(which_prompt==3)
    {    
      _LCD_PRINT("   Data Download");
      LCD_position(LINE3);
      _LCD_PRINT("     Complete");    
    }
    else if(which_prompt==0)
    {
      LCD_position(LINE1);
      _LCD_PRINT("  Insert External   ");
      LCD_position(LINE2);
      _LCD_PRINT(" Drive in USB Port  ");      
      LCD_position(LINE4);
      _LCD_PRINT("    Press ENTER     ");  
    }
  }
    else
    {
      if(which_prompt==1)
      {   
        _LCD_PRINT("   Escribiendo la"); 
        LCD_position(LINE3);
        _LCD_PRINT(" Informacion al USB");  
      }
      else if(which_prompt==2)
      {   
        _LCD_PRINT("   No se Detecta"); 
        LCD_position (LINE3);
        _LCD_PRINT("        USB");  
      }
      else if(which_prompt==3)
      {    
        _LCD_PRINT("     Descarga de"); 
        LCD_position(LINE3);
        _LCD_PRINT("Informacion Completa");     
      }
      else if(which_prompt==0)
      {
        LCD_position(LINE1);
        _LCD_PRINT("Conecte el Disposit-");  // Conecte el dispositivo de memoria externa al USB Presione iniciar
        LCD_position(LINE2);
        _LCD_PRINT("ivo de Memoria");      
        LCD_position(LINE3);
        _LCD_PRINT("Externa al USB.");
        LCD_position(LINE4);
        _LCD_PRINT("Presione Inciar.");   
      }    
    }
}


void printer_text(BYTE which_prompt)
{
  CLEAR_DISP; 
  LCD_position(LINE2);
  if(Features.language_f)
  {
    if(which_prompt==0)
    {   
      LCD_position(LINE2);
      _LCD_PRINT("   Printing Data");
      LCD_position(LINE3);
      _LCD_PRINT("    Please Wait");
    }
    else if(which_prompt==1)
    {   
      LCD_position (LINE2);
      _LCD_PRINT("     No Printer");  
      LCD_position (LINE3);
      _LCD_PRINT("      Detected!");
    }
    else if(which_prompt==2)
    {   
      LCD_position(LINE1);
      _LCD_PRINT("  Connect Printer");
      LCD_position(LINE2);
      _LCD_PRINT("     To Gauge");      
      LCD_position(LINE4);
      _LCD_PRINT("    Press ENTER");  
    } 
  }
    else
     {
      if(which_prompt==0)
      {   
        LCD_position(LINE1);
        _LCD_PRINT("   Imprimiendo la");  
        LCD_position(LINE2);
        _LCD_PRINT("    Informacion");
        LCD_position(LINE3);
        _LCD_PRINT("  Por Favor Espere.");   
      }
      else if(which_prompt==1)
      {   
        LCD_position (LINE2);
        _LCD_PRINT("   No se Detecta    ");  
        LCD_position (LINE3);
        _LCD_PRINT("    Impresora!");  
      }
      else if(which_prompt==2)
      {   
        LCD_position(LINE1);
        _LCD_PRINT("Conecte la Impresora"); 
        LCD_position(LINE2);
        _LCD_PRINT("    al Equipo.");      
        LCD_position(LINE4);
        _LCD_PRINT("  Presione Iniciar");   
      } 
    }
}
void display_offset ( struct offsets_struct  which_offsets)
{
   
  LCD_position (LINE3+17);
  //_LCD_PRINT("Offset:");
     
     if ( which_offsets.den_offset_pos )
     {
      _LCD_PRINT("D");
     }
     if ( which_offsets.moist_offset_pos )
     {
      _LCD_PRINT("M");
     }  
     if ( which_offsets.tren_offset_pos )
     {
      _LCD_PRINT("T");
     }   
  
}

void display_offset_on ( struct offsets_struct  which_offsets)
{
   
  LCD_position (LINE2 + 12);
  if ( which_offsets.den_offset_pos ||  which_offsets.moist_offset_pos ||  which_offsets.tren_offset_pos )
  { 
   _LCD_PRINT("OFFSET:Y");  
  }
  else
  { 
   _LCD_PRINT("OFFSET:N");  
  } 	

}

void display_time(BYTE cnt_temp,uint8_t line)
{
  LCD_position(line);
  if(Features.language_f)
  {
    _LCD_PRINT( "Count Time:");
    _LCD_PRINTF("%3u sec. ",cnt_temp);
  }
  else
  {
    _LCD_PRINT( "    Tiempo:");
    _LCD_PRINTF("%3u seg. ",cnt_temp);
  }
 
}
void display_station_name(char *temp_str)
{
  if(Features.language_f)
   { _LCD_PRINT("Station: "); }
    else 
   {  _LCD_PRINT("Estacion: "); } 
  _LCD_PRINTF("%s",temp_str);
}


void std_text(void)
{
  if(Features.language_f)
   { _LCD_PRINT("   Standard Count   "); }
    else
   {  _LCD_PRINT("  Cuenta Standard  ");}
}

void stat_text(void)
{
  if(Features.language_f)
     { _LCD_PRINT("Stat Test"); }
    else 
     { _LCD_PRINT("Prueba Estadistica"); }
}

void not_in_safe_text ( void )
{
 CLEAR_DISP;
 LCD_position (LINE1);
 if(Features.language_f)
 {
  _LCD_PRINT("Depth not at Safe    ");
  LCD_position (LINE2);
  _LCD_PRINT("Position             ");
 } 
 else 
 {
  _LCD_PRINT("Depth not at Safe    ");
  LCD_position (LINE2);
  _LCD_PRINT("Position             ");
 } 

}

void not_in_bs_text ( void )
{
 CLEAR_DISP;
 LCD_position (LINE1);
 if(Features.language_f)
 {
  _LCD_PRINT("Nomograph Enabled   ");
  LCD_position (LINE2);
  _LCD_PRINT("Depth not at BS     ");
  LCD_position (LINE3);
  _LCD_PRINT("Position            ");
 } 
 else 
 {
  _LCD_PRINT("Nomograph Enabled   ");
  LCD_position (LINE2);
  _LCD_PRINT("Depth not at BS     ");
  LCD_position (LINE3);
  _LCD_PRINT("Position            ");
 }  

}

void depth_not_in_bs_text ( void )
{
 CLEAR_DISP;
 LCD_position (LINE1);
 if(Features.language_f)
 {
  _LCD_PRINT("Depth not at BS     ");
  LCD_position (LINE2);
  _LCD_PRINT("Position            ");
 } 
 else 
 {
  _LCD_PRINT("Depth not at BS     ");
  LCD_position (LINE2);
  _LCD_PRINT("Position            ");
 }  

}


void depth_not_in_ac_text ( void )
{
 CLEAR_DISP;
 LCD_position (LINE1);
 if(Features.language_f)
 {
  _LCD_PRINT("Depth not at AC     ");
  LCD_position (LINE2);
  _LCD_PRINT("Position            ");
 } 
 else 
 {
  _LCD_PRINT("Depth not at AC     ");
  LCD_position (LINE2);
  _LCD_PRINT("Position            ");
 }  

}


void depth_not_in_2_text ( void )
{
  char temp_str[10];
  
  CLEAR_DISP; 
  
  if (  Features.SI_units == TRUE  )
  {
   sprintf(temp_str, "50 mm");
  }
  else
  {
   sprintf(temp_str, "2 in");
  }
 
 LCD_position (LINE1);
 if(Features.language_f)
 {
  _LCD_PRINTF("Depth not at %s",temp_str);
  LCD_position (LINE2);
  _LCD_PRINT("Position            ");
 } 
 else 
 {
  _LCD_PRINTF("Depth not at %s",temp_str);
  LCD_position (LINE2);
  _LCD_PRINT("Position            ");
 }  

}


void place_in_bs_text ( void )
{
 CLEAR_DISP;
 LCD_position (LINE1);
 
 if(Features.language_f)
 {
  _LCD_PRINT("Thin Layer Enabled  ");
  LCD_position (LINE2);
  _LCD_PRINT("Place in BS Position");
  LCD_position (LINE4);   
  _LCD_PRINT("    Press Enter    ");
 } 
 else 
 {
   _LCD_PRINT ("Thin Layer Enabled  ");
  LCD_position (LINE2);
   _LCD_PRINT ("Place in BS Position");
  LCD_position (LINE4);   
  _LCD_PRINT  ("    Press Enter     ");
 }  
}  

void place_in_ac_text ( void )
{
 CLEAR_DISP;
 LCD_position (LINE1);
 
 if(Features.language_f)
 {
  _LCD_PRINT("Thin Layer Enabled  ");
  LCD_position (LINE2);
  _LCD_PRINT("Place in AC Position");
  LCD_position (LINE4);   
  _LCD_PRINT  ("    Press Enter     ");
 } 
 else 
 {
   _LCD_PRINT("Thin Layer Enabled  ");
  LCD_position (LINE2);
  _LCD_PRINT("Place in AC Position");
  LCD_position (LINE4);   
  _LCD_PRINT  ("    Press Enter     ");
 }  
}


void nomograph_text(void)
{
  if(Features.language_f)
     { _LCD_PRINT("      Nomograph     "); }
    else 
     { _LCD_PRINT("      Nomograph     "); }
}



void drift_text(void)
{
  if(Features.language_f)
    { _LCD_PRINT("Drift Test"); }
    else
    {  _LCD_PRINT("Prueba de Desviacion");  }
}

void spec_text(void)
{
  if(Features.language_f)
    { _LCD_PRINT("Special Calibration"); }
    else
    {  _LCD_PRINT("Calibracion Especial"); }
}

void trench_text(void)
{
  if(Features.language_f)
    { _LCD_PRINT("Trench Offset Count "); }
    else
    {  _LCD_PRINT("Trench Offset Count "); }
}


void enable_disable_text(char *temp_str,char c)
{
  CLEAR_DISP;
  LCD_position (LINE1);
  if(Features.language_f)
  {
    switch(c)
    {
      case 'L':
      _LCD_PRINTF("%s LCD/Keypad",temp_str);
      LCD_position(LINE2);
      _LCD_PRINT("Backlight?");   
      break;
      
      case 'S':
      _LCD_PRINTF("%s Auto",temp_str);
      LCD_position(LINE2);
      _LCD_PRINT("Scroll?");  
      break;
      
      case 'D':
      _LCD_PRINTF("%s Auto",temp_str);
      LCD_position(LINE2);
      _LCD_PRINT("Depth?");   
      break;
      
      case 'A':
      _LCD_PRINTF("%s AVG STD",temp_str);
      LCD_position(LINE2);
      _LCD_PRINT("Mode?");      
      break;
      
      case 'O':
      _LCD_PRINTF("%s Auto",temp_str);
      LCD_position(LINE2);
      _LCD_PRINT("Store?");
      break;
      
      case 'B':
      _LCD_PRINTF("%s Buzzer?",temp_str);
      break;
      
      case 'G':
      _LCD_PRINTF("%s GPS?",temp_str);
      break;
      
      case 'M':
      _LCD_PRINTF("%s SMART MC?",temp_str);
       break;
      
      case 'Q':
      _LCD_PRINTF("%s Chi2 Test?",temp_str);
      break;

    }    
  }
    else
    {    
      switch(c)
      {
        case 'L':
        _LCD_PRINTF("%s Iluminacion",temp_str); // Encender / Apagar Iluminacion LCD
        LCD_position(LINE2);
        _LCD_PRINT("LCD?");   
        break;
      
        case 'S':
        _LCD_PRINTF("%s Cambia Auto",temp_str); // Habilitar / Deshabilitar Cambio Automático de Pantalla
        LCD_position(LINE2);
        _LCD_PRINT("de Pantalla?");  
        break;
      
        case 'D':
        _LCD_PRINTF("%s",temp_str); // Habilitar / Deshabilitar  Profundidad Automatica
        LCD_position(LINE2);
        _LCD_PRINT("Profundidad Auto?");   
        break;
      
        case 'A':
        _LCD_PRINTF("%s Modo",temp_str); // Habilitar / Deshabilitar Modo AVG STD
        LCD_position(LINE2);
        _LCD_PRINT("AVG STD?");      
        break;
      
        case 'O':
        _LCD_PRINTF("%s Auto",temp_str);  // Habilitar / Deshabilitar Auto Memoria
        LCD_position(LINE2);
        _LCD_PRINT("Memoria?");
        break;
      
        case 'B':
        _LCD_PRINTF("%s Alarma?",temp_str);  // Habilitar / Deshabilitar Alarma
        break;
      
        case 'G':
        _LCD_PRINTF("%s GPS?",temp_str);  // Habilitar / Deshabilitar  Memoria GPS
        break;
        
        case 'M':
       _LCD_PRINTF("%s SMART MC?",temp_str);
       break;

      case 'Q':
      _LCD_PRINTF("%s Chi2 Test?",temp_str);
      break;
      }    
    }
}
void reset_avg_count(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
   { _LCD_PRINT("Reset AVG Count?"); }
    else
      _LCD_PRINT("Borrar Cuentas AVG?");   //  Borrar cuentas AVG?
}

void auto_depth_settings(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Auto Depth Settings");
    LCD_position(LINE2);
    _LCD_PRINT("1. Enable/Disable");
     if ( NV_RAM_MEMBER_RD( gauge_type ) != GAUGE_3440_PLUS )
     {
      LCD_position(LINE3);
      _LCD_PRINT("2. Depth Strip Type");
     } 
    
  }
    else 
     {
      _LCD_PRINT("Profund. Automatica");   //Opciones de Profundidad Automatica 1. Habilitar / Deshabilitar 2. Calibrar.
      LCD_position(LINE2);
      _LCD_PRINT("1. Habil./Deshabil.");
      if ( NV_RAM_MEMBER_RD( gauge_type ) != GAUGE_3440_PLUS )
      {
       LCD_position(LINE3);
       _LCD_PRINT("2. Depth Strip Type");
      }        
    }
}

void depth_strip_type(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Depth Strip Type");
    LCD_position(LINE2);
    _LCD_PRINT("1. Xplorer");
    LCD_position(LINE3);
    _LCD_PRINT("2. 3440");
  }
  else 
  {
    _LCD_PRINT("Depth Strip Type");
    LCD_position(LINE2);
    _LCD_PRINT("1. Xplorer");
    LCD_position(LINE3);
    _LCD_PRINT("2. 3440");
  }
}


void date_usb_error_text ( void )
{
 CLEAR_DISP;
 LCD_position (LINE1);
 if(Features.language_f)
 {
  _LCD_PRINT("Clock/USB Disabled  ");
  LCD_position (LINE2);
  _LCD_PRINT("Unplugging Ribbon   ");
  LCD_position (LINE3);
  _LCD_PRINT("Cable May Fix Issue");
  LCD_position (LINE4);
  _LCD_PRINT("Press <ENTER> ");

 } 
 else 
 { 
  _LCD_PRINT("Clock/USB Disabled  ");
  LCD_position (LINE2);
  _LCD_PRINT("Unplugging Ribbon   ");
  LCD_position (LINE3);
  _LCD_PRINT("Cable May Fix Issue");
  LCD_position (LINE4);
  _LCD_PRINT("Press <ENTER> ");
 }  

}


void clock_set_error_text ( void )
{
 CLEAR_DISP;
 LCD_position (LINE1);
 if(Features.language_f)
 {
  _LCD_PRINT("    Error setting   ");
  LCD_position (LINE2);
  _LCD_PRINT("    Internal Clock  ");
  LCD_position (LINE3);
  _LCD_PRINT("   Restart Gauge    ");
  LCD_position (LINE4);
  _LCD_PRINT("   Press <ENTER>    ");

 } 
 else 
 { 
  _LCD_PRINT("    Error setting   ");
  LCD_position (LINE2);
  _LCD_PRINT("    Internal Clock  ");
  LCD_position (LINE3);
  _LCD_PRINT("   Restart Gauge    ");
  LCD_position (LINE4);
  _LCD_PRINT("   Press <ENTER>    ");
 }  

}

void no_data_to_store_text ( void )
{
 CLEAR_DISP;
 LCD_position (LINE1);
 if(Features.language_f)
 {
  _LCD_PRINT("  No Data To Store  ");
  LCD_position (LINE2);
  _LCD_PRINT("        Start       ");
  LCD_position (LINE3);
  _LCD_PRINT("   New Measurement  ");
 } 
 else 
 {
  _LCD_PRINT("  No Data To Store  ");
  LCD_position (LINE2);
  _LCD_PRINT("        Start       ");
  LCD_position (LINE3);
  _LCD_PRINT("   New Measurement  ");
 }  

}

void stand_mode_settings_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("1. Average Mode     ");
    LCD_position(LINE2);
    _LCD_PRINT("2. Decay Mode       ");
  }
  else 
  {
    _LCD_PRINT("1. Media Modo     ");
    LCD_position(LINE2);
    _LCD_PRINT("2. Desinteg. Modo");
  }
}


void avg_stand_mode_enabled_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    LCD_position(LINE2);
    _LCD_PRINT("Avg Std Mode Enabled");
  }
  else 
  {
   LCD_position(LINE2);
   _LCD_PRINT("Media Modo Activar");
  }
}


void decay_stand_mode_enabled_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    LCD_position(LINE2);
    _LCD_PRINT("Decay Mode Enabled  ");
  }
  else 
  {
   LCD_position(LINE2);
   _LCD_PRINT("Desint. Modo Activar");
  }
}


void gauge_in_safe(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Place Gauge in");
    LCD_position(LINE2);
    _LCD_PRINT("SAFE Position");
    LCD_position(LINE3);
    _LCD_PRINT("and Press ENTER"); 
  }
    else
    {    
      _LCD_PRINT("Coloque el Equipo");  // Coloque el equipo en posicion de seguridad.  Presione Iniciar
      LCD_position(LINE2);
      _LCD_PRINT("en Posicion de Segur");
      LCD_position(LINE3);
      _LCD_PRINT("Presione Inciar.");
    }
}


void gauge_in_bs_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Place Gauge in      ");
    LCD_position(LINE2);
    _LCD_PRINT("BS Position         ");
    LCD_position(LINE3);
    _LCD_PRINT("and Press ENTER     "); 
  }
    else
    {    
      _LCD_PRINT("Coloque el Equipo");  // Coloque el equipo en posicion de seguridad.  Presione Iniciar
      LCD_position(LINE2);
      _LCD_PRINT("en Posicion de BS");
      LCD_position(LINE3);
      _LCD_PRINT("Presione Inciar.");
    }
}


void calibration_complete_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("    Calibration");
    LCD_position(LINE2);
    _LCD_PRINT("     Complete");
  }
    else
    { 
      _LCD_PRINT("    Calibracion");  //Calibración Terminada
      LCD_position(LINE2);
      _LCD_PRINT("     Terminada");
    }
}

void calibration_fail_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("    Calibration     ");
    LCD_position(LINE3);
    _LCD_PRINT("     Failed         ");
  }
    else
    { 
      _LCD_PRINT("    Calibracion");  //Calibración Terminada
      LCD_position(LINE3);
      _LCD_PRINT("     Failed    ");
    }
}

void set_units_text(void)
{
  CLEAR_DISP;
 
  LCD_PrintAtPosition ( "1.   PCF", LINE1 );
  LCD_PrintAtPosition ( "2. kg/m3", LINE2 );
  LCD_PrintAtPosition ( "3.   GCC", LINE3 );
  
}
void offsets_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("1. Density");
    LCD_position(LINE2);
    _LCD_PRINT("2. Moisture");
    LCD_position(LINE3);
    _LCD_PRINT("3. Trench"); 
  }
    else
    {
      _LCD_PRINT("1. Densidad");  // 1.Densidad. 2. Humedad 3. Zanja.
      LCD_position(LINE2);
      _LCD_PRINT("2. Humedad");
      LCD_position(LINE3);
      _LCD_PRINT("3. Zanja"); 
    }  
}


void offset_on_off_text(char *temp_str, int enabled)
{
  if(Features.language_f)
  {
    if(enabled)                              
    {        
      LCD_position (LINE1);
      _LCD_PRINTF("%s Offset ON",temp_str);        
      LCD_position (LINE2);
      _LCD_PRINT("Want to Disable?");
    }
    else
    {
      LCD_position (LINE1);
      _LCD_PRINTF("%s Offset OFF",temp_str);             
      LCD_position (LINE2);
      _LCD_PRINT("Want to Enable?");
    }      
  }
    else
    {
      if(enabled)                             //    Densidad / Humedad / Zanja  Compensacion Habilitado Desea Deshabilitarlo?   
      {        
        LCD_position (LINE1);
        _LCD_PRINTF("%s Compensacion",temp_str);        
        LCD_position (LINE2);
        _LCD_PRINT("Habilitado Desea");
        LCD_position(LINE3); 
        _LCD_PRINT("Deshabilitarlo?");
      }
      else
      {
        LCD_position (LINE1);
        _LCD_PRINTF("%s Compensacion",temp_str);  // Densidad / Humedad / Zanja - Compensacion Deshabilitado desea Habilitarlo?               
        LCD_position (LINE2);
        _LCD_PRINT("Deshabilitado Desea");
        LCD_position(LINE3);
        _LCD_PRINT("Habilitarlo?");
      }      
    }  
}
void enter_offset_text(char *temp_str)
{
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINTF("Enter %s",temp_str);  
    LCD_position(LINE2);
    _LCD_PRINT("Offset:");
  }
    else
    {
      _LCD_PRINT("Ingresar Compensac'n");
      LCD_position(LINE2);
      _LCD_PRINTF("%s",temp_str);
    }
}

void offset_enable_disable_text(char *temp_str, int enabled)
{  
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
  {
    if(!enabled)
    {    
      _LCD_PRINTF(" %s Offset",temp_str);
      LCD_position(LINE3);
      _LCD_PRINT("   Disabled");           
    }
    else
    {
      _LCD_PRINTF(" %s Offset",temp_str);
      LCD_position(LINE3);
      _LCD_PRINT("   Enabled");
    }
  }
    else
    {  
      if(!enabled)
      {    
        _LCD_PRINTF(" %s Compensacion",temp_str);
        LCD_position(LINE3);
        _LCD_PRINT("   Deshabilitado");           
      }
      else
      {
        _LCD_PRINTF(" %s Offset",temp_str);
        LCD_position(LINE3);
        _LCD_PRINT("   Habilitado"); 
      }
    }
}




void SAV_enable_disable_text( int enabled )
{  
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    if(enabled == 0)
    {    
      LCD_position(LINE1);
      _LCD_PRINT("%%Soil Air Voids ");
      LCD_position(LINE2);
      _LCD_PRINT("   Disabled");           
      LCD_position(LINE3);
      _LCD_PRINT("Want to Enable?");           
    }
    else
    {
      LCD_position(LINE1);
      _LCD_PRINT("%%Soil Air Voids ");
      LCD_position(LINE2);
      _LCD_PRINT("   Enabled");           
      LCD_position(LINE3);
      _LCD_PRINT("Want to Disable?");  
    }
     LCD_position(LINE4);
     _LCD_PRINT("Press YES or NO ");
    
  }
  else
  {  
    if(!enabled)
    {    
      LCD_position(LINE1);
      _LCD_PRINT("%%Tiearra Aire Nulo  ");  // Soil air voids
      LCD_position(LINE2);
      _LCD_PRINT("   Deshabilitado");       // Disabled
      LCD_position(LINE3);
      _LCD_PRINT("desea habilitar?");       //Want to Enable
      
    }
    else
    {
      LCD_position(LINE1);
      _LCD_PRINT("%%Tiearra Aire Nulo  ");
      LCD_position(LINE2);
      _LCD_PRINT("   Habilitado"); 
       LCD_position(LINE3);
      _LCD_PRINT("desea desactivar?");    // Want to disable
    }
    LCD_position(LINE4);
    _LCD_PRINT("Pulsa YES or NO ");
  }
}

void enter_access_number_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Access"); 
    LCD_position(LINE2);
    _LCD_PRINT("Number: ");
  }
    else
    {
      _LCD_PRINT("Ingrese el Codego de");    //Ingrese el Código de Acceso.
      LCD_position(LINE2);
      _LCD_PRINT("Acceso: ");
    }
}

void enter_cal_den_std_text (void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Density Std   "); 
    LCD_position(LINE2);
    _LCD_PRINT("Count:              ");
  }
    else
    {
     _LCD_PRINT("Enter Density Std   "); 
     LCD_position(LINE2);
     _LCD_PRINT("Count:              ");
    }
}

void enter_cal_mois_std_text (void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Moisture Std  "); 
    LCD_position(LINE2);
    _LCD_PRINT("Count:              ");
  }
    else
    {
     _LCD_PRINT("Enter Moisture Std  "); 
     LCD_position(LINE2);
     _LCD_PRINT("Count:              ");
    }
}

void access_number_incorrect_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("    Access Code");
    LCD_position(LINE3);
    _LCD_PRINT("     Incorrect"); 
  }
    else
    {
      _LCD_PRINT("  Codego de Acceso");  // Codigo de Acceso Incorrecto
      LCD_position(LINE3);
      _LCD_PRINT("     Incorrecto"); 
    }  
}
void cal_menu_text(BYTE display_set)
{
  if(Features.language_f)
  {
    switch(display_set)
    {
      case 1:
            LCD_position(LINE1);
            _LCD_PRINT("0. Moist Consts     ");
            LCD_position(LINE2);
            _LCD_PRINT("1. Depth BS         ");
            break;
      case 2:
            LCD_position(LINE1);
            _LCD_PRINT("2. Depth 2          ");
            LCD_position(LINE2);
            _LCD_PRINT("3. Depth 3          ");
            break;
      case 3:
            LCD_position(LINE1);
            _LCD_PRINT("4. Depth 4          ");
            LCD_position(LINE2);
            _LCD_PRINT("5. Depth 5          ");
            break;
      case 4:
            LCD_position(LINE1);
            _LCD_PRINT("6. Depth 6          ");
            LCD_position(LINE2);
            _LCD_PRINT("7. Depth 7          ");
            break;
      case 5:
            LCD_position(LINE1);
            _LCD_PRINT("8. Depth 8          ");
            LCD_position(LINE2);
            _LCD_PRINT("9. Depth 9          ");
            break;
      case 6:
            LCD_position(LINE1);
            _LCD_PRINT("10. Depth 10        ");
            LCD_position(LINE2);
            _LCD_PRINT("11. Depth 11        ");
            break;
      case 7:
            LCD_position(LINE1);
            _LCD_PRINT("12. Depth 12        ");        
             LCD_position(LINE2);
            _LCD_PRINT("13. Calibration Date");       
      break;
            
      case 0:
            LCD_position(LINE1);
            _LCD_PRINT("14. Calib Std Counts");
            LCD_position(LINE2);
            _LCD_PRINT("                    ");
            break;
     
    }       
  }
    else
    {
      switch(display_set) //  0. Costantes de Humedad. 1.Retro-dispercion B. 2. Profundidad
      {
        case 1:
        LCD_position(LINE1);
        _LCD_PRINT("0. Const. de Humedad");
        LCD_position(LINE2);
        _LCD_PRINT("1. Retro-Dispercion ");
        break;
        case 2:
        LCD_position(LINE1);
        _LCD_PRINT("2. Profundidad # 2  ");
        LCD_position(LINE2);
        _LCD_PRINT("3. Profundidad # 3  ");
        break;
        case 3:
        LCD_position(LINE1);
        _LCD_PRINT("4. Profundidad # 4  ");
        LCD_position(LINE2);
        _LCD_PRINT("5. Profundidad # 5  ");
        break;
        case 4:
        LCD_position(LINE1);
        _LCD_PRINT("6. Profundidad # 6  ");
        LCD_position(LINE2);
        _LCD_PRINT("7. Profundidad # 7  ");
        break;
        case 5:
        LCD_position(LINE1);
        _LCD_PRINT("8. Profundidad # 8  ");
        LCD_position(LINE2);
        _LCD_PRINT("9. Profundidad # 9  ");
        break;
        case 6:
            LCD_position(LINE1);
            _LCD_PRINT("10. Profundidad # 10");
            LCD_position(LINE2);
            _LCD_PRINT("11. Profundidad # 11");
        break;
        case 7:
             LCD_position(LINE1);
            _LCD_PRINT("12. Profundidad # 12");        
            LCD_position(LINE2);
            _LCD_PRINT("13. Cal Date  ");
            break;
         case 0:
            LCD_position(LINE1);
            _LCD_PRINT("14. Cal Std Counts  ");
            LCD_position(LINE2);
            _LCD_PRINT("15. Thin Layer      ");
            break;

      }       
    }
}
void cal_depth_text(signed int depth_temp)
{
  char dstr[10];
  
  if ( depth_temp == 1 )
  {
   sprintf( dstr,"BS");
  }
  else if ( depth_temp == 13 )
  {
   sprintf( dstr,"AC");
  }
  else 
  {
   sprintf( dstr,"%d",depth_temp);
  }
  
  CLEAR_DISP;   
    
  LCD_position(LINE1);
  if(Features.language_f)
   { 
    _LCD_PRINTF("Depth # %s", dstr ); 
   }
   else
   {
     _LCD_PRINTF("Profundidad # %s", dstr); 
   }  
   
   
}



void reset_cal_date_text(BYTE which_prompt)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    if(which_prompt==0)
    {
      _LCD_PRINT("Reset Calibration   ");
      LCD_position(LINE2);
      _LCD_PRINT("Date?");
    }
    else
    {   
      _LCD_PRINT("  Calibration Date  ");
      LCD_position(LINE2);
      _LCD_PRINT("       Reset        ");
    }
  }
    else
    {
      if(which_prompt==0)
      {
        _LCD_PRINT("Borrar la Fecha de  ");  // Borrar la fecha de Calibracion?
        LCD_position(LINE2);
        _LCD_PRINT("Calibracion?");
      }
      else
      {   
        _LCD_PRINT("Fecha de Calibracion");  // Fecha de Calibracion Borrada
        LCD_position(LINE2);
        _LCD_PRINT("      Borrada       ");
      }
    }  
}



void moisture_const_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
   { _LCD_PRINT("MOISTURE CONST");   }
    else
      _LCD_PRINT("CONSTANTE DE HUMEDAD");
}
void bscatter_const_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
   {  _LCD_PRINT("BSCATTER CONST"); }
    else
      _LCD_PRINT("RETRO-DISPERSION");  // Constante de Retro-Dispersión
}
void memory_reset_text(void)
{
  CLEAR_DISP;
  LCD_position (LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Reset Memory");
    LCD_position(LINE2);
    _LCD_PRINT("to Default?");
  }
    else
    {
      _LCD_PRINT("Borrar Memoria");  // Borrar memoria a Inicio?
      LCD_position(LINE2);
      _LCD_PRINT("a Inicio?");
    }
}
void enable_disable_speccal_text(int condition)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    if(condition)
     { _LCD_PRINT("Disable Special"); }
    else
     { _LCD_PRINT("Enable Special");  }
    
    LCD_position(LINE2);
    _LCD_PRINT("Calibration?");
  }
    else
    {
      if(condition)
        _LCD_PRINT("Deshabilitar Calib.");  //   Habilitar / Deshabilitar Calibracion Especial?
      else
        _LCD_PRINT("Habilitar Calib."); 
      LCD_position(LINE2);
      _LCD_PRINT("Especial?");
    }
}
void view_last_speccal_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("View Last Saved     ");                  //   Use existing calibration value?
    LCD_position(LINE2);
    _LCD_PRINT("Spec Cal. Data?     ");
  }
    else
    {
      _LCD_PRINT("Vista de la Ultima");              //       Vista de la Ultima Calibracion Especial Registrada?
      LCD_position(LINE2);
      _LCD_PRINT("Cal. Esp Registrada?");
    }  
}
void vals_ok_text(void)
{
  LCD_position(LINE4);
  if(Features.language_f)
   { _LCD_PRINT("Values OK? YES/NO   "); }
    else
      _LCD_PRINT("Valores OK? SI/NO  ");  
}
void use_gauge_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Use Gauge to Derive ");
    LCD_position(LINE2);  
    _LCD_PRINT("B Value?            ");
  }
    else
    {
      _LCD_PRINT("Use el Equipo Para");  // Use el Equipo para obtener el  valor "B"
      LCD_position(LINE2);  
      _LCD_PRINT("Obtener el Valor B?");
    }
}

void enter_value_text(BYTE which_value)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Value For");
    LCD_position(LINE2);
    if(which_value==0)
      _LCD_PRINT("B =");
    else if(which_value==1)
      _LCD_PRINT("Density =");
    else if(which_value==2)
      _LCD_PRINT("Density Now?");
    else if(which_value==3)
      _LCD_PRINT("Proctor:");
    else if(which_value==4)
      _LCD_PRINT("MAX DENS:");
    else if( which_value == 5)
      _LCD_PRINT("SOIL SG:");  
    else if( which_value == 6)
    _LCD_PRINT("SHT DWN (HOURS):");     
    else if( which_value == 7)
      _LCD_PRINT("Depth Slope:");    
      
  }
    else
    {    
      _LCD_PRINT("Entre el Valor de"); //  Entre el Valor de Densidad:
      LCD_position(LINE2);
      if(which_value==0)
        _LCD_PRINT("B =");
      else if(which_value==1)
        _LCD_PRINT("Densidad=");
      else if(which_value==2)
        _LCD_PRINT("Densidad Ahora?");  // Entre el Valor de Densidad Ahora:
      else if(which_value==3)
        _LCD_PRINT("Proctor:");  // Ingresar valor Proctor
      else if(which_value==4)
        _LCD_PRINT("MAX DENS:");  // Ingresar valor Proctor
      else if( which_value == 5)
      _LCD_PRINT("SOIL SG:");    
      else if( which_value == 6)
      _LCD_PRINT("SHT DWN (HOURS):");   
      else if( which_value == 7)
      _LCD_PRINT("Depth Slope:");       
      
        
    }
}
void use_stored_count_text(void)
{
  CLEAR_DISP;
  LCD_position (LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Use Stored Count");
    LCD_position (LINE2);
    _LCD_PRINT("Data?");
  }
    else 
    {
      _LCD_PRINT("Usar las Cuentas");  // Usar las Cuentas Guardadas?
      LCD_position (LINE2);
      _LCD_PRINT("Data?");
    }
}

void activate_speccal_text(void)
{
  LCD_position (LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("Activate Special");
    LCD_position(LINE3);
    _LCD_PRINT("Calibration?");
  }
    else
    {
      _LCD_PRINT("Activar Calibracion"); //  Activar Calibracion Especial
      LCD_position(LINE3);
      _LCD_PRINT("Especial?");
    }
}
void enter_number_counts_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Number");
    LCD_position (LINE2);
    _LCD_PRINT("of Counts:   ");
  }
    else
    {
      _LCD_PRINT("Ingrese Numero de");  /// Ingrese Numero de Cuentas
      LCD_position (LINE2);
      _LCD_PRINT("de Cuentas  ");
    }
}
void start_speccal_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("# of Counts:");
    LCD_position(LINE2);
    _LCD_PRINT("Depth:");
    LCD_position(LINE3);
    _LCD_PRINT("Press START to");
    LCD_position(LINE4);
    _LCD_PRINT("Begin Count");       
  }
    else
     {
      _LCD_PRINT("# de Cuentas");  // Numero de Cuentas:     Profundidad:     Precione Iniciar para comensar a contar:      
      LCD_position(LINE2);
      _LCD_PRINT("Profundidad:");
      LCD_position(LINE3);
      _LCD_PRINT("Precione Inciar Para");
      LCD_position(LINE4);
      _LCD_PRINT("Comensar a Contar");       
    }
}
void speccal_results_text(signed int loops,unsigned int cnt_temp)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINTF("#%d Density",loops);
    LCD_position(LINE2);
    _LCD_PRINTF("Count: %u",cnt_temp);
    LCD_position(LINE3);
    _LCD_PRINT("YES to Continue");
  }
    else
    {
      _LCD_PRINTF("#%d Cuentas de",loops);  // Numero de Cuentas de Densidad " Si " para Continuar
      LCD_position(LINE2);
      _LCD_PRINTF("Densidad: %u",cnt_temp);
      LCD_position(LINE3);
      _LCD_PRINT("SI para Continuar");
    }
}
void speccal_enabled_text(void)
{
  CLEAR_DISP;
  LCD_position (LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("Special Calibration");
    LCD_position (LINE3);
    _LCD_PRINT("      Enabled");
  }
    else
    {
      _LCD_PRINT("Calibracion Especial");  // Calibración Especial Habilitada
      LCD_position (LINE3);
      _LCD_PRINT("     Habilitada");
    }
}
void enable_disable_nomograph_text(int condition)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    if(condition)
     { _LCD_PRINT("Disable Nomograph   "); }
    else
     { _LCD_PRINT("Enable Nomograph    ");  }
    
    LCD_position(LINE2);
    _LCD_PRINT("Mode?");
  }
    else
    {
      if(condition)
        _LCD_PRINT("Habilitar Comando");   // Habilitar/Deshabilitar Comando capa delgada?
      else
        _LCD_PRINT("Deshabilitar Comando");
      LCD_position(LINE2);
      _LCD_PRINT("Capa Delgada?");
    }
}


void activate_saved_vals_text(void)
{ 
  CLEAR_DISP;
  LCD_position (LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Activate Saved");
    LCD_position (LINE2);
    _LCD_PRINT("Values?");
  }
    else
    {
      _LCD_PRINT("Activar los Valores");  // Activar los Valores Guardados ?
      LCD_position (LINE2);
      _LCD_PRINT("Guardados?");
    }
}

void nomograph_vals_text(void)
{
  CLEAR_DISP;       
  LCD_position (LINE1); 
  if(Features.language_f)
  {
    _LCD_PRINT("Mat Thick=");
    LCD_position (LINE2);
    _LCD_PRINT("Bot Dens =");
    LCD_position (LINE3);
    _LCD_PRINT("Are Values OK?");
  }
    else
    {
      _LCD_PRINT("Espesor Mat");   // Espesor Material =                       Densidad  de Fondo =                  Estan Los Valores Correctos?
      LCD_position (LINE2);
      _LCD_PRINT("Dens. Fondo");
      LCD_position (LINE3);
      _LCD_PRINT("Valores Correctos?");
    }
}




void enter_mat_thickness_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Material");
    LCD_position (LINE2);
    _LCD_PRINT("Thickness:");
  }
    else
    {
      _LCD_PRINT("Enter Material"); 
      LCD_position (LINE2);
      _LCD_PRINT("Thickness:");
    }
}
void enter_bot_dense_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Bottom");
    LCD_position (LINE2);
    _LCD_PRINT("Density:");
  }
    else
    {
      _LCD_PRINT("Entre el Valor de la");  // Entre el Valor de la Densidad de fondo
      LCD_position (LINE2);
      _LCD_PRINT("Densidad");
    }
}

void nomograph_enabled_text(void)
{
  CLEAR_DISP;
  LCD_position (LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("     Nomograph");
    LCD_position (LINE3);
    _LCD_PRINT("      Enabled");
  }
    else
    {
      _LCD_PRINT("    Capa Delgada");  // Capa Delgada Habilitado
      LCD_position (LINE3);
      _LCD_PRINT("     Habilitado");
    }
}


void enter_time_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Present");
    LCD_position(LINE2);      
    _LCD_PRINT("Time: hh:mm"); 
  }
    else
    {
      _LCD_PRINT("Ingrese la Hora"); //  Ingrese la Hora Actual   hh : mm 
      LCD_position(LINE2);      
      _LCD_PRINT("Actl: hh:mm"); 
    }
}
void am_pm_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
    { _LCD_PRINT("Select"); }
    else
      _LCD_PRINT("Seleccione");   //Seleccione 1. AM - PM     
  LCD_position(LINE2);  
  _LCD_PRINT("1. AM");       
  LCD_position(LINE3);
  _LCD_PRINT("2. PM");
}

void prma_text(void)
{
  CLEAR_DISP;
  
  if(Features.language_f)  
  {
    LCD_position(LINE1);
    _LCD_PRINT("1. Proctor");
    LCD_position(LINE2);
    _LCD_PRINT("2. MAX DENS");  
    if ( Features.soil_air_voids_on == 1 )
    {
     LCD_position(LINE3);
     _LCD_PRINT("3. Soil SG");    
    }
  }  
  else
  {
    LCD_position(LINE1);
    _LCD_PRINT("1. Proctor");
    LCD_position(LINE2);
    _LCD_PRINT("2. Densidad Máxima");  
    if ( Features.soil_air_voids_on == 1 )
    {
     LCD_position(LINE3);
     _LCD_PRINT("3. Tierra GS");    
    }
  }
  
}

void change_val_text(void)
{
  LCD_position(LINE2);
  if(Features.language_f)
     {_LCD_PRINT("Change Value?");}  // Cambiar Valor? 
    else
      _LCD_PRINT("Cambiar Valor?");
}

void get_new_offset_text(void)
{
  LCD_position(LINE2);
  if(Features.language_f)
     {_LCD_PRINT("Get New Offset?     ");}  
    else
     {_LCD_PRINT("Get New Offset?     ");}  
}


void use_val_text(void)
{
  LCD_position(LINE2);
  if(Features.language_f)
     {_LCD_PRINT("Use Value?");}  // Cambiar Valor? 
    else
     { _LCD_PRINT("Use Value?");}
}

void battery_shutdwn_text(void)
{
  CLEAR_DISP;
  LCD_position (LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("    Battery Low");
    LCD_position (LINE3);
    _LCD_PRINT("  Shutdown: 5 sec.");
  }
    else
    {
      _LCD_PRINT("    Bateria Baja");   // Batería Baja       Interrupción en XX segundos
      LCD_position (LINE3);
      _LCD_PRINT("Interrupcion: 5 seg.");
    }
}
void display_alk_warning(void)
{
  CLEAR_DISP;
  LCD_position (LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("  *** WARNING ***");
    LCD_position (LINE2);
    _LCD_PRINT("  Alkaline Battery  ");
    LCD_position (LINE3);
    _LCD_PRINT("      is  Low       ");
  }
    else
    {
      _LCD_PRINT(" *** ADVERTENCIA ***");  // *** ADVERTENCIA ***    Calibrador Demasiado Caliente    ¿Voltaje Bajo?
      LCD_position (LINE2);
      _LCD_PRINT("  Bateria Alcalina  ");    //Batería Baja       Interrupción en XX segundos
      LCD_position (LINE3);
      _LCD_PRINT("      es Baja       ");
    }
}

void warning_text(BYTE which_source)
{
  CLEAR_DISP;
  LCD_position (LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("  *** WARNING ***");
    LCD_position (LINE3);
    if(which_source==0)
      { _LCD_PRINT("   Gauge Too Hot");   }
    else 
      { _LCD_PRINT("    Low Voltage ");   }
  }
    else
    {
      _LCD_PRINT(" *** ADVERTENCIA ***");   //*** ADVERTENCIA ***    Calibrador Demasiado Caliente    ¿Voltaje Bajo?
      LCD_position (LINE3);
      if(which_source==0)
      {
        _LCD_PRINT("Calibrador Demasiado");   
        LCD_position(LINE4);
        _LCD_PRINT("     Caliente!");
      }
      else
        _LCD_PRINT("    Voltaje Bajo");   
    }  
}
void gauge_temp_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
    { _LCD_PRINT("Gauge Temp ="); } // Temperatura del Densimetro 
    else
      _LCD_PRINT("Temperatura=");
}

void depth_voltage_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE2);
  
  if(Features.language_f)
  { 
   _LCD_PRINT("DS Voltage ="); 
   LCD_position(LINE3);
   _LCD_PRINT("Depth      = "); 
  } // Temperatura del Densimetro 
   else
  { 
   _LCD_PRINT("DS Voltaje =");
  }  
}
   
void press_key_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE1);
  
  if(Features.language_f)
  { 
   _LCD_PRINT("   Press Any Key    "); 
   LCD_position(LINE2);
   _LCD_PRINT("    ESC to Exit    "); 
  } 
  else
  { 
    _LCD_PRINT("   Press Any Key    "); 
   LCD_position(LINE2);
   _LCD_PRINT("    ESC to Exit    "); 
  }  
   
  
}


void high_voltage_text(void)
{
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
   { _LCD_PRINT("High Voltage =");  }
    else
      _LCD_PRINT("Voltaje Alto =");  //Voltaje Alto
}
void shut_down_text_text(void)
{
  CURSOR_OFF;
  CLEAR_DISP;
  LCD_position (LINE2 + 6);
  if(Features.language_f)
    { _LCD_PRINT("Shutdown"); }
    else
      _LCD_PRINT(" Apagar");  // Apagar
}
void shutdown_inactivity_text_text(void)
{
  //LCD_init();
  CURSOR_OFF;
  CLEAR_DISP;
  LCD_position (LINE2 + 1);
  if(Features.language_f)
    { _LCD_PRINT("Idle Shutdown"); }
    else
      _LCD_PRINT(" Inactivo Apagar");  // Apagar
}

void enter_valid_text(BYTE which_source)
{
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("    Please Enter");
    LCD_position(LINE3);  
    _LCD_PRINT("     Valid ");
    if(which_source==0)
      { _LCD_PRINT("Date"); }
    else
      { _LCD_PRINT("Time"); }
  }
    else
    {
      _LCD_PRINT("   Favor Ingresar");   //Favor ingresar fecha y hora validos
      LCD_position(LINE3);  
      if(which_source==0)
        _LCD_PRINT(" Fecha y Validos");
      else 
        _LCD_PRINT(" Hora y Validos");
    }
  delay_ms(1000);
}
void enter_project_name_text(char *temp_str)
{
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Project ");
    LCD_position(LINE2);
    _LCD_PRINT("Name: ");
  }
    else
    {
      _LCD_PRINT("Ingrese Nombre del ");   // Ingrese nombre del proyecto
      LCD_position(LINE2);
      _LCD_PRINT("Proyecto: ");
    }
  _LCD_PRINTF("%s",temp_str);
}

void project_name_text(char *temp_str)
{
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
    { _LCD_PRINT("    Project Name");   }
    else
      _LCD_PRINT("Nombre del Projecto");  // Nombre del Projecto
  LCD_position(LINE3);
  _LCD_PRINTF(  "    %s",temp_str);   
}
void station_mode_text()
{
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Station Name Mode");
    LCD_position(LINE2);
    _LCD_PRINT("1. Auto (sequential)");
    LCD_position(LINE3);
    _LCD_PRINT("2. Manual Entry"); 
  }
    else
    {
      _LCD_PRINT("Opc. Nombre Estacion");    // Opciones Nombre Estacion  1. Automatico (Secuencial)  2. Ingreso manual
      LCD_position(LINE2);
      _LCD_PRINT("1. Automatico");
      LCD_position(LINE3);
      _LCD_PRINT("2. Ingreso Manual"); 
    }
}
void station_start_text(char *temp_str)
{ 
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Starting Station");
    LCD_position(LINE2);
    _LCD_PRINTF("Number: %s",temp_str); 
  }
    else
    {
      _LCD_PRINT("Iniciacion del");   //  Iniciacion del  Numero de Estacion
      LCD_position(LINE2);
      _LCD_PRINTF("Numero de Estacion: %s",temp_str); 
    }
}
void station_increment_text(unsigned int inc)
{ 
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINTF("Station # %u",inc);
    LCD_position(LINE2);
    _LCD_PRINT("Increments by 1");
    LCD_position(LINE3);
    _LCD_PRINT("For Each Reading");
  }
    else
    {
      _LCD_PRINTF("Estacion# %u",inc);   //Estacion #  Incrementar en 1 (uno) para cada lectura 
      LCD_position(LINE2);
      _LCD_PRINT("Encrementar en 1");
      LCD_position(LINE3);
      _LCD_PRINT("Para Cada Lectura");
    }
}
void station_name_text()
{  
  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Station Name");
    LCD_position(LINE3);
    _LCD_PRINT("Before Each Reading");
  }
    else
    {
      _LCD_PRINT("Ingrese el Nombre de");   //Ingrese el Nombre de la Estación antes de Tomar cada lectura
      LCD_position(LINE3);
      _LCD_PRINT("la Estacion Antes de");
      LCD_position(LINE4);
      _LCD_PRINT("Tomar Cada Lectura");
    }
}
void project_limit_text()
{
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Project Limit");
    LCD_position(LINE2);
    _LCD_PRINT("Exceeded, Delete");
    LCD_position(LINE3);
    _LCD_PRINT("One or More Projects");    
  }
    else
    {
      _LCD_PRINT("Proyecto Limites");    //Proyecto Limites Exceso de proyectos, Borrar uno o mas proyectos. 
      LCD_position(LINE2);
      _LCD_PRINT("Exceso de Proyectos,");
      LCD_position(LINE3);
      _LCD_PRINT("Borrar Uno o Mas");
      LCD_position(LINE4);
      _LCD_PRINT("Proyectos.");
    }
}


void no_stored_projects_text()
{ 
  CLEAR_DISP;
  LCD_position(LINE2);  
  if(Features.language_f)
  {
    _LCD_PRINT("    No Projects");
    LCD_position(LINE3);
    _LCD_PRINT("      Stored"); 
  }
    else
    {
      _LCD_PRINT("  No Hay Proyectos");   //No hay proyectos en la memoria
      LCD_position(LINE3);
      _LCD_PRINT("   En la Memoria"); 
    }
}
void select_from_list_text(BYTE from_where)
{
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
  {
    if(from_where==0)
    {  _LCD_PRINT("   Select Project"); }
    else if(from_where==1)
    {  _LCD_PRINT("   Select Station"); }
   
  }
  else
  {
      if(from_where==0)
      {  _LCD_PRINT("   Select Project"); }
      else if(from_where==1)
      {  _LCD_PRINT("   Select Station"); }
  }
}

void no_data_stored_text()
{ 
  CLEAR_DISP;
  LCD_position(LINE2);

  _LCD_PRINT("   No Data Stored");   
}

void delete_data_text()
{ 
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Delete Data");
    LCD_position(LINE2);
    _LCD_PRINT("1. Delete All Data");
    LCD_position(LINE3); 
    _LCD_PRINT("2. Delete One Proj."); 
  }
    else
    {
      _LCD_PRINT("Borrar la Info.");
      LCD_position(LINE2);
      _LCD_PRINT("1. Toda Informacion");  // Borrar la Información; 1. Borrar toda Informacion  2.Borrar un projecto
      LCD_position(LINE3);  
      _LCD_PRINT("2. Un Projecto"); 
    }  
}
void enter_to_delete_text()
{  
  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("  Press ENTER to");
    LCD_position(LINE3);
    _LCD_PRINT("  Delete All Data"); 
  }
    else
    {
      _LCD_PRINT("Oprima Entrada para");  // Oprima Entrada para Borrar Toda la Información
      LCD_position(LINE3);
      _LCD_PRINT("Borrar Toda la"); 
      LCD_position(LINE4);
      _LCD_PRINT("Informacion.");    
    }  
}
void erase_data_text()
{ 
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Erase All Data");
    LCD_position(LINE2);
    _LCD_PRINT("From Memory?");
    LCD_position(LINE3);
    _LCD_PRINT("YES to Continue"); 
  }
    else
    {
      _LCD_PRINT("Borrar Toda la Info.");  // Borrar toda la Información de la Almacenada en la Memoria SI para continuar
      LCD_position(LINE2);
      _LCD_PRINT("en la Memoria?");
      LCD_position(LINE3);
      _LCD_PRINT("SI Para Continuar"); 
    }
}
void all_data_erased_text()
{  
  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("      All Data");
    LCD_position(LINE3);
    _LCD_PRINT("       Erased"); 
  }
    else
    {
      _LCD_PRINT("Toda la Informacion");  // Toda la información ha sido borrada
      LCD_position(LINE3);
      _LCD_PRINT("  Ha Sido Borrada"); 
    }
}
void delete_project_text(char *temp_str)
{  
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Press ENTER to"); 
    LCD_position(LINE2);
    _LCD_PRINT("Delete Data In");
    LCD_position(LINE3);
    _LCD_PRINT("Project: "); 
  }
    else
    {
      _LCD_PRINT("Press ENTRADA Para");   //Presione Entrada para Borrar Toda la Información del Proyecto
      LCD_position(LINE2);
      _LCD_PRINT("Borrar Toda la Info.");
      LCD_position(LINE3);
      _LCD_PRINT("Proyecto: "); 
    }
  _LCD_PRINTF("%s",temp_str);   //print project name
}
void erase_project_data_text()
{ 
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Erase Project Data?");
    LCD_position(LINE3);
    _LCD_PRINT("YES to Continue");
  }
    else
    {
      _LCD_PRINT("Borrar Informacion?");  // Borrar Informacion del Projecto?
      LCD_position(LINE3);
      _LCD_PRINT("YES to Continue");
    }
  
}
void write_USB_text()
{  
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT(" Write Data to USB"); 
    LCD_position(LINE2);
    _LCD_PRINT("1. Write All Data");
    LCD_position(LINE3);
    _LCD_PRINT("2. Write One Project"); 
  }
    else
    {
      _LCD_PRINT("Escriba Datos a USB");  //Escriba Datos a USB  1. Escriba toda la Informacion  2. Escriba un Proyecto
      LCD_position(LINE2);
      _LCD_PRINT("1. Toda la Info.");
      LCD_position(LINE3);
      _LCD_PRINT("2. Un Proyecto"); 
    }
}
void batt_volt_text()
{  
  CLEAR_DISP;
  LCD_position(LINE1);
  _LCD_PRINT("NiCd Volt =");  
  LCD_position(LINE2);
  _LCD_PRINT(" Alk Volt =");  
}

void print_data_text()
{  
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Print Data");
    LCD_position(LINE2);
    _LCD_PRINT("1. Print All Data");
    LCD_position(LINE3);
    _LCD_PRINT("2. Print One Project");  
  }
    else
    {
      _LCD_PRINT("Imprima");    // Imprima 1. Imprima toda la Informacion  2. Imprima un Proyecto
      LCD_position(LINE2);
      _LCD_PRINT("1. Toda la Info.");
      LCD_position(LINE3);
      _LCD_PRINT("2. Un Proyecto");  
    }
}

void print_complete_text()
{  
  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("    Print Data");
    LCD_position(LINE3);
    _LCD_PRINT("     Complete");
  }
    else
    {
      _LCD_PRINT("    Impresion");   
      LCD_position(LINE3);
      _LCD_PRINT("     Completa");
    }
}
void enter_station_name_text()
{  
  CLEAR_DISP;
  LCD_position(LINE1);
  if(Features.language_f)
  {
    _LCD_PRINT("Enter Station");
    LCD_position(LINE2);
    _LCD_PRINT("Name:");
  }
    else 
    {
      _LCD_PRINT("Entre Nombre de la");    // Entre nombre  de la estación.
      LCD_position(LINE2);
      _LCD_PRINT("Estacion:");
    }
}
void serial_port_text()
{ 
  CLEAR_DISP;
  LCD_position(LINE2);
  if(Features.language_f)
  {
    _LCD_PRINT("    Serial Port");
    LCD_position(LINE3);
    _LCD_PRINT("     Activated");
  }
    else
    {
      _LCD_PRINT("    Conexion de");  // Conexión de Transferencia Activada
      LCD_position(LINE3);
      _LCD_PRINT("   Transferencia");
      LCD_position(LINE4);
      _LCD_PRINT("      Activada");
    }  
}
void select_language_text()
{                                                        
  CLEAR_DISP;
  LCD_position(LINE1); 
  if(Features.language_f)
  {
    _LCD_PRINT("1. English");
    LCD_position(LINE2);
    _LCD_PRINT("2. Spanish");
  }
    else
    {
      _LCD_PRINT("1. Ingles");  // Seleccione el Idioma: 1. Ingles 2. Español
      LCD_position(LINE2);
      _LCD_PRINT("2. Espanol");
    }
}

void count_text(BYTE source)
{ 
  if(Features.language_f)
  {
    switch(source)
    {
      case  0: _LCD_PRINT("M Count:"); break;
      case  1: _LCD_PRINT("D Count:"); break;
      case  2: _LCD_PRINT("    MCR:"); break;
      case  3: _LCD_PRINT("    DCR:"); break;
      case  4: _LCD_PRINT("    DT:");  break;
      case  5: _LCD_PRINT("    WD:");  break;
      case  6: _LCD_PRINT("  %%MAX:"); break;
      case  7: _LCD_PRINT("%%Voids:"); break;
      case  8: _LCD_PRINT("Moist:");  break;
      case  9: _LCD_PRINT("%%Moist:"); break;
      case 10: _LCD_PRINT("   DD:");   break;
      case 11: _LCD_PRINT("   %%PR:");  break;
      
      case 12: _LCD_PRINT("BS Count:"); break;
      case 13: _LCD_PRINT("AC COUNT:"); break;
      case 14: _LCD_PRINT("   BS_CR:"); break;
      case 15: _LCD_PRINT("   AC_CR:"); break; 
      case 16: _LCD_PRINT("MAT THICKNESS:"); break; 
      case 17: _LCD_PRINT("   BS WD:"); break; 
      case 18: _LCD_PRINT("   AC WD:"); break; 
      case 19: _LCD_PRINT("Soil %%AV:"); break;   
   }
  }
    else
    {
      switch(source)
      {
        case  0: _LCD_PRINT("H Cuenta:"); break;
        case  1: _LCD_PRINT("D Cuenta:"); break;
        case  2: _LCD_PRINT("     HCR:"); break;
        case  3: _LCD_PRINT("     DCR:"); break;
        case  4: _LCD_PRINT("      DT:");  break;
        case  5: _LCD_PRINT("    WD:");  break;
        case  6: _LCD_PRINT("   %%HA:"); break;
        case  7: _LCD_PRINT("%%Voids:"); break;
        case  8: _LCD_PRINT(" Humedad:");  break;
        case  9: _LCD_PRINT("%%Humedad:"); break;
        case 10: _LCD_PRINT("      DD:");   break;
        case 11: _LCD_PRINT("     %%PR:");  break;
        
        case 12: _LCD_PRINT("BS Count:"); break;
        case 13: _LCD_PRINT("AC COUNT:"); break;
        case 14: _LCD_PRINT("   BS_CR:"); break;
        case 15: _LCD_PRINT("   AC_CR:"); break; 
        case 16: _LCD_PRINT("MAT THICKNESS:"); break; 
        case 17: _LCD_PRINT("   BS WD:"); break; 
        case 18: _LCD_PRINT("   AC WD:"); break;
        case 19: _LCD_PRINT("Tier. %%A:"); break;   
      }
    }
}

void stat_drift_count_text(BYTE source)
{
  if(Features.language_f)
  {
    switch(source)
    {     
      case  0: _LCD_PRINT("D%% Drift:  "); break;
      case  1: _LCD_PRINT("Dens. Avg: ");  break;
      case  2: _LCD_PRINT("M%% Drift:  "); break;
      case  3: _LCD_PRINT("Moist.AVG: ");  break;      
      case  4: _LCD_PRINT("Dens. R:   ");  break;
      case  5: _LCD_PRINT("Moist R:   ");  break;
    }
  }
    else
    {
      switch(source)
      {
        case  0: _LCD_PRINT("D%% Drift:  "); break;
        case  1: _LCD_PRINT("Dens. Avg: ");  break;
        case  2: _LCD_PRINT("H%% Drift:  0"); break;
        case  3: _LCD_PRINT("Humedad.AVG: ");  break;
        case  4: _LCD_PRINT("Dens. R:   ");  break;
        case  5: _LCD_PRINT("Humedad R:   ");  break;
      }
    }
}

void self_test_pf_text(BYTE source, int test_failed)
{ 
  char source_text[16][14] =    {{"Teclado:     "},{"Keypad:      "},
                                 {"Tubos GM:    "},{"GM Tubes:    "},
                                 {"Tubo 1 GM:   "},{"GM Tube 1:   "},                        
                                 {"Tubo 2 GM:   "},{"GM Tube 2:   "},
                                 {"Temp:        "},{"Temp:        "},                                
                                 {"Tubo He3:    "},{"He3 Tube:    "},
                                 {"FALLADO"},{"FAILED"},
                                 {"PASADO"},{"PASSED"}};
 
  _LCD_PRINTF("%s",source_text[source + Features.language_f]);
  
  if(test_failed)
   { _LCD_PRINTF("%s",source_text[12+Features.language_f]); }
  else
   { _LCD_PRINTF("%s",source_text[14+Features.language_f]);   }
}

void auto_cal_text(BYTE display_set)
{
  if(Features.language_f)
  {
    switch(display_set)
    {
      case 0:
      LCD_position(LINE1);
      _LCD_PRINT("1. Calibrate");        
      LCD_position(LINE2);
      _LCD_PRINT("2. Recount  ");
      break; 
    }       
  }
    else
    {
      switch(display_set)  // 0. Costantes de Humedad. 1.Retro-dispercion B. 2. Profundidad
      {
        case 0:
        LCD_position(LINE1);
        _LCD_PRINT("1. Calibracion");        
        LCD_position(LINE2);
        _LCD_PRINT("2. Recuento   ");   //Borrar la Fecha
        break; 
      }       
    }
}

void place_gauge_txt()
{
  if(Features.language_f)
  {   
      LCD_position(LINE1);
      _LCD_PRINT("Place the Gauge on   ");        
  }
    else
    {   
        LCD_position(LINE1);
        _LCD_PRINT("Coloque el medidor en");                  
    }
}

void block_txt(BYTE display_set)
{
  if(Features.language_f)
  {
    switch(display_set)
    {
      case 0:
      LCD_position(LINE2);
      _LCD_PRINT("Low Block           ");        
      break;

	  case 1:
      LCD_position(LINE2);
      _LCD_PRINT("Medium Block        ");      
      break;
	  
	  case 2:
      LCD_position(LINE2);
      _LCD_PRINT("High Block          ");        
      break;
	  
	  case 3:
      LCD_position(LINE2);
      _LCD_PRINT("High Moisture Block");        
      break;
	  
    }       
  }
    else
    {
      switch(display_set)
      {
        case 0:
        LCD_position(LINE2);
        _LCD_PRINT("Bloqueo bajo         ");        
        break;

	    case 1:
        LCD_position(LINE2);
        _LCD_PRINT("Bloquear Medio       ");      
        break;
	  
	    case 2:
        LCD_position(LINE2);
        _LCD_PRINT("Bloque de alta       ");        
        break;
	  
	    case 3:
        LCD_position(LINE2);
        _LCD_PRINT("Bloquear la humedad ");        
        break;
      }       
    }
}
/*
void auto_cal_depth(BYTE display_set, int j)
{
	if(Features.language_f){
	 switch(display_set)
	 {
		case 0:
		 if(bit_test(valid_depth,depth_temp))    
		 {    
			if(j == 1){
				LCD_position(LINE3);
				_LCD_PRINT("Depth: BSCA  VALID  ");
			}
			else
			{
				if(Features.SI_units){                                //           in "kg/m3" mode
					LCD_position(LINE3);
					_LCD_PRINTF("Depth: %3umm  VALID  ", j * 25);
				}
				else{
					LCD_position(LINE3);
					_LCD_PRINTF("Depth: %2uin  VALID  ", j);         //   in "PCF" mode
				}
			}
		 }
		break;
	 
		case 1:
		 if(bit_test(valid_depth,depth_temp))    
		 {    
			if(j == 1){
				LCD_position(LINE3);
				_LCD_PRINT("Depth: BSCA  INVALID");
			}
			else 
			{
				if(Features.SI_units){                               //       in "kg/m3" mode
					LCD_position(LINE3);
					_LCD_PRINTF("Depth: %3umm  INVALID", j * 25);
				}
				else{
					LCD_position(LINE3);
					_LCD_PRINTF("Depth: %2uin  INVALID", j);          //  in "PCF" mode   
				}
			}
		 }
		break;
	}
	}
	  else{
	   switch(display_set)
	   {
		  case 0:
		  if(bit_test(valid_depth,depth_temp))    
		  {    
			  if(j == 1){
				  LCD_position(LINE3);
				  _LCD_PRINT("Fondo: BSCA  VALIDO ");
			  }
			  else
			  {
				  if(Features.SI_units)
  				{                                       //    in "kg/m3" mode
					  LCD_position(LINE3);
					  _LCD_PRINTF("Fondo: %3umm  VALIDO ", j * 25);
				  }
				  else
  				{
					  LCD_position(LINE3);
					  _LCD_PRINTF("Fondo: %2uin  VALIDO ", j);         //   in "PCF" mode
				  }
			  }
		  }
		  break;
	 
		  case 1:
		  if(bit_test(valid_depth,depth_temp))    
		  {    
			  if(j == 1){
				  LCD_position(LINE3);
				  _LCD_PRINT("Fondo: BSCA INVALIDO");
			  }
			  else 
			  {
				  if(Features.SI_units){                                  //    in "kg/m3" mode
					  LCD_position(LINE3);
					  _LCD_PRINTF("Fondo: %3umm INVALIDO", j * 25);
				  }
				  else{
					  LCD_position(LINE3);
					  _LCD_PRINTF("Fondo: %2uin INVALIDO", j);      //      in "PCF" mode   
				  }
			  }
		  }
		  break;
	  }
	  }
}
*/
void yes_when_ready_txt ( void )
{
  if(Features.language_f)
  {   
      LCD_position(LINE4);
      _LCD_PRINT("Press Yes when READY");        
  }
    else
    {   
        LCD_position(LINE4);
        _LCD_PRINT("Sí, cuando PREPARADO");                  
    }
}

void shut_down_txt( void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {   
      LCD_position(LINE1);
      _LCD_PRINT("  Hold OFF Key Down ");        
      LCD_position(LINE2);
      _LCD_PRINT("    for 25secs      ");        
      LCD_position(LINE4);
      _LCD_PRINT("  Press ESC to Exit ");        
  }
  else
  {   
       LCD_position(LINE1);
      _LCD_PRINT("  Hold OFF Key Down ");        
      LCD_position(LINE2);
      _LCD_PRINT("    for 25secs      ");        
      LCD_position(LINE4);
      _LCD_PRINT("  Press ESC to Exit ");        
  }                 
  
}


void press_off_txt ( void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {   
      LCD_position(LINE1);
      _LCD_PRINT("  TEST IN PROCESS   ");        
      LCD_position(LINE2);
      _LCD_PRINT(" Keep Pressing OFF  ");        
      LCD_position(LINE4);
      _LCD_PRINT("  Press ESC to Exit ");        
  }
  else
  {   
       LCD_position(LINE1);
      _LCD_PRINT("  TEST IN PROCESS   ");        
      LCD_position(LINE2);
      _LCD_PRINT(" Keep Pressing OFF  ");        
      LCD_position(LINE4);
      _LCD_PRINT("  Press ESC to Exit ");         
  }                 
  
}


void light_test_txt( void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {   
      LCD_position(LINE1);
      _LCD_PRINT("     Press START   ");        
      LCD_position(LINE2);
      _LCD_PRINT(" to Turn on Lights ");        
      LCD_position(LINE3);
      _LCD_PRINT("    for 5 secs     ");        
      LCD_position(LINE4);
      _LCD_PRINT("  Press ESC to Exit ");        
  }
  else
  {   
      LCD_position(LINE1);
      _LCD_PRINT("     Press START    ");        
      LCD_position(LINE2);
      _LCD_PRINT(" to Turn on Lights  ");        
      LCD_position(LINE3);
      _LCD_PRINT("    for 5 secs      ");        
      LCD_position(LINE4);
      _LCD_PRINT(" Press ESC to Exit  ");    
  }                 
  
}

void lights_are_on_txt( void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {   
      
      LCD_position(LINE2);
      _LCD_PRINT("    Lights are on   ");        
      
  }
  else
  {   
    LCD_position(LINE2);
    _LCD_PRINT("    Lights are on   ");        
  }                 
  
}


void count_test_txt( void )
{
  CLEAR_DISP;
  if(Features.language_f)
  {   
      LCD_position(LINE1);
      _LCD_PRINT("  Press START for   ");        
      LCD_position(LINE2);
      _LCD_PRINT("  30 s. Tube Test   ");        
      LCD_position(LINE4);
      _LCD_PRINT("  Press ESC to Exit ");        
  }
  else
  {   
      LCD_position(LINE1);
      _LCD_PRINT("  Press START for   ");        
      LCD_position(LINE2);
      _LCD_PRINT("  30 s. Tube Test   ");        
      LCD_position(LINE4);
      _LCD_PRINT("  Press ESC to Exit ");    
  }                 
  
}


void display_count_text ( uint16_t moist, uint32_t dens,uint32_t dens_combo )
{ 

  CLEAR_DISP;
  
  if(Features.language_f)
  {
    LCD_position (LINE1);
    _LCD_PRINTF("      M Count:%u", moist); 
    LCD_position (LINE2);
    _LCD_PRINTF(" Combo DCount:%lu",dens_combo); 
    LCD_position (LINE3);
    _LCD_PRINTF("Single DCount:%lu",dens); 
    LCD_position(LINE4);
    _LCD_PRINT("  Press ESC to Exit ");  
  }
  else
  {
    LCD_position (LINE1);
    _LCD_PRINT("      M Count:      "); 
    LCD_position (LINE2);
    _LCD_PRINT(" Combo DCount:      "); 
    LCD_position (LINE3);
    _LCD_PRINT("Single DCount:      "); 
    LCD_position(LINE4);
    _LCD_PRINT("  Press ESC to Exit ");     
  }
}



void cnt_display_txt(BYTE display_set, int j)
{
  if(Features.language_f)
  {   
      LCD_position(LINE1);
      _LCD_PRINT(" Count in Progress  ");
	  switch(display_set)
	  {
		case 0:
	  	if(j == 1){
		LCD_position(LINE2);
		_LCD_PRINT("Depth:BSCA LOW BLOCK");
		}
		else{
			if(Features.SI_units){                                  //         in "kg/m3" mode
				LCD_position(LINE2);
				_LCD_PRINTF("Depth:%3umm LOW BLOCK", j * 25);
			}
			else{
				LCD_position(LINE2);
				_LCD_PRINTF("Depth:%2uin LOW BLOCK", j);        //    in "PCF" mode
			}
		}		
      break;

	  case 1:
	  	if(j == 1){
		LCD_position(LINE2);
		_LCD_PRINT("Depth:BSCA MED BLOCK");
		}
		else{
			if(Features.SI_units){                                 //          in "kg/m3" mode
				LCD_position(LINE2);
				_LCD_PRINTF("Depth:%3umm MED BLOCK", j * 25);
			}
			else{
				LCD_position(LINE2);
				_LCD_PRINTF("Depth:%2uin MED BLOCK", j);           // in "PCF" mode
			}
		}    
      break;
	  
	  case 2:
	  	if(j == 1){
		LCD_position(LINE2);
		_LCD_PRINT("Depth:BSCA HGH BLOCK");
		}
		else{
			if(Features.SI_units){                                //           in "kg/m3" mode
				LCD_position(LINE2);
				_LCD_PRINTF("Depth:%3umm HGH BLOCK", j * 25);
			}
			else{
				LCD_position(LINE2);
				_LCD_PRINTF("Depth:%2uin HGH BLOCK", j);         //   in "PCF" mode
			}
		}    
      break;
	  
	  case 3:
		LCD_position(LINE2);
		_LCD_PRINT("BSCA HGH MOIST BLOCK");	   
      break;
	 }
  }
  else
    {
      switch(display_set)
      {
        case 0:
	  	  if(j == 1){
		  LCD_position(LINE2);
		  _LCD_PRINT("BSCA Bloqueo bajo");
		  }
		  else{
			  if(Features.SI_units){                            //               in "kg/m3" mode
				  LCD_position(LINE2);
				  _LCD_PRINTF("%3umm Bloqueo bajo", j * 25);
			  }
			  else{
				  LCD_position(LINE2);
				  _LCD_PRINTF("%2uin Bloqueo bajo", j);          //  in "PCF" mode
			  }
		  }       
        break;

	    case 1:
	  	  if(j == 1){
		  LCD_position(LINE2);
		  _LCD_PRINT("BSCA Bloquear Medio");
		  }
		  else{
			  if(Features.SI_units){                                 //          in "kg/m3" mode
				  LCD_position(LINE2);
				  _LCD_PRINTF("%3umm Bloquear Medio", j * 25);
			  }
			  else{
				  LCD_position(LINE2);
				  _LCD_PRINTF("%2uin Bloquear Medio", j);           // in "PCF" mode
			  }
		  }    
        break;
	  
	    case 2:
	  	  if(j == 1){
		  LCD_position(LINE2);
		  _LCD_PRINT("Depth:BSCA Bloque de alta");
		  }
		  else{
			  if(Features.SI_units){                                   //        in "kg/m3" mode
				  LCD_position(LINE2);
				  _LCD_PRINTF("Depth:%3umm Bloque de alta", j * 25);
			  }
			  else{
				  LCD_position(LINE2);
				  _LCD_PRINTF("Depth:%2uin Bloque de alta", j);        //    in "PCF" mode
			  }
		  }    
        break;
	  
	  	    case 3:
		  LCD_position(LINE2);
		  _LCD_PRINT("BSCA BLOQUE MOIST HGH");	   
        break;
	  
      }
        
    }
}
/*
void cnt_result_display_txt(BYTE display_set, int j)
{
	
  UINT16 den_temp;
  UINT16 moist_temp;

  CLEAR_DISP;   
  
   if(Features.language_f)
   {  
    switch(display_set)
    {
      case 0:
    	  	if(j == 1)
      	  {
        		LCD_position(LINE1);
        		_LCD_PRINT("Depth:BSCA LOW BLOCK");
        		
        		den_temp = NV_RAM_MEMBER_RD(autocal_data.Depth_Low_Bscatter[0]);
        		moist_temp = NV_RAM_MEMBER_RD(autocal_data.Depth_Low_Bscatter[1] );
        		LCD_position(LINE2);
        		_LCD_PRINTF("Density Count %u", den_temp);
        		LCD_position(LINE3);
        		_LCD_PRINTF("Moisture Count %u", moist_temp);
    	  	}
    		else{
    			if(Features.SI_units)
      		{                                           in "kg/m3" mode
    				LCD_position(LINE1);
    				_LCD_PRINTF("Depth:%3umm LOW BLOCK", j * 25);
    			}
    			else{
    				LCD_position(LINE1);
    				_LCD_PRINTF("Depth:%2uin LOW BLOCK", j);            in "PCF" mode
    			}
    			
    		den_temp = NV_RAM_MEMBER_RD(autocal_data.Depth_Low_Bscatter+((j-1)*8));
    		LCD_position(LINE2);
    		_LCD_PRINTF("Density Count %u", den_temp);
    
    		}

		
      break;

	  case 1:
    	  	if (j == 1)
      	  {
        		LCD_position(LINE1);
        		_LCD_PRINT("Depth:BSCA MED BLOCK");
        		
        		den_temp = NV_RAM_MEMBER_RD(autocal_data.Depth_Med_Bscatter[0]);
        		moist_temp = NV_RAM_MEMBER_RD(autocal_data.Depth_Med_Bscatter[1]);
        		LCD_position(LINE2);
        		_LCD_PRINTF("Density Count %u", den_temp);
        		LCD_position(LINE3);
        		_LCD_PRINTF("Moisture Count %u", moist_temp);
    		
    		  }
    		else
      	{
    			if(Features.SI_units){                                           in "kg/m3" mode
    				LCD_position(LINE1);
    				_LCD_PRINTF("Depth:%3umm MED BLOCK", j * 25);
    			}
    			else{
    				LCD_position(LINE1);
    				_LCD_PRINTF("Depth:%2uin MED BLOCK", j);            in "PCF" mode
    			}
    			
    		den_temp = NV_RAM_MEMBER_RD(autocal_data.Depth_Med_Bscatter+((j-1)*8));
    		LCD_position(LINE2);
    		_LCD_PRINTF("Density Count %u", den_temp);
    		  LCD_position(LINE3);
    		  _LCD_PRINT("Moisture Count  N/A");
    		
    		}    
          break;
	  
	  case 2:
	  	if(j == 1){
		LCD_position(LINE1);
		_LCD_PRINT("Depth:BSCA HGH BLOCK");
		
		den_temp = NV_RAM_MEMBER_RD(autocal_data.Depth_High_Bscatter[0]);
		moist_temp = NV_RAM_MEMBER_RD(autocal_data.Depth_High_Bscatter[1]);
		LCD_position(LINE2);
		_LCD_PRINTF("Density Count %u", den_temp);
		LCD_position(LINE3);
		_LCD_PRINTF("Moisture Count %u", moist_temp);
		}
		else{
			if(Features.SI_units){                                           in "kg/m3" mode
				LCD_position(LINE1);
				_LCD_PRINTF("Depth:%3umm HGH BLOCK", j * 25);
			}
			else{
				LCD_position(LINE1);
				_LCD_PRINTF("Depth:%2uin HGH BLOCK", j);            in "PCF" mode
			}
			
		den_temp = NV_RAM_MEMBER_RD(autocal_data.Depth_High_Bscatter+((j-1)*8));
		LCD_position(LINE2);
		_LCD_PRINTF("Density Count %u", den_temp);

		}    
      break;
	  
	  case 3:
		LCD_position(LINE1);
		_LCD_PRINT("BSCA HGH MOIST BLOCK");
		
		den_temp = NV_RAM_MEMBER_RD(autocal_data.Depth_High_Moist_Bscatter[0]);
		moist_temp =NV_RAM_MEMBER_RD(autocal_data.Depth_High_Moist_Bscatter[1]);
		  LCD_position(LINE2);
		  _LCD_PRINT("Density Count N/A");
		LCD_position(LINE3);
		_LCD_PRINTF("Moisture Count %u", moist_temp);		   
      break;
    }
  }
}
*/


void review_consts(){ 
  if(Features.language_f)
  {
    LCD_position(LINE2); 
    _LCD_PRINT(" Review Constants?  ");
  	LCD_position(LINE3); 
    _LCD_PRINT("  Press YES or NO   "); 	
	}

 
}

void store_consts(){ 
  if(Features.language_f)
  {
    LCD_position(LINE2); 
    _LCD_PRINT(" Store Constants?  ");
    LCD_position(LINE3); 
    _LCD_PRINT("  Press YES or NO   "); 	
	}

  
}

void select_mode_text()
{                                                        
  CLEAR_DISP;
  LCD_position(LINE1); 
  _LCD_PRINT("1. CPN MC3 Elite    ");
  LCD_position(LINE2);
  _LCD_PRINT("2. SMART MC         ");
  LCD_position(LINE3);
  _LCD_PRINT("3. Strata           ");
  LCD_position(LINE4);
  _LCD_PRINT("4. CPN MC1 ELITE    ");

}

void recount_txt(){

if(Features.language_f){
LCD_position(LINE1); 
_LCD_PRINT("1.Stat     2.Drift  ");
LCD_position(LINE2); 
_LCD_PRINT("3.Low Blk  4.Med Blk");
LCD_position(LINE3); 
_LCD_PRINT("5.High Blk");
LCD_position(LINE4); 
_LCD_PRINT("Select #, ESC Exit  "); 
}
  else{
  LCD_position(LINE1); 
  _LCD_PRINT("1.Stat     2.Deriva ");
  LCD_position(LINE2); 
  _LCD_PRINT("3.Bajo     4.Medio  ");
  LCD_position(LINE3); 
  _LCD_PRINT("5.Alto");
  LCD_position(LINE4); 
  _LCD_PRINT("Sel #,ESC para Salir");
 
  }


}


void press_yes_txt(){

if(Features.language_f){
LCD_position(LINE4); 
_LCD_PRINT("      Press YES     ");
}
else{
LCD_position(LINE4); 
_LCD_PRINT("     PULSE INTRO    "); 
}
}

void press_enter_txt(){

if(Features.language_f){
LCD_position(LINE4); 
_LCD_PRINT("     Press ENTER    ");
}
else{
LCD_position(LINE4); 
_LCD_PRINT("    PULSE INTRO    "); 
}
}

void write_Calib_USB_txt(){

LCD_position(LINE1); 
_LCD_PRINT(" Write Calibra Data ");
LCD_position(LINE2); 
_LCD_PRINT("       To USB       ");
Press_YES_or_NO(LINE3);
}

/*
void gauge_type_txt ( BYTE display_set )
  {

  switch(display_set)
  {
  	case 0:
  	 LCD_position(LINE1);
       _LCD_PRINT(" Select Gauge Type ");
  	 LCD_position(LINE2);
       _LCD_PRINT("1. 8-1      2. 8-2 ");
       LCD_position(LINE3); 
       _LCD_PRINT("3. 12-1     4. 12-2");
  	 LCD_position(LINE4); 
  	 _LCD_PRINT("Select #, ESC Exit "); 
  	break;
  	
  	case 1:
  	LCD_position(LINE1);
  	_LCD_PRINT("   Gauge Type 8-1   ");
  	break;
  	
  	case 2:
  	LCD_position(LINE1);
  	_LCD_PRINT("   Gauge Type 8-2   ");
  	break;
  	
  	case 3:
  	LCD_position(LINE1);
  	_LCD_PRINT("   Gauge Type 12-1  ");
  	break;
  	
  	case 4:
  	LCD_position(LINE1);
  	_LCD_PRINT("   Gauge Type 12-2   ");
  	break;	
  		
  	case 5:
  	LCD_position(LINE2);
  	_LCD_PRINT("   Special Depth?   ");
  	LCD_position(LINE3);
  	_LCD_PRINT("   Press Yes or No  ");
  	break;
  }

}
*/
/*
  void auto_cal_depth(BYTE function, BYTE depth_temp)
  {  
    if(Features.language_f)
    {
      if((depth_temp <= 1) && auto_depth)
      {
        if((depth_temp == 0) && (function == 0))
          _LCD_PRINT("Depth: SAFE    ");             //    Safe consts.
        else 
          _LCD_PRINT("Depth: BSCATTER");
      }    
      else if(bit_test(valid_depth,depth_temp))    
      {    
        if(depth_temp == 0)
          _LCD_PRINT("Depth: BSCATTER");
        else
        {
          if(Features.SI_units)                                     //      in "kg/m3" mode
            _LCD_PRINTF("Depth: %3u mm.     ", depth_temp * 25);
          else
            _LCD_PRINTF("Depth: %2u in.     ", depth_temp);          //  in "PCF" mode   
        } 
      }
      else
        _LCD_PRINT("Depth: No Depth");  
    }
    else
    {
      if((depth_temp <= 1) && auto_depth)
      {
        if((depth_temp == 0) && (function == 0))
          _LCD_PRINT("Profundidad: SEGURO ");              //   Safe consts.
        else 
          _LCD_PRINT("Profundidad: RD     ");
      }    
      else if(bit_test(valid_depth,depth_temp))    
      {    
        if(depth_temp == 0)
          _LCD_PRINT("Profundidad: RD     ");
        else
        {
          if(Features.SI_units)                            //               in "kg/m3" mode
            _LCD_PRINTF("Profundidad: %3u mm.", depth_temp * 25);
          else
            _LCD_PRINTF("Profundidad: %2u in. ", depth_temp);         //   in "PCF" mode   
        } 
      }
      else
        _LCD_PRINT("Profundidad: Sin    ");   
    }  
  }

*/
