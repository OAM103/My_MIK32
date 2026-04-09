#include "system_config.h"
#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal.h"
#include "mik32_hal_timer32.h"
#include "mik32_hal_dac.h"
#include "mik32_hal_irq.h"
#include "lcd.h"
#include "frames.h"
#include "music.h"
#include "tracks.h"
#include "uart_lib.h"
#include "xprintf.h"

#define BG_COLOR 0x0000
#define width 480
#define height 320

FrameRLE_t frames[] = {
    {&frame_00000[0], sizeof(frame_00000)},
    {&frame_00001[0], sizeof(frame_00001)},
    {&frame_00002[0], sizeof(frame_00002)},
    {&frame_00003[0], sizeof(frame_00003)},
    {&frame_00004[0], sizeof(frame_00004)},
    {&frame_00005[0], sizeof(frame_00005)},
    {&frame_00006[0], sizeof(frame_00006)},
    {&frame_00007[0], sizeof(frame_00007)},
    {&frame_00008[0], sizeof(frame_00008)},
    {&frame_00009[0], sizeof(frame_00009)},
    {&frame_00010[0], sizeof(frame_00010)},
    {&frame_00011[0], sizeof(frame_00011)},
    {&frame_00012[0], sizeof(frame_00012)},
    {&frame_00013[0], sizeof(frame_00013)},
    {&frame_00014[0], sizeof(frame_00014)},
    {&frame_00015[0], sizeof(frame_00015)},
    {&frame_00016[0], sizeof(frame_00016)},
    {&frame_00017[0], sizeof(frame_00017)},
    {&frame_00018[0], sizeof(frame_00018)},
    {&frame_00019[0], sizeof(frame_00019)},
    {&frame_00020[0], sizeof(frame_00020)},
    {&frame_00021[0], sizeof(frame_00021)},
    {&frame_00022[0], sizeof(frame_00022)},
    {&frame_00023[0], sizeof(frame_00023)},
    {&frame_00024[0], sizeof(frame_00024)},
    {&frame_00025[0], sizeof(frame_00025)},
    {&frame_00026[0], sizeof(frame_00026)},
    {&frame_00027[0], sizeof(frame_00027)},
    {&frame_00028[0], sizeof(frame_00028)},
    {&frame_00029[0], sizeof(frame_00029)},
    {&frame_00030[0], sizeof(frame_00030)},
    {&frame_00031[0], sizeof(frame_00031)},
    {&frame_00032[0], sizeof(frame_00032)},
    {&frame_00033[0], sizeof(frame_00033)},
    {&frame_00034[0], sizeof(frame_00034)},
    {&frame_00035[0], sizeof(frame_00035)},
    {&frame_00036[0], sizeof(frame_00036)},
    {&frame_00037[0], sizeof(frame_00037)},
    {&frame_00038[0], sizeof(frame_00038)},
    {&frame_00039[0], sizeof(frame_00039)},
    {&frame_00040[0], sizeof(frame_00040)},
    {&frame_00041[0], sizeof(frame_00041)},
    {&frame_00042[0], sizeof(frame_00042)},
    {&frame_00043[0], sizeof(frame_00043)},
    {&frame_00044[0], sizeof(frame_00044)},
    {&frame_00045[0], sizeof(frame_00045)},
    {&frame_00046[0], sizeof(frame_00046)},
    {&frame_00047[0], sizeof(frame_00047)},
    {&frame_00048[0], sizeof(frame_00048)},
    {&frame_00049[0], sizeof(frame_00049)},
    {&frame_00050[0], sizeof(frame_00050)},
    {&frame_00051[0], sizeof(frame_00051)},
    {&frame_00052[0], sizeof(frame_00052)},
    {&frame_00053[0], sizeof(frame_00053)},
    {&frame_00054[0], sizeof(frame_00054)},
    {&frame_00055[0], sizeof(frame_00055)},
    {&frame_00056[0], sizeof(frame_00056)},
    {&frame_00057[0], sizeof(frame_00057)},
    {&frame_00058[0], sizeof(frame_00058)},
    {&frame_00059[0], sizeof(frame_00059)},
    {&frame_00060[0], sizeof(frame_00060)},
    {&frame_00061[0], sizeof(frame_00061)},
    {&frame_00062[0], sizeof(frame_00062)},
    {&frame_00063[0], sizeof(frame_00063)},
    {&frame_00064[0], sizeof(frame_00064)},
    {&frame_00065[0], sizeof(frame_00065)},
    {&frame_00066[0], sizeof(frame_00066)},
    {&frame_00067[0], sizeof(frame_00067)},
    {&frame_00068[0], sizeof(frame_00068)},
    {&frame_00069[0], sizeof(frame_00069)},
    {&frame_00070[0], sizeof(frame_00070)},
    {&frame_00071[0], sizeof(frame_00071)},
    {&frame_00072[0], sizeof(frame_00072)},
    {&frame_00073[0], sizeof(frame_00073)},
    {&frame_00074[0], sizeof(frame_00074)},
    {&frame_00075[0], sizeof(frame_00075)},
    {&frame_00076[0], sizeof(frame_00076)},
    {&frame_00077[0], sizeof(frame_00077)},
    {&frame_00078[0], sizeof(frame_00078)},
    {&frame_00079[0], sizeof(frame_00079)},
    {&frame_00080[0], sizeof(frame_00080)},
    {&frame_00081[0], sizeof(frame_00081)},
    {&frame_00082[0], sizeof(frame_00082)},
    {&frame_00083[0], sizeof(frame_00083)},
    {&frame_00084[0], sizeof(frame_00084)},
    {&frame_00085[0], sizeof(frame_00085)},
    {&frame_00086[0], sizeof(frame_00086)},
    {&frame_00087[0], sizeof(frame_00087)},
    {&frame_00088[0], sizeof(frame_00088)},
    {&frame_00089[0], sizeof(frame_00089)},
    {&frame_00090[0], sizeof(frame_00090)},
    {&frame_00091[0], sizeof(frame_00091)},
    {&frame_00092[0], sizeof(frame_00092)},
    {&frame_00093[0], sizeof(frame_00093)},
    {&frame_00094[0], sizeof(frame_00094)},
    {&frame_00095[0], sizeof(frame_00095)},
    {&frame_00096[0], sizeof(frame_00096)},
    {&frame_00097[0], sizeof(frame_00097)},
    {&frame_00098[0], sizeof(frame_00098)},
    {&frame_00099[0], sizeof(frame_00099)},
    {&frame_00100[0], sizeof(frame_00100)},
    {&frame_00101[0], sizeof(frame_00101)},
    {&frame_00102[0], sizeof(frame_00102)},
    {&frame_00103[0], sizeof(frame_00103)},
    {&frame_00104[0], sizeof(frame_00104)},
    {&frame_00105[0], sizeof(frame_00105)},
    {&frame_00106[0], sizeof(frame_00106)},
    {&frame_00107[0], sizeof(frame_00107)},
    {&frame_00108[0], sizeof(frame_00108)},
    {&frame_00109[0], sizeof(frame_00109)},
    {&frame_00110[0], sizeof(frame_00110)},
    {&frame_00111[0], sizeof(frame_00111)},
    {&frame_00112[0], sizeof(frame_00112)},
    {&frame_00113[0], sizeof(frame_00113)},
    {&frame_00114[0], sizeof(frame_00114)},
    {&frame_00115[0], sizeof(frame_00115)},
    {&frame_00116[0], sizeof(frame_00116)},
    {&frame_00117[0], sizeof(frame_00117)},
    {&frame_00118[0], sizeof(frame_00118)},
    {&frame_00119[0], sizeof(frame_00119)},
    {&frame_00120[0], sizeof(frame_00120)},
    {&frame_00121[0], sizeof(frame_00121)},
    {&frame_00122[0], sizeof(frame_00122)},
    {&frame_00123[0], sizeof(frame_00123)},
    {&frame_00124[0], sizeof(frame_00124)},
    {&frame_00125[0], sizeof(frame_00125)},
    {&frame_00126[0], sizeof(frame_00126)},
    {&frame_00127[0], sizeof(frame_00127)},
    {&frame_00128[0], sizeof(frame_00128)},
    {&frame_00129[0], sizeof(frame_00129)},
    {&frame_00130[0], sizeof(frame_00130)},
    {&frame_00131[0], sizeof(frame_00131)},
    {&frame_00132[0], sizeof(frame_00132)},
    {&frame_00133[0], sizeof(frame_00133)},
    {&frame_00134[0], sizeof(frame_00134)},
    {&frame_00135[0], sizeof(frame_00135)},
    {&frame_00136[0], sizeof(frame_00136)},
    {&frame_00137[0], sizeof(frame_00137)},
    {&frame_00138[0], sizeof(frame_00138)},
    {&frame_00139[0], sizeof(frame_00139)},
    {&frame_00140[0], sizeof(frame_00140)},
    {&frame_00141[0], sizeof(frame_00141)},
    {&frame_00142[0], sizeof(frame_00142)},
    {&frame_00143[0], sizeof(frame_00143)},
    {&frame_00144[0], sizeof(frame_00144)},
    {&frame_00145[0], sizeof(frame_00145)},
    {&frame_00146[0], sizeof(frame_00146)},
    {&frame_00147[0], sizeof(frame_00147)},
    {&frame_00148[0], sizeof(frame_00148)},
    {&frame_00149[0], sizeof(frame_00149)},
    {&frame_00150[0], sizeof(frame_00150)},
    {&frame_00151[0], sizeof(frame_00151)},
    {&frame_00152[0], sizeof(frame_00152)},
    {&frame_00153[0], sizeof(frame_00153)},
    {&frame_00154[0], sizeof(frame_00154)},
    {&frame_00155[0], sizeof(frame_00155)},
    {&frame_00156[0], sizeof(frame_00156)},
    {&frame_00157[0], sizeof(frame_00157)},
    {&frame_00158[0], sizeof(frame_00158)},
    {&frame_00159[0], sizeof(frame_00159)},
    {&frame_00160[0], sizeof(frame_00160)},
    {&frame_00161[0], sizeof(frame_00161)},
    {&frame_00162[0], sizeof(frame_00162)},
    {&frame_00163[0], sizeof(frame_00163)},
    {&frame_00164[0], sizeof(frame_00164)},
    {&frame_00165[0], sizeof(frame_00165)},
    {&frame_00166[0], sizeof(frame_00166)},
    {&frame_00167[0], sizeof(frame_00167)},
    {&frame_00168[0], sizeof(frame_00168)},
    {&frame_00169[0], sizeof(frame_00169)},
    {&frame_00170[0], sizeof(frame_00170)},
    {&frame_00171[0], sizeof(frame_00171)},
    {&frame_00172[0], sizeof(frame_00172)},
    {&frame_00173[0], sizeof(frame_00173)},
    {&frame_00174[0], sizeof(frame_00174)},
    {&frame_00175[0], sizeof(frame_00175)},
    {&frame_00176[0], sizeof(frame_00176)},
    {&frame_00177[0], sizeof(frame_00177)},
    {&frame_00178[0], sizeof(frame_00178)},
    {&frame_00179[0], sizeof(frame_00179)},
    {&frame_00180[0], sizeof(frame_00180)},
    {&frame_00181[0], sizeof(frame_00181)},
    {&frame_00182[0], sizeof(frame_00182)},
    {&frame_00183[0], sizeof(frame_00183)},
    {&frame_00184[0], sizeof(frame_00184)},
    {&frame_00185[0], sizeof(frame_00185)},
    {&frame_00186[0], sizeof(frame_00186)},
    {&frame_00187[0], sizeof(frame_00187)},
    {&frame_00188[0], sizeof(frame_00188)},
    {&frame_00189[0], sizeof(frame_00189)},
    {&frame_00190[0], sizeof(frame_00190)},
    {&frame_00191[0], sizeof(frame_00191)},
    {&frame_00192[0], sizeof(frame_00192)},
    {&frame_00193[0], sizeof(frame_00193)},
    {&frame_00194[0], sizeof(frame_00194)},
    {&frame_00195[0], sizeof(frame_00195)},
    {&frame_00196[0], sizeof(frame_00196)},
    {&frame_00197[0], sizeof(frame_00197)},
    {&frame_00198[0], sizeof(frame_00198)},
    {&frame_00199[0], sizeof(frame_00199)},
    {&frame_00200[0], sizeof(frame_00200)},
    {&frame_00201[0], sizeof(frame_00201)},
    {&frame_00202[0], sizeof(frame_00202)},
    {&frame_00203[0], sizeof(frame_00203)},
    {&frame_00204[0], sizeof(frame_00204)},
    {&frame_00205[0], sizeof(frame_00205)},
    {&frame_00206[0], sizeof(frame_00206)},
    {&frame_00207[0], sizeof(frame_00207)},
    {&frame_00208[0], sizeof(frame_00208)},
    {&frame_00209[0], sizeof(frame_00209)},
    {&frame_00210[0], sizeof(frame_00210)},
    {&frame_00211[0], sizeof(frame_00211)},
    {&frame_00212[0], sizeof(frame_00212)},
    {&frame_00213[0], sizeof(frame_00213)},
    {&frame_00214[0], sizeof(frame_00214)},
    {&frame_00215[0], sizeof(frame_00215)},
    {&frame_00216[0], sizeof(frame_00216)},
    {&frame_00217[0], sizeof(frame_00217)},
    {&frame_00218[0], sizeof(frame_00218)},
    {&frame_00219[0], sizeof(frame_00219)},
    {&frame_00220[0], sizeof(frame_00220)},
    {&frame_00221[0], sizeof(frame_00221)},
    {&frame_00222[0], sizeof(frame_00222)},
    {&frame_00223[0], sizeof(frame_00223)},
    {&frame_00224[0], sizeof(frame_00224)},
    {&frame_00225[0], sizeof(frame_00225)},
    {&frame_00226[0], sizeof(frame_00226)},
    {&frame_00227[0], sizeof(frame_00227)},
    {&frame_00228[0], sizeof(frame_00228)},
    {&frame_00229[0], sizeof(frame_00229)},
    {&frame_00230[0], sizeof(frame_00230)},
    {&frame_00231[0], sizeof(frame_00231)},
    {&frame_00232[0], sizeof(frame_00232)},
    {&frame_00233[0], sizeof(frame_00233)},
    {&frame_00234[0], sizeof(frame_00234)},
    {&frame_00235[0], sizeof(frame_00235)},
    {&frame_00236[0], sizeof(frame_00236)},
    {&frame_00237[0], sizeof(frame_00237)},
    {&frame_00238[0], sizeof(frame_00238)},
    {&frame_00239[0], sizeof(frame_00239)},
    {&frame_00240[0], sizeof(frame_00240)},
    {&frame_00241[0], sizeof(frame_00241)},
    {&frame_00242[0], sizeof(frame_00242)},
    {&frame_00243[0], sizeof(frame_00243)},
    {&frame_00244[0], sizeof(frame_00244)},
    {&frame_00245[0], sizeof(frame_00245)},
    {&frame_00246[0], sizeof(frame_00246)},
    {&frame_00247[0], sizeof(frame_00247)},
    {&frame_00248[0], sizeof(frame_00248)},
    {&frame_00249[0], sizeof(frame_00249)},
    {&frame_00250[0], sizeof(frame_00250)},
    {&frame_00251[0], sizeof(frame_00251)},
    {&frame_00252[0], sizeof(frame_00252)},
    {&frame_00253[0], sizeof(frame_00253)},
    {&frame_00254[0], sizeof(frame_00254)},
    {&frame_00255[0], sizeof(frame_00255)},
    {&frame_00256[0], sizeof(frame_00256)},
    {&frame_00257[0], sizeof(frame_00257)},
    {&frame_00258[0], sizeof(frame_00258)},
    {&frame_00259[0], sizeof(frame_00259)},
    {&frame_00260[0], sizeof(frame_00260)},
    {&frame_00261[0], sizeof(frame_00261)},
    {&frame_00262[0], sizeof(frame_00262)},
    {&frame_00263[0], sizeof(frame_00263)},
    {&frame_00264[0], sizeof(frame_00264)},
    {&frame_00265[0], sizeof(frame_00265)},
    {&frame_00266[0], sizeof(frame_00266)},
    {&frame_00267[0], sizeof(frame_00267)},
    {&frame_00268[0], sizeof(frame_00268)},
    {&frame_00269[0], sizeof(frame_00269)},
    {&frame_00270[0], sizeof(frame_00270)},
    {&frame_00271[0], sizeof(frame_00271)},
    {&frame_00272[0], sizeof(frame_00272)},
    {&frame_00273[0], sizeof(frame_00273)},
    {&frame_00274[0], sizeof(frame_00274)},
    {&frame_00275[0], sizeof(frame_00275)},
    {&frame_00276[0], sizeof(frame_00276)},
    {&frame_00277[0], sizeof(frame_00277)},
    {&frame_00278[0], sizeof(frame_00278)},
    {&frame_00279[0], sizeof(frame_00279)},
    {&frame_00280[0], sizeof(frame_00280)},
    {&frame_00281[0], sizeof(frame_00281)},
    {&frame_00282[0], sizeof(frame_00282)},
    {&frame_00283[0], sizeof(frame_00283)},
    {&frame_00284[0], sizeof(frame_00284)},
    {&frame_00285[0], sizeof(frame_00285)},
    {&frame_00286[0], sizeof(frame_00286)},
    {&frame_00287[0], sizeof(frame_00287)},
    {&frame_00288[0], sizeof(frame_00288)},
    {&frame_00289[0], sizeof(frame_00289)},
    {&frame_00290[0], sizeof(frame_00290)},
    {&frame_00291[0], sizeof(frame_00291)},
    {&frame_00292[0], sizeof(frame_00292)},
    {&frame_00293[0], sizeof(frame_00293)},
    {&frame_00294[0], sizeof(frame_00294)},
    {&frame_00295[0], sizeof(frame_00295)},
    {&frame_00296[0], sizeof(frame_00296)},
    {&frame_00297[0], sizeof(frame_00297)},
    {&frame_00298[0], sizeof(frame_00298)},
    {&frame_00299[0], sizeof(frame_00299)},
    {&frame_00300[0], sizeof(frame_00300)},
    {&frame_00301[0], sizeof(frame_00301)},
    {&frame_00302[0], sizeof(frame_00302)},
    {&frame_00303[0], sizeof(frame_00303)},
    {&frame_00304[0], sizeof(frame_00304)},
    {&frame_00305[0], sizeof(frame_00305)},
    {&frame_00306[0], sizeof(frame_00306)},
    {&frame_00307[0], sizeof(frame_00307)},
    {&frame_00308[0], sizeof(frame_00308)},
    {&frame_00309[0], sizeof(frame_00309)},
    {&frame_00310[0], sizeof(frame_00310)},
    {&frame_00311[0], sizeof(frame_00311)},
    {&frame_00312[0], sizeof(frame_00312)},
    {&frame_00313[0], sizeof(frame_00313)},
    {&frame_00314[0], sizeof(frame_00314)},
    {&frame_00315[0], sizeof(frame_00315)},
    {&frame_00316[0], sizeof(frame_00316)},
    {&frame_00317[0], sizeof(frame_00317)},
    {&frame_00318[0], sizeof(frame_00318)},
    {&frame_00319[0], sizeof(frame_00319)},
    {&frame_00320[0], sizeof(frame_00320)},
    {&frame_00321[0], sizeof(frame_00321)},
    {&frame_00322[0], sizeof(frame_00322)},
    {&frame_00323[0], sizeof(frame_00323)},
    {&frame_00324[0], sizeof(frame_00324)},
    {&frame_00325[0], sizeof(frame_00325)},
    {&frame_00326[0], sizeof(frame_00326)},
    {&frame_00327[0], sizeof(frame_00327)},
    {&frame_00328[0], sizeof(frame_00328)},
    {&frame_00329[0], sizeof(frame_00329)},
    {&frame_00330[0], sizeof(frame_00330)},
    {&frame_00331[0], sizeof(frame_00331)},
    {&frame_00332[0], sizeof(frame_00332)},
    {&frame_00333[0], sizeof(frame_00333)},
    {&frame_00334[0], sizeof(frame_00334)},
    {&frame_00335[0], sizeof(frame_00335)},
    {&frame_00336[0], sizeof(frame_00336)},
    {&frame_00337[0], sizeof(frame_00337)},
    {&frame_00338[0], sizeof(frame_00338)},
    {&frame_00339[0], sizeof(frame_00339)},
    {&frame_00340[0], sizeof(frame_00340)},
    {&frame_00341[0], sizeof(frame_00341)},
    {&frame_00342[0], sizeof(frame_00342)},
    {&frame_00343[0], sizeof(frame_00343)},
    {&frame_00344[0], sizeof(frame_00344)},
    {&frame_00345[0], sizeof(frame_00345)},
    {&frame_00346[0], sizeof(frame_00346)},
    {&frame_00347[0], sizeof(frame_00347)},
    {&frame_00348[0], sizeof(frame_00348)},
    {&frame_00349[0], sizeof(frame_00349)},
    {&frame_00350[0], sizeof(frame_00350)},
    {&frame_00351[0], sizeof(frame_00351)},
    {&frame_00352[0], sizeof(frame_00352)},
    {&frame_00353[0], sizeof(frame_00353)},
    {&frame_00354[0], sizeof(frame_00354)},
    {&frame_00355[0], sizeof(frame_00355)},
    {&frame_00356[0], sizeof(frame_00356)},
    {&frame_00357[0], sizeof(frame_00357)},
    {&frame_00358[0], sizeof(frame_00358)},
    {&frame_00359[0], sizeof(frame_00359)},
    {&frame_00360[0], sizeof(frame_00360)}
};

