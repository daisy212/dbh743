/*

BSD 3-Clause License

Copyright (c) 2015-2022, SwissMicros
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


  The software and related material is released as “NOMAS”  (NOt MAnufacturer Supported). 

  1. Info is released to assist customers using, exploring and extending the product
  2. Do NOT contact the manufacturer with questions, seeking support, etc. regarding
     NOMAS material as no support is implied or committed-to by the Manufacturer
  3. The Manufacturer may reply and/or update materials if and when needed solely
     at their discretion

*/


#if 0
#define LIBRARY_FN_BASE   0x08000201

#define __sysfn_malloc (*(typeof(malloc)*)(LIBRARY_FN_BASE+0))
#define __sysfn_free (*(typeof(free)*)(LIBRARY_FN_BASE+4))
#define __sysfn_calloc (*(typeof(calloc)*)(LIBRARY_FN_BASE+8))
#define __sysfn_realloc (*(typeof(realloc)*)(LIBRARY_FN_BASE+12))
#define __sysfn__write (*(typeof(_write)*)(LIBRARY_FN_BASE+16))

#define LCD_clear (*(typeof(LCD_clear)*)(LIBRARY_FN_BASE+20))
#define LCD_power_on (*(typeof(LCD_power_on)*)(LIBRARY_FN_BASE+24))
#define LCD_power_off (*(typeof(LCD_power_off)*)(LIBRARY_FN_BASE+28))
#define LCD_write_line (*(typeof(LCD_write_line)*)(LIBRARY_FN_BASE+32))
#define bitblt24 (*(typeof(bitblt24)*)(LIBRARY_FN_BASE+36))
#define lcd_line_addr (*(typeof(lcd_line_addr)*)(LIBRARY_FN_BASE+40))
#define lcd_clear_buf (*(typeof(lcd_clear_buf)*)(LIBRARY_FN_BASE+44)) // not used
#define lcd_refresh (*(typeof(lcd_refresh)*)(LIBRARY_FN_BASE+48))
#define lcd_forced_refresh (*(typeof(lcd_forced_refresh)*)(LIBRARY_FN_BASE+52))
#define lcd_refresh_lines (*(typeof(lcd_refresh_lines)*)(LIBRARY_FN_BASE+56))
#define lcd_fill_rect (*(typeof(lcd_fill_rect)*)(LIBRARY_FN_BASE+60)) // not used
#define lcd_draw_img (*(typeof(lcd_draw_img)*)(LIBRARY_FN_BASE+64))
#define lcd_draw_img_direct (*(typeof(lcd_draw_img_direct)*)(LIBRARY_FN_BASE+68))
#define lcd_draw_img_part (*(typeof(lcd_draw_img_part)*)(LIBRARY_FN_BASE+72))
#define lcd_fillLine (*(typeof(lcd_fillLine)*)(LIBRARY_FN_BASE+76))
#define lcd_fillLines (*(typeof(lcd_fillLines)*)(LIBRARY_FN_BASE+80))
#define lcd_set_buf_cleared (*(typeof(lcd_set_buf_cleared)*)(LIBRARY_FN_BASE+84))
#define lcd_get_buf_cleared (*(typeof(lcd_get_buf_cleared)*)(LIBRARY_FN_BASE+88))
#define lcd_writeNl (*(typeof(lcd_writeNl)*)(LIBRARY_FN_BASE+92))
#define lcd_prevLn (*(typeof(lcd_prevLn)*)(LIBRARY_FN_BASE+96))
#define lcd_writeClr (*(typeof(lcd_writeClr)*)(LIBRARY_FN_BASE+100))
#define lcd_setLine (*(typeof(lcd_setLine)*)(LIBRARY_FN_BASE+104))
#define lcd_lineHeight (*(typeof(lcd_lineHeight)*)(LIBRARY_FN_BASE+112))
#define lcd_baseHeight (*(typeof(lcd_baseHeight)*)(LIBRARY_FN_BASE+116))
#define lcd_fontWidth (*(typeof(lcd_fontWidth)*)(LIBRARY_FN_BASE+120))
#define lcd_setXY (*(typeof(lcd_setXY)*)(LIBRARY_FN_BASE+108))

