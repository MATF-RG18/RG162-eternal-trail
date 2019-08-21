PROGRAM = eternal_trail
CC      = gcc
FLAGS   = -lglut -lGLU -lGL -lm -lgsl -lgslcblas
CFLAGS  = -Wall -Wextra -Wno-unused-parameter -I/usr/X11R6/include -I/usr/pkg/include
LDFLAGS = -L/usr/X11R6/lib -L/usr/pkg/lib

IGSL    = -I/home/nikola/gsl/include 
LGSL    = -L/home/nikola/gsl/lib

$(PROGRAM): et_main.o et_draw.o collision.o game_state.o set_lights_materials.o
	$(CC) -o $(PROGRAM) $(LDFLAGS) $(LGSL) et_main.o et_draw.o collision.o set_lights_materials.o game_state.o $(FLAGS)

et_main.o: et_main.c et.h
	$(CC) -c et_main.c et.h $(CFLAGS) $(IGSL)

et_draw.o: et_draw.c et.h
	$(CC) -c et_draw.c et.h $(CFLAGS) $(IGSL)

collision.o: collision.c et.h
	$(CC) -c collision.c et.h $(CFLAGS) $(IGSL)

game_state.o: game_state.c et.h
	$(CC) -c game_state.c et.h $(CFLAGS) $(IGSL)

set_lights_materials.o: set_lights_materials.c et.h
	$(CC) -c set_lights_materials.c et.h $(CFLAGS) $(IGSL)



.PHONY: clean

clean: 
	rm *.o *.gch $(PROGRAM)