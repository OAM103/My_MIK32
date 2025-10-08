



// void LCD_Clear(unsigned int j) //очишает весь экран, заполняя цветом j (нет красного?может и зеленого?)
// {
//     unsigned int i, m;
//     // digitalWrite(CS,LOW);
//     Lcd_select();
//     Address_set(0, 0, 320, 480); //размер экрана
//     for (i = 0; i < 320; i++)
//         for (m = 0; m < 480; m++) {
//             Lcd_Write_Data((j >> 8) & 0xF8);
//             Lcd_Write_Data((j >> 3) & 0xFC);
//             Lcd_Write_Data(j << 3);
//         }
//     // digitalWrite(CS,HIGH);
//     Lcd_unselect();
// }


// // void GPIO_Init(){
// //     __HAL_PCC_GPIO_0_CLK_ENABLE();
// //     __HAL_PCC_GPIO_1_CLK_ENABLE();
// //     GPIO_InitTypeDef GPIO_InitStruct = {0};
// //     GPIO_InitStruct.Pin = CS;
// //     GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
// //     GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
// //     HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
// //     GPIO_InitStruct.Pin = RS;
// //     HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
// //     GPIO_InitStruct.Pin = RESET;
// //     HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
// //     GPIO_InitStruct.Pin = LED;
// //     HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);

// //     GPIO_InitStruct.Pin = SLAVE_CS0 | SLAVE_CS1 | SLAVE_CS2;
// //     GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
// //     GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
// //     if (HAL_GPIO_Init(GPIO_CS, &GPIO_InitStruct) != HAL_OK)
// //     {
// //         xprintf("SPI_Init_Error\n");
// //     }
// //     HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS_ALL, 0); // Выбрать клавиатуру
// //     HAL_GPIO_WritePin(GPIO_CS, SLAVE_CS1, 1); // Выбрать клавиатуру
// //     HAL_GPIO_WritePin(GPIO_0, CS , 1);
// //     HAL_GPIO_WritePin(GPIO_0, RS , 1);
// //     HAL_GPIO_WritePin(GPIO_0, RESET , 1);
// //     HAL_GPIO_WritePin(GPIO_0, LED , 1);
// // }



// // static void SPI1_Init(void)
// // {
// // hspi1.Instance = SPI_1;

// //     /* Режим SPI */
// //     hspi1.Init.SPI_Mode = HAL_SPI_MODE_MASTER;

// //     /* Настройки */
// //     hspi1.Init.CLKPhase = SPI_PHASE_ON;
// //     hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
// //     hspi1.Init.ThresholdTX = 4;

// //     /* Настройки для ведущего */
// //     hspi1.Init.BaudRateDiv = SPI_BAUDRATE_DIV8;
// //     hspi1.Init.Decoder = SPI_DECODER_NONE;
// //     hspi1.Init.ManualCS = SPI_MANUALCS_ON;
// //     hspi1.Init.ChipSelect = SPI_CS_0;      

// //     if (HAL_SPI_Init(&hspi1) != HAL_OK)
// //     {
// //         xprintf("SPI_Init_Error\n");
// //     }
// // }

// // void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) //Рисует линию горизонтально                 
// // {	
// //   unsigned int i,j;
// //   // digitalWrite(CS,LOW);
// //   Lcd_select();
// //   //Lcd_Write_Com(0x02c); //write_memory_start?
// //   //digitalWrite(RS,HIGH);
// //   l=l+x;
// //   Address_set(x,y,l,y);//область рисования линии
// //   j=l*2;
// //   for(i=1;i<=j;i++)//отправляет пиксельные данные на lcd
// //   {
// //       //выделяют и сдвигают нужные биты для каждого цветового канала
// //       Lcd_Write_Data((c>>8)&0xF8);
// //       Lcd_Write_Data((c>>3)&0xFC);
// //       Lcd_Write_Data(c<<3);
// //   }
// //   // digitalWrite(CS,HIGH);   
// //   Lcd_unselect();//деактивируем lcd(cs=1)
// // }

// // void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) //Рисует вертикальные линии                 
// // {	
// //   unsigned int i,j;
// //   // digitalWrite(CS,LOW);
// //   Lcd_select();
// //   Lcd_Write_Com(0x02c); //write_memory_start
// //   //digitalWrite(RS,HIGH);
// //   l=l+y;
// //   Address_set(x,y,x,l);
// //   j=l*2;
// //   for(i=1;i<=j;i++)
// //   { 
// //       Lcd_Write_Data((c>>8)&0xF8);
// //       Lcd_Write_Data((c>>3)&0xFC);
// //       Lcd_Write_Data(c<<3);
// //   }
// //   // digitalWrite(CS,HIGH);   
// //   Lcd_unselect();
// // }

