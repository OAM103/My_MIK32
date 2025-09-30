#include "mik32_hal_pcc.h"//для управления тактированием системы
#include "mik32_hal_gpio.h"//для работы с портами ввода и вывода
#include "mik32_hal_spi.h"//для spi
#include "mik32_hal_dma.h"//для работы с контроллером прямого доступа
#include <stdlib.h>
#include <string.h>
#include "mik32_hal_lcd5110_bitmaps.h"
#include "uart_lib.h"
#include "xprintf.h"
#include "png.h"

void SystemClock_Config();
static void SPI0_Init(void);
SPI_HandleTypeDef hspi0;
DMA_InitTypeDef hdma;
DMA_ChannelHandleTypeDef hdma_ch0;
static void DMA_CH0_Init(DMA_InitTypeDef *hdma);
static void DMA_Init(void);

#define CS         GPIO_PIN_4//p0_4
#define RS         GPIO_PIN_8//p0
#define RESET      GPIO_PIN_9 
#define LED        GPIO_PIN_10


void Lcd_Writ_Bus(uint8_t d) //отправляет 1 байт данных или команды
{
  uint8_t out[] = {d};
  uint8_t in[sizeof(out)];
  HAL_SPI_Exchange(&hspi0, out, in, sizeof(out),SPI_TIMEOUT_DEFAULT);//одновременная передача и приём(стандартная опирация SPI)
  // SPI.transfer(d);
}

void Lcd_Write_Com(unsigned char VH)//отправляет команду(RS)
{   
  // *(portOutputRegister(digitalPinToPort(RS))) &=  ~digitalPinToBitMask(RS);//LCD_RS=0; - альтернативный способ
  HAL_GPIO_WritePin(GPIO_0, RS, 0);//указаваем 0 -> отправляется команда
  Lcd_Writ_Bus(VH);
}

void Lcd_Write_Data(uint8_t VH)//отправляет данные
{
  // *(portOutputRegister(digitalPinToPort(RS)))|=  digitalPinToBitMask(RS);//LCD_RS=1;
  HAL_GPIO_WritePin(GPIO_0, RS, 1);//указаваем 1 -> отправляются данные
  Lcd_Writ_Bus(VH);
}

void Lcd_Write_Com_Data(unsigned char com,unsigned char dat)//отправляет команду com и данные 
{
  Lcd_Write_Com(com);
  Lcd_Write_Data(dat);
}

void Address_set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)//Устанавливает область памяти LCD, в которую будут записываться данные
{
  //(x1, y1) - левый нижний угл, (x2, y2) - правый верхний угол
  Lcd_Write_Com(0x2a);
	Lcd_Write_Data(x1>>8);
	Lcd_Write_Data(x1 & 0xFF);
	Lcd_Write_Data(x2>>8);
	Lcd_Write_Data(x2 & 0xFF);
  Lcd_Write_Com(0x2b);
	Lcd_Write_Data(y1>>8);
	Lcd_Write_Data(y1&0xFF);
	Lcd_Write_Data(y2>>8);
	Lcd_Write_Data(y2&0xFF);
	Lcd_Write_Com(0x2c);//начать запись 
  Lcd_Write_Data(0x00);				 
}

