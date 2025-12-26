#include "mik32_hal_pcc.h"
#include "mik32_hal_gpio.h"
#include "mik32_hal_adc.h"
//#include "mik32_hal_spifi.h"
#include "mik32_hal_spifi_w25.h"
#include "uart_lib.h"
#include "xprintf.h"
#include <stdint.h>

// Светодиоды
#define LED_RUN_PORT    GPIO_0
#define LED_RUN_PIN     GPIO_PIN_0   // P0.0
#define LED_STATUS_PORT GPIO_0
#define LED_STATUS_PIN  GPIO_PIN_1   // P0.1

// Кнопка
#define BTN_PORT        GPIO_2
#define BTN_PIN         GPIO_PIN_6   // P2.6

// Аналоговые входы
#define ADC_SUPPLY_CH   3   // P0.4 — питание = ADC3
#define ADC_A_CH1       4   // P0.7 = ADC4
#define ADC_A_CH2       5   // P0.9 = ADC5
#define VOLT_INPUT      12 // 12В питание

#define LOG_BASE_ADDR   0x00000000   //адрес начала логов в SPIFI
#define LOG_SECTOR_SIZE 4096         //сектор для стирания (4K) 

static uint32_t log_write_addr = LOG_BASE_ADDR; //куда писать следующий лог
static uint8_t experiment_count = 0;            //счётчик коротких нажатий для номера экспериментов

static SPIFI_HandleTypeDef hspifi;
// ADC Handles
static ADC_HandleTypeDef hadc_supply = {0};
static ADC_HandleTypeDef hadc_a1     = {0};
static ADC_HandleTypeDef hadc_a2     = {0};

// Выходы теста
typedef struct {
    GPIO_TypeDef *port;
    uint32_t pin; 
} out_t;

typedef struct {
    uint8_t TEST_NUMBER;      // № теста
    uint8_t error_state;      // 4 бита число ошибок + 1 бит состояние (state)
    uint16_t pin_mask;        // 10 бит цифровых входов (P1.0-P1.7, P1.14-P1.15)
    uint8_t result_rele;      // 8 бит реле
    uint16_t result_A1;       // 12 бит аналоговый A1
    uint16_t result_A0;       // 12 бит аналоговый A0
} log_entry_t;

const out_t TEST_OUTPUTS[8] = {
    {GPIO_1, GPIO_PIN_10}, // P1.10
    {GPIO_1, GPIO_PIN_11}, // P1.11
    {GPIO_1, GPIO_PIN_12}, // P1.12
    {GPIO_1, GPIO_PIN_13}, // P1.13
    {GPIO_0, GPIO_PIN_3},  // P0.3
    {GPIO_2, GPIO_PIN_7},  // P2.7
    {GPIO_0, GPIO_PIN_8},  // P0.8
    {GPIO_0, GPIO_PIN_10}, // P0.10
};

uint16_t adc_raw_to_volts(uint16_t raw) {
    //xprintf("raw %d\n", (uint32_t)raw * 1300 * VOLT_INPUT / 4095);
    return raw * 1300 * VOLT_INPUT / 4095;
}
//Чтение ADC по HAL
uint16_t read_adc_handle(ADC_HandleTypeDef *adc) {
    HAL_ADC_ChannelSet(adc);
    HAL_ADC_Single(adc);
    HAL_ADC_WaitValid(adc);
    return HAL_ADC_GetValue(adc);
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
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 8;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}
// Инициализация GPIO
void GPIO_Init(void) {
    GPIO_InitTypeDef gpio = {0};
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_2_CLK_ENABLE();

    // Светодиоды
    gpio.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    gpio.Pull = HAL_GPIO_PULL_NONE;

    gpio.Pin = LED_RUN_PIN;
    HAL_GPIO_Init(LED_RUN_PORT, &gpio);
    gpio.Pin = LED_STATUS_PIN;
    HAL_GPIO_Init(LED_STATUS_PORT, &gpio);

    // Кнопка
    gpio.Mode = HAL_GPIO_MODE_GPIO_INPUT;
    gpio.Pull = HAL_GPIO_PULL_UP;
    gpio.Pin  = BTN_PIN;
    HAL_GPIO_Init(BTN_PORT, &gpio);

    // Выходы теста
    gpio.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    gpio.Pull = HAL_GPIO_PULL_NONE;
    for (int i = 0; i < 8; i++) {
        gpio.Pin = TEST_OUTPUTS[i].pin;
        HAL_GPIO_Init(TEST_OUTPUTS[i].port, &gpio);
        HAL_GPIO_WritePin(TEST_OUTPUTS[i].port, TEST_OUTPUTS[i].pin, GPIO_PIN_LOW);
    }

    // Цифровые входы
    gpio.Mode = HAL_GPIO_MODE_GPIO_INPUT;
    gpio.Pull = HAL_GPIO_PULL_NONE;

    // P1.0-P1.7
    gpio.Pin = 0x00FF;
    HAL_GPIO_Init(GPIO_1, &gpio);

    // P1.14-P1.15
    gpio.Pin = GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIO_1, &gpio);
}

