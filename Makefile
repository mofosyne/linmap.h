CC      ?= cc
CP      ?= cp -f
RM      ?= rm -f
MKDIR   ?= mkdir -p
INSTALL ?= install

PREFIX  ?= /usr/local

CFLAGS += -Wall -std=c99 -pedantic
#CFLAGS += -Wall -std=c11 -pedantic

.PHONY:
all: example

.PHONY: readme_update
readme_update: example
	# Library Version (From clib package metadata)
	jq -r '.version' clib.json | xargs -I{} sed -i 's|<version>.*</version>|<version>{}</version>|' README.md
	jq -r '.version' clib.json | xargs -I{} sed -i 's|<versionBadge>.*</versionBadge>|<versionBadge>![Version {}](https://img.shields.io/badge/version-{}-blue.svg)</versionBadge>|' README.md

	./example > linmap-adc-example-output.md

.PHONY: format
format:
	# pip install clang-format
	clang-format -i *.c
	clang-format -i *.h

.PHONY:
test: test.c linmap.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o test test.c
	./test

.PHONY:
example: example.c linmap.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o example example.c
	./example

.PHONY:
%.o: %.c
	$(CC) $(DEP_FLAG) $(CFLAGS) $(LDFLAGS) -o $@ -c $<

.PHONY:
clean:
	rm -f test
	rm -f example
