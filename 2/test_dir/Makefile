IN=generator.c ../cradle.c
OUT=generator
EXE=test
FLAGS=-Wall -Werror

all:
	gcc -o $(OUT) $(IN) $(FLAGS)

build:
	./$(OUT) > $(EXE).s

run: build
	as --32 -o $(EXE).o $(EXE).s
	ld -m elf_i386 $(EXE).o -o $(EXE)
	./$(EXE)

.PHONY: clean
clean:
	rm -f $(OUT)
	rm -f $(EXE)*
