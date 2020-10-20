CC = tcc
CFLAGS = -Werror -Wall -lSDL2 -lSDL2_image -llua53

BUILDVARS = -Wl,-subsystem=gui

CFILES = main.c data.c initialize.c level_systems.c entity_systems.c inventory.c map_generation.c render_systems.c lua_systems.c
OUTFILE = world_builder.exe

debug: $(CFILES)
	
	$(CC) $(CFLAGS) -o $(OUTFILE) $(CFILES)
	
build: $(CFILES)
	$(CC) $(BUILDVARS) $(CFLAGS) -o $(OUTFILE) $(CFILES)

temp:
	CFLAGS = -Werror -Wall -LC:\Users\Chris\Code\C\SDL_TCC\lib\ -lSDL2 -static -lSDL2_image -static -llua53 -static
	$(CC) $(CFLAGS) -o $(OUTFILE) $(CFILES)

# CC = gcc
# CFLAGS = -Werror -Wall -lmingw32 -llua54 -lSDL2main -lSDL2 -lSDL2_image

# INCLUDE = -isystem C:/c_libs/include/ -LC:/c_libs/lib/

# BUILDVARS = -Wl,-subsystem=gui

# CFILES = main.c data.c initialize.c level_systems.c entity_systems.c inventory.c map_generation.c render_systems.c lua_systems.c
# OUTFILE = mingw_build/world_builder.exe

# debug: $(CFILES)
# 	$(CC) $(CFILES) $(CFLAGS) $(INCLUDE) -o $(OUTFILE)
	
# build: $(CFILES)
# 	$(CC) $(CFILES) $(INCLUDE) $(BUILDVARS) $(CFLAGS) -o $(OUTFILE)