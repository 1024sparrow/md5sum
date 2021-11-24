# sudo apt install -y gcc-mingw-w64 (Ubuntu 20.04 LTS)

CC = gcc
CFLAGS = -Wall # -funsigned-char -Wno-unused-result 
PROGRAM_NAME = md5sum

all: $(PROGRAM_NAME) $(PROGRAM_NAME)-win64.exe

$(PROGRAM_NAME)-win64.exe: *.c
	x86_64-w64-mingw32-gcc $^ -o $@

$(PROGRAM_NAME): *.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm $(PROGRAM_NAME) $(PROGRAM_NAME)-win64.exe
