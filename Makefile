
EXECS := identify
CC := clang
CFLAGS := -Wall -Wextra -Werror -Wpedantic -Ofast
LFLAGS := -lm


SOURCES := $(wildcard *.c)
COMMON_SOURCE := ${shell grep -L "int main(" ${SOURCES}}
COMMON_OBJECT := $(COMMON_SOURCE:%.c=%.o)

.PHONY: all clean format

all: $(EXECS)


$(EXECS): %: %.o $(COMMON_OBJECT)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(EXECS) *.o

format:
	clang-format -i -style=file *.[c,h]


