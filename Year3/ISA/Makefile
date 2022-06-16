
FLAGS=-g -Wall -Werror -pedantic -std=c++11
FILE=popcl

all:
	g++ $(FLAGS) $(FILE).cpp -o $(FILE) -lssl -pthread -lcrypto
clean:
	rm -rf *.o $(FILE)