// // void Rect(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)//РИсует прямоугольник (x,y)-верхний левый угол, w-ширина, h-высота, с-цвет границы
// // {
// //   H_line(x  , y  , w, c);
// //   H_line(x  , y+h, w, c);
// //   V_line(x  , y  , h, c);
// //   V_line(x+w, y  , h, c);
// // }

// // void Rectf(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)//Рисует заполненный прямоугольник ?(не эффективно) 
// // {
// //   unsigned int i;
// //   for(i=0;i<h;i++)
// //   {
// //     H_line(x  , y  , w, c);
// //     H_line(x  , y+i, w, c);
// //   }
// // }

// // int RGB(int r,int g,int b)//создает 24 битный цвет
// // {
// //   return r << 16 | g << 8 | b;
// // }

// // #define charOffset                  0x20
// // void writeChar(char t, uint8_t dlta){//функция для отрисовки символа t с маштабом dlta
// //   Address_set(0,0,8*dlta-1,5*dlta-1);//уст. область рисования для символа(ширина - 8 пикселей, высота 5 пикселей, dlta - коэф. масштаб - сколько пикселей будет отрисовано для каждого пикселя шрифта->эф.масштабирования)
// //   uint8_t color[]={250,0,0}; //цвет символа - красный
// //   Lcd_select();
// //   for(int i = 0 ; i < 5;i++){//перебор пикселей символа из таблицы шифров Char_font1. Если бит в табл.=1, то рисуется пиксель заданным цветом, иначе черный
// //     for(int d1=0;d1 < dlta;d1++){
// //       for(int j = 0 ; j < 8 ; j++){
// //         for(int d2=0;d2 < dlta;d2++){
// //           if(Char_font1[t - charOffset][i]&(1<<j)){// charOffset = 0x20 определяет смещение для символов в таблице шифров Char_font1
// //             Lcd_Write_Data(color[0]);
// //             Lcd_Write_Data(color[1]);
// //             Lcd_Write_Data(color[2]);
// //             }else {
// //               Lcd_Write_Data(0);
// //               Lcd_Write_Data(0);
// //               Lcd_Write_Data(0);
// //             }
// //         }
// //       }
// //     }
// //   }
// //   Lcd_unselect();
// // }


// // void clearChar(char t, uint8_t dlta){//очищает область экрана, занимаемую символом t с масштабом dlta
// //   Address_set(0,0,8*dlta-1,5*dlta-1);
// //   // uint8_t color[]={250,0,0};
// //   Lcd_select();
// //   for(int i = 0 ; i < 5;i++){
// //     for(int d1=0;d1 < dlta;d1++){
// //       for(int j = 0 ; j < 8 ; j++){
// //         for(int d2=0;d2 < dlta;d2++){//заполняет область белым цветом
// //           Lcd_Write_Data(0xff);
// //           Lcd_Write_Data(0xff);
// //           Lcd_Write_Data(0xff);
// //         }
// //       }
// //     }
// //   }
// //   Lcd_unselect();
// // }


// // вывод изображения в 2 части (верх и низ)
// // void PrintMass_Half(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w)
// // {
// //     uint16_t half = width / 2;  // половина высоты

// //     // --- Первая половина (сверху) ---
// //     PrintMass(mas, length, half, d_l, d_w);

// //     // --- Вторая половина (снизу) ---
// //     PrintMass(mas + half*length , length, width - half, d_l, d_w + half);
// // }


// // void ClearMass(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w) { //очищаетобласть экрана, заполняя ее одним цветом
// //     Lcd_select();
// //     Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, length - 1 + d_l);
// //     HAL_GPIO_WritePin(GPIO_0, RS, 1);
// //     uint32_t count = length * width * 3 - 1; // общ. кол-во байт, которые нужно отправить на LCD(3 байта на пиксель)
// //     hspi0.ErrorCode = HAL_SPI_ERROR_NONE; //сбрасывает код ошибки SPI

// //     hspi0.Instance->TX_THR = 1; //уст. порог FIFO передатчика SPI в 1, т.е. прерывание будет ген., когда в FIFO есть хотя бы одно свободное место
// //     /* Включить SPI если выключено */
// //     if (!(hspi0.Instance->ENABLE & SPI_ENABLE_M)) {
// //         __HAL_SPI_ENABLE(&hspi0);
// //     }
// //     while (count != 0) //отправляет байты на LCD, пока счётчик не достигнет 0
// //     {
// //         /* Проверка флага TX_FIFO_NOT_FULL */
// //         // xprintf("0\n");
// //         if ((hspi0.Instance->INT_STATUS & SPI_INT_STATUS_TX_FIFO_NOT_FULL_M)) {
// //             hspi0.Instance->TXDATA = 0xFF;
// //             count--;
// //             // xprintf("1\n");
// //         }
// //         // xprintf("0\n");
// //     }
// //     if (!(hspi0.Instance->CONFIG & SPI_CONFIG_MANUAL_CS_M)) {
// //         __HAL_SPI_DISABLE(&hspi0);
// //         hspi0.Instance->ENABLE |= SPI_ENABLE_CLEAR_TX_FIFO_M | SPI_ENABLE_CLEAR_RX_FIFO_M; /* Очистка буферов RX и TX */
// //     }

