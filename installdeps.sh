#!/bin/bash

# Install tclap
wget -O tclap.tar.bz2 "https://downloads.sourceforge.net/project/tclap/tclap-1.4.0-rc1.tar.bz2?ts=gAAAAABiQKlaFixgyTlC19bfFP34yMVk8i3A7Pg_Yw_oITW1oZ9Syv_45bZsXPk2vxim1ru6XnMPWCmYh1Hli27UAhqHFWGKPg%3D%3D&r=https%3A%2F%2Fsourceforge.net%2Fprojects%2Ftclap%2Ffiles%2Flatest%2Fdownload" tclap.tar.gz;
bzip2 -d tclap.tar.bz2;
tar -xvf tclap.tar;
rm tclap.tar;
cd tclap*;
mkdir build;
cd build;
cmake ..;
cmake --build .;
cmake --install .;
rm -r ../../tclap*

# Install SFML
apt install libsfml-dev