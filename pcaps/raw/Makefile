# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS = -Wall -Wextra -std=c++11 -ggdb

# Libraries
LDFLAGS = -lpcap -lz

# Source and Output
SRC = ports-distributions.cpp
OUT = ports-distributions

# Default target
all: $(OUT)

# Compile the program
$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

# Clean build files
clean:
	rm -f $(OUT)

# Run the program with a sample pcap file
run: $(OUT)
	./$(OUT) ports-distributions.pcap

