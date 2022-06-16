FLAGS=-g -Wall -Werror -pedantic -std=c++11
FILE=ipk-l4-scan

all:
	g++ $(FLAGS) $(FILE).cpp -o $(FILE) -lpcap -pthread
clean:
	rm -rf *.o $(FILE)
