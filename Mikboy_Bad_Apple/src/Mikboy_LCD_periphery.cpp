#include "Mikboy_LCD_periphery.hpp"
#include <mik32_hal.h>
#include <mik32_hal_def.h>
#include <mik32_hal_dma.h>
#include <mik32_hal_spi.h>
#include <mik32_ll_gpio.h>
#include <stdint.h>
#include <xprintf.h>

namespace Periphery
{
    LCD::LCD(SPI_TypeDef *spi, DMA_ChannelHandleTypeDef dma_ch)
    {
        spi_ = spi;

        channel_index_ = dma_ch.ChannelInit.Channel;
        instance_ = dma_ch.dma->Instance;
        CFG_write_ = DMA_CH_CFG_IRQ_EN_M;
        CFG_write_ |= DMA_CH_CFG_ENABLE_M | (dma_ch.ChannelInit.Priority << DMA_CH_CFG_PRIOR_S) | (dma_ch.ChannelInit.ReadMode << DMA_CH_CFG_READ_MODE_S) | (dma_ch.ChannelInit.ReadInc << DMA_CH_CFG_READ_INCREMENT_S) | (dma_ch.ChannelInit.ReadSize << DMA_CH_CFG_READ_SIZE_S) | (dma_ch.ChannelInit.ReadBurstSize << DMA_CH_CFG_READ_BURST_SIZE_S) | (dma_ch.ChannelInit.ReadRequest << DMA_CH_CFG_READ_REQUEST_S) | (dma_ch.ChannelInit.ReadAck << DMA_CH_CFG_READ_ACK_EN_S) | (dma_ch.ChannelInit.WriteMode << DMA_CH_CFG_WRITE_MODE_S) | (dma_ch.ChannelInit.WriteInc << DMA_CH_CFG_WRITE_INCREMENT_S) | (dma_ch.ChannelInit.WriteSize << DMA_CH_CFG_WRITE_SIZE_S) | (dma_ch.ChannelInit.WriteBurstSize << DMA_CH_CFG_WRITE_BURST_SIZE_S) | (dma_ch.ChannelInit.WriteRequest << DMA_CH_CFG_WRITE_REQUEST_S) | (dma_ch.ChannelInit.WriteAck << DMA_CH_CFG_WRITE_ACK_EN_S);
    }

    void LCD::pin_select() const
    {
        if (cs_ != -1)
            ll_gpio_write(cs_, SELECT);
    }
    void LCD::pin_unselect() const
    {
        if (cs_ != -1)
            ll_gpio_write(cs_, UNSELECT);
    }

    void LCD::pin_data() const
    {
        ll_gpio_write(dc_, DATA);
    }

    void LCD::pin_command() const
    {
        ll_gpio_write(dc_, COMMAND);
    }

    HAL_StatusTypeDef LCD::init(int8_t dc, int8_t cs)
    {
        if (spi_ == nullptr)
            return HAL_ERROR;
        dc_ = dc;
        cs_ = cs;
        if (dc_ != -1)
            ll_gpio_set_output(dc_);
        else
            return HAL_ERROR;
        ll_gpio_set_ds(P1_11, PIN_DS_8);
        if (cs_ != -1)
            ll_gpio_set_output(cs_);
        return HAL_OK;
    }
    HAL_StatusTypeDef LCD::init(SPI_TypeDef *spi, DMA_ChannelHandleTypeDef dma_ch, int8_t dc, int8_t cs)
    {
        spi_ = spi;

        channel_index_ = dma_ch.ChannelInit.Channel;
        instance_ = dma_ch.dma->Instance;
        CFG_write_ &= DMA_CH_CFG_IRQ_EN_M;
        CFG_write_ |= DMA_CH_CFG_ENABLE_M | (dma_ch.ChannelInit.Priority << DMA_CH_CFG_PRIOR_S) | (dma_ch.ChannelInit.ReadMode << DMA_CH_CFG_READ_MODE_S) | (dma_ch.ChannelInit.ReadInc << DMA_CH_CFG_READ_INCREMENT_S) | (dma_ch.ChannelInit.ReadSize << DMA_CH_CFG_READ_SIZE_S) | (dma_ch.ChannelInit.ReadBurstSize << DMA_CH_CFG_READ_BURST_SIZE_S) | (dma_ch.ChannelInit.ReadRequest << DMA_CH_CFG_READ_REQUEST_S) | (dma_ch.ChannelInit.ReadAck << DMA_CH_CFG_READ_ACK_EN_S) | (dma_ch.ChannelInit.WriteMode << DMA_CH_CFG_WRITE_MODE_S) | (dma_ch.ChannelInit.WriteInc << DMA_CH_CFG_WRITE_INCREMENT_S) | (dma_ch.ChannelInit.WriteSize << DMA_CH_CFG_WRITE_SIZE_S) | (dma_ch.ChannelInit.WriteBurstSize << DMA_CH_CFG_WRITE_BURST_SIZE_S) | (dma_ch.ChannelInit.WriteRequest << DMA_CH_CFG_WRITE_REQUEST_S) | (dma_ch.ChannelInit.WriteAck << DMA_CH_CFG_WRITE_ACK_EN_S);
        return init(dc, cs);
    }

