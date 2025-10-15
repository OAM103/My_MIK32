#include "lcd.h"
#include "system_config.h"
#include "mik32_hal.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_spi.h"
#include "mik32_hal_lcd5110_bitmaps.h"
#include "xprintf.h"

extern SPI_HandleTypeDef hspi0;

#define charOffset   0x20

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
    if(x1>x2){uint16_t tmp=x1;x1=x2;x2=tmp;}
    if(y1>y2){uint16_t tmp=y1;y1=y2;y2=tmp;}
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
    Lcd_Write_Data(0x66); //0x66-18bit(RGB666), 0x55-16bit(RGB565), 0x77-24bit(RGB888)

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
    Address_set(0, 0, 480-1, 320-1); //размер экрана
    for (i = 0; i < 320; i++)
        for (m = 0; m < 480; m++) {
            Lcd_Write_Data((j >> 8) & 0xF8);
            Lcd_Write_Data((j >> 3) & 0xFC);
            Lcd_Write_Data(j << 3);
        }
    // digitalWrite(CS,HIGH);
    HAL_GPIO_WritePin(GPIO_0, CS, 1);
}


void writeChar(char t, uint8_t dlta){//функция для отрисовки символа t с маштабом dlta
  Address_set(0,0,8*dlta-1,5*dlta-1);//уст. область рисования для символа(ширина - 8 пикселей, высота 5 пикселей, dlta - коэф. масштаб - сколько пикселей будет отрисовано для каждого пикселя шрифта->эф.масштабирования)
  uint8_t color[]={250,0,0}; //цвет символа - красный
  Lcd_select();
  for(int i = 0 ; i < 5;i++){//перебор пикселей символа из таблицы шифров Char_font1. Если бит в табл.=1, то рисуется пиксель заданным цветом, иначе черный
    for(int d1=0;d1 < dlta;d1++){
      for(int j = 0 ; j < 8 ; j++){
        for(int d2=0;d2 < dlta;d2++){
          if(Char_font1[t - charOffset][i]&(1<<j)){// charOffset = 0x20 определяет смещение для символов в таблице шифров Char_font1
            Lcd_Write_Data(color[0]);
            Lcd_Write_Data(color[1]);
            Lcd_Write_Data(color[2]);
            }else {
              Lcd_Write_Data(0);
              Lcd_Write_Data(0);
              Lcd_Write_Data(0);
            }
        }
      }
    }
  }
  Lcd_unselect();
}

void clearChar(char t, uint8_t dlta){//очищает область экрана, занимаемую символом t с масштабом dlta
  Address_set(0,0,8*dlta-1,5*dlta-1);
  // uint8_t color[]={250,0,0};
  Lcd_select();
  for(int i = 0 ; i < 5;i++){
    for(int d1=0;d1 < dlta;d1++){
      for(int j = 0 ; j < 8 ; j++){
        for(int d2=0;d2 < dlta;d2++){//заполняет область белым цветом
          Lcd_Write_Data(0xff);
          Lcd_Write_Data(0xff);
          Lcd_Write_Data(0xff);
        }
      }
    }
  }
  Lcd_unselect();
}

void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) //Рисует линию горизонтально                 
{	
  unsigned int i,j;
  // digitalWrite(CS,LOW);
  Lcd_select();
  //Lcd_Write_Com(0x02c); //write_memory_start?
  //digitalWrite(RS,HIGH);
  l=l+x;
  Address_set(x,y,l,y);//область рисования линии
  j=l*2;
  for(i=1;i<=j;i++)//отправляет пиксельные данные на lcd
  {
      //выделяют и сдвигают нужные биты для каждого цветового канала
      Lcd_Write_Data((c>>8)&0xF8);
      Lcd_Write_Data((c>>3)&0xFC);
      Lcd_Write_Data(c<<3);
  }
  // digitalWrite(CS,HIGH);   
  Lcd_unselect();//деактивируем lcd(cs=1)
}

void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) //Рисует вертикальные линии                 
{	
  unsigned int i,j;
  // digitalWrite(CS,LOW);
  Lcd_select();
  Lcd_Write_Com(0x02c); //write_memory_start
  //digitalWrite(RS,HIGH);
  l=l+y;
  Address_set(x,y,x,l);
  j=l*2;
  for(i=1;i<=j;i++)
  { 
      Lcd_Write_Data((c>>8)&0xF8);
      Lcd_Write_Data((c>>3)&0xFC);
      Lcd_Write_Data(c<<3);
  }
  // digitalWrite(CS,HIGH);   
  Lcd_unselect();
}

void Rect(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)//РИсует прямоугольник (x,y)-верхний левый угол, w-ширина, h-высота, с-цвет границы
{
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}

void Rectf(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)//Рисует заполненный прямоугольник ?(не эффективно) 
{
  for(unsigned int i=0;i<h;i++)
  {
    H_line(x  , y+i, w, c);
  }
}

