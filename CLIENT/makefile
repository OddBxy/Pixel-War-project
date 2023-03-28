client: main.o color_convert.o 
	gcc -Wall main.o color_convert.o -lm -o client
	
main.o: Client.c header.h
	gcc -Wall -c Client.c -o main.o
	
color_convert.o: color_convert.c header.h
	gcc -Wall -c color_convert.c -lm -o color_convert.o
