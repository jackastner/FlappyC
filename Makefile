GCC_OPTIONS=-Wall -O
OBJECT_FILES=main.o drawlogic.o controllerlogic.o gamelogic.o
LINKS= -lSDL2_image-2.0 -lSDL2 -lSDL2_ttf
EXECUTABLE=FlappyBird

HS_BUILD_FILES=gamelogic.hi gamelogic_stub.h

C2HS_BUILD_FILES=gamelogic.chi gamelogic.chs.h gamelogic.hs

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECT_FILES)
	ghc --make -no-hs-main $(GCC_OPTIONS) -o $(EXECUTABLE) $(OBJECT_FILES) $(LINKS)

main.o: main.c gamelogic.h	
	ghc -c $(GCC_OPTIONS) -o main.o main.c

drawlogic.o: drawlogic.c drawlogic.h gamelogic.h
	ghc -c $(GCC_OPTIONS) -o drawlogic.o drawlogic.c

controllerlogic.o: controllerlogic.c controllerlogic.h gamelogic.h
	ghc -c $(GCC_OPTIONS) -o controllerlogic.o controllerlogic.c

gamelogic.h:gamestructs.h gamelogic_stub.h

gamelogic.o gamelogic_stub.h: gamelogic.hs
	ghc -c --make gamelogic.hs

gamelogic.hs: gamelogic.chs gamestructs.h
	c2hs gamelogic.chs

test-haskell: gamelogic.hs
	ghci -fobject-code gamelogic.hs
clean:
	rm $(OBJECT_FILES) $(C2HS_BUILD_FILES) $(HS_BUILD_FILES)  $(EXECUTABLE)