// //     volatile uint32_t unused = hspi0.Instance->INT_STATUS; /* Очистка флагов ошибок чтением */
// //     (void)unused;

// //     Lcd_unselect();
// // }



// int read_coordinate(unsigned char command) {
//   int16_t value;
//   // unsigned char high_byte, low_byte;

//   // Отправить команду
//   // spi_transfer(command);
//   uint8_t master_output[] = {command, 0, 0};
//   uint8_t master_input[sizeof(master_output)];
//   master_input[0] = 0;
//   HAL_StatusTypeDef SPI_Status = HAL_SPI_Exchange(&hspi1, master_output, master_input, sizeof(master_output), SPI_TIMEOUT_DEFAULT);
//   if (SPI_Status != HAL_OK)
//   {
//       xprintf("SPI_Error %d, OVR %d, MODF %d\n", SPI_Status, hspi0.ErrorCode & HAL_SPI_ERROR_OVR, hspi0.ErrorCode & HAL_SPI_ERROR_MODF);
//       HAL_SPI_ClearError(&hspi0);
//   }
//   // Скомбинировать байты в 12-битное значение
//   value = (master_input[1] << 4) ;

//   return value;
// }

// // int read_coordinate(unsigned char command) {
// //     int16_t value;

// //     // Подготовка пакета (Команда + 2 байта для приема)
// //     uint8_t master_output[] = {command, 0x00, 0x00}; //команда  и 2 байта для приёма данных. Тачпад отправляет12-битные координваты, упакованные в 2 байта
// //     uint8_t master_input[3] = {0}; // Инициализируем нулями

// //     // Обмен данными по SPI (Блокирующий режим)
// //     HAL_StatusTypeDef SPI_Status = HAL_SPI_Exchange(&hspi1, master_output, master_input, sizeof(master_output), SPI_TIMEOUT_DEFAULT); //отправляет команду и одновременно принимает данные от тачпада
    
// //     if (SPI_Status != HAL_OK) {
// //         xprintf("SPI_Error %d, OVR %d, MODF %d\n", SPI_Status, hspi0.ErrorCode & HAL_SPI_ERROR_OVR, hspi0.ErrorCode & HAL_SPI_ERROR_MODF);
// //         HAL_SPI_ClearError(&hspi0);
// //         return -1; // Возвращаем ошибку
// //     }

// //     /*Комбинирование 12 бит данных

// //     Байты 1 и 2 содержат 16 бит, из которых 12 бит - это полезная нагрузка (координата).
// //     Обычно 4 младших бита первого байта бесполезны, а 4 старших бита второго байта бесполезны.
  
// //     Собираем 12 бит: (master_input[1] << 4) | (master_input[2] >> 4)
    
// //     value = ((master_input[1] & 0x0F) << 8) | master_input[2]; // Простая сборка 16 бит

// //     Старший байт (master_input[1]) содержит 4 старших бита
// //     Младший байт (master_input[2]) содержит 8 младших бит*/

// //     value = (master_input[1] << 4) | (master_input[2] >> 4); 

// //     return value;
// // }

// // uint16_t transform(uint16_t value, uint16_t r1_max, uint16_t r1_min, uint16_t r2_max, uint16_t r2_min){
// //   uint16_t scale = (r2_max - r2_min) * 1000 / (r1_max - r1_min) ;
// //   uint16_t res = (value - r1_min) * scale / 1000;
// //   return res;
// // }

// // void rect(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w , uint16_t color){
// //   unsigned int i,m;
// //   uint8_t colout[3];
// //   Lcd_select();
// //   HAL_GPIO_WritePin(GPIO_0, RS, 0);
// //   Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, length - 1 + d_l);
// //   HAL_GPIO_WritePin(GPIO_0, RS, 1);
// //   // uint32_t count = length*width*3 - 1;
// //   hspi0.ErrorCode = HAL_SPI_ERROR_NONE;

