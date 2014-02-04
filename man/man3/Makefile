FILES = libmarkov.3

install:
	for i in $(FILES); do cp $$i /usr/share/man/man3 && chmod 644 /usr/share/man/man3/$$i; done;

uninstall:
	for i in $(FILES); do rm -i /usr/share/man/man3/$$i || echo $$i nicht gefunden.; done;

.PHONY: install uninstall
