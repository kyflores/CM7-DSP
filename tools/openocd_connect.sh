/home/vagrant/gnu-mcu-eclipse/openocd/0.10.0-7-20180123-1217/bin/openocd \
-f /home/vagrant/gnu-mcu-eclipse/openocd/0.10.0-7-20180123-1217/scripts/board/stm32f7discovery.cfg \
-c "init" -c "arm semihosting enable" -c "reset halt"