#define lcd_writeText (*(typeof(lcd_writeText)*)(LIBRARY_FN_BASE+124))
#define lcd_textWidth (*(typeof(lcd_textWidth)*)(LIBRARY_FN_BASE+128))
#define lcd_charWidth (*(typeof(lcd_charWidth)*)(LIBRARY_FN_BASE+132))
#define lcd_textToWidth (*(typeof(lcd_textToWidth)*)(LIBRARY_FN_BASE+136))
#define lcd_writeTextWidth (*(typeof(lcd_writeTextWidth)*)(LIBRARY_FN_BASE+140))
#define lcd_textForWidth (*(typeof(lcd_textForWidth)*)(LIBRARY_FN_BASE+144))
#define lcd_nextFontNr (*(typeof(lcd_nextFontNr)*)(LIBRARY_FN_BASE+148))
#define lcd_prevFontNr (*(typeof(lcd_prevFontNr)*)(LIBRARY_FN_BASE+152))
#define lcd_switchFont (*(typeof(lcd_switchFont)*)(LIBRARY_FN_BASE+156))
#define lcd_toggleFontT (*(typeof(lcd_toggleFontT)*)(LIBRARY_FN_BASE+160))

#define lcd_draw_menu_bg (*(typeof(lcd_draw_menu_bg)*)(LIBRARY_FN_BASE+164))
#define lcd_draw_menu_key (*(typeof(lcd_draw_menu_key)*)(LIBRARY_FN_BASE+168))
#define lcd_draw_menu_keys (*(typeof(lcd_draw_menu_keys)*)(LIBRARY_FN_BASE+172))

#define lcd_print (*(typeof(lcd_print)*)(LIBRARY_FN_BASE+176))

#define lcd_for_calc (*(typeof(lcd_for_calc)*)(LIBRARY_FN_BASE+180))

#define get_wday_shortcut (*(typeof(get_wday_shortcut)*)(LIBRARY_FN_BASE+184))
#define get_month_shortcut (*(typeof(get_month_shortcut)*)(LIBRARY_FN_BASE+188))
#define julian_day (*(typeof(julian_day)*)(LIBRARY_FN_BASE+192))
#define julian_to_date (*(typeof(julian_to_date)*)(LIBRARY_FN_BASE+196))

#define rtc_read (*(typeof(rtc_read)*)(LIBRARY_FN_BASE+204))
#define rtc_write (*(typeof(rtc_write)*)(LIBRARY_FN_BASE+208))
#define rtc_read_century (*(typeof(rtc_read_century)*)(LIBRARY_FN_BASE+212))
#define rtc_write_century (*(typeof(rtc_write_century)*)(LIBRARY_FN_BASE+216))
#define rtc_read_min (*(typeof(rtc_read_min)*)(LIBRARY_FN_BASE+220))
#define rtc_read_sec (*(typeof(rtc_read_sec)*)(LIBRARY_FN_BASE+224))
#define rtc_wakeup_delay (*(typeof(rtc_wakeup_delay)*)(LIBRARY_FN_BASE+228))


#define get_hw_id (*(typeof(get_hw_id)*)(LIBRARY_FN_BASE+200))
#define read_power_voltage (*(typeof(read_power_voltage)*)(LIBRARY_FN_BASE+232))
#define get_lowbat_state (*(typeof(get_lowbat_state)*)(LIBRARY_FN_BASE+236))
#define get_vbat (*(typeof(get_vbat)*)(LIBRARY_FN_BASE+240))

#define start_buzzer_freq (*(typeof(start_buzzer_freq)*)(LIBRARY_FN_BASE+244))
#define stop_buzzer (*(typeof(stop_buzzer)*)(LIBRARY_FN_BASE+248))
#define beep_volume_up (*(typeof(beep_volume_up)*)(LIBRARY_FN_BASE+252))
#define beep_volume_down (*(typeof(beep_volume_down)*)(LIBRARY_FN_BASE+256))
#define get_beep_volume (*(typeof(get_beep_volume)*)(LIBRARY_FN_BASE+260))

