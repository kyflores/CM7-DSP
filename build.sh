# Run this in a container like fedora toolbox.

sudo wget --directory-prefix=/opt/ https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2018q2/gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2
sudo tar xvjf /opt/gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2 -C /opt/
sudo rm /opt/gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2

sudo wget --directory-prefix=/opt/ https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.gz
sudo tar xvf /opt/boost_1_67_0.tar.gz -C /opt/
sudo rm /opt/boost_1_67_0.tar.gz

sudo dnf install -y openocd cmake make
mkdir build/
cd build/
cmake -DBOOST_ROOT=/opt -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=CM7.cmake ..