// void ClearMass(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w) { //очищаетобласть экрана, заполняя ее одним цветом
//     Lcd_select();
//     Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, length - 1 + d_l);
//     HAL_GPIO_WritePin(GPIO_0, RS, 1);
//     uint32_t count = length * width * 3 - 1; // общ. кол-во байт, которые нужно отправить на LCD(3 байта на пиксель)
//     hspi0.ErrorCode = HAL_SPI_ERROR_NONE; //сбрасывает код ошибки SPI

//     hspi0.Instance->TX_THR = 1; //уст. порог FIFO передатчика SPI в 1, т.е. прерывание будет ген., когда в FIFO есть хотя бы одно свободное место
//     /* Включить SPI если выключено */
//     if (!(hspi0.Instance->ENABLE & SPI_ENABLE_M)) {
//         __HAL_SPI_ENABLE(&hspi0);
//     }
//     while (count != 0) //отправляет байты на LCD, пока счётчик не достигнет 0
//     {
//         /* Проверка флага TX_FIFO_NOT_FULL */
//         // xprintf("0\n");
//         if ((hspi0.Instance->INT_STATUS & SPI_INT_STATUS_TX_FIFO_NOT_FULL_M)) {
//             hspi0.Instance->TXDATA = 0xFF;
//             count--;
//             // xprintf("1\n");
//         }
//         // xprintf("0\n");
//     }
//     if (!(hspi0.Instance->CONFIG & SPI_CONFIG_MANUAL_CS_M)) {
//         __HAL_SPI_DISABLE(&hspi0);
//         hspi0.Instance->ENABLE |= SPI_ENABLE_CLEAR_TX_FIFO_M | SPI_ENABLE_CLEAR_RX_FIFO_M; /* Очистка буферов RX и TX */
//     }

//     volatile uint32_t unused = hspi0.Instance->INT_STATUS; /* Очистка флагов ошибок чтением */
//     (void)unused;

//     Lcd_unselect();
// }
void ClearMass(uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t color) {//как получаются цвета?
    Lcd_select();
    Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, length - 1 + d_l);
    HAL_GPIO_WritePin(GPIO_0, RS, 1);
    uint32_t count = length * width; // Кол-во пикселей, а не байт, потому что мы отправляем 3 байта за раз
    hspi0.ErrorCode = HAL_SPI_ERROR_NONE;
    hspi0.Instance->TX_THR = 1;

    // Извлекаем компоненты цвета (предполагаем RGB565)
    uint8_t red   = (color >> 8) & 0xF8;
    uint8_t green = (color >> 3) & 0xFC;
    uint8_t blue  = (color << 3);

    if (!(hspi0.Instance->ENABLE & SPI_ENABLE_M)) {
        __HAL_SPI_ENABLE(&hspi0);
    }
    while (count != 0) {
        if ((hspi0.Instance->INT_STATUS & SPI_INT_STATUS_TX_FIFO_NOT_FULL_M)) {
            hspi0.Instance->TXDATA = red;   // Отправляем красный
            hspi0.Instance->TXDATA = green; // Отправляем зеленый
            hspi0.Instance->TXDATA = blue;  // Отправляем синий
            count--;
        }
    }
    if (!(hspi0.Instance->CONFIG & SPI_CONFIG_MANUAL_CS_M)) {
        __HAL_SPI_DISABLE(&hspi0);
        hspi0.Instance->ENABLE |= SPI_ENABLE_CLEAR_TX_FIFO_M | SPI_ENABLE_CLEAR_RX_FIFO_M;
    }
    volatile uint32_t unused = hspi0.Instance->INT_STATUS;
    (void)unused;
    Lcd_unselect();
}
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
        // запуск DMA на всю строку (width*3 байт-1)
        HAL_DMA_Start(&hdma_ch0, (void*)dma_buffer, (void*)&hspi0.Instance->TXDATA, width*3-1);
        // ожидание завершения DMA
        HAL_DMA_Wait(&hdma_ch0, DMA_TIMEOUT_DEFAULT); 
    }
    HAL_SPI_Disable(&hspi0);
    Lcd_unselect();
}
void PrintMassDMA_DoubleBuffer(const uint16_t *mas,uint16_t length,uint16_t width,uint16_t d_l,uint16_t d_w)
{
    HAL_SPI_Enable(&hspi0);
    Lcd_select();

    // Настройка области вывода
    Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, length - 1 + d_l);
    HAL_GPIO_WritePin(GPIO_0, RS, 1); // RS = Data

    // Два рабочих буфера
    static uint8_t bufA[DMA_BUFFER_SIZE];
    static uint8_t bufB[DMA_BUFFER_SIZE];
    uint8_t *active = bufA;
    uint8_t *next = bufB;

    // --- подготовить первую строку ---
    for (uint16_t i = 0; i < width; i++) {
        uint16_t color = *(mas + i * length - 1); // твоя формула без изменений
        active[i * 3 + 2] = (color >> 8) & 0xF8;  // R
        active[i * 3 + 0] = (color >> 3) & 0xFC;  // G
        active[i * 3 + 1] = (color << 3);         // B
    }

    // --- запустить DMA первой строки ---
    HAL_DMA_Start(&hdma_ch0, (void *)active, (void *)&hspi0.Instance->TXDATA, width * 3-1);

    // --- последующие строки ---
    for (uint16_t m = 1; m < length; m++) {

        // подготовка следующей строки, пока DMA занят
        for (uint16_t i = 0; i < width; i++) {
            uint16_t color = *(mas + i * length - 1 + m);
            next[i * 3 + 2] = (color >> 8) & 0xF8;
            next[i * 3 + 0] = (color >> 3) & 0xFC;
            next[i * 3 + 1] = (color << 3);
        }

        // дождаться окончания предыдущего DMA
        HAL_DMA_Wait(&hdma_ch0, DMA_TIMEOUT_DEFAULT);

        // запустить следующий DMA
        HAL_DMA_Start(&hdma_ch0, (void *)next, (void *)&hspi0.Instance->TXDATA, width * 3-1);

        // обмен буферов
        uint8_t *tmp = active;
        active = next;
        next = tmp;
    }

    // дождаться окончания последней строки
    HAL_DMA_Wait(&hdma_ch0, DMA_TIMEOUT_DEFAULT);

    HAL_SPI_Disable(&hspi0);
    Lcd_unselect();
}