// //   hspi0.Instance->TX_THR = 1;
// //   /* Включить SPI если выключено */
// //   if (!(hspi0.Instance->ENABLE & SPI_ENABLE_M))
// //   {
// //       __HAL_SPI_ENABLE(&hspi0);
// //   }
// //   // for(m=0;m<length;m++){//перебирает строки изображения
// //   //   for(i=0;i<width;i++)//перебирает пиксели в строке
// //   //   {
// //   //     //перевод из RGB565 в RGB888
// //   //     colout[2] = ((*(mas+i*length - 1 + m)>>8) & 0xF8);//извленает красный компонент из 16-битного знач. цвета, сдвигает на 8 бит и маскирует, чтобы получить 5 старших бит(& 0xF8 - 8-битное знач. умножаем на 255/31)
// //   //     colout[0] = ((*(mas+i*length - 1 + m)>>3) & 0xFC);//извлекает зеленый компонент сдвигает и маскирует(& 0xFC - 255/63)
// //   //     colout[1] = ((*(mas+i*length - 1 + m))<<3);//извлекает синий бит(<<3 - умножаем на 255/31)
// //   //     hspi0.pTxBuffPtr = &colout[0];//устанавливаем указатель на буффер передачи SPI(colout)
// //   //     hspi0.TxCount = 3;//кол-во байт для RGB888
// //   //     // xprintf("spi_start\n");
// //   //     while ((hspi0.TxCount > 0))//пока все 3 байта не будут переданы по SPI
// //   //     {
// //   //       /* Проверка флага TX_FIFO_NOT_FULL */
// //   //       if ((hspi0.Instance->INT_STATUS & SPI_INT_STATUS_TX_FIFO_NOT_FULL_M) && (hspi0.TxCount > 0))//есть ли место в FIFO передатчика SPI
// //   //       {
// //   //           hspi0.Instance->TXDATA = *(hspi0.pTxBuffPtr);//записывает байт из буфера colout в регистр передачи SPI
// //   //           hspi0.pTxBuffPtr++;//увелич. указатель на следующий байт
// //   //           hspi0.TxCount--;// уменьш. счетчик остат. байт
// //   //           // xprintf("1\n");
// //   //       }
// //   //     }
// //   //     // xprintf("spi_end\n");
// //   //   }  
// //   for(m=0;m<length;m++){
// //     for(i=0;i<width;i++)
// //     {
// //       colout[2] = ((color>>8) & 0xF8);
// //       colout[0] = ((color>>3) & 0xFC);
// //       colout[1] = ((color)<<3);
// //       hspi0.pTxBuffPtr = &colout[0];
// //       hspi0.TxCount = 3;
// //       // xprintf("spi_start\n");
// //       while ((hspi0.TxCount > 0))
// //       {
// //         /* Проверка флага TX_FIFO_NOT_FULL */
// //         if ((hspi0.Instance->INT_STATUS & SPI_INT_STATUS_TX_FIFO_NOT_FULL_M) && (hspi0.TxCount > 0))//рис масс?
// //         {
// //             hspi0.Instance->TXDATA = *(hspi0.pTxBuffPtr);
// //             hspi0.pTxBuffPtr++;
// //             hspi0.TxCount--;
// //             // xprintf("1\n");
// //         }
// //       }
// //       // xprintf("spi_end\n");
// //     }  
// //   }
// //   if (!(hspi0.Instance->CONFIG & SPI_CONFIG_MANUAL_CS_M))
// //   {
// //       __HAL_SPI_DISABLE(&hspi0);
// //       hspi0.Instance->ENABLE |= SPI_ENABLE_CLEAR_TX_FIFO_M | SPI_ENABLE_CLEAR_RX_FIFO_M; /* Очистка буферов RX и TX */
// //   }

// //   volatile uint32_t unused = hspi0.Instance->INT_STATUS; /* Очистка флагов ошибок чтением */
// //   (void) unused;

// //   Lcd_unselect();
// // }


// int main() {
//     SystemClock_Config();
//     UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M | UART_CONTROL1_RE_M, 0, 0);

//     SPI0_Init();
//     GPIO_Init();
//     //SPI1_Init();
//     Lcd_Init();
//     //HAL_Init();
//     DMA_Init();

//     //ClearMass(480, 320, 0 , 0);
//     //Rect(10,10,100,134,21);
//     //int x, y;
//     //int coord_x, coord_y;
//     PrintMassDMA(&img[0],479,319,0,0);
//     while (1) {
//         //PrintMass(&img[0],479,319,0,0);
//         //HAL_DelayMs(500);
        
//         //HAL_DelayMs(500);
//         // x = read_coordinate(READ_X);
//         // y = read_coordinate(READ_Y);
        
//         // if(x >  0 && y <2032){
//         //   if(x < 150)x = 150;
//         //   if(y < 100)y = 100;
//         //   coord_x = transform(x, 2032, 150, 320, 0);
//         //   coord_y = transform(y, 2032, 100, 480, 0);
//         //   xprintf("x = %d, y = %d \n", x, y);
          
//         //   rect(30, 30 , coord_y, coord_x, 0xf000);
//         // }
        
//     }
// }


#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_spi.h"
#include "mik32_hal_dma.h"
#include <stdlib.h>
#include <string.h>
#include "mik32_hal_lcd5110_bitmaps.h"
#include "uart_lib.h"
#include "xprintf.h"
#include "png.h"

