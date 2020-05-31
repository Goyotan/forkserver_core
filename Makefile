all: forkserver_core.so

forkserver_core.so: forkserver_core.c
	@gcc -O3 -shared -fPIC $< -o$@

clean:
	rm forkserver_core.so