// void PrintMassDMA_Optimized(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w, uint16_t char_color, uint16_t background_color) {

//     HAL_SPI_Enable(&hspi0);
//     Lcd_select();
//     Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, length - 1 + d_l); // область памяти, в которую будут записываться пиксели изображения, с учетом данных смещения
//     HAL_GPIO_WritePin(GPIO_0, RS, 1); // RS в 1

//     for (uint16_t m = 0; m < length; m++) {   // строки
//         // собрать строку в буфер
//         for (uint16_t i = 0; i < width; i++) {
//             uint16_t pixel = *(mas + i*length - 1 + m);  // Получаем значение пикселя из изображения

//             if (pixel != 0x0000) { // Если пиксель не черный (прозрачный) - это часть символа
//                 // Устанавливаем цвет символа
//                 dma_buffer[i*3 + 2] = ((char_color >> 8) & 0xF8);   // Red
//                 dma_buffer[i*3 + 0] = ((char_color >> 3) & 0xFC);   // Green
//                 dma_buffer[i*3 + 1] = (char_color << 3);          // Blue
//             } else {
//                 // Если пиксель черный (прозрачный) - оставляем цвет фона
//                 dma_buffer[i*3 + 2] = ((background_color >> 8) & 0xF8);   // Red
//                 dma_buffer[i*3 + 0] = ((background_color >> 3) & 0xFC);   // Green
//                 dma_buffer[i*3 + 1] = (background_color << 3);          // Blue
//             }
//         }
//         // ожидание завершения DMA
//         HAL_DMA_Wait(&hdma_ch0, DMA_TIMEOUT_DEFAULT);
//         // запуск DMA на всю строку (width*3 байт)
//         HAL_DMA_Start(&hdma_ch0, (void*)dma_buffer, (void*)&hspi0.Instance->TXDATA, width*3-1);
//     }
//     HAL_SPI_Disable(&hspi0);
//     Lcd_unselect();
// }
void PrintMassDMA_Optimized(const uint16_t *mas, uint16_t length, uint16_t width, uint16_t d_l, uint16_t d_w) {

    HAL_SPI_Enable(&hspi0);
    Lcd_select();
    Address_set(0 + d_w, 0 + d_l, width - 1 + d_w, length - 1 + d_l);//область памяти, в которую будут записываться пиксели изображения, с учетом данных смещения
    HAL_GPIO_WritePin(GPIO_0, RS, 1);//RS в 1

    for (uint16_t m = 0; m < length; m++) {   // строки
        // собрать строку в буфер
        for (uint16_t i = 0; i < width; i++) {
            uint16_t pixel = *(mas + i*length - 1 + m);  // Получаем значение пикселя из изображения

            if (pixel != 0x0000) { // Если пиксель не черный (прозрачный)
              // Вычисляем цвет символа
              dma_buffer[i*3 + 2] = (*(mas + i*length - 1 + m) >> 8) & 0xF8;   
              dma_buffer[i*3 + 0] = (*(mas + i*length - 1 + m) >> 3) & 0xFC;  
              dma_buffer[i*3 + 1] = (*(mas + i*length - 1 + m) << 3);
            } 
            else {
              dma_buffer[i*3 + 2] = 0;   // Red
              dma_buffer[i*3 + 0] = 0;   // Green
              dma_buffer[i*3 + 1] = 0;   // Blue
            }
        }
        HAL_DMA_Start(&hdma_ch0, (void*)dma_buffer, (void*)&hspi0.Instance->TXDATA, width*3-1);
        HAL_DMA_Wait(&hdma_ch0, DMA_TIMEOUT_DEFAULT);
    }
    HAL_SPI_Disable(&hspi0);
    Lcd_unselect();
}

void scrollImageLoop(const uint16_t *image, uint16_t height, uint16_t width) {
    uint16_t N = 0;

    PrintMassDMA(image, height, width, N, 0);

    N+=5;
    if (N == height) {
        N = 480; // начать заново
    }
}
