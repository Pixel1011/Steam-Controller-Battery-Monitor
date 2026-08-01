.PHONY: all release
all: range steam-haptics-player measure
release: all


CXXFLAGS = -std=c++20 -Wall -Werror

DEBUG_FLAGS = -g -Og
RELEASE_FLAGS = -O2

ifeq ($(filter release,$(MAKECMDGOALS)), release)
  CXXFLAGS += $(RELEASE_FLAGS)
else
  CXXFLAGS += $(DEBUG_FLAGS)
endif

ifeq ($(OS),Windows_NT)
HIDAPI_PKG ?= hidapi
UNICODE_FLAG ?= -municode
CXXFLAGS += -static
else
HIDAPI_PKG ?= hidapi-hidraw
UNICODE_FLAG ?=
endif

CXXFLAGS += `pkg-config --libs --cflags $(HIDAPI_PKG)`


TRITON_SRC = $(wildcard tritonLib/*.cpp tritonLib/*/*.cpp tritonLib/*.c tritonLib/*/*.c)
BATTERYMON_SRC = $(wildcard src/*.cpp src/*/*.cpp src/*.c src/*/*.c)

scbattery-monitor: $(RANGE_SRC) $(SHARED_SRC)
	g++ -ItritonLib -o scbattery-monitor $^ $(CXXFLAGS)