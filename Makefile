DIR := ${CURDIR}

exec = bin/tcc-test.out
CC = g++
sources = $(wildcard src/*.cpp)
objects = $(sources:.cpp=.o)
CFLAGS = -Wall -g
STATIC_LIBS = libs/libjsoncpp.a libs/libcurlpp.a libs/libmidifile.a
# CFLAGS = -Wall -g
flags = -Wall -D__LITTLE_ENDIAN__ -D__LINUX_ALSA__ -I/usr/include/stk -std=c++20

$(exec): $(objects)
	g++ $(objects) $(STATIC_LIBS) $(flags) -o $(exec) -lstk -Llib -lcurl

%.o: %.cpp include/%.hpp
	g++ -c $(flags) $< -o $@ -lstk -Llib -lcurl

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

request: $(exec)
	./$(exec) -f --no-play
# build:
# 	g++ -c $(flags) -o $(exec) stk-test.cpp -lstk

#  g++ -Wall -D__LITTLE_ENDIAN__ -D__LINUX_ALSA__ -I/usr/include/stk -o tcc-test.out threebes.cpp -lstk
