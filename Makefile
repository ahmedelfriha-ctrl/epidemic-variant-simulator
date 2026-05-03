Question1:
	mkdir -p build
	gcc  src/Question_1.c src/Simulation.c src/Model.c -o build/simulation
	./build/simulation

Question2:
	mkdir -p build
	gcc  src/Question_2.c src/Simulation.c src/Model.c -o build/simulation
	./build/simulation

Question3:
	mkdir -p build
	gcc  src/Question_3.c src/Simulation.c src/Model.c -o build/simulation
	./build/simulation

Question4:
	mkdir -p build
	gcc  src/Question_4.c src/Simulation.c src/Model.c -o build/simulation
	./build/simulation

clean:
	rm -rf build