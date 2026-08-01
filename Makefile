.PHONY: all release clean
all: scbattery-monitor
release: all


CXXFLAGS = -std=c++20 -Wall -Werror

DEBUG_FLAGS := -g -Og
RELEASE_FLAGS := -O2

ifeq ($(filter release,$(MAKECMDGOALS)), release)
  CXXFLAGS += $(RELEASE_FLAGS)
  IS_RELEASE = 1
else
  CXXFLAGS += $(DEBUG_FLAGS)
endif

ifeq ($(OS),Windows_NT)
  HIDAPI_PKG ?= hidapi

  ifeq ($(IS_RELEASE),1)
    LDFLAGS += -mwindows
  endif
else
  HIDAPI_PKG ?= hidapi-hidraw
endif

QT_PKG := Qt6Widgets

CXXFLAGS += $(shell pkg-config --cflags $(HIDAPI_PKG))
CXXFLAGS += $(shell pkg-config --cflags $(QT_PKG))

LDLIBS += $(shell pkg-config --libs $(HIDAPI_PKG))
LDLIBS += $(shell pkg-config --libs $(QT_PKG))

TRITON_SRC := $(wildcard tritonLib/*.cpp tritonLib/*/*.cpp tritonLib/*.c tritonLib/*/*.c)
BATTERYMON_SRC := $(wildcard src/*.cpp src/*/*.cpp src/*.c src/*/*.c)


scbattery-monitor: $(BATTERYMON_SRC) $(TRITON_SRC)
	g++ -ItritonLib -o scbattery-monitor $^ $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

clean:
	rm -f scbattery-monitor scbattery-monitor.exe qrc_resources.cpp