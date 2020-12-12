all: first

first: first.c
	gcc -Werror -Wall -fsanitize=address first.c -o first -lm

clean:
	rm -rf first
