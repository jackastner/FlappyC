OBJECT_FILES= gamelogic_c.o controllerlogic.o drawlogic.o gamelogic.o main.o
C2HS_TRASH=gamelogic.hs gamelogic.chi gamelogic.chs.h
HS_TRASH=gamelogic.hi gamelogic_stub.h


EXECUTABLE=FlappyBird
LINKS=-lSDL2_ttf -lSDL2_image -lSDL2
PACKAGES=-package transformers -package random 

$(EXECUTABLE): $(OBJECT_FILES)
	ghc -no-hs-main -o $(EXECUTABLE) $(OBJECT_FILES) $(PACKAGES) $(LINKS) 


controllerlogic.o: controllerlogic.c controllerlogic.h gamelogic.h
	gcc -c controllerlogic.c

drawlogic.o: drawlogic.c drawlogic.h gamelogic.h
	gcc -c drawlogic.c
	
gamelogic_c.o: gamelogic.c gamelogic.h 
	gcc -c -o gamelogic_c.o gamelogic.c

main.o: main.c drawlogic.h controllerlogic.h gamelogic.h
	gcc -c main.c -I`ghc --print-libdir`/include/


gamelogic.hs: gamelogic.chs gamelogic.h
	c2hs gamelogic.chs

gamelogic.o: gamelogic.hs
	ghc -c gamelogic.hs


clean:
	rm -f $(OBJECT_FILES) $(C2HS_TRASH) $(HS_TRASH) $(EXECUTABLE)
