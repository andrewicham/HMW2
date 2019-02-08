all: simple-shell

simple-shell: simple-shell.c
	gcc simple-shell.c -o simple-shell

clean:
	rm -rf simple-shell

run: 
	./simple-shell

