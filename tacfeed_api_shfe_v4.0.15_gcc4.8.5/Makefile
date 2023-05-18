CC := gcc
CXX := g++
.DEFAULT_GOAL := all

########## for frontend dev BEGIN ###########################
module_list := wfmd-fpga-shfe-mc.so
wfmd-fpga-shfe-mc.so: tacfeed_shfe.o
        $(CXX) -shared -o $@ $^ -L ./lib -ltacfeedmd_tacnic -lrt -lpthread
########## for frontend dev END #############################

DEST = ~/.local
CFLAGS := -Wall -I $(DEST)/include -I ./include -L ./lib -fPIC -O2 -g -ltacfeedmd_tacnic -lrt -lpthread
CXXFLAGS := $(CFLAGS) --std=c++11

all: $(module_list)

install: $(module_list)
        install -d $(DEST)/lib
        install -p $(module_list) $(DEST)/lib

clean:
        $(RM) $(module_list) *.o

.PHONY: all install clean
