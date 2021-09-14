#pragma once

#include "stm32f7xx_hal.h"

/* Performs the first stage init for processor resources:
 * Enable caches
 * Configure clock operation
 * Enable peripheral clocks
 */
class Core {
 public:
    Core() : initialized_(false) {}
    HAL_StatusTypeDef Initialize();

 private:
    HAL_StatusTypeDef ConfigureOscillator();
    HAL_StatusTypeDef ConfigureClockDomains();
    void EnableCaches();
    void EnablePeripheralClocks();

    bool initialized_;
};
