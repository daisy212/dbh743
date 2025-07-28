/*

undefined symbol hrtc referenced by symbol rtc_read (section .text.rtc_read in file db_hardware_def.o)

*/
#include "dmcp.h"
#include "db_hardware_def.h"
#include "bsp.h"
#include "string.h"

#include "SEGGER_RTT.h"
//#include "IP.h"
#include <stdio.h>
#include "LS027B7DH01.h"

OS_MAILBOX      Mb_Keyboard;
//OS_EVENT  _EV_KEYB;
OS_TIMER T_LcdVcom;
OS_TIMER T_Cnt_ms;


uint32_t Cnt_ms ;       // à éviter
uint64_t scrut_last;
bool usb_connected;
int aa;  // for compiling tests.cc


const uint32_t blink_seq_def[blk_last][10]={
   {500,500,0},
   {100,400,100,400,0},
   {500,500,0},
};

uint32_t blink_seq_nb = 0;
uint32_t blink_seq=0;
uint32_t blink_cnt=0;

void Ti_cb_Count_ms(void)
{
   OS_TIMER_Restart(&T_Cnt_ms);
   Cnt_ms += 2;

   if (blink_cnt <= 10){ BSP_SetLED(0);}

   if ((blink_cnt % 1000) == blink_seq_def[blink_seq_nb][blink_seq]){

      BSP_ToggleLED(0); 
      blink_seq++;
      if (blink_seq_def[blink_seq_nb][blink_seq] == 0){blink_cnt = 0; blink_seq = 0;}
   }
   blink_cnt += 2;
}


void blink_error(uint32_t errnb){
   if (errnb < blk_last){
      blink_seq_nb = errnb;
      blink_seq=0;
      blink_cnt=0;
   }
}


void Error_Handler(void){
   while(1){
      OS_TASK_Delay(50);
   }
}




void rtc_read(tm_t * tm, dt_t *dt)
{
    time_t           now;
    struct tm        utm;
    struct timeval      tv;

/*  utilisation des fonction std avec  __SEGGER_RTL_X_get_time_of_day() déclarée
         corruption mémoire ???

   time(&now);
    localtime_r(&now, &utm);
    gettimeofday(&tv, NULL);
*/
   RTC_TimeTypeDef      time_rtc;
   RTC_DateTypeDef      date_rtc;
// toujours ensemble !!!!!!
   HAL_RTC_GetTime(&hrtc, &time_rtc, RTC_FORMAT_BIN);
   HAL_RTC_GetDate(&hrtc, &date_rtc, RTC_FORMAT_BIN);
   uint32_t prescaler = hrtc.Init.SynchPrediv + 1;
   uint32_t msec = 1000 - ((time_rtc.SubSeconds * 1000) / prescaler);

// Fill the struct tm utm
    utm.tm_sec  = time_rtc.Seconds;
    utm.tm_min  = time_rtc.Minutes;
    utm.tm_hour = time_rtc.Hours;
    utm.tm_mday = date_rtc.Date;
    utm.tm_mon  = date_rtc.Month - 1;   // struct tm month range: 0-11
    utm.tm_year = date_rtc.Year + 100;  // struct tm year = years since 1900 (assuming 2000-based RTC)
    utm.tm_wday = date_rtc.WeekDay % 7; // tm_wday: Sunday = 0, RTC: Monday = 1

// time_t epoch_time = mktime(&utm);


// result setting
    dt->year = 1900 + utm.tm_year;
    dt->month = utm.tm_mon + 1;
    dt->day = utm.tm_mday;

    tm->hour = utm.tm_hour;
    tm->min = utm.tm_min;
    tm->sec = utm.tm_sec;
    tm->csec = msec/10;
 //   tm->csec = tv.tv_usec/10000;
    tm->dow = (utm.tm_wday + 6) % 7;
}