void Lcd_Init(void)
{
    
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
  HAL_GPIO_WritePin(GPIO_0, CS, 0);

  Lcd_Write_Com(0xF7);  //команда разблокировка
  Lcd_Write_Data(0xA9); //данные
  Lcd_Write_Data(0x51); 
  Lcd_Write_Data(0x2C); 
  Lcd_Write_Data(0x82);  

  Lcd_Write_Com(0xC0);  //установка опорного напряжения
  Lcd_Write_Data(0x11); //значения
  Lcd_Write_Data(0x09); 

  Lcd_Write_Com(0xC1);  //режим энергосбережения
  Lcd_Write_Data(0x41); 

  Lcd_Write_Com(0xC5);  //напряжение для включения  и выключения пикселей
  Lcd_Write_Data(0x00); 
  Lcd_Write_Data(0x0A); 
  Lcd_Write_Data(0x80);

  Lcd_Write_Com(0xB1); //частота кадров
  Lcd_Write_Data(0xB0); 
  Lcd_Write_Data(0x11); 

  Lcd_Write_Com(0xB4);  //режим инверсии, как пиксели будут переключатся между состояниями
  Lcd_Write_Data(0x02); 

  Lcd_Write_Com(0xB6);   // режим отображения 
  Lcd_Write_Data(0x02);
  Lcd_Write_Data(0x22);  

  Lcd_Write_Com(0xB7);   // настройка источника данных
  Lcd_Write_Data(0xC6);  

  Lcd_Write_Com(0xBE);  //для vscan  
  Lcd_Write_Data(0x00);   
  Lcd_Write_Data(0x04); 

  Lcd_Write_Com(0xE9);  //для параметров интерфейса
  Lcd_Write_Data(0x00);   

  Lcd_Write_Com(0x36);  //MAC/MDAD-указывает направление, в котором данные записываются в LCD, влияет на ориентацию дисплея
  Lcd_Write_Data(0x88);   

  Lcd_Write_Com(0x3A);   //уст. формат пикселей, используемый для передачи данных на LCD 
  Lcd_Write_Data(0x66); 

  Lcd_Write_Com(0xE0);   //гамма-=коррекция
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
  HAL_GPIO_WritePin(GPIO_0, CS, 1);
}

// void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) //Рисует линию горизонтально                 
// {	
//   unsigned int i,j;
//   // digitalWrite(CS,LOW);
//   HAL_GPIO_WritePin(GPIO_0, CS, 0);
//   //Lcd_Write_Com(0x02c); //write_memory_start?
//   //digitalWrite(RS,HIGH);
//   l=l+x;
//   Address_set(x,y,l,y);//область рисования линии
//   j=l*2;
//   for(i=1;i<=j;i++)//отправляет пиксельные данные на lcd
//   {
//       //выделяют и сдвигают нужные биты для каждого цветового канала
//       Lcd_Write_Data((c>>8)&0xF8);
//       Lcd_Write_Data((c>>3)&0xFC);
//       Lcd_Write_Data(c<<3);
//   }
//   // digitalWrite(CS,HIGH);   
//   HAL_GPIO_WritePin(GPIO_0, CS, 1);//деактивируем lcd(cs=1)
// }

// void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) //Рисует вертикальные линии                 
// {	
//   unsigned int i,j;
//   // digitalWrite(CS,LOW);
//   HAL_GPIO_WritePin(GPIO_0, CS, 0);
//   Lcd_Write_Com(0x02c); //write_memory_start
//   //digitalWrite(RS,HIGH);
//   l=l+y;
//   Address_set(x,y,x,l);
//   j=l*2;
//   for(i=1;i<=j;i++)
//   { 
//       Lcd_Write_Data((c>>8)&0xF8);
//       Lcd_Write_Data((c>>3)&0xFC);
//       Lcd_Write_Data(c<<3);
//   }
//   // digitalWrite(CS,HIGH);   
//   HAL_GPIO_WritePin(GPIO_0, CS, 1);
// }

// void Rect(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)//РИсует прямоугольник (x,y)-верхний левый угол, w-ширина, h-высота, с-цвет границы
// {
//   H_line(x  , y  , w, c);
//   H_line(x  , y+h, w, c);
//   V_line(x  , y  , h, c);
//   V_line(x+w, y  , h, c);
// }

// void Rectf(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)//Рисует заполненный прямоугольник ?(не эффективно) 
// {
//   unsigned int i;
//   for(i=0;i<h;i++)
//   {
//     H_line(x  , y  , w, c);
//     H_line(x  , y+i, w, c);
//   }
// }

// int RGB(int r,int g,int b)//создает 24 битный цвет
// {
//   return r << 16 | g << 8 | b;
// }


