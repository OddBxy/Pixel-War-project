serveur: main.o fonction.o parsing.o
	gcc -Wall main.o fonction.o parsing.o -o serveur
	
main.o: main.c header.h
	gcc -Wall -c main.c -o main.o
	
fonction.o: fonction.c header.h
	gcc -Wall -c fonction.c -o fonction.o

parsing.o: parsing.c header.h
	gcc -Wall -c parsing.c -o parsing.o
