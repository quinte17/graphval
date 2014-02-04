all: bin/graphval plugins-all

test: bin/graphval plugins-all
	LD_LIBRARY_PATH="plugins/treeprint/" valgrind --tool=massif bin/graphval tp tree.dot
	ms_print massif.out.*

install: plugins-install man-install
	cp bin/graphval /usr/bin/graphval
	chmod 755 /usr/bin/graphval

uninstall: plugins-uninstall man-uninstall
	([ -e /usr/bin/graphval ] && rm -i /usr/bin/graphval) || echo "graphval ist nicht Installiert"

bin/graphval: bin gram.tab.o lex.yy.o
	gcc gram.tab.o lex.yy.o -ldl -o bin/graphval

lex.yy.c: lex.l
	flex lex.l

lex.yy.o: lex.yy.c
	gcc -c lex.yy.c

gram.tab.h: gram.y
	bison -d gram.y

gram.tab.o: gram.tab.h lex.yy.c
	gcc -c gram.tab.c

bin:
	mkdir -p bin

plugins-all:
	cd plugins && make all

plugins-install:
	cd plugins && make install

plugins-uninstall:
	cd plugins && make uninstall

plugins-clean:
	cd plugins && make clean

man-install:
	cd man && make install

man-uninstall:
	cd man && make uninstall

clean: plugins-clean
	rm -f bin/graphval
	rm -Rf bin/
	rm -f lex.yy.c
	rm -f lex.yy.o
	rm -f gram.tab.c
	rm -f gram.tab.h
	rm -f gram.tab.o
	rm -f massif.out.*

.PHONY: clean test install uninstall plugins-all plugins-clean
