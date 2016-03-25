all: as5.c hydrogen.c as5.h
	gcc -Wno-cpp -std=c99 -o as5 as5.c
	gcc -Wno-cpp -std=c99 -o hydrogen hydrogen.c
	gcc -Wno-cpp -std=c99 -o carbon carbon.c
	chmod 755 as5

clean:
	-rm carbon hydrogen as5