void rtc_write(tm_t * tm, dt_t *dt)
{
    SEGGER_RTT_printf(0, "\nWriting RTC %u/%u/%u %u:%u:%u (ignored)",
           dt->day, dt->month, dt->year,
           tm->hour, tm->min, tm->sec);


}








/*   A voir                       */

void      ui_refresh(void){

   LCD_UpdateDisplay(&hlcd);


}
uint      ui_refresh_count(){

}
void      ui_screenshot(){}
void      ui_push_key(int k){


}


void      ui_ms_sleep(uint ms_delay){
   OS_TASK_Delay_us(ms_delay*1000);
}



int       ui_file_selector(const char *title,
                           const char *base_dir,
                           const char *ext,
                           file_sel_fn callback,
                           void       *data,
                           int         disp_new,
                           int         overwrite_check){
                     
   char buff[80];
   snprintf(buff, sizeof(buff), "\nui_file selector : %s, %s, %s", title, base_dir, ext);
   SEGGER_RTT_WriteString(0,  buff);
}                    
               
const char cmd1[][16] = {"nothing", "state", "last"};
uint32_t get_index(  const char *name){
   for (uint32_t ii = 0; ii < sizeof(cmd1)/ sizeof(cmd1[0]); ii++){
      if (strncmp(name, cmd1[ii], sizeof(*name)) == 0 ) return ii;
   }
   return 0;
}                 

void      ui_save_setting(const char *name, const char *value){
   char buff[60];
   uint32_t index= get_index(name);
   if (index != 0) 
      bkSRAM_WriteString( index, (char *)value, sizeof(value));

   snprintf(buff, sizeof(buff), "\nui_save_setting : %s, %s", cmd1[index], value);
   SEGGER_RTT_WriteString(0,  buff);
}


size_t    ui_read_setting(const char *name, char *value, size_t maxlen){
   char buff[60];
   uint32_t index= get_index(name);
   if (index != 0) {
      bkSRAM_ReadString(1, value, maxlen);
      return sizeof(value);
   }
   return 0;
}


uint      ui_battery()         // Between 0 and 1000
{
   return 1000;
}

bool      ui_charging()        // On USB power
{
   return usb_connected;
}

void      ui_start_buzzer(uint frequency){}
void      ui_stop_buzzer(){}
//void      ui_draw_message(const char *hdr){}


void      ui_load_keymap(const char *path)
{
                while(1){}
                
                }


int runner_get_key(int *repeat)
{

    return repeat ? key_pop_last() :  key_pop();
}



void bkSRAM_ReadString(uint16_t read_adress, char* read_data, uint32_t length)
{
   uint32_t siz = length;
   if (length >256) siz = 256;
   if ((read_adress >10)||(read_adress==0)) 
      return;
   HAL_PWR_EnableBkUpAccess();
   __HAL_RCC_BKPRAM_CLK_ENABLE();
// char * pstring =  (char*) (0x38800000 + read_adress*0x100);
   memcpy( read_data, (void *)(0x38800000 + read_adress*0x100), siz);

   __HAL_RCC_BKPRAM_CLK_DISABLE();
   HAL_PWR_DisableBkUpAccess();
}

void bkSRAM_WriteString(uint16_t read_adress, char* write_data, uint32_t length)
{
   uint32_t siz = length;
   if (length >256) siz = 256;
   if ((read_adress >10)||(read_adress==0)) 
      return;
   HAL_PWR_EnableBkUpAccess();
   __HAL_RCC_BKPRAM_CLK_ENABLE();
// char * pstring =  (char*) (0x38800000 + read_adress*0x100);
   memcpy( (void*) (0x38800000 + read_adress*0x100), write_data, siz);
   __HAL_RCC_BKPRAM_CLK_DISABLE();
   HAL_PWR_DisableBkUpAccess();
}

void bkSRAM_ReadVariable(uint16_t read_adress, uint32_t* read_data)
{
       HAL_PWR_EnableBkUpAccess();
       __HAL_RCC_BKPRAM_CLK_ENABLE();
       *read_data =  *(uint32_t*) (0x38800000 + read_adress);
        __HAL_RCC_BKPRAM_CLK_DISABLE();
        HAL_PWR_DisableBkUpAccess();
}

