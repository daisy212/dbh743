#include "db_hardware_def.h"
#include "dmcp.h"
//#include "user_interface.h"
#include "LS027B7DH01.h"

#include "SEGGER_RTT.h"
#include "RTOS.h"
#include "FS.h"
#include "FS_OS.h"

extern OS_EVENT        KBD_Event;

/* Mapping
 * send a uint32_t value 
 Key_1_allready_pressed<<8 | 
 Key_2_allready_pressed<<16 |
 Key_3_allready_pressed<<24 |
 key  +100 si release

    KB_F1,      11,
    KB_F2,      12,
    KB_F3,      13,
    KB_F4,      14,
    KB_F5,      15,
    KB_F6,      16,

    KB_A,       21,
    KB_B,       22,
    KB_C,       23,
    KB_D,       24,
    KB_E,       25,
    KB_F,       26,

    KB_G,       31,
    KB_H,       32,
    KB_I,       33,
    KB_J,       34,
    KB_K,       35,
    KB_L,       36,

    KB_ENT,     41,
    KB_M,       42,
    KB_N,       43,
    KB_O,       44,
    KB_BKS,     45,

    KB_UP,      51,
    KB_P,       52,
    KB_Q,       53,
    KB_R,       54,
    KB_S,       55,

    KB_DN,      61,
    KB_T,       62,
    KB_U,       63,
    KB_V,       64,
    KB_W,       65,

    KB_SHIFT,   71,
    KB_X,       72,
    KB_Y,       73,
    KB_Z,       74,
    KB_SUB,     75,


    KB_ON,      81,
    KB_0,       82,
    KB_DOT,     83,
    KB_SPC,     84,
    KB_ADD,     85
*/

const st_Pin kbd_row[]=
// defition of pins raw, outputs, low = scrutation
{
   { ROW_A_GPIO_Port, ROW_A_Pin},
   { ROW_B_GPIO_Port, ROW_B_Pin},
   { ROW_C_GPIO_Port, ROW_C_Pin},
   { ROW_D_GPIO_Port, ROW_D_Pin},
   { ROW_E_GPIO_Port, ROW_E_Pin},
   { ROW_F_GPIO_Port, ROW_F_Pin},
   { ROW_G_GPIO_Port, ROW_G_Pin},
   { ROW_H_GPIO_Port, ROW_H_Pin},
   { ROW_I_GPIO_Port, ROW_I_Pin}
};

const st_Pin kbd_col[]=
// definition of pins col, inputs, (with interrupts)
{
   { COL_1_GPIO_Port, COL_1_Pin},
   { COL_2_GPIO_Port, COL_2_Pin},
   { COL_3_GPIO_Port, COL_3_Pin},
   { COL_4_GPIO_Port, COL_4_Pin},
   { COL_5_GPIO_Port, COL_5_Pin},
   { COL_6_GPIO_Port, COL_6_Pin},
};

uint32_t RTT_Key_Decode( int key){
   switch (key){
   
   case 0x0a:
      return 41;
   case 0x2b:
      return 85;
   case 0x2d:
      return 75;
   case 0x2a:
      return 65;
   case 0x2f:
      return 55;

   case 0x2e:
      return 83;
   case 0x30:
      return 82;
   case 0x31:
      return 72;
   case 0x32:
      return 73;

   case 0x33:
      return 74;
   case 0x34:
      return 62;
   case 0x35:
      return 63;
   case 0x36:
      return 64;
   case 0x37:
      return 52;
   case 0x38:
      return 53;
   case 0x39:
      return 54;


   case 0x41:
      return 21;
   case 0x42:
      return 22;
   case 0x43:
      return 23;
   case 0x44:
      return 24;
   case 0x45:
      return 25;
   case 0x46:
      return 26;
   case 0x47:
      return 31;
   case 0x48:
      return 32;
   case 0x49:
      return 33;
   case 0x4a:
      return 34;
   case 0x4b:
      return 35;
   case 0x4c:
      return 36;

   case 0x4d:
      return 42;
   case 0x4e:
      return 43;
   case 0x4f:
      return 44;


   
   }
   return 0;
}


