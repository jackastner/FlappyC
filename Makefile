LINKS=-lSDL2_image-2.0 -lSDL2 -lSDL2_ttf
EXECUTABLE=Flappy
HSC=ghc
HSC_OPTIONS=--make -O

$(EXECUTABLE): main.o drawlogic.o controllerlogic.o gamelogic.o
	$(HSC) $(HSC_OPTIONS) -no-hs-main main.o drawlogic.o controllerlogic.o gamelogic -o $(EXECUTABLE) $(LINKS)

main.o: main.c  gamelogic.h drawlogic.h controllerlogic.h
	$(HSC) -c $(HSC_OPTIONS) main.c

drawlogic.o: drawlogic.c drawlogic.h gamelogic.h
	$(HSC) -c $(HSC_OPTIONS) drawlogic.c

controllerlogic.o: controllerlogic.c controllerlogic.h gamelogic.h
	$(HSC) -c $(HSC_OPTIONS) controllerlogic.c

gamelogic.o gamelogic_stub.h: gamelogic.hs
	$(HSC) -c $(HSC_OPTIONS) gamelogic.hs

gamelogic.h: gamestructs.h gamelogic_stub.h

gamelogic.hs: gamelogic.chs gamestructs.h
	c2hs gamelogic.chs
	
clean:
	rm Flappy gamelogic.chi gamelogic.chs.h gamelogic.hi gamelogic.o gamelogic_stub.h gamelogic.hs main.o controllerlogic.o drawlogic.o