void ADC_Init(void) {
    __HAL_PCC_ANALOG_REGS_CLK_ENABLE();
    HAL_DelayMs(10); 

    hadc_a1.Instance = ANALOG_REG;
    hadc_a1.Init.Sel = ADC_A_CH1;
    hadc_a1.Init.EXTRef = ADC_EXTREF_OFF;
    hadc_a1.Init.EXTClb = 0;
    HAL_ADC_Init(&hadc_a1);
    HAL_DelayMs(10);
    read_adc_handle(&hadc_a1);

    hadc_a2.Instance = ANALOG_REG;
    hadc_a2.Init.Sel = ADC_A_CH2;
    hadc_a2.Init.EXTRef = ADC_EXTREF_OFF;
    hadc_a2.Init.EXTClb = ADC_EXTCLB_ADCREF;
    HAL_ADC_Init(&hadc_a2);
    HAL_DelayMs(10);
    read_adc_handle(&hadc_a2);

    hadc_supply.Instance = ANALOG_REG;
    hadc_supply.Init.Sel = ADC_SUPPLY_CH;
    hadc_supply.Init.EXTRef = ADC_EXTREF_OFF;
    hadc_supply.Init.EXTClb = ADC_EXTCLB_ADCREF;
    HAL_ADC_Init(&hadc_supply);
    HAL_DelayMs(10);
    read_adc_handle(&hadc_supply); 
}

void SPIFI_Init(void)
{
    hspifi.Instance = SPIFI_CONFIG;
    HAL_SPIFI_MspInit(&hspifi);
    HAL_SPIFI_Reset(&hspifi);
}

static void logs_erase(void) //Для очистки Флеш
{
    HAL_SPIFI_W25_SectorErase4K(&hspifi, LOG_BASE_ADDR);
    log_write_addr = LOG_BASE_ADDR;
}

static void write_log(uint8_t test_number, uint8_t error_count, uint16_t pin_mask, uint8_t result_rele, uint16_t result_A1, uint16_t result_A0, uint8_t state) //запись во флеш
{

    log_entry_t entry;
    entry.TEST_NUMBER   = test_number;
    entry.error_state   = ((error_count & 0x0F) << 4) | (state & 0x01);
    entry.pin_mask      = pin_mask & 0x3FF; // только 10 бит
    entry.result_rele   = result_rele;
    entry.result_A1     = result_A1 & 0x0FFF; // 12 бит
    entry.result_A0     = result_A0 & 0x0FFF; // 12 бит

    HAL_SPIFI_W25_PageProgram(&hspifi, log_write_addr, sizeof(log_entry_t), (uint8_t *)&entry);
    log_write_addr += sizeof(log_entry_t);
}

static void dump_logs(void) //Вывод из флеш
{
    xprintf(" === DUMP LOGS ===\n");
    uint32_t addr = LOG_BASE_ADDR;
    log_entry_t entry;

    while (addr < log_write_addr) {
        HAL_SPIFI_W25_ReadData(&hspifi, addr, sizeof(log_entry_t), (uint8_t *)&entry);

        uint8_t error_count = (entry.error_state >> 4) & 0x0F;
        xprintf( "%04u %X %010b %08b %012b %012b \n", entry.TEST_NUMBER, error_count,  entry.pin_mask, entry.result_rele,  entry.result_A1,  entry.result_A0);
        addr += sizeof(log_entry_t);
    }
}

