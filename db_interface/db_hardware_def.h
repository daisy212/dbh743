#ifndef DB_INTERFACE_H
#define DB_INTERFACE_H


//#define db_u585 1
#define db_h743 1

#include "stdbool.h"
#include "stdint.h"
#include "RTOS.h"
#include <time.h>
#include "IP.h"
#include "SEGGER_RTT.h" // utilisation SEGGER_RTT_printf


#ifdef __cplusplus
    extern "C" {
#endif

typedef uint8_t  byte;
typedef unsigned int uint;

#if db_h743
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
/*
 * definition des e/s
*/
#define LED_Pin GPIO_PIN_3
#define LED_GPIO_Port GPIOE
#define SW2_Pin GPIO_PIN_13
#define SW2_GPIO_Port GPIOC
#define COL_1_Pin GPIO_PIN_0
#define COL_1_GPIO_Port GPIOA
#define COL_2_Pin GPIO_PIN_1
#define COL_2_GPIO_Port GPIOA
#define COL_3_Pin GPIO_PIN_2
#define COL_3_GPIO_Port GPIOA
#define COL_4_Pin GPIO_PIN_3
#define COL_4_GPIO_Port GPIOA
#define COL_5_Pin GPIO_PIN_4
#define COL_5_GPIO_Port GPIOA
#define COL_6_Pin GPIO_PIN_5
#define COL_6_GPIO_Port GPIOA
#define ROW_A_Pin GPIO_PIN_6
#define ROW_A_GPIO_Port GPIOA
#define ROW_B_Pin GPIO_PIN_7
#define ROW_B_GPIO_Port GPIOA
#define ROW_C_Pin GPIO_PIN_4
#define ROW_C_GPIO_Port GPIOC
#define ROW_D_Pin GPIO_PIN_5
#define ROW_D_GPIO_Port GPIOC
#define ROW_E_Pin GPIO_PIN_0
#define ROW_E_GPIO_Port GPIOB
#define ROW_F_Pin GPIO_PIN_1
#define ROW_F_GPIO_Port GPIOB
#define ROW_G_Pin GPIO_PIN_7
#define ROW_G_GPIO_Port GPIOE
#define ROW_H_Pin GPIO_PIN_8
#define ROW_H_GPIO_Port GPIOE
#define ROW_I_Pin GPIO_PIN_9
#define ROW_I_GPIO_Port GPIOE
/*  non utilisés directement */
#define LCD_CS_Pin GPIO_PIN_11
#define LCD_CS_GPIO_Port GPIOE
#define LCD_SCK_Pin GPIO_PIN_12
#define LCD_SCK_GPIO_Port GPIOE
#define LCD_OFF_Pin GPIO_PIN_13
#define LCD_OFF_GPIO_Port GPIOE
#define LCD_DI_Pin GPIO_PIN_14
#define LCD_DI_GPIO_Port GPIOE
#define SD_Card_Pin GPIO_PIN_4
#define SD_Card_GPIO_Port GPIOD




#elif db_u585

#include "stm32u5xx_hal.h"

#endif // db_h743

#define BKPSRAMMAGIC	(0x11d23346)

extern OS_MAILBOX      	Mb_Keyboard;
extern OS_EVENT			_EV_KEYB;
extern uint32_t 		Cnt_ms ; 

extern uint64_t	scrut_last;



typedef enum {
    blk_ok = 0,
    blk_lcd,
    blk_,
    blk_last,

} BLK_seq;



/* Gestion clavier
*/
extern const uint8_t DM_keyb[];
extern const char kb_fct_name[][3];

#define KB_ROW 8
#define KB_COL 6

/* 4 touches au max, limiter à 2 ? */
#define KB_MAX_KEY 4
#define KB_SHIFT_SCRUT 3

#define KB_SCRUT_PERIOD		(15)
#define KB_REPEAT_PERIOD	(1500)
#define KB_REPEAT_COUNT		(KB_REPEAT_PERIOD/KB_SCRUT_PERIOD+1)
#define KB_VALID_COUNT		(3)

#define KB_DB_FIRST_REPEAT	(1000)
#define KB_DB_FIRST_PERIOD	(100)



#define EV_KB_key 1


#define KB_SCRUT_KEY(x)		((x>0)&&(x<i_key_last)?((uint64_t)1<<(uint64_t)(x+KB_SHIFT_SCRUT)):((uint64_t)0))
#define KB_RD_KEY(x)		(((x>0)&&(x<9999))?((x%100)%i_key_last):0)
#define KB_RD_FCT(x)		(((x>0)&&(x<9999))?((x/100)%7):0)
#define KB_CHAR_ROW_KEY(x)	(((x>0)&&(x<9999))?((char)('A'+((x-1)%100)/6)):((char)'-'))
#define KB_CHAR_COL_KEY(x)	(((x>0)&&(x<9999))?((char)('1'+((x-1)%100)%6)):((char)'-'))
//#define KB_STRING_FCT_KEY(x)	((char*)(&kb_fct_name[KB_RD_FCT(x)][3]))
#define KB_FREE42_KEY(x)	((uint8_t)(DM_keyb[KB_RD_KEY(x)]))
#define KB_FCT_PRESS		1
#define KB_FCT_RELEASE		2
#define KB_FCT_REPEAT		3
#define KB_FCT_SHIFT		4
#define KB_FCT_SHIFT_REL	5
#define KB_FCT_SHIFT_REPEAT	6

enum key_map1
// keys name definition
{	// 
	kXX = 0,
	kA1,
	kA2,
	kA3,
	kA4,
	kA5,
	kA6,
	kB1,
	kB2,
	kB3,
	kB4,
	kB5,
	kB6,
	kC1,
	kC2,
	kC3,
	kC4,
	kC5,
	kC6,
	kD1,
	kD2,
	kD3,
	kD4,
	kD5,
	kD6,
	kE1,
	kE2,
	kE3,
	kE4,
	kE5,
	kE6,
	kF1,
	kF2,
	kF3,
	kF4,
	kF5,
	kF6,
	kG1,
	kG2,
	kG3,
	kG4,
	kG5,
	kG6,
	kH1,
	kH2,
	kH3,
	kH4,
	kH5,
	kH6,
#if KB_ROW==9
	kI1,
	kI2,
	kI3,
	kI4,
	kI5,
	kI6,
#endif
	i_key_last,
};



typedef	struct {
	GPIO_TypeDef * gpio;
	uint32_t pin;
}
st_Pin;

typedef	struct {
	uint8_t		key;
	uint32_t    count;
	bool	k_act;
	bool	k_long;
}
st_Key;


typedef struct {
  uint16_t year;
  uint8_t  month;
  uint8_t  day;
} dt_t;

typedef struct {
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  uint8_t csec;
  uint8_t dow;
} tm_t;

// Simple time structure for embedded use
typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
    uint8_t weekday;
    uint8_t is_dst;
} local_time_t;



