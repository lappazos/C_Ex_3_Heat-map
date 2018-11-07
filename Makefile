CC=gcc
CFLAGS = -Wextra -Wall -Wvla -std=c99

# Executables

make: reader.c calculator.c heat_eqn.c
	$(CC) $(CFLAGS) -o ex3 reader.c calculator.c heat_eqn.c

all: make
	./ex3 input.txt

# Object Files

calculator: calculator.c calculator.h
	$(CC) $(CFLAGS) -c calculator.c

heat_eqn: heat_eqn.c heat_eqn.h
	$(CC) $(CFLAGS) -c heat_eqn.c

# Other Targets

clean:
	rm -f *.o ex3