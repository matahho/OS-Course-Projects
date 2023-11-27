CC = g++ -std=c++17

all: main.out office.out building.out bills.out

building.out: building.cpp
	$(CC) building.cpp utils.o -o building.out

bills.o: bills.cpp csvReader.o
	$(CC) -c bills.cpp -o bills.o

csvReader.o: csvReader.cpp csvReader.h 
	$(CC) -c csvReader.cpp -o csvReader.o

bills.out: bills.o csvReader.o
	$(CC) bills.o csvReader.o -o bills.out

main.out: main.cpp bills.out building.out utils.o
	$(CC) main.cpp utils.o -o main.out

utils.o: utils.cpp utils.h
	$(CC) -c utils.cpp -o utils.o

office.out: office.cpp utils.o officeCsvReader.o
	$(CC) office.cpp utils.o officeCsvReader.o -o office.out

officeCsvReader.o: officeCsvReader.cpp officeCsvReader.h
	$(CC) -c officeCsvReader.cpp -o officeCsvReader.o

.PHONY: clean
clean:
	rm -f building.out bills.out csvReader.o bills.o main.out utils.o office.out officeCsvReader.o
