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
readme_update: output/example.md output/example-avr.md
	# Library Version (From clib package metadata)
	jq -r '.version' clib.json | xargs -I{} sed -i 's|<version>.*</version>|<version>{}</version>|' README.md
	jq -r '.version' clib.json | xargs -I{} sed -i 's|<versionBadge>.*</versionBadge>|<versionBadge>![Version {}](https://img.shields.io/badge/version-{}-blue.svg)</versionBadge>|' README.md

	# https://stackoverflow.com/questions/5227295/how-do-i-delete-all-lines-in-a-file-starting-from-after-a-matching-line
	sed -i '/<!--- GENERATED CONTENT BELOW --->/q' README.md
	cat output/example.md >> README.md
	cat output/example-avr.md >> README.md

.PHONY: format
format:
	# pip install clang-format
	clang-format -i *.c
	clang-format -i *.h

example: example.c linmap.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o example example.c

output/example.md: example
	./example | tee ./output/example.md

example-avr.elf: example-avr.c linmap.h
	avr-gcc -mmcu=atmega328p -Os -std=c99 -DF_CPU=16000000UL -Wl,-u,vfprintf -lprintf_flt -lm -o example-avr.elf example-avr.c

output/example-avr.md: example-avr.elf
	simavr -m atmega328p -f 16000000 --no-color ./example-avr.elf 2>&1 >/dev/null | sed -r "s/\x1B\[[0-9;]*[mK]//g" | sed -r "s/\.\.\$$//" | tee ./output/example-avr.md

.PHONY:
%.o: %.c
	$(CC) $(DEP_FLAG) $(CFLAGS) $(LDFLAGS) -o $@ -c $<

.PHONY:
clean:
	$(RM) test
	$(RM) example