#define mark_region (*(typeof(mark_region)*)(LIBRARY_FN_BASE+264))
#define no_region (*(typeof(no_region)*)(LIBRARY_FN_BASE+268))
#define set_reset_magic (*(typeof(set_reset_magic)*)(LIBRARY_FN_BASE+272))
#define is_reset_state_file (*(typeof(is_reset_state_file)*)(LIBRARY_FN_BASE+276))
#define get_reset_state_file (*(typeof(get_reset_state_file)*)(LIBRARY_FN_BASE+280))
#define set_reset_state_file (*(typeof(set_reset_state_file)*)(LIBRARY_FN_BASE+284))
#define usb_powered (*(typeof(usb_powered)*)(LIBRARY_FN_BASE+288))
#define aux_buf_ptr (*(typeof(aux_buf_ptr)*)(LIBRARY_FN_BASE+292))
#define write_buf_ptr (*(typeof(write_buf_ptr)*)(LIBRARY_FN_BASE+296))
#define print_byte (*(typeof(print_byte)*)(LIBRARY_FN_BASE+300))
#define printer_get_delay (*(typeof(printer_get_delay)*)(LIBRARY_FN_BASE+304))
#define printer_set_delay (*(typeof(printer_set_delay)*)(LIBRARY_FN_BASE+308))
#define printer_advance_buf (*(typeof(printer_advance_buf)*)(LIBRARY_FN_BASE+312))
#define printer_busy_for (*(typeof(printer_busy_for)*)(LIBRARY_FN_BASE+316))
#define rtc_check_unset (*(typeof(rtc_check_unset)*)(LIBRARY_FN_BASE+320))
#define run_set_time (*(typeof(run_set_time)*)(LIBRARY_FN_BASE+324))
#define run_set_date (*(typeof(run_set_date)*)(LIBRARY_FN_BASE+328))
#define disp_disk_info (*(typeof(disp_disk_info)*)(LIBRARY_FN_BASE+332))
#define file_selection_screen (*(typeof(file_selection_screen)*)(LIBRARY_FN_BASE+336))
#define power_check_screen (*(typeof(power_check_screen)*)(LIBRARY_FN_BASE+340))
#define handle_menu (*(typeof(handle_menu)*)(LIBRARY_FN_BASE+344))
#define rb_str (*(typeof(rb_str)*)(LIBRARY_FN_BASE+348))
#define sel_str (*(typeof(sel_str)*)(LIBRARY_FN_BASE+352))
#define opt_str (*(typeof(opt_str)*)(LIBRARY_FN_BASE+356))
#define date_str (*(typeof(date_str)*)(LIBRARY_FN_BASE+360))
#define time_str (*(typeof(time_str)*)(LIBRARY_FN_BASE+364))
#define read_file_items (*(typeof(read_file_items)*)(LIBRARY_FN_BASE+368))
#define sort_file_items (*(typeof(sort_file_items)*)(LIBRARY_FN_BASE+372))
#define create_screenshot (*(typeof(create_screenshot)*)(LIBRARY_FN_BASE+376))

#define key_empty (*(typeof(key_empty)*)(LIBRARY_FN_BASE+380))
#define key_push (*(typeof(key_push)*)(LIBRARY_FN_BASE+384))
#define key_tail (*(typeof(key_tail)*)(LIBRARY_FN_BASE+388))

#define key_pop (*(typeof(key_pop)*)(LIBRARY_FN_BASE+392))
#define key_pop_last (*(typeof(key_pop_last)*)(LIBRARY_FN_BASE+396))
#define key_pop_all (*(typeof(key_pop_all)*)(LIBRARY_FN_BASE+400))

#define wait_for_key_press (*(typeof(wait_for_key_press)*)(LIBRARY_FN_BASE+408))
#define runner_get_key (*(typeof(runner_get_key)*)(LIBRARY_FN_BASE+412))
#define runner_get_key_delay (*(typeof(runner_get_key_delay)*)(LIBRARY_FN_BASE+416))
#define wait_for_key_release (*(typeof(wait_for_key_release)*)(LIBRARY_FN_BASE+420))
#define runner_key_tout_value (*(typeof(runner_key_tout_value)*)(LIBRARY_FN_BASE+424))
#define runner_key_tout_init (*(typeof(runner_key_tout_init)*)(LIBRARY_FN_BASE+428))
#define toggle_slow_autorepeat (*(typeof(toggle_slow_autorepeat)*)(LIBRARY_FN_BASE+432))
#define is_slow_autorepeat (*(typeof(is_slow_autorepeat)*)(LIBRARY_FN_BASE+436))
#define key_to_nr (*(typeof(key_to_nr)*)(LIBRARY_FN_BASE+404))

#define reset_auto_off (*(typeof(reset_auto_off)*)(LIBRARY_FN_BASE+440))
#define is_auto_off (*(typeof(is_auto_off)*)(LIBRARY_FN_BASE+444))
#define is_menu_auto_off (*(typeof(is_menu_auto_off)*)(LIBRARY_FN_BASE+448))
#define sys_auto_off_cnt (*(typeof(sys_auto_off_cnt)*)(LIBRARY_FN_BASE+452))

