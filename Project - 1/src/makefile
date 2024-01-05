CC = gcc
CFLAGS = -Wall
LDFLAGS = -lcjson  # Link flags if using cjson library


recipes.o: recipes.c recipes.h
	$(CC) -c recipes.c $(LDFLAGS)

restaurant.o: restaurant.c recipes.h 
	$(CC) -c restaurant.c 

customer.o: customer.c 
	$(CC) -c customer.c 


c.out: customer.o recipes.o recipes.h
	$(CC) -o c.out customer.o recipes.o $(LDFLAGS)

s.out: supplier.c 
	$(CC) supplier.c -o s.out 

r.out: restaurant.o recipes.o recipes.h 
	$(CC) -o r.out restaurant.o recipes.o $(LDFLAGS)


.PHONY: clean
clean:
	rm -f r.out restaurant.o recipes.o s.out c.out