void KeyboardInit(void)
/* pin initialisation, interrupts for column */
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* GPIO Ports Clock Enable */
   __HAL_RCC_GPIOA_CLK_ENABLE();
   __HAL_RCC_GPIOB_CLK_ENABLE();
   __HAL_RCC_GPIOC_CLK_ENABLE();
   __HAL_RCC_GPIOD_CLK_ENABLE();
   __HAL_RCC_GPIOE_CLK_ENABLE();
   __HAL_RCC_GPIOF_CLK_ENABLE();
   __HAL_RCC_GPIOH_CLK_ENABLE();
// init row
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   for (uint32_t i_row = 0 ; i_row < KB_ROW; i_row++){
      GPIO_InitStruct.Pin = kbd_row[i_row].pin;
      HAL_GPIO_WritePin(kbd_row[i_row].gpio, kbd_row[i_row].pin, GPIO_PIN_SET);
      HAL_GPIO_Init(kbd_row[i_row].gpio, &GPIO_InitStruct);
   }

// init col
   GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
   GPIO_InitStruct.Pull = GPIO_PULLUP;
   HAL_GPIO_Init(COL_3_GPIO_Port, &GPIO_InitStruct);
   for (uint32_t i_col = 0; i_col < KB_COL; i_col++){
      GPIO_InitStruct.Pin = kbd_col[i_col].pin;
      HAL_GPIO_Init(kbd_col[i_col].gpio, &GPIO_InitStruct);
   }
// init interrupts
  /* EXTI interrupt init*/
   HAL_NVIC_SetPriority(COL_1_EXTI_IRQn, 12, 0);
   HAL_NVIC_DisableIRQ(COL_1_EXTI_IRQn);

   HAL_NVIC_SetPriority(COL_2_EXTI_IRQn, 12, 0);
   HAL_NVIC_DisableIRQ(COL_2_EXTI_IRQn);

   HAL_NVIC_SetPriority(COL_3_EXTI_IRQn, 12, 0);
   HAL_NVIC_DisableIRQ(COL_3_EXTI_IRQn);

   HAL_NVIC_SetPriority(COL_4_EXTI_IRQn, 12, 0);
   HAL_NVIC_DisableIRQ(COL_4_EXTI_IRQn);

   HAL_NVIC_SetPriority(COL_5_EXTI_IRQn, 12, 0);
   HAL_NVIC_DisableIRQ(COL_5_EXTI_IRQn);

   HAL_NVIC_SetPriority(COL_6_EXTI_IRQn, 12, 0);
   HAL_NVIC_DisableIRQ(COL_6_EXTI_IRQn);
}

uint32_t keyb_corr(uint32_t key)
/* keyboard correction for missing keys in the 8*6 matrix */
{
   if (((key > 41) &&(key <47))||
      ((key > 52) &&(key <57))||
      ((key > 62) &&(key <67))||
      ((key > 72) &&(key <77))||
      ((key > 82) &&(key <87))) return key -1;
   return key;
}

typedef  enum {
   kb_scrut_std,
   kb_scrut_int,
   kb_scrut_int_exit
}keyb_mode;


typedef struct {
   uint32_t key_time[KB_MAX_KEY];
   uint32_t key_value[KB_MAX_KEY];
   uint32_t sleeping_soon;
   uint32_t c_sec;

   uint64_t raw;
   keyb_mode mode;
} keyboard;

keyboard keybd;


