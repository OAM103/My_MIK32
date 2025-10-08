#include "touchpad.h"
#include "lcd.h"
#include "system_config.h"
#include "mik32_hal.h"
#include "mik32_hal_spi.h"
#include "xprintf.h"

//extern SPI_HandleTypeDef hspi0;
//extern SPI_HandleTypeDef hspi1;

#define READ_X  0xD0
#define READ_Y  0x90

int read_coordinate(unsigned char command) {
    int16_t value;

    // Подготовка пакета (Команда + 2 байта для приема)
    uint8_t master_output[] = {command, 0x00, 0x00}; //команда  и 2 байта для приёма данных. Тачпад отправляет12-битные координваты, упакованные в 2 байта
    uint8_t master_input[3] = {0}; // Инициализируем нулями

    // Обмен данными по SPI (Блокирующий режим)
    HAL_StatusTypeDef SPI_Status = HAL_SPI_Exchange(&hspi1, master_output, master_input, sizeof(master_output), SPI_TIMEOUT_DEFAULT); //отправляет команду и одновременно принимает данные от тачпада
    
    if (SPI_Status != HAL_OK) {
        xprintf("SPI_Error %d, OVR %d, MODF %d\n", SPI_Status, hspi0.ErrorCode & HAL_SPI_ERROR_OVR, hspi0.ErrorCode & HAL_SPI_ERROR_MODF);
        HAL_SPI_ClearError(&hspi0);
        return -1; // Возвращаем ошибку
    }

    /*Комбинирование 12 бит данных

    Байты 1 и 2 содержат 16 бит, из которых 12 бит - это полезная нагрузка (координата).
    Обычно 4 младших бита первого байта бесполезны, а 4 старших бита второго байта бесполезны.
  
    Собираем 12 бит: (master_input[1] << 4) | (master_input[2] >> 4)
    
    value = ((master_input[1] & 0x0F) << 8) | master_input[2]; // Простая сборка 16 бит

    Старший байт (master_input[1]) содержит 4 старших бита
    Младший байт (master_input[2]) содержит 8 младших бит*/

    value = (master_input[1] << 4) | (master_input[2] >> 4); 

    return value;
}

uint16_t transform(uint16_t value, uint16_t r1_max, uint16_t r1_min, uint16_t r2_max, uint16_t r2_min){//value - коорд. тачпада, которую нужно преобраз., мин. и макс. знач координат с тачпада, мин. макс. знач с экрана 
  uint16_t scale = (r2_max - r2_min) * 1000 / (r1_max - r1_min) ;
  uint16_t res = (value - r1_min) * scale / 1000; //преобраз. коорд с тачпада в коорд. с экрана
  return res;
}


void rect(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w , uint16_t color){
  unsigned int i,m;
  uint8_t colout[3];
  Lcd_select();
  HAL_GPIO_WritePin(GPIO_0, RS, 0);
  Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, length - 1 + d_l);
  HAL_GPIO_WritePin(GPIO_0, RS, 1);
  // uint32_t count = length*width*3 - 1;
  hspi0.ErrorCode = HAL_SPI_ERROR_NONE;

  hspi0.Instance->TX_THR = 1;
  /* Включить SPI если выключено */
  if (!(hspi0.Instance->ENABLE & SPI_ENABLE_M))
  {
      __HAL_SPI_ENABLE(&hspi0);
  }
  for(m=0;m<length;m++){
    for(i=0;i<width;i++)
    {
      colout[2] = ((color>>8) & 0xF8);
      colout[0] = ((color>>3) & 0xFC);
      colout[1] = ((color)<<3);
      hspi0.pTxBuffPtr = &colout[0];
      hspi0.TxCount = 3;
      // xprintf("spi_start\n");
      while ((hspi0.TxCount > 0))
      {
        /* Проверка флага TX_FIFO_NOT_FULL */
        if ((hspi0.Instance->INT_STATUS & SPI_INT_STATUS_TX_FIFO_NOT_FULL_M) && (hspi0.TxCount > 0))
        {
            hspi0.Instance->TXDATA = *(hspi0.pTxBuffPtr);
            hspi0.pTxBuffPtr++;
            hspi0.TxCount--;
            // xprintf("1\n");
        }
      }
      // xprintf("spi_end\n");
    }  
  }
  if (!(hspi0.Instance->CONFIG & SPI_CONFIG_MANUAL_CS_M))
  {
      __HAL_SPI_DISABLE(&hspi0);
      hspi0.Instance->ENABLE |= SPI_ENABLE_CLEAR_TX_FIFO_M | SPI_ENABLE_CLEAR_RX_FIFO_M; /* Очистка буферов RX и TX */
  }

  volatile uint32_t unused = hspi0.Instance->INT_STATUS; /* Очистка флагов ошибок чтением */
  (void) unused;

  Lcd_unselect();
}

void process_touchpad(){
    int x, y;
    int coord_x, coord_y;

    x = read_coordinate(READ_X);
    y = read_coordinate(READ_Y);
    
    if(x >  0 && y <2032){
        if(x < 150)x = 150;
        if(y < 100)y = 100;
        coord_x = transform(x, 2032, 150, 320, 0);
        coord_y = transform(y, 2032, 100, 480, 0);
        xprintf("x = %d, y = %d \n", x, y);
        
        rect(30, 30 , coord_y, coord_x, 0xf000);
    }
}