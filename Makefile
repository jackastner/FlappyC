GCC_OPTIONS=-Wpedantic -O
OBJECT_FILES=main.o gamelogic.o drawlogic.o controllerlogic.o
LINKS= -lSDL2_image-2.0 -lSDL2 -lSDL2_ttf
EXECUTABLE=FlappyBird

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECT_FILES)
	gcc $(GCC_OPTIONS) -o $(EXECUTABLE) $(OBJECT_FILES) $(LINKS)

main.o: main.c
	gcc -c $(GCC_OPTIONS) -o main.o main.c

gamelogic.o: gamelogic.c gamelogic.h
	gcc -c $(GCC_OPTIONS) -o gamelogic.o gamelogic.c

drawlogic.o: drawlogic.c drawlogic.h
	gcc -c $(GCC_OPTIONS) -o drawlogic.o drawlogic.c

controllerlogic.o: controllerlogic.c controllerlogic.h
	gcc -c $(GCC_OPTIONS) -o controllerlogic.o controllerlogic.c

clean:
	rm $(OBJECT_FILES) $(EXECUTABLE)