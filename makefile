all:
	g++ main.cpp DataVector.cpp TreeIndex.cpp -o my

run:
	./my

clean:
	rm -rf my