static void logs_find_write_addr(void)
{
    log_entry_t entry;
    uint32_t addr = LOG_BASE_ADDR;

    while (addr + sizeof(log_entry_t) <= LOG_BASE_ADDR + LOG_SECTOR_SIZE) {

        HAL_SPIFI_W25_ReadData( &hspifi,addr,sizeof(log_entry_t),(uint8_t *)&entry);

        // если вся запись = 0xFF -> дальше пусто
        bool empty = true;
        uint8_t *p = (uint8_t *)&entry;
        for (uint32_t i = 0; i < sizeof(log_entry_t); i++) {
            if (p[i] != 0xFF) {
                empty = false;
                break;
            }
        }

        if (empty) {
            break;
        }

        addr += sizeof(log_entry_t);
    }

    log_write_addr = addr;
}


static uint16_t check_digital_errors(const out_t *active_out)
{
    uint16_t error_mask = 0;

    // P1.0 – P1.7
    for (uint8_t bit = 0; bit < 8; bit++) {
        uint16_t pin = (1u << bit);

        GPIO_PinState state = HAL_GPIO_ReadPin(GPIO_1, pin);

        //xprintf( "P1.%u = %s%s\n",bit, state == GPIO_PIN_HIGH ? "HIGH" : "LOW", (active_out->port == GPIO_1 && active_out->pin == pin) ? " (ACTIVE OUT)" : "");

        if (active_out->port == GPIO_1 && active_out->pin == pin)
            continue;

        if (state == GPIO_PIN_LOW)
            error_mask |= (1u << bit);
    }

    // P1.14 -> бит 8
    {
        GPIO_PinState state = HAL_GPIO_ReadPin(GPIO_1, GPIO_PIN_14);

        //xprintf("P1.14 = %s%s\n", state == GPIO_PIN_HIGH ? "HIGH" : "LOW", (active_out->port == GPIO_1 && active_out->pin == GPIO_PIN_14) ? " (ACTIVE OUT)" : "" );

        if (!(active_out->port == GPIO_1 && active_out->pin == GPIO_PIN_14)) {
            if (state == GPIO_PIN_LOW)
                error_mask |= (1u << 8);
        }
    }

    // P1.15 -> бит 9
    {
        GPIO_PinState state = HAL_GPIO_ReadPin(GPIO_1, GPIO_PIN_15);
        //xprintf( "P1.15 = %s%s\n",state == GPIO_PIN_HIGH ? "HIGH" : "LOW", (active_out->port == GPIO_1 && active_out->pin == GPIO_PIN_15) ? " (ACTIVE OUT)" : "");
        if (!(active_out->port == GPIO_1 && active_out->pin == GPIO_PIN_15)) {
            if (state == GPIO_PIN_LOW)
                error_mask |= (1u << 9);
        }
    }

    //xprintf("ERROR MASK = %010b\n", error_mask);

    return error_mask;
}


// static int check_button_action(void)
// {
//     if (HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) == GPIO_PIN_LOW) {
//         HAL_DelayMs(20);
//         if (HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) != GPIO_PIN_LOW) return 0;
//         uint16_t time = 0;
//         while (HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) == GPIO_PIN_LOW) {
//             HAL_DelayMs(50);
//             time += 50;
//             if (time >= 2200) return 2; // длинное
//         }
//         return 1; // короткое
//     }
//     return 0;
// }
static int check_button_action(void)
{
    if (HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) != GPIO_PIN_LOW)
        return 0;

    HAL_DelayMs(20);  // антидребезг
    if (HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) != GPIO_PIN_LOW)
        return 0;

    uint16_t time = 0;
    while (HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN) == GPIO_PIN_LOW) {
        HAL_DelayMs(50);
        if ((time += 50) >= 2200)
            return 2;  // длинное нажатие
    }
    return 1;  // короткое нажатие
}
static void indicate_experiment_number(uint8_t experiment)
{
    for (uint8_t i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(
            TEST_OUTPUTS[i].port,
            TEST_OUTPUTS[i].pin,
            (i < experiment) ? GPIO_PIN_HIGH : GPIO_PIN_LOW
        );
    }
}
// static log_entry_t run_test_cycle(uint32_t vin_mV)
// {
//     log_entry_t res = {0};

