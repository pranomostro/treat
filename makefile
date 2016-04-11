include config.mk

all: install

install: $(TARGET)
	mkdir -p $(PREFIX)/bin $(PREFIX)/share/man/man1
	cp $(TARGET) $(PREFIX)/bin
	cp $(TARGET).1 $(PREFIX)/share/man/man1
	chmod 755 $(PREFIX)/bin/$(TARGET)
	chmod 0644 $(PREFIX)/share/man/man1/$(TARGET).1

uninstall:
	rm -f $(PREFIX)/bin/$(TARGET) $(PREFIX)/share/man/man1/$(TARGET).1
