include config.mk

all: $(BIN)

$(BIN): $(OBJ)
$(OBJ): $(CONF)

.o:
	$(CC) $(LDFLAGS) -o $@ $(OBJ)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN) $(OBJ)

install: $(BIN)
	mkdir -p $(PREFIX)/bin $(PREFIX)/share/man/man1
	cp $(BIN) $(PREFIX)/bin
	cp $(MAN) $(PREFIX)/share/man/man1
	chmod 755 $(PREFIX)/bin/$(BIN)
	chmod 644 $(PREFIX)/share/man/man1/$(MAN)

uninstall:
	rm -f $(PREFIX)/bin/$(BIN) $(PREFIX)/share/man/man1/$(MAN)

.PHONY: all clean install uninstall