static const uint8_t *rle_data; //массив байтов (RLE)
static uint32_t rle_len;        //длина
static uint32_t rle_idx;        //индекс
static uint8_t current_run;     //сколько раз повторять цвет
static uint8_t current_val;     //байт (8 пикселей)
static int bit_pos; // 7..0     //какой бит сейчас читаем
static uint16_t x, y;           //координаты текущего пикселя
static uint8_t started;         //состояние кадра (стартует)
static uint8_t finished;        //состояние кадра (закончился)
//двойная буферизация
static uint8_t line_buf1[width * 3];
static uint8_t line_buf2[width * 3];
static uint8_t *draw_buf = line_buf1; //CPU рисует
static uint8_t *dma_buf = line_buf2;  //DMA отправляет

void Video_Start(const uint8_t *rle, uint32_t len) //Инициализация нового кадра
{
    rle_data = rle;
    rle_len = len;

    rle_idx = 0;
    current_run = 0;
    current_val = 0;
    bit_pos = -1;

    x = 0;
    y = 0;

    started = 0;
    finished = 0;
}
void Video_Task(void)
{
    if (finished) return; //проверка окончания предыдущего кадра

    if (!started) //настройка экрана один раз на кадр
    {
        Lcd_select();
        Address_set(0, 0, width - 1, height - 1);
        HAL_GPIO_WritePin(GPIO_0, RS, 1);

        if (!(hspi0.Instance->ENABLE & SPI_ENABLE_M))
            __HAL_SPI_ENABLE(&hspi0);

        started = 1;
    }

    uint8_t *p = draw_buf; //указатель на буфер

    while (y < height)
    {
        if (current_run == 0)
        {
            if (rle_idx >= rle_len)
            {
                finished = 1;
                Lcd_unselect();
                return;
            }

            current_run = rle_data[rle_idx++];
            current_val = rle_data[rle_idx++];
            bit_pos = 7;
        }

        while (current_run > 0)
        {
            uint8_t bit = (current_val >> bit_pos) & 1; //извлекаем бит
            uint16_t color = bit ? 0xFFFF : 0x0000;     //какой у него цвет
            //3 байта
            *p++ = color;
            *p++ = color;
            *p++ = color;

            x++;
            bit_pos--; //двигаем позицию

            if (bit_pos < 0)//если байт закончился, переходим к следующему
            {
                bit_pos = 7;
                current_run--;
            }

            if (x >= width)
            {
                //  меняем буферы местами
                uint8_t *tmp = dma_buf;
                dma_buf = draw_buf;
                draw_buf = tmp;

                //  запускаем DMA
                HAL_DMA_Wait(&hdma_ch0, DMA_TIMEOUT_DEFAULT);
                HAL_DMA_Start(&hdma_ch0,dma_buf,(void*)&hspi0.Instance->TXDATA,width * 3 - 1);

                x = 0;
                y++;
                return;
            }
        }
    }

    if (y >= height)
    {
        finished = 1;
        Lcd_unselect();
    }
}