void Kbd_Scrut_Set(keyb_mode mode)
// mode kb_scrut, no interrupt, one row at low level at a time, normally all to high
{
   switch(mode){
      case    kb_scrut_std:
         // wake up from interrupt, normal scrutation
         HAL_NVIC_DisableIRQ(COL_1_EXTI_IRQn);
         HAL_NVIC_DisableIRQ(COL_2_EXTI_IRQn);
         HAL_NVIC_DisableIRQ(COL_3_EXTI_IRQn);
         HAL_NVIC_DisableIRQ(COL_4_EXTI_IRQn);
         HAL_NVIC_DisableIRQ(COL_5_EXTI_IRQn);
         HAL_NVIC_DisableIRQ(COL_6_EXTI_IRQn);
         for (uint32_t i_row = 0 ; i_row < KB_ROW; i_row++){
            HAL_GPIO_WritePin(kbd_row[i_row].gpio, kbd_row[i_row].pin, GPIO_PIN_SET);
         }
         break;
      case    kb_scrut_int:
         // allow interrupts
         __HAL_GPIO_EXTI_CLEAR_IT(COL_1_Pin);
         __HAL_GPIO_EXTI_CLEAR_IT(COL_2_Pin);
         __HAL_GPIO_EXTI_CLEAR_IT(COL_3_Pin);
         __HAL_GPIO_EXTI_CLEAR_IT(COL_4_Pin);
         __HAL_GPIO_EXTI_CLEAR_IT(COL_5_Pin);
         __HAL_GPIO_EXTI_CLEAR_IT(COL_6_Pin);
         OS_TASK_Delay_us(150);
         HAL_NVIC_EnableIRQ(COL_1_EXTI_IRQn);
         HAL_NVIC_EnableIRQ(COL_2_EXTI_IRQn);
         HAL_NVIC_EnableIRQ(COL_3_EXTI_IRQn);
         HAL_NVIC_EnableIRQ(COL_4_EXTI_IRQn);
         HAL_NVIC_EnableIRQ(COL_5_EXTI_IRQn);
         HAL_NVIC_EnableIRQ(COL_6_EXTI_IRQn);
         OS_TASK_Delay_us(150);        // purge all pending interrupts
         OS_EVENT_Reset(&KBD_Event);   // purge event 
         for (uint32_t i_row = 0 ; i_row < KB_ROW; i_row++){
            HAL_GPIO_WritePin(kbd_row[i_row].gpio, kbd_row[i_row].pin, GPIO_PIN_RESET);
         }
         break;
      case    kb_scrut_int_exit:
         __HAL_GPIO_EXTI_CLEAR_IT(COL_1_Pin);
         OS_TASK_Delay_us(150);
         HAL_NVIC_EnableIRQ(COL_1_EXTI_IRQn);
         HAL_NVIC_DisableIRQ(COL_2_EXTI_IRQn);
         HAL_NVIC_DisableIRQ(COL_3_EXTI_IRQn);
         HAL_NVIC_DisableIRQ(COL_4_EXTI_IRQn);
         HAL_NVIC_DisableIRQ(COL_5_EXTI_IRQn);
         HAL_NVIC_DisableIRQ(COL_6_EXTI_IRQn);
         HAL_GPIO_WritePin(kbd_row[0].gpio, kbd_row[0].pin, GPIO_PIN_RESET);
         for (uint32_t i_row = 1 ; i_row < KB_ROW; i_row++){
            HAL_GPIO_WritePin(kbd_row[i_row].gpio, kbd_row[i_row].pin, GPIO_PIN_SET);
         }
         OS_TASK_Delay_us(150);        // purge all pending interrupts
         OS_EVENT_Reset(&KBD_Event);   // purge event 
      break;
   }
}


