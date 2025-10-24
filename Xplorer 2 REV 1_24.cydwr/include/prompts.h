
#ifndef PROMPTS_H
#define PROMPTS_H

#include "Globals.h"

void date_usb_error_text ( void );
void trench_text(void);
void shutdown_inactivity_text_text(void);
void select_test_or_spec_cal(int16_t inspec);
void ESC_to_Exit(BYTE position);
void up_down_select_text(int16_t both);
void mat_thickness_error ( void );
void display_version(void);
void calibration_due();
void initializing(void);
void main_menu_display(uint8_t menu_trk);
void project_menu_display(uint8_t menu_trk) ;
void diag_menu_display(uint8_t menu_trk);
void display_depth(BYTE function, BYTE depth_temp);
void use_val_text(void);
void ready(BYTE cnt_temp, int line);
void display_count_time(BYTE value_temp, uint8_t position );
void select_test_or_spec_cal(int16_t inspec);
void new_std_count();
void YES_to_Accept(BYTE position);
void Enter_to_Accept(BYTE position);
void up_down_select_text(int16_t both);
void display_version(void);
void calibration_due();
void initializing(void);
void main_menu_display(uint8_t menu_trk);
void project_menu_display(uint8_t menu_trk);
void display_depth(BYTE function, BYTE depth_temp);
void select_test_or_spec_cal(int16_t inspec);
void new_std_count();
void press_start(BYTE which_test);
void use_new_std_count();
void max_stations_text(char *temp_str);
void no_project_selected();
void disable_auto_depth();
void invalid_std_or_const(void);
void reading(void);
void self_test_text(BYTE which_test);
void voltage_out_of_range(float volts_temp);
void diagnostics_text(BYTE which_prompt);
void get_new_offset_text(void);
void USB_text(BYTE which_prompt);
void printer_text(BYTE which_prompt);
void display_time(BYTE cnt_temp,uint8_t line);
void display_station_name(char *temp_str);
void std_text(void);
void drift_text(void);
void spec_text(void);
void spec_text(void);
void enable_disable_text(char *temp_str,char c);
void reset_avg_count(void);
void auto_depth_settings(void);

void gauge_in_safe(void);

void calibration_complete_text(void);
void set_units_text(void);
void offsets_text(void);
void offset_on_off_text(char *temp_str, int16_t enabled);
void enter_offset_text(char *temp_str);
void offset_enable_disable_text(char *temp_str, int16_t enabled);
void enter_access_number_text(void);
void access_number_incorrect_text(void);
void cal_menu_text(BYTE display_set);
void cal_depth_text(int16_t depth_temp);
void reset_cal_date_text(BYTE which_prompt);
void moisture_const_text(void);
void bscatter_const_text(void);
void memory_reset_text(void);
void enable_disable_speccal_text(int16_t condition);
void view_last_speccal_text(void);

void vals_ok_text(void);
void use_gauge_text(void);
void enter_value_text(BYTE which_value);
void use_stored_count_text(void);

void activate_speccal_text(void);
void enter_number_counts_text(void);

void start_speccal_text(void);
void Press_YES_or_NO (BYTE position);
//void gauge_type_txt(BYTE display_set);
void press_enter_txt();
void recount_txt();
void progress_bar( BYTE display_set);
void trench_count_press_start(void);
void store_consts();
void  chi_squared_settings( void );
void review_consts();
void block_densities_txt ( BYTE display_set);
void cnt_result_display_txt(BYTE display_set, int j);

void cnt_display_txt(BYTE display_set, int j);
void yes_when_ready_txt();
void auto_cal_depth(BYTE display_set, int j);
void block_txt(BYTE display_set);
void place_gauge_txt();
void auto_cal_text(BYTE display_set);
void self_test_pf_text(BYTE source, int16_t test_failed);
void stat_drift_count_text(BYTE source);
void count_text(BYTE source);
void select_language_text();
void serial_port_text();
void enter_station_name_text();
void print_complete_text();
void print_data_text();
void batt_volt_text();
void depth_voltage_text(void);
void write_USB_text();
void erase_project_data_text();
void delete_project_text(char *temp_str);
void all_data_erased_text();
void erase_data_text();
void enter_to_delete_text();
void delete_data_text();
void no_data_stored_text();
void select_from_list_text(BYTE from_where);
void no_stored_projects_text();
void project_limit_text();
void station_name_text();
void station_increment_text(uint16_t inc);
void station_start_text(char *temp_str);
void station_mode_text();
void project_name_text(char *temp_str);
void enter_project_name_text(char *temp_str);
void enter_valid_text(BYTE which_source);
void shutdown_inactivity_text_text(void);
void shut_down_text_text(void);
void high_voltage_text(void);

void stat_text(void);
void speccal_enabled_text(void);
void std_text(void);
void drift_text(void);
void speccal_results_text(int16_t loops, uint16_t cnt_temp);
void enable_disable_nomograph_text(int16_t condition);
void activate_saved_vals_text(void);
void nomograph_vals_text(void);
void no_data_to_store_text ( void );
void depth_not_in_ac_text ( void );
void depth_not_in_bs_text ( void );
void place_in_ac_text ( void );
void place_in_bs_text ( void );

void nomograph_text(void);
void enter_mat_thickness_text(void);
void enter_bot_dense_text(void);
void nomograph_enabled_text(void);
void enter_time_text(void);
void am_pm_text(void);
void prma_text(void);
void change_val_text(void);
void battery_shutdwn_text(void);
void display_alk_warning(void);
void warning_text(BYTE which_source);
void gauge_temp_text(void);
void high_voltage_text(void);
void shut_down_text_text(void);
void shutdown_inactivity_text_text(void);
void enter_valid_text(BYTE which_source);
void speccal_enabled_text(void);
void no_valid_depth_selected (void);
void current_project_text ( char *temp_str );
void std_count_press_start(void);
void enter_cal_mois_std_text (void);
void enter_cal_den_std_text (void);
void gauge_in_bs_text(void);
void calibration_fail_text(void);
void stand_mode_settings_text(void);
void decay_stand_mode_enabled_text(void);
void avg_stand_mode_enabled_text(void);
void calibration_date_txt(void);
void not_in_safe_text ( void );
void not_in_bs_text ( void );
void invalid_den_std_text ( void );
void invalid_mois_std_text( void );
void select_mode_text();
void mode_menu_display(unsigned int menu_trk);
void press_key_text(void);
void press_off_txt(void);
void shut_down_txt( void );
void print_menu_display( void ) ;

void count_test_txt( void );
void display_count_text ( uint16_t moist, uint32_t dens,uint32_t dens_combo );
void light_test_txt( void );
void lights_are_on_txt( void );

void depth_not_in_2_text ( void );
void clock_set_error_text ( void );
void display_offset ( struct offsets_struct  which_offsets);
void display_offset_on ( struct offsets_struct  which_offsets);
void sd_menu_display(uint8_t menu_trk) ;
void up_down_ENTER_select_text();
void SAV_enable_disable_text( int enabled );
void depth_strip_type(void);
void eeprom_msg(void);

#endif


