FLAGS=-g -Wall -Werror -pedantic -std=c++14
DEST=bin
PROJ=GreenDeal

all: ims

ims: ims.o ./src/ArgParser/ArgParser.o ./src/GreenDealSimulation/GreenDealSimulation.o
	g++ $(FLAGS) -o ${DEST}/${PROJ} $^

run: clean ims
	./${DEST}/${PROJ}

clean:
	rm -rf ims *.o
	rm -rf ./src/ArgParser/*.o
	rm -rf ./src/GreenDealSimulation/*.o
