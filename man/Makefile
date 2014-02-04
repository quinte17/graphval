DIRS = man1

install:
	for i in $(DIRS); do make -C $$i install; done;

uninstall:
	for i in $(DIRS); do make -C $$i uninstall; done;

.PHONY: install uninstall
