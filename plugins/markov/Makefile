all: gv_markov.so libmarkov.a

pmarkov.o: pmarkov.h pmarkov.c ../../graphval.h
	gcc -I../../ -fPIC -Wall -g -c pmarkov.c

gv_markov.so: pmarkov.o
	gcc -g -shared -Wl,-soname,gv_markov.so.0 -o gv_markov.so pmarkov.o -lc

libmarkov.a: markov.o
	ar qf libmarkov.a markov.o

markov.o: markov.h markov.c
	gcc -c markov.c

install:
	cp gv_markov.so /usr/lib/gv_markov.so
	chmod 644 /usr/lib/gv_markov.so
	cp libmarkov.a /usr/lib/libmarkov.a
	chmod 644 /usr/lib/libmarkov.a
	cp markov.h /usr/include/markov.h
	chmod 644 /usr/include/markov.h

uninstall:
	([ -e /usr/lib/gv_markov.so ] && rm -i /usr/lib/gv_markov.so) || echo "graphval: gv_markov.so ist nicht Installiert."
	([ -e /usr/lib/libmarkov.a ] && rm -i /usr/lib/libmarkov.a) || echo "graphval: libmarkov.a ist nicht Installiert."
	([ -e /usr/include/markov.h ] && rm -i /usr/include/markov.h) || echo "graphval: markov.h ist nicht Installiert."

clean:
	rm -f pmarkov.o
	rm -f markov.o
	rm -f libmarkov.a
	rm -f gv_markov.so

.PHONY: clean install uninstall