#define check_create_dir (*(typeof(check_create_dir)*)(LIBRARY_FN_BASE+464))
#define set_fat_label (*(typeof(set_fat_label)*)(LIBRARY_FN_BASE+468))
#define file_exists (*(typeof(file_exists)*)(LIBRARY_FN_BASE+472))
#define sys_disk_ok (*(typeof(sys_disk_ok)*)(LIBRARY_FN_BASE+476))
#define sys_disk_write_enable (*(typeof(sys_disk_write_enable)*)(LIBRARY_FN_BASE+480))
#define sys_disk_check_valid (*(typeof(sys_disk_check_valid)*)(LIBRARY_FN_BASE+484))
#define sys_is_disk_write_enable (*(typeof(sys_is_disk_write_enable)*)(LIBRARY_FN_BASE+488))
#define sys_clear_write_buf_used (*(typeof(sys_clear_write_buf_used)*)(LIBRARY_FN_BASE+492))


#define print_dmy_date (*(typeof(print_dmy_date)*)(LIBRARY_FN_BASE+456))
#define print_clk24_time (*(typeof(print_clk24_time)*)(LIBRARY_FN_BASE+460))
#define sys_write_buf_used (*(typeof(sys_write_buf_used)*)(LIBRARY_FN_BASE+496))
#define sys_timer_disable (*(typeof(sys_timer_disable)*)(LIBRARY_FN_BASE+500))
#define sys_timer_start (*(typeof(sys_timer_start)*)(LIBRARY_FN_BASE+504))
#define sys_timer_active (*(typeof(sys_timer_active)*)(LIBRARY_FN_BASE+508))
#define sys_timer_timeout (*(typeof(sys_timer_timeout)*)(LIBRARY_FN_BASE+512))
#define sys_delay (*(typeof(sys_delay)*)(LIBRARY_FN_BASE+516))

#define sys_tick_count (*(typeof(sys_tick_count)*)(LIBRARY_FN_BASE+520))

#define sys_current_ms (*(typeof(sys_current_ms)*)(LIBRARY_FN_BASE+524))
#define sys_critical_start (*(typeof(sys_critical_start)*)(LIBRARY_FN_BASE+528))
#define sys_critical_end (*(typeof(sys_critical_end)*)(LIBRARY_FN_BASE+532))
#define sys_sleep (*(typeof(sys_sleep)*)(LIBRARY_FN_BASE+536))

#define sys_free_mem (*(typeof(sys_free_mem)*)(LIBRARY_FN_BASE+540))

#define sys_reset (*(typeof(sys_reset)*)(LIBRARY_FN_BASE+544))
#define sys_last_key (*(typeof(sys_last_key)*)(LIBRARY_FN_BASE+548))

#define run_help (*(typeof(run_help)*)(LIBRARY_FN_BASE+552))
#define draw_power_off_image (*(typeof(draw_power_off_image)*)(LIBRARY_FN_BASE+556))
#define reset_off_image_cycle (*(typeof(reset_off_image_cycle)*)(LIBRARY_FN_BASE+560))
#define f_open (*(typeof(f_open)*)(LIBRARY_FN_BASE+564))
#define f_close (*(typeof(f_close)*)(LIBRARY_FN_BASE+568))
#define f_read (*(typeof(f_read)*)(LIBRARY_FN_BASE+572))
#define f_write (*(typeof(f_write)*)(LIBRARY_FN_BASE+576))
#define f_lseek (*(typeof(f_lseek)*)(LIBRARY_FN_BASE+580))
#define run_help_file (*(typeof(run_help_file)*)(LIBRARY_FN_BASE+584))
#define set_buzzer (*(typeof(set_buzzer)*)(LIBRARY_FN_BASE+588))
//#define __sysfn_read_key (*(typeof(read_key)*)(LIBRARY_FN_BASE+592))
#define get_tim1_timer (*(typeof(get_tim1_timer)*)(LIBRARY_FN_BASE+596))
#define update_bmp_file_header (*(typeof(update_bmp_file_header)*)(LIBRARY_FN_BASE+600))
#define make_date_filename (*(typeof(make_date_filename)*)(LIBRARY_FN_BASE+604))


