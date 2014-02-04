all: gv_tp.so

treeprint.o: treeprint.h treeprint.c ../../graphval.h
	gcc -I../../ -fPIC -Wall -g -c treeprint.c

gv_tp.so: treeprint.o
	gcc -g -shared -Wl,-soname,gv_tp.so.0 -o gv_tp.so treeprint.o -lc

install:
	cp gv_tp.so /usr/lib/gv_tp.so
	chmod 755 /usr/lib/gv_tp.so

uninstall:
	([ -e /usr/lib/gv_tp.so ] && rm -i /usr/lib/gv_tp.so) || echo "graphval: gv_tp.so ist nicht Installiert."

clean:
	rm -f treeprint.o
	rm -f gv_tp.so

.PHONY: clean install uninstall