void bkSRAM_WriteVariable(uint16_t write_adress,uint32_t vall)
{
        HAL_PWR_EnableBkUpAccess();
         __HAL_RCC_BKPRAM_CLK_ENABLE();
         *(__IO uint32_t*)(0x38800000 + write_adress) = vall ;
          SCB_CleanDCache_by_Addr((uint32_t *)(0x38800000 + write_adress),8);
         __HAL_RCC_BKPRAM_CLK_DISABLE();
         HAL_PWR_DisableBkUpAccess();
}

bool bkSRAM_Init(void)
{
   HAL_FLASH_Unlock();
    /*DBP : Enable access to Backup domain */
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_BKPRAM_CLK_ENABLE();
    /*BRE : Enable backup regulator
      BRR : Wait for backup regulator to stabilize */

    HAL_PWREx_EnableBkUpReg();
   /*DBP : Disable access to Backup domain */
    __HAL_RCC_BKPRAM_CLK_DISABLE();
    HAL_PWR_DisableBkUpAccess();
    HAL_FLASH_Lock();


   uint32_t magic_check = 0;
   bkSRAM_ReadVariable(0, &magic_check);
   if (BKPSRAMMAGIC != magic_check){
// init backup sram, 
      char buff[256] = {0};
      bkSRAM_WriteVariable( 0, BKPSRAMMAGIC);
      snprintf(buff, sizeof(buff), "state\\State1.48s");
      bkSRAM_WriteString(1, buff, sizeof(buff));
// ajouter la timezone
      return 0;
   }
   return 1;
}

/******************************************************************************
 * reading time from sntp server and converting it to local
 * from Claude
 */

// NTP epoch starts January 1, 1900
// Unix epoch starts January 1, 1970
// Difference is 70 years = 2208988800 seconds
#define NTP_UNIX_OFFSET 2208988800UL

// Timezone structure with DST periods
typedef struct {
    const char* name;
    const char* tz_string;
    int offset_hours;
    const char* description;
    const char* dst_period;
} timezone_info_t;


static const timezone_info_t timezones[] = {
    {"UTC", "UTC0", 0, "Coordinated Universal Time", "No DST"},
    {"GMT", "GMT0", 0, "Greenwich Mean Time", "No DST"},
    {"EST", "EST5EDT,M3.2.0,M11.1.0", -5, "Eastern Time (US/Canada)", "DST: Mar 2nd Sun - Nov 1st Sun"},
    {"CST", "CST6CDT,M3.2.0,M11.1.0", -6, "Central Time (US/Canada)", "DST: Mar 2nd Sun - Nov 1st Sun"},
    {"MST", "MST7MDT,M3.2.0,M11.1.0", -7, "Mountain Time (US/Canada)", "DST: Mar 2nd Sun - Nov 1st Sun"},
    {"PST", "PST8PDT,M3.2.0,M11.1.0", -8, "Pacific Time (US/Canada)", "DST: Mar 2nd Sun - Nov 1st Sun"},
    {"CET", "CET-1CEST,M3.5.0,M10.5.0/3", 1, "Central European Time", "DST: Mar Last Sun - Oct Last Sun"},
    {"JST", "JST-9", 9, "Japan Standard Time", "No DST"},
    {"AEST", "AEST-10AEDT,M10.1.0,M4.1.0/3", 10, "Australian Eastern Time", "DST: Oct 1st Sun - Apr 1st Sun"},
    {"IST", "IST-5:30", 5, "India Standard Time", "No DST"},
    {"BST", "GMT0BST,M3.5.0/1,M10.5.0", 0, "British Summer Time", "DST: Mar Last Sun - Oct Last Sun"},
    {"HST", "HST10", -10, "Hawaii Standard Time", "No DST"}
};

#define NUM_TIMEZONES (sizeof(timezones) / sizeof(timezones[0]))

