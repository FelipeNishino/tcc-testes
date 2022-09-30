DIR := ${CURDIR}

exec = bin/tcc-test.out
CC = g++
sources = $(wildcard src/*.cpp)
objects = $(sources:.cpp=.o)
CFLAGS = -Wall -g
# CFLAGS = -Wall -g
flags = -Wall -D__LITTLE_ENDIAN__ -D__LINUX_ALSA__ -I/usr/include/stk -std=c++20

$(exec): $(objects)
	g++ $(objects) libs/libcurlpp.a libs/libmidifile.a $(flags) -o $(exec) -lstk

%.o: %.cpp include/%.hpp
	g++ -c $(flags) $< -o $@ -lstk

clean:
	-rm $(exec)
	# -rm *.o
	# -rm *.a
	-rm src/*.o

run: $(exec)
	./$(exec)

example:
	g++ testes/threebes.cpp $(flags) -o bin/threebes.out -lstk

swift:
	swift src/swift/main.swift

# build:
# 	g++ -c $(flags) -o $(exec) stk-test.cpp -lstk

#  g++ -Wall -D__LITTLE_ENDIAN__ -D__LINUX_ALSA__ -I/usr/include/stk -o tcc-test.out threebes.cpp -lstk
