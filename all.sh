#!/bin/bash
# $1:   媒体服务器的视频端口
# $2:   媒体服务器的音频端口
# $3:   推流的数量
SRC0="/home/b1shen/Videos/heiyiren3.ts"
SRC1="/home/b1shen/Videos/xialuotefannao.mp4.ts"
SRC3="/home/b1shen/Videos/1min.ts"
USED_SRC=$SRC1

#./client <inputfile> <v_port> <a_port> <global_id>

for ((i=0; i<$3; ++i))
do
    nohup ./client $USED_SRC $1 $2 $i &
done
