all:
	g++ src/main.cpp -Werror -Wall -ansi -pedantic -o rshell
	mkdir bin
	mv rshell bin/
	
clean:
	rm -rf bin
