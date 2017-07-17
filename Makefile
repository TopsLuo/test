CFLAGS  = -I../deps/install/include
LDFLAGS = -L../deps/install/lib
LIBS    = -lpt -lavdevice -lavformat -lavfilter \
          -lpostproc -lavcodec -lswresample -lswscale -lavutil -lpthread -ldl \
          -lxml2 -lz -ljsoncpp -lx264 -lfreetype -lglog -ldl -lm -w -g
SOURCES = $(wildcard *.cxx *.c)
OBJS = $(sort $(basename $(SOURCES)))
THIRD_OBJS = $(addsuffix .o, $(OBJS))

#all : main createjson codec aac parsejson transcode_aac fff
.phony: all clean
all : $(OBJS)

server:server.o
	gcc -o server server.o $(CFLAGS) $(LDFLAGS) $(LIBS)

epollserver:epollserver.o
	gcc -o epollserver epollserver.o

client:client.o
	gcc -o client client.o $(CFLAGS) $(LDFLAGS) $(LIBS)

create_json:
	g++ -o create_json  create_json.cxx $(CFLAGS) $(LDFLAGS) $(LIBS)

parse_json:
	g++ -o parse_json  parse_json.cxx $(CFLAGS) $(LDFLAGS) $(LIBS)

codec:codec.o
	gcc -o codec codec.o $(CFLAGS) $(LDFLAGS) $(LIBS)

aacCodec:aacCodec.o
	gcc -o aacCodec aacCodec.o $(CFLAGS) $(LDFLAGS) $(LIBS)

list:
	g++ -o list list.cxx $(CFLAGS) $(LDFLAGS) $(LIBS)

resample_mix:resample_mix.o
	gcc -o resample_mix resample_mix.o $(CFLAGS) $(LDFLAGS) $(LIBS)

filter_audio:filter_audio.o
	gcc -o filter_audio filter_audio.o $(CFLAGS) $(LDFLAGS) $(LIBS)

mixaudio:mixaudio.o
	gcc -o mixaudio mixaudio.o

layout:
	g++ -c layout.cxx -o layout.o
	g++ -o layout layout.o

recv_client:recv_client.o
	gcc -o recv_client recv_client.o -lpthread -g

tcp_client:tcp_client.o
	gcc -o tcp_client tcp_client.o -lpthread

tcp_server:tcp_server.o
	gcc -o tcp_server tcp_server.o -lpthread

transcoding:transcoding.o
	gcc -o transcoding transcoding.o $(CFLAGS) $(LDFLAGS) $(LIBS)

mux:mux.o
	gcc -o mux mux.o $(CFLAGS) $(LDFLAGS) $(LIBS)

freetype:freetype.o
	gcc -o freetype freetype.o $(CFLAGS) $(LDFLAGS) $(LIBS)

glog:
	g++ -o glog glog.cxx $(CFLAGS) $(LDFLAGS) $(LIBS)

map:
	g++ -o map map.cxx

time:time.o
	gcc -o time time.o $(CFLAGS) $(LDFLAGS) $(LIBS)

time_spend:
	g++ -o time_spend.o -c time_spend.cxx
	g++ -o time_spend time_spend.o

clean:
	rm -rf $(OBJECTS) $(OBJS) $(THIRD_OBJS)
