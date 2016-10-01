#!/bin/bash

echo "***********"
echo "** Build **"
echo "***********"

mkdir ./build/
cd ./build/
cmake ..
make
