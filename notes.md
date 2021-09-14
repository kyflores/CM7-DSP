## Init Sequence
* Enable CPU Cache
* HAL_Init()
    * Enable Prefetch buffers
    * Set NVIC Priority Grouping
    * ADC Clock On
    * ADC Pins Clock On
    * DMA Clock on
    * GPIO Init
    * DMA Init
    * Link DMA
    * Enable ADC_DMA_IRQ
* System clock config
* ADC Init
* ADC configure Pins
* `HAL_ADC_Start_DMA`

## Peripheral Config for 1 channel DMA
### ADC
```
  AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV4;
  AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;
  AdcHandle.Init.ScanConvMode          = DISABLE;                       
  AdcHandle.Init.ContinuousConvMode    = ENABLE;                        
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       
  AdcHandle.Init.NbrOfDiscConversion   = 0;
  AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;        
  AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.NbrOfConversion       = 1;
  AdcHandle.Init.DMAContinuousRequests = ENABLE;
  AdcHandle.Init.EOCSelection          = DISABLE;
```

### DMA
```
  hdma_adc.Init.Channel  = ADCx_DMA_CHANNEL;
  hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
  hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_adc.Init.Mode = DMA_CIRCULAR;
  hdma_adc.Init.Priority = DMA_PRIORITY_HIGH;
  hdma_adc.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  hdma_adc.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
  hdma_adc.Init.MemBurst = DMA_MBURST_SINGLE;
  hdma_adc.Init.PeriphBurst = DMA_PBURST_SINGLE;
```

### Callbacks
* Only HAL_ADC_ConvCpltCallback is implemented and gets run by the DMA IRQ
* DMA IRQ is `HAL_DMA_IRQHandler`
