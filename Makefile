all: color_rotate_zrle

%:%.c
	gcc -lz -O3 $^ -o $@
