math_program: objects/main.o objects/advanced_math.o objects/basic_math.o objects/geometry.o
	g++ objects/*.o -o math_program

objects/main.o: src/main.cpp includes/basic_math.h includes/advanced_math.h includes/geometry.h
	g++ -Iincludes -c src/main.cpp -o objects/main.o

objects/basic_math.o: src/basic_math.cpp includes/basic_math.h
	g++ -Iincludes -c src/basic_math.cpp -o objects/basic_math.o

objects/advanced_math.o: src/advanced_math.cpp includes/advanced_math.h
	g++ -Iincludes -c src/advanced_math.cpp -o objects/advanced_math.o

objects/geometry.o: src/geometry.cpp includes/geometry.h
	g++ -Iincludes -c src/geometry.cpp -o objects/geometry.o