void LCD_Clear(unsigned int j) //очишает весь экран, заполняя цветом j                 
{	
  unsigned int i,m;
  // digitalWrite(CS,LOW);
  HAL_GPIO_WritePin(GPIO_0, CS, 0);
  Address_set(0,0,320,480);//размер экрана
  for(i=0;i<320;i++)
    for(m=0;m<480;m++)
    {
      Lcd_Write_Data((j>>8)&0xF8);
      Lcd_Write_Data((j>>3)&0xFC);
      Lcd_Write_Data(j<<3);
    }
  // digitalWrite(CS,HIGH);  
  HAL_GPIO_WritePin(GPIO_0, CS, 1); 
}

void GPIO_Init(){
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = CS;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = RS;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = RESET;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = LED;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIO_0, CS , 1);
    HAL_GPIO_WritePin(GPIO_0, RS , 1);
    HAL_GPIO_WritePin(GPIO_0, RESET , 1);
    HAL_GPIO_WritePin(GPIO_0, LED , 1);
}

void SystemClock_Config(void)
{
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


static void SPI0_Init(void)
{
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
    hspi0.Init.BaudRateDiv = SPI_BAUDRATE_DIV16;
    //hspi0.Init.BaudRateDiv = 0;
    hspi0.Init.Decoder = SPI_DECODER_NONE;
    hspi0.Init.ManualCS = SPI_MANUALCS_OFF;
    hspi0.Init.ChipSelect = SPI_CS_0;      

    if (HAL_SPI_Init(&hspi0) != HAL_OK)
    {
        xprintf("SPI_Init_Error\n");
    }
}

// #define charOffset                  0x20
// void writeChar(char t, uint8_t dlta){//функция для отрисовки символа t с маштабом dlta
//   Address_set(0,0,8*dlta-1,5*dlta-1);//уст. область рисования для символа(ширина - 8 пикселей, высота 5 пикселей, dlta - коэф. масштаб - сколько пикселей будет отрисовано для каждого пикселя шрифта->эф.масштабирования)
//   uint8_t color[]={250,0,0}; //цвет символа - красный
//   HAL_GPIO_WritePin(GPIO_0, CS, 0);
//   for(int i = 0 ; i < 5;i++){//перебор пикселей символа из таблицы шифров Char_font1. Если бит в табл.=1, то рисуется пиксель заданным цветом, иначе черный
//     for(int d1=0;d1 < dlta;d1++){
//       for(int j = 0 ; j < 8 ; j++){
//         for(int d2=0;d2 < dlta;d2++){
//           if(Char_font1[t - charOffset][i]&(1<<j)){// charOffset = 0x20 определяет смещение для символов в таблице шифров Char_font1
//             Lcd_Write_Data(color[0]);
//             Lcd_Write_Data(color[1]);
//             Lcd_Write_Data(color[2]);
//             }else {
//               Lcd_Write_Data(0);
//               Lcd_Write_Data(0);
//               Lcd_Write_Data(0);
//             }
//         }
//       }
//     }
//   }
//   HAL_GPIO_WritePin(GPIO_0, CS, 1);
// }


// void clearChar(char t, uint8_t dlta){//очищает область экрана, занимаемую символом t с масштабом dlta
//   Address_set(0,0,8*dlta-1,5*dlta-1);
//   // uint8_t color[]={250,0,0};
//   HAL_GPIO_WritePin(GPIO_0, CS, 0);
//   for(int i = 0 ; i < 5;i++){
//     for(int d1=0;d1 < dlta;d1++){
//       for(int j = 0 ; j < 8 ; j++){
//         for(int d2=0;d2 < dlta;d2++){//заполняет область белым цветом
//           Lcd_Write_Data(0xff);
//           Lcd_Write_Data(0xff);
//           Lcd_Write_Data(0xff);
//         }
//       }
//     }
//   }
//   HAL_GPIO_WritePin(GPIO_0, CS, 1);
// }

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

// void PrintMassDMA_RGB565(const uint16_t *mas, uint16_t height, uint16_t width, uint16_t d_l, uint16_t d_w) {
//     uint16_t row;
//     uint16_t *currentBuffer = lineBuffer1;
//     uint16_t *nextBuffer = lineBuffer2;

//     HAL_GPIO_WritePin(GPIO_0, CS, 0); // активируем TFT
//     // HAL_GPIO_WritePin(GPIO_0, CS, 0);//CS в 0
//     // Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, width - 1 + d_l);//область памяти, в которую будут записываться пиксели изображения, с учетом данных смещения
//     // HAL_GPIO_WritePin(GPIO_0, RS, 1);//RS в 1
//     if (!(hspi0.Instance->ENABLE & SPI_ENABLE_M))//включен ли SPI интерфейс
//     {
//         __HAL_SPI_ENABLE(&hspi0);//включаем SPI
//     }

//     for(row = 0; row < height; row++) {
//         // Копируем строку из общего массива в текущий буфер
//         for(uint16_t col = 0; col < width; col++) {
//             currentBuffer[col] = mas[row * width + col];
//         }

//         // Задаём область для записи одной строки
//         Address_set(d_w, row + d_l, d_w + width - 1, row + d_l);
//         HAL_GPIO_WritePin(GPIO_0, RS, 1); // данные

//         // Запускаем DMA для текущей строки
//         HAL_DMA_Start(&hdma_ch0, currentBuffer, (void *)&hspi0.Instance->TXDATA, width*2); // *2 потому что RGB565
//         HAL_DMA_Wait(&hdma_ch0, DMA_TIMEOUT_DEFAULT);

//         // Меняем буферы (двойная буферизация)
//         uint16_t *tmp = currentBuffer;
//         currentBuffer = nextBuffer;
//         nextBuffer = tmp;
//     }

//     HAL_SPI_Disable(&hspi0);
//     HAL_GPIO_WritePin(GPIO_0, CS, 1); // деактивируем TFT
// }

// // вывод изображения в 2 части (верх и низ)
// void PrintMass_Half(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w)
// {
//     uint16_t half = width / 2;  // половина высоты

//     // --- Первая половина (сверху) ---
//     PrintMass(mas, length, half, d_l, d_w);

//     // --- Вторая половина (снизу) ---
//     PrintMass(mas + half*length , length, width - half, d_l, d_w + half);
// }


// /* Вспомогательные: адрес регистра TX данных SPI (используется как периферийный адрес) */
// static void *SPI0_TXDATA_ADDR(void)
// {
//     /* Адрес регистра передачи SPI:
//        в твоем коде использовался hspi0.Instance->TXDATA (доступ к регистру).
//        Передаём адрес этого регистра как void* для DMA. */
//     return (void *)&(hspi0.Instance->TXDATA);
// }

// /* Инициализация канала DMA для SPI -> вызывай один раз в начале (если ещё не сделано) */
// void DMA_CH0_Init_default(void)
// {
//     // предполагаем, что hdma и hdma_ch0 глобальны
//     // Настроим структуру ChannelInit под передачу MEMORY -> PERIPH (SPI TX)
//     hdma_ch0.dma = &hdma;

//     hdma_ch0.ChannelInit.Channel = DMA_CHANNEL_0;
//     hdma_ch0.ChannelInit.Priority = DMA_CHANNEL_PRIORITY_HIGH;

//     // Читаем из памяти
//     hdma_ch0.ChannelInit.ReadMode = DMA_CHANNEL_MODE_MEMORY;
//     hdma_ch0.ChannelInit.ReadInc  = DMA_CHANNEL_INC_ENABLE;
//     hdma_ch0.ChannelInit.ReadSize = DMA_CHANNEL_SIZE_BYTE;
//     hdma_ch0.ChannelInit.ReadAck  = DMA_CHANNEL_ACK_DISABLE;
//     hdma_ch0.ChannelInit.ReadBurstSize = 0; // 2^0 = 1 байт (если нужно пакетная настройка — поправь)
//     hdma_ch0.ChannelInit.ReadRequest = DMA_CHANNEL_SPI_0_REQUEST; // не используется для MEM mode

//     // Пишем в периферию (SPI TX)
//     hdma_ch0.ChannelInit.WriteMode = DMA_CHANNEL_MODE_PERIPHERY;
//     hdma_ch0.ChannelInit.WriteInc = DMA_CHANNEL_INC_DISABLE;
//     hdma_ch0.ChannelInit.WriteSize = DMA_CHANNEL_SIZE_BYTE;
//     hdma_ch0.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;
//     hdma_ch0.ChannelInit.WriteBurstSize = 0;
//     hdma_ch0.ChannelInit.WriteRequest = DMA_CHANNEL_SPI_0_REQUEST;

//     // Инициализация DMA (ядра)
//     HAL_DMA_Init(&hdma);

//     // Назначаем канал
//     HAL_DMA_SetChannel(&hdma_ch0, DMA_CHANNEL_0);

//     // Включаем локальное прерывание при завершении (опционально)
//     HAL_DMA_LocalIRQEnable(&hdma_ch0, DMA_IRQ_ENABLE);
// }

// /* Новая версия PrintMass, использующая DMA. */
// void PrintMass_DMA(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w)
// {
//   LCD_Clear(0xF7E0);
//   HAL_DelayMs(5000);
//     // length — высота (строки)
//     // width  — ширина (пиксели в строке)
//     if (!mas || length == 0 || width == 0) return;

//     // Буфер для одной строки в формате RGB888 (3 байта на пиксель)
//     // Выделим динамически один раз (максимум width*3 байт)
//     uint32_t row_bytes = (uint32_t)width * 3U;
//     uint8_t *row_buf = (uint8_t *)malloc(row_bytes);
//     if (!row_buf) {
//         // Не удалось выделить — упадём на старый метод (без DMA) или возвращаем ошибку
//         LCD_Clear(0xF700);
//         HAL_DelayMs(5000);
//         return;
//     }

//     // Убедимся, что SPI включён
//     if (!(hspi0.Instance->ENABLE & SPI_ENABLE_M)) {
//         __HAL_SPI_ENABLE(&hspi0);
//         LCD_Clear(0xE7E0);
//         HAL_DelayMs(5000);
//     }

//     // Устанавливаем область памяти в дисплее (Address_set) — как у тебя в оригинале:
//     // Область: (0+d_w, 0+d_l)-(width-1+d_w, length-1+d_l)
//     HAL_GPIO_WritePin(GPIO_0, CS, 0); // CS в 0
//     Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, length - 1 + d_l);
//     HAL_GPIO_WritePin(GPIO_0, CS, 1); // поднять CS, данные будем отправлять по строкам
// // Перебираем строки (m = 0..length-1)
  
//     for (uint16_t m = 0; m < length; ++m)
//     {
//         // Заполняем row_buf: каждую точку mas[m*width + i] -> RGB888 3 байта
//         for (uint16_t i = 0; i < width; ++i)
//         {
//             uint16_t pix = mas[(uint32_t)m * width + i]; // RGB565

//             // извлекаем компоненты
//             // R5 = bits 11..15, G6 = bits 5..10, B5 = bits 0..4
//             uint8_t r5 = (pix >> 11) & 0x1F;
//             uint8_t g6 = (pix >> 5) & 0x3F;
//             uint8_t b5 = (pix) & 0x1F;

//             // масштабируем в 8-бит?
//             // лучше делать: r8 = (r5 * 255) / 31  -> эквивалентно (r5 << 3) | (r5 >> 2)
//             uint8_t r8 = (r5 << 3) | (r5 >> 2);
//             uint8_t g8 = (g6 << 2) | (g6 >> 4);
//             uint8_t b8 = (b5 << 3) | (b5 >> 2);

//             // Порядок байт у тебя в оригинале — colout[0]=G, colout[1]=B, colout[2]=R (тcolout[2]=red, colout[0]=green, colout[1]=blue).
//             // Чтобы сохранить совместимость — положим так же: [G, B, R]
//             uint32_t idx = (uint32_t)i * 3;
//             row_buf[idx + 0] = g8; // green
//             row_buf[idx + 1] = b8; // blue
//             row_buf[idx + 2] = r8; // red
//         }

//         // Передача строки через DMA:
//         // Устанавливаем RS/DC = 1 (данные)
//         HAL_GPIO_WritePin(GPIO_0, RS, 1);

//         // CS низкий перед отправкой строки
//         HAL_GPIO_WritePin(GPIO_0, CS, 0);

//         // Запуск DMA: читаем из row_buf (память), пишем в SPI0 TXDATA (периферия)
//         // len в байтах
//         uint32_t len_bytes = row_bytes;

//         // HAL_DMA_Start(hdma_channel, Source, Destination, Len)
//         HAL_DMA_Start(&hdma_ch0, (void *)row_buf, SPI0_TXDATA_ADDR(), len_bytes);

//         // Ожидаем завершения (блокируем)
//         HAL_DMA_Wait(&hdma_ch0, DMA_TIMEOUT_DEFAULT);

//         // После завершения — снимаем CS
//         HAL_GPIO_WritePin(GPIO_0, CS, 1);
//     }

//     free(row_buf);
// }

/* --- Пример инициализации перед использованием ---
   Вызывай в main после HAL_Init() и перед PrintMass_DMA:
     DMA_CH0_Init_default();
   Убедись также, что SPI0 инициализирован (SPI0_Init()).
*/
// void PrintMassDMA(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w ){
//   uint8_t i,m;
//   uint8_t colour1[3];
//   HAL_GPIO_WritePin(GPIO_0, CS, 0);
//   Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, length - 1 + d_l);
//   HAL_GPIO_WritePin(GPIO_0, RS, 1);
//   xprintf("start\n");
//   for(m=0;m<length;m++){
//     for(i=0;i<width;i++)
//     {
//       colour1[0] = (*(mas+i*length - 1 + m)>>8)&0xF8;
//       colour1[1] = (*(mas+i*length - 1 + m)>>3)&0xFC;
//       colour1[2] = (*(mas+i*length - 1 + m))<<3;
//       HAL_SPI_Enable(&hspi0);
//       HAL_DMA_Start(&hdma_ch0, &colour1, (void *)&hspi0.Instance->TXDATA, 3);
//       HAL_SPI_Disable(&hspi0);
//     }  
//   }
//   xprintf("end\n");
//   HAL_GPIO_WritePin(GPIO_0, CS, 1); 
// }

// void PrintMassDMA16(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w){
//   HAL_GPIO_WritePin(GPIO_0, CS, 0);
//   Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, length - 1 + d_l);
//   HAL_GPIO_WritePin(GPIO_0, RS, 1);
//   HAL_SPI_Enable(&hspi0);
//   HAL_DMA_Start(&hdma_ch0, mas, (void *)&hspi0.Instance->TXDATA, length * width);
//   HAL_SPI_Disable(&hspi0);
//   HAL_GPIO_WritePin(GPIO_0, CS, 1); 
// }


/*
  uint8_t out[] = {d};
  uint8_t in[sizeof(out)];
  HAL_SPI_Exchange(&hspi0, out, in, sizeof(out),SPI_TIMEOUT_DEFAULT);
  */ 

// void ClearMass(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w ){//очищаетобласть экрана, заполняя ее одним цветом
//   HAL_GPIO_WritePin(GPIO_0, CS, 0);
//   Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, length - 1 + d_l);
//   HAL_GPIO_WritePin(GPIO_0, RS, 1);
//   uint32_t count = length*width*3 - 1;// общ. кол-во байт, которые нужно отправить на LCD(3 байта на пиксель)
//   hspi0.ErrorCode = HAL_SPI_ERROR_NONE;//сбрасывает код ошибки SPI

//   hspi0.Instance->TX_THR = 1;//уст. порог FIFO передатчика SPI в 1, т.е. прерывание будет ген., когда в FIFO есть хотя бы одно свободное место
//   /* Включить SPI если выключено */
//   if (!(hspi0.Instance->ENABLE & SPI_ENABLE_M))
//   {
//       __HAL_SPI_ENABLE(&hspi0);
//   }
//   while (count != 0 )//отправляет байты на LCD, пока счётчик не достигнет 0
//   {
//     /* Проверка флага TX_FIFO_NOT_FULL */
//     // xprintf("0\n");
//     if ((hspi0.Instance->INT_STATUS & SPI_INT_STATUS_TX_FIFO_NOT_FULL_M))
//     {
//         hspi0.Instance->TXDATA = 0x0F;
//         count--;
//         // xprintf("1\n");
//     }
//     // xprintf("0\n");
//   }
//   if (!(hspi0.Instance->CONFIG & SPI_CONFIG_MANUAL_CS_M))
//   {
//       __HAL_SPI_DISABLE(&hspi0);
//       hspi0.Instance->ENABLE |= SPI_ENABLE_CLEAR_TX_FIFO_M | SPI_ENABLE_CLEAR_RX_FIFO_M; /* Очистка буферов RX и TX */
//   }

//   volatile uint32_t unused = hspi0.Instance->INT_STATUS; /* Очистка флагов ошибок чтением */
//   (void) unused;

//   HAL_GPIO_WritePin(GPIO_0, CS, 1); 
// }

// void ClearMassDMA(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w ){//использует DMA для очистки области экрана белым цветом?(DMA запускается на каждом пикселе)
//   uint8_t i,m;
//   uint8_t colour1[3] = {0xff,0xff, 0xff};
//   HAL_GPIO_WritePin(GPIO_0, CS, 0);
//   Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, length - 1 + d_l);
//   HAL_GPIO_WritePin(GPIO_0, RS, 1);
//   xprintf("start\n");
  
//   for(m=0;m<length;m++){
//     for(i=0;i<width;i++)
//     {
//       HAL_SPI_Enable(&hspi0);
//       HAL_DMA_Start(&hdma_ch0, &colour1, (void *)&hspi0.Instance->TXDATA, 3);  
//       HAL_SPI_Disable(&hspi0);
//     }  
//   }
  
//   xprintf("end\n");
//   HAL_GPIO_WritePin(GPIO_0, CS, 1); 
// }

int main()
{
    //SystemClock_Config();
    //UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M | UART_CONTROL1_RE_M, 0, 0);
    // xprintf("start\n");
    SPI0_Init();
    GPIO_Init();
    Lcd_Init();
    LCD_Clear(0xff83);
    //LCD_Clear(0xaf83);
    //PrintMass(&img[0],479, 319, 0, 0 );
    //PrintMass(&image[0],480, 320, 0, 0 );

    //DMA_Init();
    // PrintMassDMA16(&mikron[0],477, 165, 0, 100);

    // ClearMass(480, 320, 0 , 0);
    // PrintMass(&mas[0],16, 9, 100, 100);
    // PrintMass(&mikron[0], 477, 165, 0, 100);
    // PrintMass(&mik23[0],480, 200, 0, 50 );
    // PrintMass(&mas[0], 16, 3, 100, 100 );
    // printString("djkjsd",6, 0,0,3);
    // writeChar('i', 10);
    while (1)
    {
      // PrintMass(&mikron[0],479, 319, 0, 0 );
      // HAL_DelayMs(5000);
      // PrintMass(&image[0],480, 320, 0, 0 );
      // HAL_DelayMs(5000);
      //LCD_Clear(0xF7E0);//?
      //HAL_DelayMs(2000);
      //PrintMass(&mik23[0],479, 319, 0, 0 );
      //HAL_DelayMs(5000);
      //PrintMass(&board[0],479, 319, 0, 0 );
      //HAL_DelayMs(5000);
      // write_paje();
      // LCD_Clear(0xffff);
      // LCD_Clear(0x001f);
      // LCD_Clear(0xf000); 
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
    hdma_ch0.ChannelInit.WriteBurstSize = 0;                /* write_burst_size должно быть кратно read_size */
    hdma_ch0.ChannelInit.WriteRequest = DMA_CHANNEL_SPI_0_REQUEST;
    hdma_ch0.ChannelInit.WriteAck = DMA_CHANNEL_ACK_DISABLE;
}