uint64_t Scrutation(keyboard *p_kbd)
// scrutation clavier
{
st_key_data dts;

   bool key_pressed_counting = false;
   bool send_B1r = false;
   uint32_t key_value_act = 0;
//   uint32_t key_data_to_send = 0;
   uint32_t shift = 0;
   p_kbd->raw = 0;
   for (uint32_t i_row = 0 ; i_row < KB_ROW; i_row++){
      HAL_GPIO_WritePin(kbd_row[i_row].gpio, kbd_row[i_row].pin, GPIO_PIN_RESET);
      OS_TASK_Delay_us(10);
      for (uint32_t i_col = 0; i_col < KB_COL; i_col++){
         key_pressed_counting = false;
         key_value_act = (8 - i_row)*10 + (i_col+1);
         if (HAL_GPIO_ReadPin(kbd_col[i_col].gpio, kbd_col[i_col].pin) == GPIO_PIN_RESET)
         { // key pressed
            p_kbd->sleeping_soon = 0;
            p_kbd->raw |= (uint64_t)1<< (uint64_t)( 6 * i_row + i_col + KB_SHIFT_SCRUT);   
//            key_data_to_send = 0;
            shift = 0;
            dts.sys =0;
            dts.keys.key = 0;
            dts.keys.key1 =0;
            dts.keys.key2 =0;
            dts.keys.key3 =0;
            for (uint32_t ii = 0; ii < KB_MAX_KEY; ii++){
               if (key_value_act == p_kbd->key_value[ii]){ 
                  p_kbd->key_time[ii]++;
                  key_pressed_counting = true;
                  if (p_kbd->key_time[ii] == KB_VALID_COUNT){
                     dts.keys.key = keyb_corr(p_kbd->key_value[ii]);
                  }
               }else 
               {
                  if (p_kbd->key_time[ii] >= KB_VALID_COUNT){
                     shift += 8;
//                     key_data_to_send |= (keyb_corr(p_kbd->key_value[ii])) << shift;
                     if (8 == shift) dts.keys.key1 = keyb_corr(p_kbd->key_value[ii]);
                     if (16 == shift) dts.keys.key2 = keyb_corr(p_kbd->key_value[ii]);
                     if (24 == shift) dts.keys.key3 = keyb_corr(p_kbd->key_value[ii]);
                  }
               }
            }
            if    (dts.keys.key)     {
               dts.keys.released = 0;      
               OS_MAILBOX_Put(&Mb_Keyboard, &dts);     // nouvelle touche pressée             
            }
            if (key_pressed_counting == false){
               for (uint32_t ii = 0; ii < KB_MAX_KEY; ii++){
                  if (p_kbd->key_time[ii] == 0) {
                     p_kbd->key_value[ii] = key_value_act;
                     break;   
                  } 
               }
            }           
         }else 
         { // key not pressed
            dts.sys =0;
            dts.keys.key =0;
            dts.keys.key1 =0;
            dts.keys.key2 =0;
            dts.keys.key3 =0;
            shift = 0;
            for (uint32_t ii = 0; ii < KB_MAX_KEY; ii++){
               if ((key_value_act == p_kbd->key_value[ii])&&
                  (p_kbd->key_time[ii] >= KB_VALID_COUNT)){ 
                  // envoyer un message, touche relachée
                     dts.keys.key = keyb_corr(key_value_act);
                     dts.keys.released = 1;
                     p_kbd->key_time[ii] = 0;
                     p_kbd->key_value[ii] = 0;
                  }else 
                  {
                     if (p_kbd->key_time[ii] >= KB_VALID_COUNT){
                        shift += 8;
//                        key_data_to_send |= (keyb_corr(p_kbd->key_value[ii])) << shift;
                        if (8 == shift) dts.keys.key1 = keyb_corr(p_kbd->key_value[ii]);
                        if (16 == shift) dts.keys.key2 = keyb_corr(p_kbd->key_value[ii]);
                        if (24 == shift) dts.keys.key3 = keyb_corr(p_kbd->key_value[ii]);
                     }
                  }
               }
            if    (dts.keys.key)     {
               OS_MAILBOX_Put(&Mb_Keyboard, &dts);     //  touche relachée                             
            }
         }  
      }  
      HAL_GPIO_WritePin(kbd_row[i_row].gpio, kbd_row[i_row].pin, GPIO_PIN_SET);
   }
   return p_kbd->raw;
}

uint64_t Scrutation_old(keyboard *p_kbd)
// scrutation clavier
{
   uint64_t scrut_act = 0;
   bool key_pressed_counting = false;
   bool send_B1r = false;
   uint32_t key_value_act = 0;
   uint32_t key_data_to_send = 0;
   uint32_t shift = 0;
   uint32_t k_tmp = 0;
   for (uint32_t i_row = 0 ; i_row < KB_ROW; i_row++){
      HAL_GPIO_WritePin(kbd_row[i_row].gpio, kbd_row[i_row].pin, GPIO_PIN_RESET);
      OS_TASK_Delay_us(10);
      for (uint32_t i_col = 0; i_col < KB_COL; i_col++){
         key_pressed_counting = false;
         key_value_act = (8 - i_row)*10 + (i_col+1);
         if (HAL_GPIO_ReadPin(kbd_col[i_col].gpio, kbd_col[i_col].pin) == GPIO_PIN_RESET){
            p_kbd->sleeping_soon = 0;
            scrut_act |= (uint64_t)1<< (uint64_t)( 6 * i_row + i_col + KB_SHIFT_SCRUT);   
            key_data_to_send = 0;
            shift = 0;
            k_tmp = 0;
            for (uint32_t ii = 0; ii < KB_MAX_KEY; ii++){
               if (key_value_act == p_kbd->key_value[ii]){ 
                  p_kbd->key_time[ii]++;
                  key_pressed_counting = true;
                  if (p_kbd->key_time[ii] == KB_VALID_COUNT){
                     k_tmp = keyb_corr(p_kbd->key_value[ii]);
                  }
               }else 
               {
                  if (p_kbd->key_time[ii] >= KB_VALID_COUNT){
                     shift += 8;
                     key_data_to_send |= (keyb_corr(p_kbd->key_value[ii])) << shift;
                  }
               }
            }
            if    (k_tmp)     {
               k_tmp |= key_data_to_send;          
               OS_MAILBOX_Put(&Mb_Keyboard, &k_tmp);     // nouvelle touche pressée             
            }
            if (key_pressed_counting == false){
               for (uint32_t ii = 0; ii < KB_MAX_KEY; ii++){
                  if (p_kbd->key_time[ii] == 0) {
                     p_kbd->key_value[ii] = key_value_act;
                     break;   
                  } 
               }
            }           
         }else 
         { // touche non pressée 
            key_data_to_send = 0;
            k_tmp = 0;
            shift = 0;
            for (uint32_t ii = 0; ii < KB_MAX_KEY; ii++){
               if ((key_value_act == p_kbd->key_value[ii])&&
                  (p_kbd->key_time[ii] >= KB_VALID_COUNT)){ 
                  // envoyer un message, touche relachée
                     k_tmp = keyb_corr(key_value_act) + 100;                  
                     p_kbd->key_time[ii] = 0;
                     p_kbd->key_value[ii] = 0;
                  }else 
                  {
                     if (p_kbd->key_time[ii] >= KB_VALID_COUNT){
                        shift += 8;
                        key_data_to_send |= (keyb_corr(p_kbd->key_value[ii])) << shift;
                     }
                  }
               }
            if    (k_tmp)     {
               k_tmp |= key_data_to_send;          
               OS_MAILBOX_Put(&Mb_Keyboard, &k_tmp);     //  touche relachée              
            }
         }  
      }  
      HAL_GPIO_WritePin(kbd_row[i_row].gpio, kbd_row[i_row].pin, GPIO_PIN_SET);
   }
   return scrut_act;
}

