#!/bin/bash

#-------2017年 06月 02日 星期五 10:18:48 CST---------
#layout测试脚本，目前的布局有0 - 27，一共28种
#远行该脚本会有相应的YUV（以布局的名称命名）生
#成在/tmp目录中
#----------------------------------------------------
for i in `seq 0 27`
do
    ./layout $i
done