volatile int buf_pos = 0;
volatile int need_fill = 0;
int main(void)
{
    HAL_Init();
    SystemClock_Config();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);

    uint8_t dma_buffer[DMA_BUFFER_SIZE];
    set_dma_buf(&dma_buffer);

    GPIO_Init();
    SPI0_Init();

    DAC_Init();
    DAC2_Init();
    DMA_Init();
    Lcd_Init();
    Timer32_Init();

    fill_dac_buf(dac_buf0);
    fill_dac_buf(dac_buf1);

    int i = 0;

    ClearMassDMA(320, 480, 0, 0, BG_COLOR);

    HAL_Timer32_Value_Clear(&htimer32);

    HAL_EPIC_MaskLevelSet(HAL_EPIC_TIMER32_1_MASK);
    HAL_IRQ_EnableInterrupts();

    HAL_Timer32_Compare_Start_IT(&htimer32, &htimer32_channel);

    Video_Start(frames[i].data, frames[i].len);
    while (1)
    {
        if (need_fill)
        {
            need_fill = 0;
            if (active_buf == 0) fill_dac_buf(dac_buf1);
            else fill_dac_buf(dac_buf0);
        }

        Video_Task();

        if (finished)
        {
            i++;
            if (i > 360) i = 0;
            Video_Start(frames[i].data, frames[i].len);
            // HAL_DelayMs(1);
        }

    }
    
}
void trap_handler(void)
{
    if (EPIC_CHECK_TIMER32_1())
    {
        uint32_t status = HAL_Timer32_InterruptFlags_Get(&htimer32);

        if (status & TIMER32_INT_OC_M(TIMER32_CHANNEL_0))
        {
            uint16_t *current_buf = (active_buf == 0) ? dac_buf0 : dac_buf1;
            uint16_t sample = current_buf[buf_pos];

            hdac1.Instance_dac->VALUE = sample;

            buf_pos++;

            if (buf_pos >= DAC_BUF_SIZE)
            {
                buf_pos = 0;
                active_buf ^= 1;
                need_fill = 1;
            }
        }
        HAL_TIMER32_INTERRUPTFLAGS_CLEAR(&htimer32);
    }
    HAL_EPIC_Clear(0xFFFFFFFF);
}
