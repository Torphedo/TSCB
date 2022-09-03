#!/bin/bash
mkdir build
cd build
cmake -G "Unix Makefiles" ../src
cmake --build . --config Release
cp ../MainField.tscb .
./TSCB
