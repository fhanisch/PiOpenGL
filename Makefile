INC=-I/home/felix/Entwicklung/userland
LIB=-lbcm_host -lEGL -lGLESv2 -lSDL2 -L/home/felix/Entwicklung/userland/build/lib
EXEC = PiOpenGL

all: $(EXEC)

$(EXEC): main.c
	gcc -Wall -o $@ main.c $(INC) $(LIB)
	
clean:
	rm $(EXEC)