void Ti_cb_Count_ms(void);
void blink_error(uint32_t errnb);


extern RTC_HandleTypeDef hrtc;



// gestion lcd





// ============================================================================
//
//   Quick and dirty interface with simulator user interface
//
// ============================================================================

typedef int (*file_sel_fn)(const char *fpath, const char *fname, void *data);


void      ui_refresh();
uint      ui_refresh_count();
void      ui_screenshot();
void      ui_push_key(int k);
void      ui_ms_sleep(uint delay);
int       ui_file_selector(const char *title,
                           const char *base_dir,
                           const char *ext,
                           file_sel_fn callback,
                           void       *data,
                           int         disp_new,
                           int         overwrite_check);
void      ui_save_setting(const char *name, const char *value);
size_t    ui_read_setting(const char *name, char *value, size_t maxlen);
uint      ui_battery();         // Between 0 and 1000
bool      ui_charging();        // On USB power

void      ui_start_buzzer(uint frequency);
void      ui_stop_buzzer();
void      ui_draw_message(const char *hdr);
/*
int       ui_wrap_io(file_sel_fn callback,
                     const char *path,
                     void       *data,
                     bool        writing);
*/
void      ui_load_keymap(const char *path);


void Keyb_Enable(void);


void rtc_read(tm_t * tm, dt_t *dt);
void rtc_write(tm_t * tm, dt_t *dt);



void Error_Handler(void);

bool bkSRAM_Init(void);
void bkSRAM_ReadString(uint16_t read_adress, char* read_data, uint32_t length);
void bkSRAM_WriteString(uint16_t read_adress, char* write_data, uint32_t length);
void bkSRAM_ReadVariable(uint16_t read_adress, uint32_t* read_data);
void bkSRAM_WriteVariable(uint16_t write_adress,uint32_t vall);


void ntp_convert_to_local_time_TZ(const IP_NTP_TIMESTAMP* ntp_time,const char* tz_name, local_time_t* local_time);
void set_rtc(local_time_t* local_time);




/*  lcd  */
/* LCD Specifications */




#ifdef __cplusplus
   }
#endif




#endif // DB_INTERFACE_H

