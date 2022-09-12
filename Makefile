DIR := ${CURDIR}

exec = bin/tcc-test.out
CC = g++
sources = $(wildcard src/*.cpp)
objects = $(sources:.cpp=.o)
CFLAGS = -Wall -g
# CFLAGS = -Wall -g
flags = -Wall -D__LITTLE_ENDIAN__ -D__LINUX_ALSA__ -I/usr/include/stk

$(exec): $(objects)
	g++ $(objects) $(flags) -o $(exec) -lstk

%.o: %.cpp include/%.h
	g++ -c $(flags) $< -o $@ -lstk

clean:
	-rm *.out
	-rm *.o
	-rm *.a
	-rm src/*.o

run: $(exec)
	./$(exec)

# build:
# 	g++ -c $(flags) -o $(exec) stk-test.cpp -lstk

#  g++ -Wall -D__LITTLE_ENDIAN__ -D__LINUX_ALSA__ -I/usr/include/stk -o tcc-test.out threebes.cpp -lstk