void SystemClock_Config();
static void SPI0_Init(void);
SPI_HandleTypeDef hspi0;
SPI_HandleTypeDef hspi1;
DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0;
static void DMA_CH0_Init(DMA_InitTypeDef *hdma);
static void DMA_Init(void);

#define CS          GPIO_PIN_4 //p0_4
#define RS          GPIO_PIN_8 //p0_8
#define RESET       GPIO_PIN_9
#define LED         GPIO_PIN_10
#define TFT_WIDTH   480
#define TFT_HEIGHT  320

#define GPIO_CS GPIO_1
#define SLAVE_CS0 GPIO_PIN_10
#define SLAVE_CS1 GPIO_PIN_11
#define SLAVE_CS2 GPIO_PIN_14
#define SLAVE_CS_ALL (SLAVE_CS2 | SLAVE_CS1 | SLAVE_CS0)

#define READ_X  0xD0
#define READ_Y  0x90

// Два буфера для построчной двойной буферизации
static uint16_t lineBuffer1[TFT_WIDTH];
static uint16_t lineBuffer2[TFT_WIDTH];



void Lcd_select() {
    HAL_GPIO_WritePin(GPIO_0, CS, GPIO_PIN_LOW);
}

void Lcd_unselect() {
    HAL_GPIO_WritePin(GPIO_0, CS, GPIO_PIN_HIGH);
}

void Lcd_Writ_Bus(uint8_t d) { //отправляет 1 байт данных или команды
    uint8_t out[] = {d};
    uint8_t in[sizeof(out)];
    HAL_SPI_Exchange(&hspi0, out, in, sizeof(out), SPI_TIMEOUT_DEFAULT); //одновременная передача и приём(стандартная опирация SPI)
    //SPI.transfer(d);
}

void Lcd_Write_Com(unsigned char VH) { //отправляет команду(RS)
    // *(portOutputRegister(digitalPinToPort(RS))) &=  ~digitalPinToBitMask(RS);//LCD_RS=0; - альтернативный способ
    HAL_GPIO_WritePin(GPIO_0, RS, 0); //указаваем 0 -> отправляется команда
    Lcd_Writ_Bus(VH);
}

void Lcd_Write_Data(uint8_t VH) { //отправляет данные
    // *(portOutputRegister(digitalPinToPort(RS)))|=  digitalPinToBitMask(RS);//LCD_RS=1;
    HAL_GPIO_WritePin(GPIO_0, RS, 1); //указаваем 1 -> отправляются данные
    Lcd_Writ_Bus(VH);
}

void Lcd_Write_Com_Data(unsigned char com, unsigned char dat) { //отправляет команду com и данные
    Lcd_Write_Com(com);
    Lcd_Write_Data(dat);
}

void Address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) { //Устанавливает область памяти LCD, в которую будут записываться данные
    //(x1, y1) - левый нижний угл, (x2, y2) - правый верхний угол
    Lcd_Write_Com(0x2a);
    Lcd_Write_Data(x1 >> 8);
    Lcd_Write_Data(x1 & 0xFF);
    Lcd_Write_Data(x2 >> 8);
    Lcd_Write_Data(x2 & 0xFF);
    Lcd_Write_Com(0x2b);
    Lcd_Write_Data(y1 >> 8);
    Lcd_Write_Data(y1 & 0xFF);
    Lcd_Write_Data(y2 >> 8);
    Lcd_Write_Data(y2 & 0xFF);
    Lcd_Write_Com(0x2c); //начать запись
    Lcd_Write_Data(0x00);
}

