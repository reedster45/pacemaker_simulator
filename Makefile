#
#	Makefile for hello project
#

DEBUG = -g
CC = q++
LD = q++


TARGET = -Vgcc_ntox86_64
#TARGET = -Vgcc_ntox86
#TARGET = -Vgcc_ntoarmv7le
#TARGET = -Vgcc_ntoaarch64le


CFLAGS += $(DEBUG) $(TARGET) -Wall
LDFLAGS+= $(DEBUG) $(TARGET)
BINS = pacemaker heart
all: $(BINS)

clean:
	rm -f *.o $(BINS);
#	cd solutions; make clean


pacemaker.o: pacemaker.cpp pacemaker.h
heart.o: heart.cpp pacemaker.h