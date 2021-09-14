# CM7-DSP
DSP on the CM7 based STM32F7

### OS Environment
The environment is Ubuntu 18.04 Server. 

### Configuring CMake
For a standalone build:

`cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/vagrant/CM7.cmake ..`

For a test or debug build that needs semihosting:

`cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=/vagrant/CM7.cmake ..`

### Launching program on debug interface
* Run tools/openocd_connect.sh
* Then `/home/vagrant/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-gdb -x /vagrant/tools/connect.gdb` 