#define reverse_byte (*(typeof(reverse_byte)*)(LIBRARY_FN_BASE+608))
#define f_rename (*(typeof(f_rename)*)(LIBRARY_FN_BASE+612))
#define file_size (*(typeof(file_size)*)(LIBRARY_FN_BASE+616))
#define start_timer2 (*(typeof(start_timer2)*)(LIBRARY_FN_BASE+620))
#define start_timer3 (*(typeof(start_timer3)*)(LIBRARY_FN_BASE+624))
#define stop_timer2 (*(typeof(stop_timer2)*)(LIBRARY_FN_BASE+628))
#define stop_timer3 (*(typeof(stop_timer3)*)(LIBRARY_FN_BASE+632))
#define __sysfn_suspended_bg_key_read (*(typeof(suspended_bg_key_read)*)(LIBRARY_FN_BASE+636))
#define __sysfn_resume_bg_key_read (*(typeof(resume_bg_key_read)*)(LIBRARY_FN_BASE+640))

#define lcd_refresh_dma (*(typeof(lcd_refresh_dma)*)(LIBRARY_FN_BASE+644))
#define lcd_refresh_wait (*(typeof(lcd_refresh_wait)*)(LIBRARY_FN_BASE+648))

#define lcd_textToBox (*(typeof(lcd_textToBox)*)(LIBRARY_FN_BASE+652))
#define item_sel_init (*(typeof(item_sel_init)*)(LIBRARY_FN_BASE+656))
#define item_sel_reinit (*(typeof(item_sel_reinit)*)(LIBRARY_FN_BASE+660))
#define item_sel_header (*(typeof(item_sel_header)*)(LIBRARY_FN_BASE+664))
#define item_sel_engine (*(typeof(item_sel_engine)*)(LIBRARY_FN_BASE+668))
#define sys_flashing_init (*(typeof(sys_flashing_init)*)(LIBRARY_FN_BASE+672))
#define sys_flashing_finish (*(typeof(sys_flashing_finish)*)(LIBRARY_FN_BASE+676))
#define sys_flash_erase_block (*(typeof(sys_flash_erase_block)*)(LIBRARY_FN_BASE+680))
#define sys_flash_write_block (*(typeof(sys_flash_write_block)*)(LIBRARY_FN_BASE+684))
#define write_buf_size (*(typeof(write_buf_size)*)(LIBRARY_FN_BASE+692))

#define msg_box (*(typeof(msg_box)*)(LIBRARY_FN_BASE+688))

#define get_rtc_ticks (*(typeof(get_rtc_ticks)*)(LIBRARY_FN_BASE+696))
#define rtc_update_ticks (*(typeof(rtc_update_ticks)*)(LIBRARY_FN_BASE+700))
#define rtc_set_alarm (*(typeof(rtc_set_alarm)*)(LIBRARY_FN_BASE+704))
#define rtc_cancel_alarm (*(typeof(rtc_cancel_alarm)*)(LIBRARY_FN_BASE+708))


#define rtc_update_time_sec (*(typeof(rtc_update_time_sec)*)(LIBRARY_FN_BASE+712))

#define run_help_file_style (*(typeof(run_help_file_style)*)(LIBRARY_FN_BASE+716))
#define print_buffer (*(typeof(print_buffer)*)(LIBRARY_FN_BASE+720))
#define print_is_ready (*(typeof(print_is_ready)*)(LIBRARY_FN_BASE+724))
#define run_menu_item_sys (*(typeof(run_menu_item_sys)*)(LIBRARY_FN_BASE+728))

#define lcd_fill_ptrn (*(typeof(lcd_fill_ptrn)*)(LIBRARY_FN_BASE+732))

#define usb_acm_on (*(typeof(usb_acm_on)*)(LIBRARY_FN_BASE+736))
#define usb_turn_off (*(typeof(usb_turn_off)*)(LIBRARY_FN_BASE+740))
#define usb_is_on (*(typeof(usb_is_on)*)(LIBRARY_FN_BASE+744))
#define acm_puts (*(typeof(acm_puts)*)(LIBRARY_FN_BASE+748))
#define switch_usb_powered_freq (*(typeof(switch_usb_powered_freq)*)(LIBRARY_FN_BASE+752))
#define qspi_user_write (*(typeof(qspi_user_write)*)(LIBRARY_FN_BASE+756))
#define qspi_user_addr (*(typeof(qspi_user_addr)*)(LIBRARY_FN_BASE+760))
#define qspi_user_size (*(typeof(qspi_user_size)*)(LIBRARY_FN_BASE+764))


#define f_unlink (*(typeof(f_unlink)*)(LIBRARY_FN_BASE+768))
#endif //DM42
