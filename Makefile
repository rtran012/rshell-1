all:
	g++ src/main.cpp -Werror -Wall -ansi -pedantic -o rshell
	mkdir bin
	mv rshell bin/
	touch status.txt
	mv status.txt bin/
	
clean:
	rm -rf bin
	rm status.txt
