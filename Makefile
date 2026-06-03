CC      ?= cc
RM      ?= rm -f
INSTALL ?= install

PREFIX  ?= /usr/local

CFLAGS += -Wall -std=c99 -pedantic

.PHONY: all check test install format readme_update clean help

all: example

# ---- Desktop build & output ----------------------------------------------

example: example.c adc_linmap.h linmap.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o example example.c

output/:
	mkdir output

output/example.md: example output/
	./example | tee ./output/example.md

# ---- AVR build & simulation ----------------------------------------------

example-avr.elf: example-avr.c adc_linmap.h linmap.h simavr_cmd.h
	avr-gcc -mmcu=atmega328p -Os -std=c99 -DF_CPU=16000000UL \
	  -Wl,-u,vfprintf -lprintf_flt -lm -o example-avr.elf example-avr.c

output/example-avr.md: example-avr.elf output/
	@bash -c 'simavr -m atmega328p -f 16000000 --no-color ./example-avr.elf 2>&1 >/dev/null \
	  | sed -r "s/\x1B\[[0-9;]*[mK]//g" | sed -r "s/\.\.$$//" | tee ./output/example-avr.md; \
	  exit $${PIPESTATUS[0]}'

# Run AVR unit tests and print PASS/FAIL summary; simavr exit code signals result
test: example-avr.elf
	@echo "Running AVR unit tests under simavr..."
	@bash -c 'simavr -m atmega328p -f 16000000 --no-color ./example-avr.elf 2>&1 >/dev/null \
	  | sed -r "s/\x1B\[[0-9;]*[mK]//g" | sed -r "s/\.\.$$//" \
	  | grep -E "^(PASS|FAIL|##)"; \
	  exit $${PIPESTATUS[0]}'

# Build everything and run tests (use this before pushing)
check: example output/example.md example-avr.elf output/example-avr.md test

# ---- Installation --------------------------------------------------------

install: linmap.h adc_linmap.h
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/include
	$(INSTALL) -m 644 linmap.h adc_linmap.h $(DESTDIR)$(PREFIX)/include

# ---- Maintenance ---------------------------------------------------------

# Regenerate the generated section of README.md from current build output
readme_update: output/example.md output/example-avr.md
	jq -r '.version' clib.json \
	  | xargs -I{} sed -i \
	    's|<versionBadge>.*</versionBadge>|<versionBadge>![Version {}](https://img.shields.io/badge/version-{}-blue.svg)</versionBadge>|' \
	    README.md
	sed -i '/<!--- GENERATED CONTENT BELOW --->/q' README.md
	cat output/example.md >> README.md
	cat output/example-avr.md >> README.md

# Auto-format all C sources and headers (requires clang-format)
format:
	clang-format -i *.c *.h

clean:
	$(RM) example example-avr.elf
	$(RM) -r output/

help:
	@echo "Targets:"
	@echo "  all            Build desktop example (default)"
	@echo "  check          Full build + AVR unit tests"
	@echo "  test           Run AVR unit tests under simavr"
	@echo "  install        Install headers to PREFIX (default: /usr/local)"
	@echo "  readme_update  Regenerate README from current output"
	@echo "  format         clang-format all .c/.h files"
	@echo "  clean          Remove build artifacts and output/"
	@echo ""
	@echo "Variables:"
	@echo "  PREFIX=$(PREFIX)  CC=$(CC)"
