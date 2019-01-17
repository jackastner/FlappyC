SRC_DIR=src
GCC_OPTIONS=-Wall -O -g
OBJECT_FILES=main.o gamelogic.o drawlogic.o controllerlogic.o
EXECUTABLE=FlappyBird
PKG_CONFIG=`pkg-config --libs --cflags sdl2 SDL2_ttf SDL2_image`

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECT_FILES)
	g++ $(GCC_OPTIONS) -o $(EXECUTABLE) $(OBJECT_FILES) $(PKG_CONFIG)

main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/drawlogic.h $(SRC_DIR)/gamelogic.h $(SRC_DIR)/controllerlogic.h
	g++ -c $(GCC_OPTIONS) -o main.o $(SRC_DIR)/main.cpp

gamelogic.o: $(SRC_DIR)/gamelogic.cpp $(SRC_DIR)/gamelogic.h 
	g++ -c $(GCC_OPTIONS) -o gamelogic.o $(SRC_DIR)/gamelogic.cpp

drawlogic.o: $(SRC_DIR)/drawlogic.cpp $(SRC_DIR)/drawlogic.h $(SRC_DIR)/gamelogic.h
	g++ -c $(GCC_OPTIONS) -o drawlogic.o $(SRC_DIR)/drawlogic.cpp

controllerlogic.o: $(SRC_DIR)/controllerlogic.cpp $(SRC_DIR)/controllerlogic.h $(SRC_DIR)/gamelogic.h $(SRC_DIR)/drawlogic.h
	g++ -c $(GCC_OPTIONS) -o controllerlogic.o $(SRC_DIR)/controllerlogic.cpp

clean:
	rm -f $(OBJECT_FILES) $(EXECUTABLE)
