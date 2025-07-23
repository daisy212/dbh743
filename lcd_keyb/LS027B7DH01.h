/* From claude, mainly
 * LS027B7DH01 Sharp Memory Display Driver, 
 * For STM32U585, SPI2, LSB First, Software VCOM
 * Static allocation with modified line tracking and batch DMA transfer

// SPI2 configuration in CubeMX or manually:
// - Mode: Master
// - NSS: Hardware Output (automatic CS control)
// - SPI_NSS_POLARITY_HIGH : CS actif haut
// - Data Size: 8 Bits
// - First Bit: LSB First  <-- Important!
// - Clock Polarity: Low
// - Clock Phase: 1 Edge
//   hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;		<-- fSCLK datasheet = 2Mhz = 16Mhz / 8
//   hspi2.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_03CYCLE;	<-- tsSPS datasheet = 3µs, delay sck after CS

//   HAL_SPI_MspInit() : initialisation SPI & DMA
//   handle_GPDMA1_Channel0.Init.Direction = DMA_MEMORY_TO_PERIPH;
//   handle_GPDMA1_Channel0.Init.SrcInc = DMA_SINC_INCREMENTED;

// Memory usage summary:
// - Framebuffer: 12,000 bytes (400x240 pixels)
// - DMA Buffer: 12,481 bytes (max: all 240 lines + headers + dummy)
// - Command Buffer: 2 bytes
// - Modified Lines Table: 240 bytes (in LCD_Handle_t)
// Total static allocation: ~24,723 bytes

 * ( 1 : pixel transparent,  0 : pixel noir)
 
 */

#ifndef LS027B7DH01_H
#define LS027B7DH01_H

//#include "BSP.h"
#include "db_hardware_def.h"

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
    extern "C" {
#endif


#define LCD_CS_Pin GPIO_PIN_11
#define LCD_CS_GPIO_Port GPIOE
#define LCD_OFF_Pin GPIO_PIN_13
#define LCD_OFF_GPIO_Port GPIOE



//#include <string.h>

/* LCD Specifications */
#define LCD_MESSAGE_LENGTH	32

#define LCD_WIDTH           400
#define LCD_HEIGHT          240
#define LCD_BYTES_PER_LINE  50  // 400 pixels / 8 bits per byte
#define LCD_TOTAL_BYTES     (LCD_HEIGHT * LCD_BYTES_PER_LINE)
#define LCD_LINE_HEADER_SIZE 2  // Command + Line number
#define LCD_LINE_TOTAL_SIZE (LCD_LINE_HEADER_SIZE + LCD_BYTES_PER_LINE)
#define LCD_MAX_DMA_SIZE    (LCD_HEIGHT * LCD_LINE_TOTAL_SIZE + 1) // All lines + final dummy

/* Command bits */
#define LCD_CMD_WRITE_LINE  0x01
#define LCD_CMD_VCOM        0x02
#define LCD_CMD_CLEAR_ALL   0x04

/* Driver Status */
typedef enum {
    LCD_OK = 0,
    LCD_ERROR,
    LCD_BUSY,
    LCD_TIMEOUT,
	LCD_DMA1,
	LCD_DMA2,
	LCD_LAST
} LCD_Status_t;


/* Driver Configuration */


typedef struct {
    SPI_HandleTypeDef *hspi;
    bool use_dma;
    uint32_t timeout_ms;
} LCD_Config_t;


/* Modified line tracking */
typedef struct {
    bool lines[LCD_HEIGHT];     // Track which lines are modified
    uint16_t first_modified;    // First modified line index
    uint16_t last_modified;     // Last modified line index
    uint16_t count;             // Number of modified lines
    bool has_changes;           // Any changes pending
} LCD_ModifiedLines_t;

/* Driver Handle */
typedef struct {
    LCD_Config_t config;
    LCD_ModifiedLines_t modified;
    bool vcom_state;
    volatile bool transfer_complete;
    bool initialized;
} LCD_Handle_t;

extern SPI_HandleTypeDef hspi4;

extern LCD_Handle_t hlcd;
//extern uint8_t lcd_framebuffer[LCD_TOTAL_BYTES];
extern const char LCD_status_Desc[LCD_LAST][LCD_MESSAGE_LENGTH];

/* Function Prototypes */
LCD_Status_t LCD_Sharp_Init(LCD_Handle_t *hlcd, bool use_dma);
LCD_Status_t LCD_DeInit(LCD_Handle_t *hlcd);
LCD_Status_t LCD_Clear(LCD_Handle_t *hlcd);
LCD_Status_t LCD_ClearFramebuffer(LCD_Handle_t *hlcd);
LCD_Status_t LCD_UpdateDisplay(LCD_Handle_t *hlcd);
LCD_Status_t LCD_UpdateModifiedLines(LCD_Handle_t *hlcd);
LCD_Status_t LCD_SetPixel(LCD_Handle_t *hlcd, uint16_t x, uint16_t y, int8_t pixel);
bool LCD_GetPixel(LCD_Handle_t *hlcd, uint16_t x, uint16_t y);
LCD_Status_t LCD_FillRect(LCD_Handle_t *hlcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, int8_t pixel);
void LCD_DrawLine(LCD_Handle_t *hlcd,uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, int8_t pixel);
void LCD_DrawRect(LCD_Handle_t *hlcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, int8_t pixel);
void draw_char_12x24(LCD_Handle_t *hlcd, uint16_t x, uint16_t y, char c, int8_t pixel);
void display_text_12x24(LCD_Handle_t *hlcd, uint16_t x, uint16_t y, const char* text, int8_t pixel);
void display_text_16x32(LCD_Handle_t *hlcd, uint16_t x, uint16_t y, const char* text, int8_t pixel);


void LCD_ToggleVCOM(LCD_Handle_t *hlcd);
LCD_Status_t LCD_WaitForTransfer(LCD_Handle_t *hlcd);
void LCD_MarkLineModified(LCD_Handle_t *hlcd, uint16_t line);
void LCD_MarkAllLinesModified(LCD_Handle_t *hlcd);
void LCD_ClearModifiedLines(LCD_Handle_t *hlcd);
uint16_t LCD_GetModifiedLinesCount(LCD_Handle_t *hlcd);

/* SPI Callback */
//void LCD_DMA_TxCpltCallback(DMA_HandleTypeDef *hdma);
//void LCD_SPI_TxCpltCallback(SPI_HandleTypeDef *spi);


/* Memory allocation functions */
uint8_t*	LCD_GetFramebuffer(void);
uint8_t*	LCD_GetDMABuffer(void);
bool*		LCD_GetModifiedLinesTable(void);

extern LCD_Handle_t hlcd;


#ifdef __cplusplus
   }
#endif


#endif /* LS027B7DH01_H */






/*

// old
// VCOM toggle function - call this periodically (e.g., in timer interrupt)
extern OS_TIMER	T_LcdVcom;
void Sharp_LCD_VCOM_Toggle(void) {
    uint8_t cmd = SHARP_LCD_CMD_VCOM | (vcom_state ? SHARP_LCD_CMD_VCOM : 0);
    
    Sharp_LCD_CS_High();
    HAL_SPI_Transmit(SHARP_LCD_SPI_HANDLE, &cmd, 1, HAL_MAX_DELAY);
    Sharp_LCD_CS_Low();
       BSP_ToggleLED(0);
	OS_TIMER_Restart(&T_LcdVcom);

    vcom_state = !vcom_state;
}

*/