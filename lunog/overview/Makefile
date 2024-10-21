CC = g++
CFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lboost_serialization -lboost_iostreams -lboost_system -lz

SRCS = basic-stats.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC1 = basic-stats


.PHONY: all clean

all: $(EXEC1)

$(EXEC1): basic-stats.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(EXEC1) $(EXEC2)
