#pragma once

#include <functional>
#include <cstdint>
#include "boost/circular_buffer.hpp"
#include "stm32f7xx_hal.h"
#include "core.h"

typedef std::function<void(void)> InterruptHandler;

/*
 * About this class:
 * This sets up ADC1 to convert multiple channels and use DMA to
 * place the results into a staging buffer.
 * The half complete callback copies the first 1/2 of the buffer into
 * separate memory to be processed, and the complete callback does
 * the same for the upper half. The data must be copied and processed
 * before the second half of the array is allowed to fill. The size of
 * DMA target buffer and DMA length should extended appropriately.
 * This is a substitute for double buffering since the HAL driver
 * does not seem to implement ADC + dual DMA buffer directly.
 * 
 * A primitive lock check (that I'm not sure even works) is there,
 * which may catch an overrun if a handler is copying data and 
 * the other interrupt goes off to preempt. Need to check if they'll
 * just nest in this case?
 * 
 * The handlers push their data into fixed sized circular buffer, and a
 * getter is provided to pull data from said buffer.
 */

class DspAdc {
 public:
    enum class Channels {
       C3_PA3 = 1,
       C10_PC0,
       CONV_CHANNEL_MAX = C10_PC0
    };

    static constexpr size_t CONV_CHANNELS = 2;
    static constexpr size_t DMA_BUFFER_WIDTH = 128;
    DspAdc(Core& core) :
        core_(core), initialized_(false),
        sample_buffer_(DMA_BUFFER_WIDTH) {}
    HAL_StatusTypeDef Initialize();
    HAL_StatusTypeDef Start();
    HAL_StatusTypeDef Stop();
    size_t GetNSamples(uint32_t* dst, size_t N);


 private:
    void Handler();
    void CompleteCallback();
    void HalfCompleteCallback();
    HAL_StatusTypeDef ConfigurePins();
    HAL_StatusTypeDef ConfigureConverter();
    HAL_StatusTypeDef ConfigureDMA();
    HAL_StatusTypeDef ConfigureTimer(uint32_t target_hz);

    Core& core_;
    ADC_HandleTypeDef adc_handle_;
    DMA_HandleTypeDef dma_handle_;
    TIM_HandleTypeDef tim_handle_;
    bool initialized_;
    bool is_overrun_detected_;

    // DMA target buffer is volatile
    boost::circular_buffer<uint32_t> sample_buffer_;
};
