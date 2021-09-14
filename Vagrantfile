# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/bionic64"

  config.vm.provider "virtualbox" do |vb|
    # Display the VirtualBox GUI when booting the machine
    vb.gui = false
    vb.name = "CM Tools"

    # Customize the amount of memory on the VM:
    vb.memory = "4096"
    vb.cpus = "2"
    vb.customize ["modifyvm", :id, "--vram", "32"]

    # Enable the USB controller and grab the STLINK debug interface.
    vb.customize ["modifyvm", :id, "--usbxhci", "on"]
    vb.customize ['usbfilter', 'add', '0', '--target', :id, '--name', 'STLINK', '--vendorid', '0x0483', '--productid', '0x374b']
  end

  config.vm.synced_folder "./", "/vagrant"

  config.vm.provision "shell", inline: <<-SHELL
    apt-get update
    apt-get upgrade
    apt-get install -y cmake git qemu-system-arm build-essential
    apt-get install -y openocd
    cd /home/vagrant

    # Install GNU-ARM tools
    wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2018q2/gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2
    tar xvjf gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2
    rm gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2

    # Install patched OpenOCD for STM32F7
    wget https://github.com/gnu-mcu-eclipse/openocd/releases/download/v0.10.0-7-20180123/gnu-mcu-eclipse-openocd-0.10.0-7-20180123-1217-centos64.tgz
    tar xvf ./gnu-mcu-eclipse-openocd-0.10.0-7-20180123-1217-centos64.tgz
    chmod -R -w gnu-mcu-eclipse/openocd/0.10.0-7-20180123-1217/
    rm gnu-mcu-eclipse-openocd-0.10.0-7-20180123-1217-centos64.tgz

    # Boost 1.67
    wget https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.gz
    tar xvf boost_1_67_0.tar.gz
    rm boost_1_67_0.tar.gz

  SHELL
end
