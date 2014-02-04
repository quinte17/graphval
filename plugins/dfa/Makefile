all: gv_dfa.so libdfa.a

pdfa.o: pdfa.h pdfa.c ../../graphval.h FIFO_list.h t_node_matrix.h dot_generator.h c_generator.h
	gcc -I../../ -fPIC -Wall -g -c pdfa.c

FIFO_list.o: FIFO_list.h FIFO_list.c ../../graphval.h
	gcc -I../../ -fPIC -Wall -g -c FIFO_list.c

t_node_matrix.o: t_node_matrix.h t_node_matrix.c ../../graphval.h FIFO_list.h
	gcc -I../../ -fPIC -Wall -g -c t_node_matrix.c

dot_generator.o: dot_generator.h dot_generator.c ../../graphval.h FIFO_list.h 
	gcc -I../../ -fPIC -Wall -g -c dot_generator.c

c_generator.o: c_generator.h c_generator.c ../../graphval.h FIFO_list.h t_node_matrix.h 
	gcc -I../../ -fPIC -Wall -g -c c_generator.c

gv_dfa.so: pdfa.o FIFO_list.o t_node_matrix.o dot_generator.o c_generator.o
	gcc -g -shared -Wl,-soname,gv_dfa.so.0 -o gv_dfa.so pdfa.o FIFO_list.o t_node_matrix.o dot_generator.o c_generator.o  -lc

libdfa.a: dfa.o
	ar qf libdfa.a dfa.o

dfa.o: dfa.h dfa.c
	gcc -c dfa.c

install:
	cp gv_dfa.so /usr/lib/gv_dfa.so
	chmod 644 /usr/lib/gv_dfa.so
	cp libdfa.a /usr/lib/libdfa.a
	chmod 644 /usr/lib/libdfa.a
	cp dfa.h /usr/include/dfa.h
	chmod 644 /usr/include/dfa.h

uninstall:
	([ -e /usr/lib/gv_dfa.so ] && rm -i /usr/lib/gv_dfa.so) || echo "graphval: gv_dfa.so ist nicht installiert."
	([ -e /usr/lib/libdfa.a ] && rm -i /usr/lib/libdfa.a) || echo "graphval: libdfa.a ist nicht installiert."
	([ -e /usr/include/dfa.h ] && rm -i /usr/include/dfa.h) || echo "graphval: dfa.h ist nicht installiert."

clean:
	rm -f pdfa.o
	rm -f gv_dfa.so
	rm -f FIFO_list.o
	rm -f t_node_matrix.o
	rm -f dot_generator.o
	rm -f c_generator.o
	rm -f dfa.o
	rm -f libdfa.a

.PHONY: clean install uninstall
