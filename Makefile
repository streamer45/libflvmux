LIB_NAME=flvmux

CFLAGS+=-I. -Imodules/
LDFLAGS+=

all:
	gcc -Wall -Werror -c -g -fPIC modules/buffer.c -o buffer.o
	gcc -Wall -Werror -c -g -fPIC modules/nal.c -o nal.o
	gcc -Wall -Werror -c -g -fPIC modules/adts.c -o adts.o
	gcc -Wall -Werror -c -g -fPIC lib.c $(CFLAGS) -o $(LIB_NAME).o
	gcc -shared -Wl,-soname,lib$(LIB_NAME).o -lm $(CFLAGS) -o lib$(LIB_NAME).so \
	 $(LIB_NAME).o buffer.o nal.o adts.o
	make -C tests/

test:
	make -C tests/

clean:
	rm -f *.so
	rm -f *.o
	rm -f *.a
	make clean -C tests/