void Lcd_Init(void) {

    // digitalWrite(RESET,HIGH); //сброс
    HAL_GPIO_WritePin(GPIO_0, RESET, 1);
    HAL_DelayMs(2);
    HAL_GPIO_WritePin(GPIO_0, RESET, 0);
    // digitalWrite(RESET,LOW);
    HAL_DelayMs(5);
    // digitalWrite(RESET,HIGH);
    HAL_GPIO_WritePin(GPIO_0, RESET, 1);
    HAL_DelayMs(2);

    // digitalWrite(CS,LOW);  //CS //для активации
    Lcd_select();

    Lcd_Write_Com(0xF7); //команда разблокировка
    Lcd_Write_Data(0xA9); //данные
    Lcd_Write_Data(0x51);
    Lcd_Write_Data(0x2C);
    Lcd_Write_Data(0x82);

    Lcd_Write_Com(0xC0); //установка опорного напряжения
    Lcd_Write_Data(0x11); //значения
    Lcd_Write_Data(0x09);

    Lcd_Write_Com(0xC1); //режим энергосбережения
    Lcd_Write_Data(0x41);

    Lcd_Write_Com(0xC5); //напряжение для включения  и выключения пикселей
    Lcd_Write_Data(0x00);
    Lcd_Write_Data(0x0A);
    Lcd_Write_Data(0x80);

    Lcd_Write_Com(0xB1); //частота кадров
    Lcd_Write_Data(0xB0);
    Lcd_Write_Data(0x11);

    Lcd_Write_Com(0xB4);  //режим инверсии, как пиксели будут переключатся между состояниями
    Lcd_Write_Data(0x02);

    Lcd_Write_Com(0xB6);  // режим отображения
    Lcd_Write_Data(0x02);
    Lcd_Write_Data(0x22);

    Lcd_Write_Com(0xB7);  // настройка источника данных
    Lcd_Write_Data(0xC6);

    Lcd_Write_Com(0xBE);  //для vscan
    Lcd_Write_Data(0x00);
    Lcd_Write_Data(0x04);

    Lcd_Write_Com(0xE9);  //для параметров интерфейса
    Lcd_Write_Data(0x00);

    Lcd_Write_Com(0x36);  //MAC/MDAD-указывает направление, в котором данные записываются в LCD, влияет на ориентацию дисплея
    Lcd_Write_Data(0x88);    
    Lcd_Write_Com(0x3A);  //уст. формат пикселей, используемый для передачи данных на LCD 
    Lcd_Write_Data(0x66); //0x66-18bit(RGB565), 0x55-16bit(RGB666), 0x77-24bit(RGB888)

    Lcd_Write_Com(0xE0);  //гамма-=коррекция
    Lcd_Write_Data(0x00);
    Lcd_Write_Data(0x07);
    Lcd_Write_Data(0x10);
    Lcd_Write_Data(0x09);
    Lcd_Write_Data(0x17);
    Lcd_Write_Data(0x0B);
    Lcd_Write_Data(0x41);
    Lcd_Write_Data(0x89);
    Lcd_Write_Data(0x4B);
    Lcd_Write_Data(0x0A);
    Lcd_Write_Data(0x0C);
    Lcd_Write_Data(0x0E);
    Lcd_Write_Data(0x18);
    Lcd_Write_Data(0x1B);
    Lcd_Write_Data(0x0F);

    Lcd_Write_Com(0xE1);    //гамма-коррекция
    Lcd_Write_Data(0x00);
    Lcd_Write_Data(0x17);
    Lcd_Write_Data(0x1A);
    Lcd_Write_Data(0x04);
    Lcd_Write_Data(0x0E);
    Lcd_Write_Data(0x06);
    Lcd_Write_Data(0x2F);
    Lcd_Write_Data(0x45);
    Lcd_Write_Data(0x43);
    Lcd_Write_Data(0x02);
    Lcd_Write_Data(0x0A);
    Lcd_Write_Data(0x09);
    Lcd_Write_Data(0x32);
    Lcd_Write_Data(0x36);
    Lcd_Write_Data(0x0F);

    Lcd_Write_Com(0x11);    //Exit Sleep
    HAL_DelayMs(120);
    Lcd_Write_Com(0x29);    //Display on

    // digitalWrite(CS,HIGH);
    // HAL_GPIO_WritePin(GPIO_0, CS, 1);
    Lcd_unselect();
}

void LCD_Clear(unsigned int j) //очишает весь экран, заполняя цветом j (нет красного?может и зеленого?)
{
    unsigned int i, m;
    // digitalWrite(CS,LOW);
    HAL_GPIO_WritePin(GPIO_0, CS, 0);
    Address_set(0, 0, 320, 480); //размер экрана
    for (i = 0; i < 320; i++)
        for (m = 0; m < 480; m++) {
            Lcd_Write_Data((j >> 8) & 0xF8);
            Lcd_Write_Data((j >> 3) & 0xFC);
            Lcd_Write_Data(j << 3);
        }
    // digitalWrite(CS,HIGH);
    HAL_GPIO_WritePin(GPIO_0, CS, 1);
}

void GPIO_Init() {
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = CS;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    GPIO_InitStruct.DS = HAL_GPIO_DS_8MA;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = RS;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = RESET;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = LED;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    // HAL_GPIO_WritePin(GPIO_0, CS , 1);
    // HAL_GPIO_WritePin(GPIO_0, RS , 1);
    // HAL_GPIO_WritePin(GPIO_0, RESET , 1);
    HAL_GPIO_WritePin(GPIO_0, LED, 1);
}

void SystemClock_Config(void) {
    PCC_InitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 125;
    PCC_OscInit.LSI32KCalibrationValue = 8;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}

