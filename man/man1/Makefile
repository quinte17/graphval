FILES = graphval.1 gv_dfa.1

install:
	for i in $(FILES); do cp $$i /usr/share/man/man1 && chmod 644 /usr/share/man/man1/$$i; done;

uninstall:
	for i in $(FILES); do rm -i /usr/share/man/man1/$$i || echo $$i nicht gefunden.; done;

.PHONY: install uninstall
