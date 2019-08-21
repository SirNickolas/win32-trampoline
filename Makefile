.PHONY: all clean

COMMAND := \"gcc\"

CC := gcc
CFLAGS := \
    -std=c89 \
    '-DCOMMAND=L"$(COMMAND)"' -DUNICODE -D_UNICODE \
    -O2 -s \
    -Wall -Wextra -pedantic -Wformat=2 -Wfloat-equal -Wlogical-op \
    -Wredundant-decls -Wconversion -Wcast-qual -Wcast-align \
    -Wno-format-nonliteral

all: a.exe

clean:
	$(RM) a.exe

a.exe: main.c
	$(CC) $(CFLAGS) -o$@ $^
