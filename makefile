#Establish the makefile variables
CC = gcc-10
CFLAGS = -Wall -g -fsanitize=leak -fsanitize=address

#creates the macD executable
macD: macD.c
	$(CC) $(CFLAGS) $^ -o $@
	chmod -cf 777 ./$@

#removes all executable files
clean:
	rm macD
