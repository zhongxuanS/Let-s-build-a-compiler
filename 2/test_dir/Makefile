IN=generator.c ../cradle.c
OUT=generator
EXE=test
FLAGS=-Wall -Werror

generator: generator.c ../cradle.c
	gcc -o $(OUT) $(IN) $(FLAGS)

build: generator
	./$(OUT) > $(EXE).s

run: build
	as --32 -o $(EXE).o $(EXE).s
	ld -m elf_i386 $(EXE).o -o $(EXE)
	./$(EXE)

.PHONY: clean build run
clean:
	rm -f $(OUT)
	rm -f $(EXE)*
