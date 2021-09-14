#include "dsp_adc.h"

InterruptHandler DMAHandler;
InterruptHandler ConvCompleteHandler;
InterruptHandler HalfConvCompleteHandler;

/* This buffer only behaves properly when placed globally. Initially,
 * it was setup as a member variable of DspAdc, but it would not update
 * as expected every time after the first. Tested on O0, O2, with
 * or without volatile.
 */
volatile uint32_t dma_buffer_[DspAdc::DMA_BUFFER_WIDTH];

HAL_StatusTypeDef DspAdc::Initialize() {
    if (initialized_)
        return HAL_OK;
    
    HAL_StatusTypeDef error = core_.Initialize();
    if (error)
        return error;

    error = ConfigurePins();
    if (error)
        return error;

    error = ConfigureDMA();
    if (error)
        return error;

    error = ConfigureConverter();
    if (error)
        return error;

    error = ConfigureTimer(84000);
    if (error)
        return error;

    DMAHandler = [this] {
        Handler();
    };

    ConvCompleteHandler = [this] {
        CompleteCallback();
    };

    HalfConvCompleteHandler = [this] {
        HalfCompleteCallback();
    };

    initialized_ = true;
    return HAL_OK;
}

HAL_StatusTypeDef DspAdc::Start() {
    HAL_TIM_PWM_Start(&tim_handle_, TIM_CHANNEL_1);
    return HAL_ADC_Start_DMA(&adc_handle_, (uint32_t*)dma_buffer_, DMA_BUFFER_WIDTH);
}

HAL_StatusTypeDef DspAdc::Stop() {
    HAL_TIM_PWM_Stop(&tim_handle_, TIM_CHANNEL_1);
    return HAL_ADC_Stop_DMA(&adc_handle_);
}

// Returns the number of samples received
size_t DspAdc::GetNSamples(uint32_t* dst, size_t N) {
    for(size_t n = 0; n < N; n++) {
        if (sample_buffer_.empty()) {
            return n;
        }
        dst[n] = sample_buffer_.front();
        sample_buffer_.pop_front();
    }
    return N;
}

void DspAdc::Handler() {
    HAL_DMA_IRQHandler(adc_handle_.DMA_Handle);
}

void DspAdc::HalfCompleteCallback() {
    // Operate on the lower 1/2 of the target buffer
    for(size_t i = 0; i < DMA_BUFFER_WIDTH / 2; i++) {
        sample_buffer_.push_back((uint32_t)dma_buffer_[i]);
    }
}

void DspAdc::CompleteCallback() {
    // Operate on the upper 1/2 of the target buffer
    for(size_t i = DMA_BUFFER_WIDTH / 2 ; i < DMA_BUFFER_WIDTH; i++) {
        sample_buffer_.push_back((uint32_t)dma_buffer_[i]);
    }
}

HAL_StatusTypeDef DspAdc::ConfigurePins() {
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    return HAL_OK;
}

HAL_StatusTypeDef DspAdc::ConfigureConverter() {
    // The State variable here comes up as a garbage value at init
    // which confuses HAL and starts it in an error state.
    // The garbage value is observed before ADC clocks come on,
    // but doesn't seem to get reset correctly.
    adc_handle_.State = 0;
    adc_handle_.Instance = ADC1;
    adc_handle_.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    adc_handle_.Init.Resolution = ADC_RESOLUTION_12B;
    adc_handle_.Init.ScanConvMode = ADC_SCAN_ENABLE;
    adc_handle_.Init.ContinuousConvMode = DISABLE;
    adc_handle_.Init.DiscontinuousConvMode = DISABLE;
    adc_handle_.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc_handle_.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    adc_handle_.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
    adc_handle_.Init.NbrOfConversion = CONV_CHANNELS;
    adc_handle_.Init.DMAContinuousRequests = ENABLE;
    adc_handle_.Init.EOCSelection = ADC_EOC_SEQ_CONV;


    ADC_ChannelConfTypeDef c3_config;
    c3_config.Channel = ADC_CHANNEL_3;
    c3_config.Rank = static_cast<uint32_t>(Channels::C3_PA3);
    c3_config.SamplingTime = ADC_SAMPLETIME_112CYCLES;

    ADC_ChannelConfTypeDef c10_config;
    c10_config.Channel = ADC_CHANNEL_10;
    c3_config.Rank = static_cast<uint32_t>(Channels::C10_PC0);
    c10_config.SamplingTime = ADC_SAMPLETIME_112CYCLES;

    HAL_StatusTypeDef error;
    error = HAL_ADC_Init(&adc_handle_);
    if (error)
        return error;
    error = HAL_ADC_ConfigChannel(&adc_handle_, &c3_config);
    if (error)
        return error;

    error = HAL_ADC_ConfigChannel(&adc_handle_, &c10_config);
    if (error)
        return error;

    return HAL_OK;

}

HAL_StatusTypeDef DspAdc::ConfigureTimer(uint32_t target_hz) {
    constexpr uint32_t psc = 4;  
    uint32_t period = 216000000 / psc / target_hz;
    tim_handle_.Instance = TIM1;
    tim_handle_.Init.Prescaler = psc - 1; // The operating ratio is psc + 1
    tim_handle_.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim_handle_.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim_handle_.Init.Period = period - 1;
    tim_handle_.Init.RepetitionCounter = 0;
    HAL_StatusTypeDef error = HAL_TIM_PWM_Init(&tim_handle_);
    if (error)
        return error;
    
    TIM_OC_InitTypeDef oc_init;
    oc_init.OCMode = TIM_OCMODE_PWM1;
    oc_init.Pulse = period / 2; // Duty cycle is fine at 50% for timer triggering.
    oc_init.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc_init.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    oc_init.OCFastMode = TIM_OCFAST_DISABLE;
    oc_init.OCIdleState = TIM_OCIDLESTATE_RESET;
    oc_init.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    error = HAL_TIM_PWM_ConfigChannel(&tim_handle_, &oc_init, TIM_CHANNEL_1);
    if (error)
        return error;

    return error;
}

HAL_StatusTypeDef DspAdc::ConfigureDMA() {
    dma_handle_.Instance = DMA2_Stream0;
    dma_handle_.Init.Channel = DMA_CHANNEL_0;
    dma_handle_.Init.Direction = DMA_PERIPH_TO_MEMORY;
    dma_handle_.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_handle_.Init.MemInc = DMA_MINC_ENABLE;
    dma_handle_.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_handle_.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    dma_handle_.Init.Mode = DMA_CIRCULAR;
    dma_handle_.Init.Priority = DMA_PRIORITY_HIGH;
    dma_handle_.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    dma_handle_.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    dma_handle_.Init.MemBurst = DMA_MBURST_SINGLE;
    dma_handle_.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_StatusTypeDef error = HAL_DMA_Init(&dma_handle_);
    if (error)
        return error;

    __HAL_LINKDMA(&adc_handle_, DMA_Handle, dma_handle_);
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    return HAL_OK;
}

extern "C"{
    void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
        if (hadc->Instance == ADC1) {
            ConvCompleteHandler();
        }
    }

    void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
        if (hadc->Instance == ADC1) {
            HalfConvCompleteHandler();
        }
    }

    void DMA2_Stream0_IRQHandler() {
        // Implement this one.
        DMAHandler();
    }
}
