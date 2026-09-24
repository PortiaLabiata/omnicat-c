ifeq ($(OS),Windows_NT)
	CC := cl
	RM := rd /q
	CFLAGS := /W4
	LDFLAGS := 
else
	CC := clang
	RM := rm
	CFLAGS := -Wall \
			  -Wextra \
			  -Wpedantic \
			  -Og \
			  -Wno-missing-braces \
			  -g
	LDFLAGS := -static 
endif
LD := $(CC)

SRCS := main.c \
		transport.c \
		tstdio.c \
		cJSON/cJSON.c \
		json.c \
		options.c
OBJS := $(SRCS:.c=.o)
TARGET := omnicat

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJS)
	$(RM) $(TARGET)
