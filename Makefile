.PHONY: cmake compile run

cmake:
	cmake -B build

compile:
	make -C build

run:
	./build/minetest