    HAL_StatusTypeDef LCD::command(uint8_t command) const
    {
        HAL_StatusTypeDef error = HAL_OK;
        pin_select();
        dma_wait();
        pin_command();
        error = print_mas(&command, 1);
        dma_wait();
        pin_data();
        pin_unselect();
        return error;
    }

    HAL_StatusTypeDef LCD::data(uint8_t data) const
    {
        HAL_StatusTypeDef error = HAL_OK;
        pin_select();
        error = print_mas(&data, 1);
        pin_unselect();
        return error;
    }

    HAL_StatusTypeDef LCD::print_mas(uint8_t *mas, uint32_t size) const
    {
        write_spi_dma(mas, size - 1);
        return HAL_OK;
    }
    // HAL_StatusTypeDef 
    void LCD::print_mas(const uint16_t *mas, uint32_t size) const
    {
        write_spi_dma((uint8_t*)mas, size * 2 - 1);
    }
    // лучше не использовать многократно
    HAL_StatusTypeDef LCD::dma_wait() const
    {
        uint32_t mask = (1 << channel_index_) << DMA_STATUS_READY_S;
        uint32_t Timeout = DMA_TIMEOUT_DEFAULT;
        while (Timeout-- != 0)
        {
            if ((instance_->CONFIG_STATUS & mask) != 0)
            {
                return HAL_OK;
            }
        }
        return HAL_TIMEOUT;
    }
    void LCD::wait_dma() const
    {
        uint32_t mask = (1 << channel_index_) << DMA_STATUS_READY_S;
        while (instance_->CONFIG_STATUS & mask)
        {
            // ждём окончания передачи
        }
    }

    uint8_t LCD::dma_busy() const
    {
        return 1; // TODO когда-нибудь
    }

    HAL_StatusTypeDef LCD::write_spi(uint8_t *data, uint32_t size) const
    {
        if (!(spi_->ENABLE & SPI_ENABLE_M))
            spi_->ENABLE = SPI_ENABLE_M;
        uint32_t wait = 0;
        uint32_t i = 0;
        while (i < size && wait < expectation)
        {
            wait++;
            if ((spi_->INT_STATUS & SPI_INT_STATUS_TX_FIFO_NOT_FULL_M))
            {
                spi_->TXDATA = data[i];
                i++;
                wait = 0;
            }
        }
        if (wait == expectation)
            return HAL_TIMEOUT;

        volatile uint32_t unused = spi_->INT_STATUS; // Очистка флагов ошибок чтением
        (void)unused;
        return HAL_OK;
    }

    void LCD::write_spi_dma(const uint8_t *data, uint32_t size) const
    {
        uint32_t mask = (1 << channel_index_) << DMA_STATUS_READY_S;
        uint32_t Timeout = DMA_TIMEOUT_DEFAULT;
        while (Timeout-- != 0)
        {
            if ((instance_->CONFIG_STATUS & mask) != 0)
            {
                instance_->CHANNELS[channel_index_].SRC = (uint32_t)data;
                instance_->CHANNELS[channel_index_].DST = (uint32_t)&spi_->TXDATA;
                instance_->CHANNELS[channel_index_].LEN = size;
                instance_->CHANNELS[channel_index_].CFG = CFG_write_;
                break;
            }
        }
    }
}