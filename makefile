build: main.o gengetopt/cmdline.o
	gcc -o dirwalk.out $^