// Days in each month (non-leap year, leap year)
static const uint8_t days_in_month[2][12] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},  // Non-leap year
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}   // Leap year
};

static const char* month_names[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const char* weekday_names[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};



// Function prototypes
//static uint8_t is_leap_year(uint16_t year);
//static uint32_t days_since_epoch(uint16_t year, uint8_t month, uint8_t day);
//static void unix_time_to_utc(uint32_t unix_time, local_time_t* time_struct);
//static void apply_timezone_offset(local_time_t* time_struct, const timezone_info_t* tz);
static void print_timezone_list(void);
static int find_timezone_index(const char* tz_name);

// Helper functions implementation
static uint8_t is_leap_year(uint16_t year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}


static void unix_time_to_utc(uint32_t unix_time, local_time_t* time_struct) {
    uint32_t days = unix_time / 86400;
    uint32_t seconds_today = unix_time % 86400;
    
    // Calculate weekday (January 1, 1970 was a Thursday = 4)
    time_struct->weekday = (days + 4) % 7;
    
    // Calculate time of day
    time_struct->hour = seconds_today / 3600;
    time_struct->minute = (seconds_today % 3600) / 60;
    time_struct->second = seconds_today % 60;
    
    // Calculate date (start from 1970)
    time_struct->year = 1970;
    
    while (1) {
        uint16_t days_this_year = is_leap_year(time_struct->year) ? 366 : 365;
        if (days < days_this_year) break;
        days -= days_this_year;
        time_struct->year++;
    }
    
    // Find month and day
    time_struct->month = 1;
    while (1) {
        uint8_t days_this_month = days_in_month[is_leap_year(time_struct->year)][time_struct->month - 1];
        if (days < days_this_month) break;
        days -= days_this_month;
        time_struct->month++;
    }
    
    time_struct->day = days + 1;
}

static uint32_t days_since_epoch(uint16_t year, uint8_t month, uint8_t day) {
    uint32_t days = 0;
    uint16_t y;
    uint8_t m;
    
    // Add days for complete years since 1970
    for (y = 1970; y < year; y++) {
        days += is_leap_year(y) ? 366 : 365;
    }
    
    // Add days for complete months in current year
    for (m = 1; m < month; m++) {
        days += days_in_month[is_leap_year(year)][m - 1];
    }
    
    // Add remaining days
    days += day - 1;
    
    return days;
}


static void apply_timezone_offset(local_time_t* time_struct, const timezone_info_t* tz) {
    // For embedded implementation, we'll use a simplified approach
    // This doesn't implement full TZ string parsing but handles the common cases
    
    int16_t offset_minutes;
    uint8_t is_dst = 0;
    
    // Calculate base offset in minutes
    if (strcmp(tz->name, "IST") == 0) {
        offset_minutes = 330; // +5:30
    } else {
        offset_minutes = tz->offset_hours * 60;
    }
    
    // Simple DST detection based on month for common zones
    // This is a simplified implementation - full TZ parsing would be complex
    if (strstr(tz->tz_string, "EDT") || strstr(tz->tz_string, "CDT") || 
        strstr(tz->tz_string, "MDT") || strstr(tz->tz_string, "PDT")) {
        // US timezones: DST from March to November (approximate)
        if (time_struct->month >= 3 && time_struct->month <= 10) {
            is_dst = 1;
            offset_minutes += 60; // Add 1 hour for DST
        }
    } else if (strstr(tz->tz_string, "CEST")) {
        // Central European Summer Time: DST from March to October (approximate)
        if (time_struct->month >= 3 && time_struct->month <= 10) {
            is_dst = 1;
            offset_minutes += 60; // Add 1 hour for DST
        }
    } else if (strstr(tz->tz_string, "AEDT")) {
        // Australian Eastern Daylight Time: DST from October to April (approximate)
        if (time_struct->month >= 10 || time_struct->month <= 4) {
            is_dst = 1;
            offset_minutes += 60; // Add 1 hour for DST
        }
    } else if (strstr(tz->tz_string, "BST")) {
        // British Summer Time: DST from March to October (approximate)
        if (time_struct->month >= 3 && time_struct->month <= 10) {
            is_dst = 1;
            offset_minutes += 60; // Add 1 hour for DST
        }
    }
    
    time_struct->is_dst = is_dst;
    
    // Apply offset
    int32_t total_minutes = time_struct->hour * 60 + time_struct->minute + offset_minutes;
    int32_t day_offset = 0;
    
    // Handle day boundary crossings
    while (total_minutes < 0) {
        total_minutes += 1440; // 24 * 60
        day_offset--;
    }
    while (total_minutes >= 1440) {
        total_minutes -= 1440;
        day_offset++;
    }
    
    time_struct->hour = total_minutes / 60;
    time_struct->minute = total_minutes % 60;
    
    // Adjust date if necessary
    if (day_offset != 0) {
        // This is simplified - for production code, you'd want more robust date arithmetic
        int16_t new_day = time_struct->day + day_offset;
        
        if (new_day < 1) {
            // Previous month
            time_struct->month--;
            if (time_struct->month < 1) {
                time_struct->month = 12;
                time_struct->year--;
            }
            time_struct->day = days_in_month[is_leap_year(time_struct->year)][time_struct->month - 1] + new_day;
        } else if (new_day > days_in_month[is_leap_year(time_struct->year)][time_struct->month - 1]) {
            // Next month
            new_day -= days_in_month[is_leap_year(time_struct->year)][time_struct->month - 1];
            time_struct->month++;
            if (time_struct->month > 12) {
                time_struct->month = 1;
                time_struct->year++;
            }
            time_struct->day = new_day;
        } else {
            time_struct->day = new_day;
        }
        
        // Recalculate weekday
        time_struct->weekday = (time_struct->weekday + day_offset) % 7;
        if (time_struct->weekday < 0) time_struct->weekday += 7;
    }
}
/*
// Main conversion function
void ntp_convert_to_local_time(const IP_NTP_TIMESTAMP* ntp_time, uint8_t tz_index, local_time_t* local_time) {
    uint32_t unix_time;
    
    // Bounds check
    if (tz_index >= NUM_TIMEZONES) {
        SEGGER_RTT_printf(0, "Error: Invalid timezone index %d\n", tz_index);
        return;
    }
    
    // Check for valid NTP time
    if (ntp_time->Seconds < NTP_UNIX_OFFSET) {
        SEGGER_RTT_printf(0, "Error: Invalid NTP time (before Unix epoch)\n");
        return;
    }
    
    // Convert NTP to Unix time
    unix_time = ntp_time->Seconds - NTP_UNIX_OFFSET;
    
    // Convert to UTC time structure
    unix_time_to_utc(unix_time, local_time);
    
    // Calculate milliseconds from NTP fractions
    local_time->millisecond = (uint16_t)((uint64_t)ntp_time->Fractions * 1000 / 0x100000000ULL);
    
    // Apply timezone offset
    apply_timezone_offset(local_time, &timezones[tz_index]);
}



// Display functions
void print_ntp_time_info(const IP_NTP_TIMESTAMP* ntp_time, uint8_t tz_index) {
    local_time_t local_time;
    
    if (tz_index >= NUM_TIMEZONES) {
        SEGGER_RTT_printf(0, "Error: Invalid timezone index\n");
        return;
    }
    
    SEGGER_RTT_printf(0, "=== NTP Time Conversion ===\n");
    SEGGER_RTT_printf(0, "NTP Time: %u.%08X\n", ntp_time->Seconds, ntp_time->Fractions);
    SEGGER_RTT_printf(0, "Unix Time: %u\n", ntp_time->Seconds - NTP_UNIX_OFFSET);
    SEGGER_RTT_printf(0, "Timezone: %s (%s)\n", 
                      timezones[tz_index].name, timezones[tz_index].description);
    SEGGER_RTT_printf(0, "DST Info: %s\n", timezones[tz_index].dst_period);
    
    ntp_convert_to_local_time(ntp_time, tz_index, &local_time);
    
    SEGGER_RTT_printf(0, "Local Time: %04d-%02d-%02d %02d:%02d:%02d.%03d\n",
                      local_time.year, local_time.month, local_time.day,
                      local_time.hour, local_time.minute, local_time.second,
                      local_time.millisecond);
    
    SEGGER_RTT_printf(0, "Day: %s, DST: %s\n", 
                      weekday_names[local_time.weekday],
                      local_time.is_dst ? "Active" : "Inactive");
    
    // Show current season info
    if (local_time.is_dst) {
        SEGGER_RTT_printf(0, "Season: Summer Time (+1 hour)\n");
    } else {
        if (strcmp(timezones[tz_index].dst_period, "No DST") == 0) {
            SEGGER_RTT_printf(0, "Season: Standard Time (no seasonal change)\n");
        } else {
            SEGGER_RTT_printf(0, "Season: Winter Time (standard offset)\n");
        }
    }
}
*/
void print_timezone_list(void) {
    SEGGER_RTT_printf(0, "\nAvailable Timezones:\n");
    SEGGER_RTT_printf(0, "===================\n");
    
    for (uint8_t i = 0; i < NUM_TIMEZONES; i++) {
        int hours = timezones[i].offset_hours;
        int minutes = 0;
        
        // Handle fractional hours like IST (+5:30)
        if (strcmp(timezones[i].name, "IST") == 0) {
            hours = 5;
            minutes = 30;
        }
        
        SEGGER_RTT_printf(0, "%2d. %s (%+03d:%02d) - %s\n", 
                          i, timezones[i].name, hours, minutes,
                          timezones[i].description);
        SEGGER_RTT_printf(0, "    %s\n\n", timezones[i].dst_period);
    }
}

int find_timezone_index(const char* tz_name) {
    for (uint8_t i = 0; i < NUM_TIMEZONES; i++) {
        if (strcmp(timezones[i].name, tz_name) == 0) {
            return i;
        }
    }
    return -1;
}


/*
void ntp_convert_to_local_time_TZ(const IP_NTP_TIMESTAMP* ntp_time,const char* tz_name, local_time_t* local_time) {

   int tz_index = find_timezone_index(tz_name);
   ntp_convert_to_local_time( ntp_time, tz_index, local_time) ;

}
*/

void set_rtc(local_time_t* local_time){


   RTC_TimeTypeDef      time_rtc;
   RTC_DateTypeDef      date_rtc;
time_rtc.Hours = local_time->hour;
time_rtc.Minutes = local_time->minute;
time_rtc.Seconds = local_time->second;
time_rtc.TimeFormat = 0;
date_rtc.Date = local_time->day;
date_rtc.Month = local_time->month;
date_rtc.WeekDay = local_time->weekday;
date_rtc.Year = local_time->year-2000;


/* Fill the struct tm utm
    utm.tm_sec  = time_rtc.Seconds;
    utm.tm_min  = time_rtc.Minutes;
    utm.tm_hour = time_rtc.Hours;
    utm.tm_mday = date_rtc.Date;
    utm.tm_mon  = date_rtc.Month - 1;   // struct tm month range: 0-11
    utm.tm_year = date_rtc.Year + 100;  // struct tm year = years since 1900 (assuming 2000-based RTC)
    utm.tm_wday = date_rtc.WeekDay % 7; // tm_wday: Sunday = 0, RTC: Monday = 1
*/

// toujours ensemble !!!!!!
HAL_StatusTypeDef res;
HAL_PWR_EnableBkUpAccess();
   res = HAL_RTC_SetDate(&hrtc, &date_rtc, RTC_FORMAT_BIN);
   res = HAL_RTC_SetTime(&hrtc, &time_rtc, RTC_FORMAT_BIN);

   
}

