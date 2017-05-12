PREFIX=/usr/local

CC=cc

CFLAGS=-D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE -Wall -Wextra -std=c89 -pedantic -O2
LDFLAGS=-s

BIN=treat
OBJ=$(BIN:=.o)
MAN=$(BIN:=.1)
CONF=config.mk config.h
