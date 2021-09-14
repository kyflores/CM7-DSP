#include <iostream>
#include "core.h"
#include "dsp_adc.h"

uint32_t mem1[1024] = {0};
uint32_t mem2[1024] = {0};

int main() {
    Core core;
    core.Initialize();
    DspAdc adc(core);
    HAL_StatusTypeDef err = adc.Initialize();
    err = adc.Start();

    size_t counter = 0;
    while(counter < 1024) {
        uint32_t result[2] = {0};
        size_t n = adc.GetNSamples(result, 2);
        if(n == 2) {
            mem1[counter] = result[0];
            mem2[counter] = result[1];
            counter++;
        }
    }
    std::cout << "Done." << std::endl;

}