#define T_AUTOPOWEROFF (1000*60*3)

void Send_key(uint8_t key){
   st_key_data dts;
   dts.sys = 0;

   dts.keys.key = key;
   dts.keys.key1 = 0;
   dts.keys.key2 = 0;
   dts.keys.key3 = 0;
   dts.keys.released = 0;
   OS_MAILBOX_Put(&Mb_Keyboard, &dts); // press             
   dts.keys.released = 1;
   OS_MAILBOX_Put(&Mb_Keyboard, &dts); // release        
}

void KbdTask(void) 
/* keyboard and scheduler task
 * key repetition by db48x
 * send struct st_key_data.
 * with data.sys = 1 : system event : second, minute, Reset...
 * with data.sys = 0 : key event
 * active key : bool released, int key, allready pressed : key1, key2, key3 

 * using db_power_state
 */
{
// moved in MainTask non 
// 
// KeyboardInit();
   st_key_data dts;
   char result = 0;

   for (uint32_t ii = 0; ii < KB_MAX_KEY; ii++){
      keybd.key_value[ii] = 0; 
      keybd.key_time[ii] = 0;
   }
   while(1) 
   {
      switch (db_power_state){
         case PW_wake_up:
            // wait for [exit] release
            #if DBh743
            while (HAL_GPIO_ReadPin(kbd_col[0].gpio, kbd_col[0].pin) == 0)
            {
               OS_TASK_Delay( KB_SCRUT_PERIOD);
            }
            #endif
            SEGGER_RTT_printf(0, "\nPW : wake up");
            OS_TASK_Delay( KB_SCRUT_PERIOD);
            // normal scrutation
            Kbd_Scrut_Set(kb_scrut_std);
            // check if special cmd at power on
            dts.sys = 1;
            dts.sys_cmd = SYS_WAKE_UP;
            OS_MAILBOX_Put(&Mb_Keyboard, &dts);  
            db_power_state = PW_running;
            break;

// add power wake up confirmation from db ???

         case PW_running:
            OS_TASK_Delay(KB_SCRUT_PERIOD);
            keybd.sleeping_soon +=KB_SCRUT_PERIOD ;
            scrut_last = Scrutation( &keybd);
            if (keybd.sleeping_soon > 30000) {
               db_power_state = PW_sleeping;
               Kbd_Scrut_Set(kb_scrut_int);
               SEGGER_RTT_printf(0, "\nPW_Entering sleep");
            }
            break;
         case PW_sleeping:
            result = OS_EVENT_GetTimed(&KBD_Event, 1000*60); 
            if (result ==0) {
               // going back to PW_running
               // wake up from interrupt, normal scrutation
               Kbd_Scrut_Set(kb_scrut_std);
               keybd.sleeping_soon = 0;
               db_power_state = PW_running;
            }
            else {
               // sending one minute
               dts.sys = 1;
               dts.sys_cmd = SYS_1mn;
               OS_MAILBOX_Put(&Mb_Keyboard, &dts);  
               keybd.sleeping_soon += 60000;
            }
            if (keybd.sleeping_soon > T_AUTOPOWEROFF){
               Send_key(71);  // [shift]
               Send_key(81);  // [exit]
               // set by sysmenu.cc
               // db_power_state = PW_near_deep_sleep;
            }
            break;
         case PW_near_deep_sleep:
            OS_TASK_Delay(250);
            const char img_name[] = {"\\img\\circles-complex-offline.bmp"};
            //const char img_name[] = {"\\img\\space.bmp"};
            LCD_disp_bmp(img_name);
            // shutting clocks
            SEGGER_RTT_printf(0, "\nPW : Shutting down");

//            dts.sys = 1;
//            dts.sys_cmd = SYS_P_OFF;
//            OS_MAILBOX_Put(&Mb_Keyboard, &dts);  
            
            Kbd_Scrut_Set(kb_scrut_int_exit);   // wake up only with [exit] key A1
            db_power_state = PW_deep_sleep;
            break;

         case PW_deep_sleep:
            OS_EVENT_Reset(&KBD_Event);
            OS_EVENT_GetBlocked(&KBD_Event);
            db_power_state = PW_wake_up;
            keybd.sleeping_soon = 0;
            break;
       }
      if ( keybd.sleeping_soon % 1000 >= 1000-KB_SCRUT_PERIOD) 
      // sending sys 1sec         
      {
         dts.sys = 1;
         dts.sys_cmd = SYS_1sec;
         OS_MAILBOX_Put(&Mb_Keyboard, &dts);  
      }

      if (scrut_last == 0x0004200000000008)   
      // reset with Exit + F1 + F6         
      {
         SEGGER_RTT_printf(0,  "\nF1 F6 EXIT : reboot");  
         dts.sys = 1;
         dts.sys_cmd = SYS_RESET;
         OS_MAILBOX_Put(&Mb_Keyboard, &dts);  
         OS_TASK_Delay(1500);
         NVIC_SystemReset();
      }

      // receiving keys from segger j-link 
      int key = SEGGER_RTT_GetKey();
      if (key >= 0) {
         keybd.sleeping_soon = 0;
       // Key was pressed, 'key' contains the ASCII value          
         uint32_t db_key_from_RTT = RTT_Key_Decode(key);
         if  (db_key_from_RTT != 0) {
            Send_key(db_key_from_RTT);
         } else  {
            SEGGER_RTT_printf(0, "\nYou pressed: %c (0x%02X)", key, key);
         }
      } 
   }
}

