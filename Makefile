SRC_DIR=src
GCC_OPTIONS=-Wall -O
OBJECT_FILES=main.o gamelogic.o drawlogic.o controllerlogic.o
EXECUTABLE=FlappyBird
PKG_CONFIG=`pkg-config --libs --cflags sdl2 SDL2_ttf SDL2_image`

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECT_FILES)
	gcc $(GCC_OPTIONS) -o $(EXECUTABLE) $(OBJECT_FILES) $(PKG_CONFIG)

main.o: $(SRC_DIR)/main.c $(SRC_DIR)/drawlogic.h $(SRC_DIR)/gamelogic.h $(SRC_DIR)/controllerlogic.h
	gcc -c $(GCC_OPTIONS) -o main.o $(SRC_DIR)/main.c

gamelogic.o: $(SRC_DIR)/gamelogic.c $(SRC_DIR)/gamelogic.h 
	gcc -c $(GCC_OPTIONS) -o gamelogic.o $(SRC_DIR)/gamelogic.c

drawlogic.o: $(SRC_DIR)/drawlogic.c $(SRC_DIR)/drawlogic.h $(SRC_DIR)/gamelogic.h
	gcc -c $(GCC_OPTIONS) -o drawlogic.o $(SRC_DIR)/drawlogic.c

controllerlogic.o: $(SRC_DIR)/controllerlogic.c $(SRC_DIR)/controllerlogic.h $(SRC_DIR)/gamelogic.h
	gcc -c $(GCC_OPTIONS) -o controllerlogic.o $(SRC_DIR)/controllerlogic.c

clean:
	rm -f $(OBJECT_FILES) $(EXECUTABLE)
