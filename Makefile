GCC_OPTIONS=-Wall -O
OBJECT_FILES=main.o gamelogic.o drawlogic.o controllerlogic.o
EXECUTABLE=FlappyBird
PKG_CONFIG=`pkg-config --libs --cflags sdl2 SDL2_ttf SDL2_image`

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECT_FILES)
	gcc $(GCC_OPTIONS) -o $(EXECUTABLE) $(OBJECT_FILES) $(PKG_CONFIG)

main.o: main.c drawlogic.h gamelogic.h controllerlogic.h
	gcc -c $(GCC_OPTIONS) -o main.o main.c

gamelogic.o: gamelogic.c gamelogic.h 
	gcc -c $(GCC_OPTIONS) -o gamelogic.o gamelogic.c

drawlogic.o: drawlogic.c drawlogic.h gamelogic.h
	gcc -c $(GCC_OPTIONS) -o drawlogic.o drawlogic.c

controllerlogic.o: controllerlogic.c controllerlogic.h gamelogic.h
	gcc -c $(GCC_OPTIONS) -o controllerlogic.o controllerlogic.c

clean:
	rm -f $(OBJECT_FILES) $(EXECUTABLE)
