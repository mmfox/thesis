# Makefile for Thesis Debugging
# Matt Fox - 10/8/14

CC = g++
CFLAGS = -I.
DEPS = liChannel.h baseclass.h multiplier.h timingModel.h memStream.h fifo.h multiChannel.h tia.h
OBJ = liChannel.o
OBJ2 = fifo.o
OBJ3 = multiplier.o
OBJ4 = memStream.o
OBJ5 = convolution.o
OBJ6 = multiChannel.o
OBJ7 = tia.o
OBJ8 = nonlinearity.o
OBJ9 = max.o
OBJ10 = average.o
OBJ11 = shift_ave.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Each of these allows you to build modules by themselves for debugging purposes
# You will have to add a ghost main function in the cpp file, though if you want to run all but timing

channel: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)

fifo: $(OBJ2)
	g++ -o $@ $^ $(CFLAGS)

multiply: $(OBJ3)
	g++ -o $@ $^ $(CFLAGS)

mem: $(OBJ4)
	g++ -o $@ $^ $(CFLAGS)

multi: $(OBJ6)
	g++ -o $@ $^ $(CFLAGS)

tia: $(OBJ7)
	g++ -o $@ $^ $(CFLAGS)

# Main commands that will be run.  This compiles the timing model for a given example and all of the modules together

conv: $(OBJ5)
	g++ -o $@ $^ $(CFLAGS)

nonlin: $(OBJ8)
	g++ -o $@ $^ $(CFLAGS)

max: $(OBJ9)
	g++ -o $@ $^ $(CFLAGS)

average: $(OBJ10)
	g++ -o $@ $^ $(CFLAGS)

shift_ave: $(OBJ11)
	g++ -o $@ $^ $(CFLAGS)
