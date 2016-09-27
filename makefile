include config.mk

all: install

install: $(TARGET)
	mkdir -p $(PREFIX)/bin $(PREFIX)/share/man/man1
	cp $(TARGET) $(PREFIX)/bin
	cp treat.1 $(PREFIX)/share/man/man1
	cd $(PREFIX)/bin && chmod 755 $(TARGET)
	chmod 644 $(PREFIX)/share/man/man1/treat.1

uninstall:
	rm -f $(PREFIX)/share/man/man1/$(TARGET).1
	cd $(PREFIX)/bin && rm -f $(TARGET)

.PHONY: all install uninstall
