CC := cc
RM := rm
CFLAGS += -Wall \
			-Wextra \
			-Wpedantic \
			-Og \
			-Wno-missing-braces
LDFLAGS := -static 

LD := $(CC)

SRCS := main.c \
		transport.c \
		tstdio.c \
		tudp.c \
		tfile.c \
		tserial.c \
		ttcp.c \
		cJSON/cJSON.c \
		json.c \
		options.c
OBJS := $(SRCS:.c=.o)
TARGET := omnicat

.PHONY: all clean install

all: $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/bin

$(TARGET): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJS)
	$(RM) $(TARGET)

