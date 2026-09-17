COMPILER = g++
DEPEND = -MMD -MP
DELETE = del

all: main.exe clean

main.exe: main.o tgaimage.o model.o
	$(COMPILER) -o main.exe main.o tgaimage.o model.o

main.o: main.cpp
	$(COMPILER) $(DEPEND) -c main.cpp

tgaimage.o: tgaimage.cpp
	$(COMPILER) $(DEPEND) -c tgaimage.cpp

model.o: model.cpp
	$(COMPILER) $(DEPEND) -c model.cpp

.PHONY: clean

clean:
	$(DELETE) /Q /F *.o *.d

-include $(wildcard *.d)