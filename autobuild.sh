#!/bin/bash

set -e
#在脚本中运行的每个命令都会被监控，如果任何命令的退出状态码不为0（即失败），
#整个脚本就会立即停止执行，并且将失败的命令的错误状态码传递给脚本的调用者。

#rm -rf `pwd`/build/*

cd `pwd`/build/ && 
    cmake ..    &&
    make        

cd ..

#cp -r `pwd`/src/include `pwd`/lib/