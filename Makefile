INC=-I/home/felix/Entwicklung/userland
LIB=-lbcm_host -lEGL -lGLESv2 -lusb-1.0 -lm -L/home/felix/Entwicklung/userland/build/lib
EXEC = PiOpenGL
SRC = main.c renderer.c shader.c renderobject.c matrix.c

all: $(EXEC)

$(EXEC): $(SRC)
	gcc -Wall -o $@ $+ $(INC) $(LIB)

clean:
	rm $(EXEC)