//     uint8_t  error_count        = 0;
//     uint16_t combined_pin_mask  = 0;
//     uint8_t  combined_rele_mask = 0;
//     uint16_t max_A1             = 0;
//     uint16_t max_A0             = 0;

//     uint8_t  flag = 0;

//     for (uint8_t i = 0; i < 8; i++) {

//         const out_t *out = &TEST_OUTPUTS[i];

//         // === Включаем выход ===
//         HAL_GPIO_WritePin(out->port, out->pin, GPIO_PIN_HIGH);
//         HAL_DelayMs(80);

//         uint16_t A1_val = adc_raw_to_volts(read_adc_handle(&hadc_a1));
//         uint16_t A0_val = adc_raw_to_volts(read_adc_handle(&hadc_a2));
//         uint16_t pin_mask = check_digital_errors(out);

//         // Проверка как у заказчика при включении выхода
//         if (((pin_mask != 0 && pin_mask != 0x03FF) || A0_val > vin_mV || A1_val > vin_mV) && flag != 0) {
//             error_count++;
//             combined_rele_mask |= (1u << i);  // фиксируем номер реле
//             combined_pin_mask |= (pin_mask != 0x03FF ? pin_mask : 0); // логируем только реальные входы
//         }

//         // === Выключаем выход ===
//         HAL_GPIO_WritePin(out->port, out->pin, GPIO_PIN_LOW);
//         HAL_DelayMs(50);

//         A1_val = adc_raw_to_volts(read_adc_handle(&hadc_a1));
//         A0_val = adc_raw_to_volts(read_adc_handle(&hadc_a2));
//         pin_mask = check_digital_errors(out);

//         // Проверка как у заказчика при выключении выхода
//         if (((pin_mask != 0x03FF) || A0_val < 500 || A1_val < 500) && flag != 0) {
//             error_count++;
//             combined_rele_mask |= (1u << i);
//             combined_pin_mask |= (pin_mask != 0x03FF ? pin_mask : 0);
//         }
//         if (A1_val > max_A1) max_A1 = A1_val;
//         if (A0_val > max_A0) max_A0 = A0_val;
//         flag++;
//     }

//     res.error_state = error_count;
//     res.pin_mask    = combined_pin_mask ^ 0x03FF;
//     res.result_rele = combined_rele_mask ^ 0xFF;
//     res.result_A1   = max_A1;
//     res.result_A0   = max_A0;

//     return res;
// }
static log_entry_t run_test_cycle(uint32_t vin_mV)
{
    log_entry_t res = {0};

    uint8_t  error_count        = 0;
    uint16_t combined_pin_mask  = 0;
    uint8_t  combined_rele_mask = 0;
    uint16_t max_A1             = 0;
    uint16_t max_A0             = 0;

    uint8_t flag = 0;

    for (uint8_t i = 0; i < 8; i++) {

        const out_t *out = &TEST_OUTPUTS[i];
        HAL_GPIO_WritePin(out->port, out->pin, GPIO_PIN_HIGH);
        HAL_DelayMs(80);

        uint16_t A1_val = adc_raw_to_volts(read_adc_handle(&hadc_a1));
        uint16_t A0_val = adc_raw_to_volts(read_adc_handle(&hadc_a2));

        uint16_t pin_mask = check_digital_errors(out);

        //Выход не сработал
        if (pin_mask == 0x03FF) { // 10 бит: 1111111111

            error_count++;
            combined_rele_mask |= (1u << i);
        }
        //Частичный отказ входов
        else if (pin_mask != 0) {

            error_count++;
            combined_pin_mask |= pin_mask;
        }
        //Аналоговая ошибка
        if (A1_val > vin_mV || A0_val > 1000) {
            error_count++;
            combined_rele_mask |= (1u << i);
        }

        //Выключаем выход
        HAL_GPIO_WritePin(out->port, out->pin, GPIO_PIN_LOW);
        HAL_DelayMs(50);

        // Сохраняем максимум
        if (A1_val > max_A1 && flag!=0) max_A1 = A1_val;
        if (A0_val > max_A0 && flag!=0) max_A0 = A0_val;
        flag++;
    }
    res.error_state = (error_count > 0);

    // инвертируем маски для логов
    res.pin_mask    = combined_pin_mask ^ 0x03FF; // 10 бит
    res.result_rele = combined_rele_mask ^ 0xFF; // 8 бит

    if (res.pin_mask == 0x03FF) {
    res.result_A1 = (max_A1 > 300) ? (max_A1 - 300) : 0;
    res.result_A0 = (max_A0 > 300) ? (max_A0 - 300) : 0;
    } else {
        res.result_A1 = max_A1;
        res.result_A0 = max_A0;
    }
    
    HAL_GPIO_TogglePin(LED_RUN_PORT, LED_RUN_PIN);

    return res;
}

