CC = g++ -std=c++17


building.out: bills.out building.cpp
	$(CC) building.cpp -o building.out


bills.o: csvReader.o bills.cpp
	$(CC) -c bills.cpp -o bills.o

csvReader.o: csvReader.cpp csvReader.h 
	$(CC) -c csvReader.cpp

bills.out: bills.o csvReader.o
	$(CC) bills.o csvReader.o -o bills.out

main.out: main.cpp bills.out building.out
	$(CC) main.cpp -o main.out

.PHONY: clean
clean:
	rm -f bills.out csvReader.o bills.o building.out main.out

