CXX := g++
CXXFLAGS := -Wextra -Wall -std=c++17
LIBS := -larchive -lcurl

TARGET := main

SRC_DIR := src
BUILD_DIR := build

SRC := src/main.cpp
TARGET := apm
LIBS := -larchive -lcurl

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)