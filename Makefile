INC=-I/home/felix/Entwicklung/userland
LIB=-lbcm_host -lEGL -lGLESv2 -lusb-1.0 -L/home/felix/Entwicklung/userland/build/lib
EXEC = PiOpenGL

all: $(EXEC)

$(EXEC): main.c renderer.c shader.c renderobject.c
	gcc -Wall -o $@ $+ $(INC) $(LIB)
	
clean:
	rm $(EXEC)
