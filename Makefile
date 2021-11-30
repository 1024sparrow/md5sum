# sudo apt install -y gcc-mingw-w64 (Ubuntu 20.04 LTS)

CC = gcc
CFLAGS = -Wall # -funsigned-char -Wno-unused-result 
PROGRAM_NAME = md5sum

all: $(PROGRAM_NAME) $(PROGRAM_NAME)-win64.exe

$(PROGRAM_NAME)-win64.exe: *.c SRC
	#x86_64-w64-mingw32-gcc $^ -o $@
	x86_64-w64-mingw32-gcc *.c -o $@

$(PROGRAM_NAME): *.c SRC
	$(CC) $(CFLAGS) *.c -o $@

clean:
	rm $(PROGRAM_NAME) $(PROGRAM_NAME)-win64.exe
