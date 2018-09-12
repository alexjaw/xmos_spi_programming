CC = gcc
CFLAGS = 
DEPS = spiXMOS.h
OBJ = test_spiXMOS.o spiXMOS.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

test: $(OBJ)
	gcc $(CFLAGS) -o $@ $^

clean: 
	rm $(OBJ)