static void SPI0_Init(void) {
    hspi0.Instance = SPI_0;

    /* Режим SPI */
    hspi0.Init.SPI_Mode = HAL_SPI_MODE_MASTER;

    /* Настройки */
    hspi0.Init.CLKPhase = SPI_PHASE_ON;
    hspi0.Init.CLKPolarity = SPI_POLARITY_HIGH;
    //hspi0.Init.ThresholdTX = 4;
    hspi0.Init.ThresholdTX = SPI_THRESHOLD_DEFAULT;
    //hspi0.Init.CLKPolarity = SPI_POLARITY_LOW;

    /* Настройки для ведущего */
    hspi0.Init.BaudRateDiv = 0;
    //hspi0.Init.BaudRateDiv = 0;
    hspi0.Init.Decoder = SPI_DECODER_NONE;
    hspi0.Init.ManualCS = SPI_MANUALCS_ON;
    hspi0.Init.ChipSelect = SPI_CS_0;

    if (HAL_SPI_Init(&hspi0) != HAL_OK) {
        xprintf("SPI_Init_Error\n");
    }
}

void ClearMass(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w) { //очищаетобласть экрана, заполняя ее одним цветом
    Lcd_select();
    Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, length - 1 + d_l);
    HAL_GPIO_WritePin(GPIO_0, RS, 1);
    uint32_t count = length * width * 3 - 1; // общ. кол-во байт, которые нужно отправить на LCD(3 байта на пиксель)
    hspi0.ErrorCode = HAL_SPI_ERROR_NONE; //сбрасывает код ошибки SPI

    hspi0.Instance->TX_THR = 1; //уст. порог FIFO передатчика SPI в 1, т.е. прерывание будет ген., когда в FIFO есть хотя бы одно свободное место
    /* Включить SPI если выключено */
    if (!(hspi0.Instance->ENABLE & SPI_ENABLE_M)) {
        __HAL_SPI_ENABLE(&hspi0);
    }
    while (count != 0) //отправляет байты на LCD, пока счётчик не достигнет 0
    {
        /* Проверка флага TX_FIFO_NOT_FULL */
        // xprintf("0\n");
        if ((hspi0.Instance->INT_STATUS & SPI_INT_STATUS_TX_FIFO_NOT_FULL_M)) {
            hspi0.Instance->TXDATA = 0xFF;
            count--;
            // xprintf("1\n");
        }
        // xprintf("0\n");
    }
    if (!(hspi0.Instance->CONFIG & SPI_CONFIG_MANUAL_CS_M)) {
        __HAL_SPI_DISABLE(&hspi0);
        hspi0.Instance->ENABLE |= SPI_ENABLE_CLEAR_TX_FIFO_M | SPI_ENABLE_CLEAR_RX_FIFO_M; /* Очистка буферов RX и TX */
    }

    volatile uint32_t unused = hspi0.Instance->INT_STATUS; /* Очистка флагов ошибок чтением */
    (void)unused;

    Lcd_unselect();
}

#define DMA_BUFFER_SIZE (TFT_WIDTH * 3) // Размер буфера для одной строки

uint8_t dma_buffer[DMA_BUFFER_SIZE]; // Статический буфер

//отображает изображение, представленное массивом данных mas(указатель на массив 16-битных данных цветов писселей изображений RGB565, высота изображ. в пикселях, ширина, смещение по вертикалит от вержнего края экрана, смещение по горизонтали в пикселях)
void PrintMass(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w ){
  unsigned int i,m;//переменные для циклов
  uint8_t colout[3];//массив для храниения RGB888(24-битного) представления цвета пикселя
  // xprintf("start\n");
  HAL_GPIO_WritePin(GPIO_0, CS, 0);//CS в 0
  Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, length - 1 + d_l);//область памяти, в которую будут записываться пиксели изображения, с учетом данных смещения
  HAL_GPIO_WritePin(GPIO_0, RS, 1);//RS в 1
  if (!(hspi0.Instance->ENABLE & SPI_ENABLE_M))//включен ли SPI интерфейс
  {
      __HAL_SPI_ENABLE(&hspi0);//включаем SPI
  }
  // xprintf("spi_enable\n");
  for(m=0;m<length;m++){//перебирает строки изображения
    for(i=0;i<width;i++)//перебирает пиксели в строке
    {
      //перевод из RGB565 в RGB888
      colout[2] = ((*(mas+i*length - 1 + m)>>8) & 0xF8);//извленает красный компонент из 16-битного знач. цвета, сдвигает на 8 бит и маскирует, чтобы получить 5 старших бит(& 0xF8 - 8-битное знач. умножаем на 255/31)
      colout[0] = ((*(mas+i*length - 1 + m)>>3) & 0xFC);//извлекает зеленый компонент сдвигает и маскирует(& 0xFC - 255/63)
      colout[1] = ((*(mas+i*length - 1 + m))<<3);//извлекает синий бит(<<3 - умножаем на 255/31)
      hspi0.pTxBuffPtr = &colout[0];//устанавливаем указатель на буффер передачи SPI(colout)
      hspi0.TxCount = 3;//кол-во байт для RGB888
      // xprintf("spi_start\n");
      while ((hspi0.TxCount > 0))//пока все 3 байта не будут переданы по SPI
      {
        /* Проверка флага TX_FIFO_NOT_FULL */
        if ((hspi0.Instance->INT_STATUS & SPI_INT_STATUS_TX_FIFO_NOT_FULL_M) && (hspi0.TxCount > 0))//есть ли место в FIFO передатчика SPI
        {
            hspi0.Instance->TXDATA = *(hspi0.pTxBuffPtr);//записывает байт из буфера colout в регистр передачи SPI
            hspi0.pTxBuffPtr++;//увелич. указатель на следующий байт
            hspi0.TxCount--;// уменьш. счетчик остат. байт
            // xprintf("1\n");
        }
      }
      // xprintf("spi_end\n");
    }  
  }
  if (!(hspi0.Instance->CONFIG & SPI_CONFIG_MANUAL_CS_M))//если автомат. управ. CS пином выключено
  {
      __HAL_SPI_DISABLE(&hspi0);//включ. SPI интерфейс
      hspi0.Instance->ENABLE |= SPI_ENABLE_CLEAR_TX_FIFO_M | SPI_ENABLE_CLEAR_RX_FIFO_M; /* Очистка буферов RX и TX */
  }
  // xprintf("stop\n");
  HAL_GPIO_WritePin(GPIO_0, CS, 1); //деактивирует LCD, устан. пин CS в 1
}

