CC = g++
#CFLAGS = -std=c++11 -Wall -Wextra
CFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lboost_serialization

SRCS = countpcapitems.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = countpcapitems

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(EXEC)

