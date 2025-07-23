


#include "db_hardware_def.h"
#include "dmcp.h"
//#include "user_interface.h"

#include "SEGGER_RTT.h"
#include "RTOS.h"
#include "FS.h"
#include "FS_OS.h"



/* Mapping
 * send a uint32_t value 
 Key_1_allready_pressed<<24 | 
 Key_2_allready_pressed<<16 |
 Key_3_allready_pressed<<8 |
 key  +100 si releases

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


const uint8_t DM_keyb[i_key_last]=
// equivalence with DM & Free42
{
0,
KEY_EXIT,
KEY_0,
0,
KEY_DOT,  
KEY_RUN,  
KEY_ADD,  

KEY_SHIFT,
KEY_1,   
0,
KEY_2,   
KEY_3,   
KEY_SUB, 

KEY_DOWN,
KEY_4,     
0,
KEY_5,     
KEY_6,     
KEY_MUL,   

KEY_UP,    
KEY_7,     
0,
KEY_8,     
KEY_9,     
KEY_DIV,   

KEY_ENTER, 
0,
KEY_SWAP,  
KEY_CHS,   
KEY_E,     
KEY_BSP,   

KEY_STO,   
KEY_RCL,   
KEY_RDN,   
KEY_SIN,   
KEY_COS,   
KEY_TAN,   

KEY_SIGMA, 
KEY_INV,    
KEY_SQRT,   
KEY_LOG,    
KEY_LN,     
KEY_XEQ,   

KEY_F1,    
KEY_F2,    
KEY_F3,    
KEY_F4,    
KEY_F5,    
KEY_F6,    
};

const char kb_fct_name[][3]= {"- ", "p ", "r ", "t ", "s ", "sr", "st"};

#define ROL32(value, shift) (((value) << (shift)) | ((value) >> (32 - (shift))))
#define ROR32(value, shift) (((value) >> (shift)) | ((value) << (32 - (shift))))

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
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(COL_3_GPIO_Port, &GPIO_InitStruct);
	for (uint32_t i_col = 0; i_col < KB_COL; i_col++){
		GPIO_InitStruct.Pin = kbd_col[i_col].pin;
		HAL_GPIO_Init(kbd_col[i_col].gpio, &GPIO_InitStruct);
	}
// init interrupts
  /* EXTI interrupt init*/
/*	HAL_NVIC_SetPriority(COL_1_EXTI_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(COL_1_EXTI_IRQn);

	HAL_NVIC_SetPriority(COL_2_EXTI_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(COL_2_EXTI_IRQn);

	HAL_NVIC_SetPriority(COL_3_EXTI_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(COL_3_EXTI_IRQn);

	HAL_NVIC_SetPriority(COL_4_EXTI_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(COL_4_EXTI_IRQn);

	HAL_NVIC_SetPriority(COL_5_EXTI_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(COL_5_EXTI_IRQn);

	HAL_NVIC_SetPriority(COL_6_EXTI_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(COL_6_EXTI_IRQn);
*/

}


uint32_t keyb_corr(uint32_t key){
	if (((key > 41) &&(key <47))||
		((key > 52) &&(key <57))||
		((key > 62) &&(key <67))||
		((key > 72) &&(key <77))||
		((key > 82) &&(key <87))) return key -1;
	return key;
}



void KbdTask(void) 
/* scrutation clavier	v2
 * la répétition est gérée par db48x
 * envoie pression et relachement de chaque touche
 * 32 bits : trois touches pressées max dans les 3 octects supérieurs, 
 structure kbd_update : touche pressée une, deux, trois, 4éme octet : o.7=0 touche pressée, o.7=1 touche relachée 
 */
{
	bool key_pressed_counting = false;
	bool send_B1r = false;

	uint32_t key_time[KB_MAX_KEY] = {0};
	uint32_t key_value[KB_MAX_KEY] = {0};

	uint32_t	key_value_act = 0;
	uint64_t	scrut_act = 0;
	uint32_t	key_data_to_send = 0;
	uint32_t shift = 0;
	uint32_t k_tmp = 0;

// déplacé dans MainTask
//	KeyboardInit();
	
	while (1) {
	    OS_TASK_Delay(KB_SCRUT_PERIOD);
// dans nouvelle version :
// 30 sec avec KB_SCRUT_PERIOD, puis attendre une interruption

		scrut_act = 0;
		for (uint32_t i_row = 0 ; i_row < KB_ROW; i_row++){
			HAL_GPIO_WritePin(kbd_row[i_row].gpio, kbd_row[i_row].pin, GPIO_PIN_RESET);
			OS_TASK_Delay_us(10);
			
			for (uint32_t i_col = 0; i_col < KB_COL; i_col++){
				key_pressed_counting = false;
//				key_value_act = 1 + 6 * i_row + i_col;
// db 48x
				key_value_act = (8 - i_row)*10 + (i_col+1);

				if (HAL_GPIO_ReadPin(kbd_col[i_col].gpio, kbd_col[i_col].pin) == GPIO_PIN_RESET){
					scrut_act |= (uint64_t)1<< (uint64_t)( 6 * i_row + i_col + KB_SHIFT_SCRUT);	
					key_data_to_send = 0;
					shift = 0;
					k_tmp = 0;
					for (uint32_t ii = 0; ii < KB_MAX_KEY; ii++){
						if (key_value_act == key_value[ii]){ 
							key_time[ii]++;
							key_pressed_counting = true;
							if (key_time[ii] == KB_VALID_COUNT){
								k_tmp = keyb_corr(key_value[ii]);
							}
						}else 
						{
							if (key_time[ii] >= KB_VALID_COUNT){
								shift += 8;
								key_data_to_send |= (keyb_corr(key_value[ii])) << shift;
							}
						}
					}
					if 	(k_tmp)		{
						k_tmp |= key_data_to_send;				
						OS_MAILBOX_Put(&Mb_Keyboard, &k_tmp);		// nouvelle touche pressée					
					}
					if (key_pressed_counting == false){
						for (uint32_t ii = 0; ii < KB_MAX_KEY; ii++){
							if (key_time[ii] == 0) {
								key_value[ii] = key_value_act;
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
						if ((key_value_act == key_value[ii])&&
							(key_time[ii] >= KB_VALID_COUNT)){ 
							// envoyer un message, touche relachée
								k_tmp = keyb_corr(key_value_act) + 100;						
								key_time[ii] = 0;
								key_value[ii] = 0;
							}else 
							{
								if (key_time[ii] >= KB_VALID_COUNT){
									shift += 8;
									key_data_to_send |= (keyb_corr(key_value[ii])) << shift;
								}
							}
						}
					if 	(k_tmp)		{
						k_tmp |= key_data_to_send;				
						OS_MAILBOX_Put(&Mb_Keyboard, &k_tmp);		//  touche relachée					
					}
				}	
			}	
			HAL_GPIO_WritePin(kbd_row[i_row].gpio, kbd_row[i_row].pin, GPIO_PIN_SET);
		}
// critical section
		scrut_last = scrut_act;
// reset with Exit + F1 + F6
		if (scrut_act == 0x0004200000000008) 	{
			SEGGER_RTT_printf(0,  "\nF1 F6 EXIT : reboot");  
			k_tmp = 0xffffffff;
			OS_MAILBOX_Put(&Mb_Keyboard, &k_tmp);	

		    OS_TASK_Delay(1500);

			NVIC_SystemReset();
		}
	}
}

