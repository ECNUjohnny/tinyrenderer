COMPILER = g++
DEPEND = -MMD -MP
DELETE = del
DELETE_FLAGS = /Q /F

all: main.exe clean

main.exe: main.o tgaimage.o model.o our_gl.o
	$(COMPILER) -o main.exe main.o tgaimage.o model.o our_gl.o

main.o: main.cpp
	$(COMPILER) $(DEPEND) -c main.cpp

tgaimage.o: tgaimage.cpp
	$(COMPILER) $(DEPEND) -c tgaimage.cpp

model.o: model.cpp
	$(COMPILER) $(DEPEND) -c model.cpp

our_gl.o: our_gl.cpp
	$(COMPILER) $(DEPEND) -c our_gl.cpp

.PHONY: clean

clean:
	$(DELETE) $(DELETE_FLAGS) *.o *.d

-include $(wildcard *.d)
