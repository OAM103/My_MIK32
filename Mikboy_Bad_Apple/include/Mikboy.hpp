#ifndef __MIKBOY__
#define __MIKBOY__

#include "mik32_hal_dma.h"
#include "Mikboy_LCD_ST7735.hpp"
#include "mik32_hal_scr1_timer.h"
#include <array>

enum class BUTTON_PIN : uint8_t
{
    A = P1_10,
    B = P1_11,
    UP = P1_6,
    DOWN = P2_6,
    LEFT = P2_7,
    RIGHT = P1_7,
    SIZE = 6
};
enum class BUTTON : uint8_t
{
    A = 0,
    B,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SIZE
};
const std::array<BUTTON_PIN, static_cast<size_t>(BUTTON_PIN::SIZE)> button_mas = {{BUTTON_PIN::A,
                                                                                   BUTTON_PIN::B,
                                                                                   BUTTON_PIN::UP,
                                                                                   BUTTON_PIN::DOWN,
                                                                                   BUTTON_PIN::LEFT,
                                                                                   BUTTON_PIN::RIGHT}};
class Mikboy
{
private:
    ST7735::LCD lcd_;
    DMA_InitTypeDef hdma_;
    ST7735::LCD LCD_Init();
    void GPIO_init();
    void SPI0_Init();
    void SystemClock_Config(void);

public:
    Mikboy(/* args */);
    ~Mikboy() = default;

    /// @brief Доступ к дисплею 
    /// @return  ссылка на объект дисплея
    ST7735::LCD &lcd()
    {
        return lcd_;
    }
};

#endif