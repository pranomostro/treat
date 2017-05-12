PREFIX=/usr/local

CC=cc

CFLAGS=-D_POSIX_C_SOURCE=200809L -Wall -Wextra -std=c89 -pedantic -g
LDFLAGS=-g

BIN=treat
OBJ=$(BIN:=.o)
MAN=$(BIN:=.1)
CONF=config.mk config.h
