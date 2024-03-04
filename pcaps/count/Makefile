CC = g++
CFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lboost_serialization

SRCS = countpcapitems.cpp querycount.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC1 = countpcapitems
EXEC2 = querycount

.PHONY: all clean

all: $(EXEC1) $(EXEC2)

$(EXEC1): countpcapitems.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(EXEC2): querycount.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(EXEC1) $(EXEC2)
