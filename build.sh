#!/bin/bash

target="build"

if [ ! -d $target ]; then
    mkdir -p $target
fi

cd $target

echo "*******************************************************************************"
echo "start to build asr_jd ..."


cmake -DENABLE_TEST=1 ..
cmake --build .

if [ $? -ne 0 ]; then
    echo "FAILED!"
    cd ..
    exit 1
fi

echo "*******************************************************************************"
echo "start to run tests..."

./test/test_asr_jd --gtest_color=yes $1 $2

if [ $? -ne 0 ]; then
    echo "FAILED!"
    cd ..
    exit 1
fi

cd ..

echo "*******************************************************************************"
echo "SUCCESS!"
exit 0
