#!/bin/bash
#功能：批量离开会议，不再向媒体服务器推流
#$1:    离开的数量
LeaveTemplate="{\"room_id\" : 123,\"message_type\" : \"leave\",\"to\" : \"media_server\",\"device_type\" : \"minicc_2\",\"device_sn\" : \"DEVICESN\"}"
for ((i=0; i<$1; ++i))
do
    temp0=`echo ${LeaveTemplate/DEVICESN/device_$i}`
    echo $temp0
    ./goStr "$temp1"
done
