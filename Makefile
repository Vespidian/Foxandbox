CC = tcc
CFLAGS = -Werror -Wall -lSDL2 -lSDL2_image -llua53

BUILDVARS = -Wl,-subsystem=gui

CFILES = main.c data.c initialize.c tileMap.c collision.c inventory.c mapGeneration.c renderSystems.c
OUTFILE = world_builder.exe

debug: $(CFILES)
	
	$(CC) $(CFLAGS) -o $(OUTFILE) $(CFILES)
	
build: $(CFILES)
	$(CC) $(BUILDVARS) $(CFLAGS) -o $(OUTFILE) $(CFILES)

temp:
	CFLAGS = -Werror -Wall -LC:\Users\Chris\Code\C\SDL_TCC\lib\ -lSDL2 -static -lSDL2_image -static -llua53 -static
	$(CC) $(CFLAGS) -o $(OUTFILE) $(CFILES)