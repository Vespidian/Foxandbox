CC = gcc
CFLAGS = -Werror -Wall -lmingw32 -llua54 -lSDL2main -lSDL2 -lSDL2_image -g

INCLUDE = -isystem C:/c_libs/include/ -LC:/c_libs/lib/

C_SOURCES = $(wildcard *.c */*.c)
HEADERS = $(wildcard *.h */*.h)

OBJ = $(C_SOURCES:.c=.o)


../bin/foXandbox.exe: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDE)
	
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
	
	
clean:
	rm -rf *.o
	rm -rf */*.o
	rm ../log.txt
	rm ../bin/*.exe

wipe_levels:
	cd ../ && find . -name "*,*" -type f -delete
