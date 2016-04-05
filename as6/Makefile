all: as6.c hydrogen.c as6.h
	gcc -Wno-cpp -std=c99 -o as6 as6.c
	gcc -Wno-cpp -std=c99 -o hydrogen hydrogen.c
	gcc -Wno-cpp -std=c99 -o carbon carbon.c
	chmod 755 as6

clean:
	-rm carbon hydrogen as6