static void check_uart_command(void)
{
    static uint8_t pos = 0;
    static char cmd[4];
    char c;

    if (UART_IsRxFifoEmpty(UART_0)) return;

    c = (char)UART_ReadByte(UART_0);

    if (c == '\r' || c == '\n') {          // конец команды
        cmd[pos] = 0;
        if (pos == 3 && cmd[0] == 'l' && cmd[1] == 'o' && cmd[2] == 'g')
            dump_logs();
        pos = 0;
    } else if (pos < 3) {                  // сохраняем символ
        cmd[pos++] = c;
    }
}
int main(void)
{
    SystemClock_Config();
    HAL_Init();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_RE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    GPIO_Init();
    ADC_Init();
    SPIFI_Init();
    // logs_erase(); // для отчистки флеш
    logs_find_write_addr();

    uint32_t vin_mV = adc_raw_to_volts(read_adc_handle(&hadc_supply));

    if (vin_mV < 5000) {
        HAL_GPIO_WritePin(LED_STATUS_PORT, LED_STATUS_PIN, GPIO_PIN_HIGH);
        while (1); // питание низкое — просто мигаем статусом
    }

    while (1) 
    {
        check_uart_command();

        int action = check_button_action();
        if (action == 1) { // короткое нажатие
            experiment_count = (experiment_count % 8) + 1; // циклически 1-8
            indicate_experiment_number(experiment_count);
        }

        if (action == 2) { // длинное нажатие — запуск теста
            HAL_GPIO_WritePin(LED_RUN_PORT, LED_RUN_PIN, GPIO_PIN_LOW);
            HAL_GPIO_WritePin(LED_STATUS_PORT, LED_STATUS_PIN, GPIO_PIN_LOW);

            // очистка индикации
            indicate_experiment_number(0);

            uint8_t total_error_count = 0;
            uint16_t total_pin_mask = 0;
            uint8_t total_rele_mask = 0;
            uint16_t worst_A1 = 0, worst_A0 = 0;

            for (int i = 0; i < 60; i++) {
                log_entry_t r = run_test_cycle(vin_mV);

                total_error_count += (r.error_state != 0);
                total_pin_mask  |= r.pin_mask;
                total_rele_mask |= r.result_rele;
                if (r.result_A1 > worst_A1) worst_A1 = r.result_A1;
                if (r.result_A0 > worst_A0) worst_A0 = r.result_A0;
            }

            uint8_t final_state = (total_error_count > 0);
            write_log(experiment_count, total_error_count, total_pin_mask, total_rele_mask, worst_A1, worst_A0, final_state);

            // индикация результата
            HAL_GPIO_WritePin(LED_RUN_PORT, LED_RUN_PIN, final_state ? GPIO_PIN_LOW : GPIO_PIN_HIGH);
            HAL_GPIO_WritePin(LED_STATUS_PORT, LED_STATUS_PIN, GPIO_PIN_HIGH);

            experiment_count = (experiment_count == 0) ? 8 : experiment_count - 1; // корректировка номера
        }
    }
}
