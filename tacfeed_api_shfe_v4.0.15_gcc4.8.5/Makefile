SF_LIB_PATH = /usr/local/lib

CC = g++
CFLAGS = -O3 -std=c++11 -Iinclude
LDFLAGS = -Llib -Wl,-rpath=lib
LDLIBS = -lpthread -lrt

ifeq ($(nic), tacnic)
    LDLIBS += -ltacfeedmd_tacnic
else
    LDFLAGS += -L$(SF_LIB_PATH)
    LDLIBS += -lonload_zf -ltacfeedmd
endif

all: quote_demo_shfe compare_shfe

compare_shfe: compare_local.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

quote_demo_shfe: quote_demo_shfe.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f quote_demo_shfe

.PHONY: all quote_demo_shfe clean
