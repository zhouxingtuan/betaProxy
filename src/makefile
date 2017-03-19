
LIB_NAME := libcore.a

TARGET = proxy

BIN = ../bin

INC_PATH := -I$(THIRD_CURL_INC)
LIB_PATH :=
LIBS = -lssl -lcrypto -lz -lrt -lstdc++ -ldl

# 注意，这里的 -ldl 需要放在flag的最后才能生效
CXXFLAGS = -O2 -Wall -pthread -std=c++11 $(INC_PATH) $(LIB_PATH) $(LIBS)
CFLAGS = -O2 -Wall -pthread $(INC_PATH) $(LIB_PATH) $(LIBS)
CC = gcc
AR = ar
ARFLAGS = cr
DEBUG= -g -ggdb


OBJS = common.o systemsignal.o token.o buffer.o packet.o object.o timer.o epoll.o \
		accept.o client.o listener.o proxy.o


COBJS =

all: $(COBJS) $(OBJS) $(LIB_NAME) $(TARGET)

$(OBJS): %.o:%.cpp
	$(CC) $(DEBUG) -c $< -o $@ $(CXXFLAGS)

$(COBJS): %.o:%.c
	$(CC) $(DEBUG) -c $< -o $@ $(CFLAGS)

main.o: main.cpp
	$(CC) $(DEBUG) -c $< -o $@ $(CFLAGS)

$(LIB_NAME):$(OBJS) $(COBJS)
	$(AR) $(ARFLAGS) $(LIB_NAME) $(OBJS) $(COBJS)

$(TARGET): main.o $(OBJS)
	$(MAKE) -C ./../third/jemalloc
	$(MAKE) -C ./..//libcore
	$(CC) $(DEBUG) main.o $(OBJS) $(STATIC_LIB) -o $(BIN)/$(TARGET) $(CFLAGS)

clean:
	-$(RM) $(BIN)/$(TARGET)
	-$(RM) $(LIB_NAME)
	-$(RM) *.o

common.o:common.cpp common.h

systemsignal.o:systemsignal.cpp systemsignal.h

token.o:common.o token.cpp token.h

buffer.o:common.o buffer.cpp buffer.h

packet.o:buffer.o packet.cpp packet.h

object.o:common.o object.cpp object.h

timer.o:common.o timer.cpp timer.h

epoll.o:common.o epoll.cpp epoll.h

accept.o:object.o epoll.o packet.o timer.o accept.cpp accept.h

client.o:accept.o client.cpp client.h

listener.o:object.o epoll.o listener.cpp listener.h

proxy.o:accept.o client.o listener.o proxy.cpp proxy.h










