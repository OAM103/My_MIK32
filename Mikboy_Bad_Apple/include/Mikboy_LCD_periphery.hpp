#ifndef MIKBOY_LCD_PEREPHERY
#define MIKBOY_LCD_PEREPHERY
#include "mik32_hal_spi.h"
#include "mik32_ll_gpio.h"
#include "mik32_hal_dma.h"
#include <stdint.h>
#include "xprintf.h"
#include "uart_lib.h"

namespace Periphery
{
    constexpr uint32_t expectation = 200000;
    constexpr uint8_t SELECT = 0;
    constexpr uint8_t UNSELECT = 1;
    constexpr uint8_t DATA = 1;
    constexpr uint8_t COMMAND = 0;

    class LCD
    {
    private:
        SPI_TypeDef *spi_ = nullptr;
        // DMA_ChannelHandleTypeDef* dma_ch_ = nullptr;
        uint32_t channel_index_;
        DMA_CONFIG_TypeDef *instance_;
        uint32_t CFG_write_;

        int8_t cs_ = -1; // опциональный
        int8_t dc_ = -1;

    protected:
        uint8_t dma_busy() const; // TODO решить нужно ли и сделать если да

        HAL_StatusTypeDef write_spi(uint8_t *data, uint32_t size) const;

        void pin_select() const;
        void pin_unselect() const;
        void pin_data() const;
        void pin_command() const;

    public:
        
        /// @brief ожидает окончания передачи данных по DMA
        /// @return HAL_OK - при удачном завершении или состояние ошибки
        HAL_StatusTypeDef dma_wait() const;

        void wait_dma() const;

        /// @brief  
        /// @param spi указатель на структуру SPIx
        /// @param dma_ch структуру настройка канала DMA
        LCD(SPI_TypeDef *spi, DMA_ChannelHandleTypeDef dma_ch);

        LCD() = default;
        ~LCD() = default;
        
        /// @brief отправляет массив данных по SPI с DMA
        /// @param data указатель на начало массива 
        /// @param size количество элементов в массиве
        void write_spi_dma(const uint8_t *data, uint32_t size) const;

        /// @brief инициализирует ножки дисплея
        /// @param dc пин data/comand
        /// @param cs пин cs если не используется аппаратный cs
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef init(int8_t dc, int8_t cs = -1);

        /// @brief отдельная инициализация дисплея
        /// @param spi указатель на структуру SPIx
        /// @param dma_ch структуру настройка канала DMA
        /// @param dc пин data/comand
        /// @param cs пин cs если не используется аппаратный cs
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef init(SPI_TypeDef *spi, DMA_ChannelHandleTypeDef dma_ch, int8_t dc, int8_t cs = -1);

        /// @brief отправляет одну команду на дисплей
        /// @param command команда
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef command(uint8_t command) const;

        /// @brief отправляет данные на дисплей
        /// @param data данные 
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef data(uint8_t data) const;

        /// @brief отправляет массив данных на дисплей
        /// @param mas указатель на начало массива
        /// @param size количество элементов в массиве
        /// @return HAL_OK или состояние ошибки
        HAL_StatusTypeDef print_mas(uint8_t *mas, uint32_t size) const;
        //HAL_StatusTypeDef 
        void print_mas(const uint16_t *mas, uint32_t size) const;


    };
}

#endif