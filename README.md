# Linear-Cryptanalysis-using-SAT
Implemented Sypher00D to find it's Linear Trail using SAT Solver

**#Installation Guidance to run this code. First install these packages.**

sudo apt-get install build-essential cmake libgmp-dev

# not required but very useful
sudo apt-get install zlib1g-dev

git clone https://github.com/meelgroup/cadical
cd cadical
git checkout mate-only-libraries-1.8.0
./configure
make
cd ..

git clone https://github.com/meelgroup/cadiback
cd cadiback
git checkout mate
./configure
make
cd ..

git clone https://github.com/msoos/cryptominisat
cd cryptominisat
mkdir build && cd build
cmake ..
make
sudo make install
sudo ldconfig

**//Path setup guide** 
At the end of installation the path will be shown like this (/home/ajay/Desktop/Programs/Improved_Attacks_GIFT64-main/cadiback:/home/ajay/Desktop/Programs/Improved_Attacks_GIFT64-main/cadical/build) then just copy it and type over terminal
**<<<<<<<<Path Set up>>>>>>>>>>>>**

export LD_LIBRARY_PATH=/home/ajay/Desktop/Programs/Improved_Attacks_GIFT64-main/cadiback:/home/ajay/Desktop/Programs/Improved_Attacks_GIFT64-main/cadical/build

**#To compile this code**
g++ Differential.cpp /usr/local/lib/libcryptominisat5.so

**#To run just type**
./a.out
