include config.mk

all: install

install: $(BIN)
	mkdir -p $(PREFIX)/bin $(PREFIX)/share/man/man1
	cp $(BIN) $(PREFIX)/bin
	cp $(MAN) $(PREFIX)/share/man/man1
	cd $(PREFIX)/bin/ && chmod 755 $(BIN)
	cd $(PREFIX)/share/man/man1 && chmod 644 $(MAN)

uninstall:
	cd $(PREFIX)/bin && rm -f $(BIN)
	cd $(PREFIX)/share/man/man1/ && rm -f $(MAN)

.PHONY: all install uninstall
