all:
	mkdir -p bin
	g++ -fopenmp -o ./bin/tas tas.cpp
	g++ -fopenmp -o ./bin/ttas ttas.cpp
	g++ -fopenmp -o ./bin/anderson anderson.cpp
	g++ -fopenmp -o ./bin/clh clh.cpp
	g++ -fopenmp -o ./bin/mcs mcs.cpp
	./bin/tas
	./bin/ttas
	./bin/anderson
	./bin/clh
	./bin/mcs
clean:
	rm -rf bin