void KbdTask_old(void) 
/* keyboard and scheduler task
 * key repetition by db48x
 * send struct st_key_data.
 * with data.sys = 1 : system event : second, minute, Reset...
 * with data.sys = 0 : key event
 * active key : bool released, int key, allready pressed : key1, key2, key3 
 */
{
// moved in MainTask non 
// 
// KeyboardInit();
   st_key_data dts;

   for (uint32_t ii = 0; ii < KB_MAX_KEY; ii++){
      keybd.key_value[ii] = 0; 
      keybd.key_time[ii] = 0;
   }
   while(1) 
   {
      while ( keybd.sleeping_soon < 30000) {
         OS_TASK_Delay(KB_SCRUT_PERIOD);
         keybd.sleeping_soon +=KB_SCRUT_PERIOD ;
         scrut_last = Scrutation( &keybd);
//         if (__builtin_popcount(scrut_last) >=3)
//            SEGGER_RTT_printf(0,  "\nKey 3x : %08X %08X", (scrut_last>>32), (scrut_last & 0xffffffff) );  


         if (scrut_last == 0x0004200000000008)   
// reset with Exit + F1 + F6         
         {
            SEGGER_RTT_printf(0,  "\nF1 F6 EXIT : reboot");  
            dts.sys = 1;
            dts.sys_cmd = SYS_RESET;
            OS_MAILBOX_Put(&Mb_Keyboard, &dts);  
            OS_TASK_Delay(1500);
            NVIC_SystemReset();
         }

// receiving keys from segger j-link 
         int key = SEGGER_RTT_GetKey();
         if (key >= 0) {
            keybd.sleeping_soon = 0;
          // Key was pressed, 'key' contains the ASCII value          
            uint32_t db_key_from_RTT = RTT_Key_Decode(key);
            dts.sys = 0;
            dts.keys.key = db_key_from_RTT;
            dts.keys.key1 = 0;
            dts.keys.key2 = 0;
            dts.keys.key3 = 0;

            if  (db_key_from_RTT != 0) {
               dts.keys.released = 0;
               OS_MAILBOX_Put(&Mb_Keyboard, &dts); // press             
               dts.keys.released = 1;
               OS_MAILBOX_Put(&Mb_Keyboard, &dts); // release        
            } else  {
               SEGGER_RTT_printf(0, "\nYou pressed: %c (0x%02X)", key, key);
            }
         } 

         if ( keybd.sleeping_soon % 1000 >= 1000-KB_SCRUT_PERIOD) 
// sending sys 1sec         
         {
            dts.sys = 1;
            dts.sys_cmd = SYS_1sec;
            OS_MAILBOX_Put(&Mb_Keyboard, &dts);  
         }
      }

// sleep, waiting interrupt
      Kbd_Scrut_Set(kb_scrut_int);
      SEGGER_RTT_printf(0, "\nEntering sleep");
      char result = 0;
      for (uint32_t ii =0; ii<10;ii++){
         result = OS_EVENT_GetTimed(&KBD_Event, 1000*60); 
         if (result ==0) break;
         else { 
// sending one minute
            dts.sys = 1;
            dts.sys_cmd = SYS_1mn;
            OS_MAILBOX_Put(&Mb_Keyboard, &dts);  
         }
      }
      if (result != 0){
// power off
         SEGGER_RTT_printf(0, "\nPower Off");
         dts.sys = 1;
         dts.sys_cmd = SYS_P_OFF;
         OS_MAILBOX_Put(&Mb_Keyboard, &dts);  

         // wake up only with [exit] key A1
         Kbd_Scrut_Set(kb_scrut_int_exit);
// to do : shutting clocks
         OS_EVENT_Reset(&KBD_Event);
         OS_EVENT_GetBlocked(&KBD_Event);
      }

      keybd.sleeping_soon = 0;
      SEGGER_RTT_printf(0, "\nWake-up");
// wait for [exit] release
      while (HAL_GPIO_ReadPin(kbd_col[0].gpio, kbd_col[0].pin) == 0)
      {
         OS_TASK_Delay( KB_SCRUT_PERIOD);
      }
      OS_TASK_Delay( KB_SCRUT_PERIOD);
// wake up from interrupt, normal scrutation
      Kbd_Scrut_Set(kb_scrut_std);
// check if special cmd at power on
      dts.sys = SYS_WAKE_UP;
      dts.sys_cmd = SYS_P_OFF;
      OS_MAILBOX_Put(&Mb_Keyboard, &dts);  
   }
}

