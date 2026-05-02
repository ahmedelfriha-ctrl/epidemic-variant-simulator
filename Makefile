all:
	mkdir -p build
	gcc src/*.c -o build/simulation

run: all
	./build/simulation

clean:
	rm -rf build