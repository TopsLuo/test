#!/bin/bash
#功能：批量加入媒体服务器的脚本
#参数$1: 外部的参数，表示join的数量
#使用方法: ./join.sh 16
JoinTemplate="{\"room_id\" : 123, \"message_type\" : \"join\",\"to\" : \"medis_server\",\"stream_type\" : 3,\"device_type\" : \"minicc_2\", \"device_sn\" : \"DEVICESN\",\"global_id\" : GLOBALID,\"coordinater\" : \"TopsLuo\",\"passwd\" : \"killer\",\"video\" : 1,\"audio\" : 1}"

for ((i=0; i<$1; ++i))
do
    temp0=`echo ${JoinTemplate/DEVICESN/device_$i}`
    temp1=`echo ${temp0/GLOBALID/$i}`
    ./goStr "$temp1"
done
