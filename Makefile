COMPILER = g++
DEPEND = -MMD -MP
DELETE = del


main.exe: main.o tgaimage.o
	$(COMPILER) -o main.exe main.o tgaimage.o

main.o: main.cpp
	$(COMPILER) $(DEPEND) -c main.cpp

tgaimage.o: tgaimage.cpp
	$(COMPILER) $(DEPEND) -c tgaimage.cpp

.PHONY: clean

clean:
	$(DELETE) /Q /F *.exe *.o *.d

-include $(wildcard *.d)