void PrintMassDMA(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w) {

    HAL_SPI_Enable(&hspi0);
    Lcd_select();
    Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, length - 1 + d_l);//область памяти, в которую будут записываться пиксели изображения, с учетом данных смещения
    HAL_GPIO_WritePin(GPIO_0, RS, 1);//RS в 1

    for (uint16_t m = 0; m < length; m++) {   // строки
        // собрать строку в буфер
        for (uint16_t i = 0; i < width; i++) {
            dma_buffer[i*3 + 2] = (*(mas + i*length - 1 + m) >> 8) & 0xF8;   
            dma_buffer[i*3 + 0] = (*(mas + i*length - 1 + m) >> 3) & 0xFC;  
            dma_buffer[i*3 + 1] = (*(mas + i*length - 1 + m) << 3);          
        }
        // ожидание завершения DMA
        HAL_DMA_Wait(&hdma_ch0, DMA_TIMEOUT_DEFAULT);
        // запуск DMA на всю строку (width*3 байт)
        HAL_DMA_Start(&hdma_ch0, (void*)dma_buffer, (void*)&hspi0.Instance->TXDATA, width*3-1);
        
    }
    HAL_SPI_Disable(&hspi0);
    Lcd_unselect();
}
int main() {
    SystemClock_Config();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M | UART_CONTROL1_RE_M, 0, 0);
    xprintf("start\n");

    SPI0_Init();
    GPIO_Init();
    Lcd_Init();
    DMA_Init();


    while (1) {
        PrintMass(&mikron[0],479,319,0,0);
        HAL_DelayMs(500);
        PrintMassDMA(&img[0],479,319,0,0);
        HAL_DelayMs(500);
        // _PrintMassDMA_(&mikron[0],479,319,0,0);
        // HAL_DelayMs(10000);
        // PrintMass(&img[0],479,319,0,0);
        // HAL_DelayMs(10000);
        //PrintMass(&mikron[0], 479, 319, 0, 0);
        //PrintMass(&img[0],479,319,0,0);
    }
}

static void DMA_Init(void)
{
    /* Настройки DMA */
    hdma.Instance = DMA_CONFIG;
    hdma.CurrentValue = DMA_CURRENT_VALUE_ENABLE;
    if (HAL_DMA_Init(&hdma) != HAL_OK)
    {
        xprintf("DMA_Init Error\n");
    }

    /* Инициализация канала */
    DMA_CH0_Init(&hdma);
}

static void DMA_CH0_Init(DMA_InitTypeDef *hdma)
{
    hdma_ch0.dma = hdma;
    /* Настройки канала */
    hdma_ch0.ChannelInit.Channel = DMA_CHANNEL_0;
    hdma_ch0.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_VERY_HIGH;
    hdma_ch0.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
    hdma_ch0.ChannelInit.ReadInc = DMA_CHANNEL_INC_ENABLE;
    hdma_ch0.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE; /* data_len должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadBurstSize = 0;                /* read_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.ReadRequest = DMA_CHANNEL_SPI_0_REQUEST;
    hdma_ch0.ChannelInit.ReadAck = DMA_CHANNEL_ACK_DISABLE;
    hdma_ch0.ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
    hdma_ch0.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
    hdma_ch0.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE; /* data_len должно быть кратно write_size */
    hdma_ch0.ChannelInit.WriteRequest = DMA_CHANNEL_SPI_0_REQUEST;
    hdma_ch0.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;
}



