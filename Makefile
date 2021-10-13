CC = gcc
CFLAGS = -Wall -Wextra -pedantic
TARGET = ggalloc

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c
