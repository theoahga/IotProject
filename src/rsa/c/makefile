CC = gcc
CFLAGS = -Wall -Wextra -g
LFLAGS = -I/usr/include/openssl -L/usr/lib/x86_64-linux-gnu 
TARGET = rsa_sample

all: clean $(TARGET)

$(TARGET): 
	$(CC) $(CFLAGS) $(LFLAGS) rsa.c sample.c -o $(TARGET) -lssl -lcrypto


clean:
	rm -f $(TARGET)

