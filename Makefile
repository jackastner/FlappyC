Flappy: main.o drawlogic.o controllerlogic.o 
	ghc --make -no-hs-main main.o drawlogic.o controllerlogic.o gamelogic -o  Flappy -lSDL2_image-2.0 -lSDL2 -lSDL2_ttf

main.o: main.c  gamelogic.h drawlogic.h controllerlogic.h
	ghc -c --make main.c

drawlogic.o: drawlogic.c drawlogic.h gamelogic.h
	ghc -c --make drawlogic.c

controllerlogic.o: controllerlogic.c controllerlogic.h gamelogic.h
	ghc -c --make controllerlogic.c

gamelogic.o gamelogic_stub.h: gamelogic.hs
	ghc -c gamelogic.hs

gamelogic.h: gamestructs.h gamelogic_stub.h

gamelogic.hs: gamelogic.chs gamestructs.h
	c2hs gamelogic.chs
	
clean:
	rm Flappy gamelogic.chi gamelogic.chs.h gamelogic.hi gamelogic.o gamelogic_stub.h gamelogic.hs main.o controllerlogic.o drawlogic.o
