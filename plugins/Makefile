DIRS = treeprint markov dfa

all:
	for i in $(DIRS); do make -C $$i all; done;

install:
	for i in $(DIRS); do make -C $$i install; done;

uninstall:
	for i in $(DIRS); do make -C $$i uninstall; done;

clean:
	for i in $(DIRS); do make -C $$i clean; done;

.PHONY: all install uninstall clean