// interrupts
void EXTI0_IRQHandler(void){
   HAL_GPIO_EXTI_IRQHandler(COL_1_Pin);
   OS_INT_Enter();
   OS_EVENT_Set(&KBD_Event);
   OS_INT_Leave();
}
void EXTI1_IRQHandler(void){
   HAL_GPIO_EXTI_IRQHandler(COL_2_Pin);
   OS_INT_Enter();
   OS_EVENT_Set(&KBD_Event);
   OS_INT_Leave();
}
void EXTI2_IRQHandler(void){
   HAL_GPIO_EXTI_IRQHandler(COL_3_Pin);
   OS_INT_Enter();
   OS_EVENT_Set(&KBD_Event);
   OS_INT_Leave();
}
void EXTI3_IRQHandler(void){
   HAL_GPIO_EXTI_IRQHandler(COL_4_Pin);
   OS_INT_Enter();
   OS_EVENT_Set(&KBD_Event);
   OS_INT_Leave();
}
void EXTI4_IRQHandler(void){
   HAL_GPIO_EXTI_IRQHandler(COL_5_Pin);
   OS_INT_Enter();
   OS_EVENT_Set(&KBD_Event);
   OS_INT_Leave();
}
void EXTI9_5_IRQHandler(void){
   HAL_GPIO_EXTI_IRQHandler(COL_6_Pin);
   OS_INT_Enter();
   OS_EVENT_Set(&KBD_Event);
   OS_INT_Leave();
}

