/*
 * simavr_cmd.h
 * Minimal firmware-side interface for signalling simavr exit codes.
 *
 * Compatible ABI with simavr's avr_mcu_section.h command register mechanism.
 * See: https://github.com/buserror/simavr/blob/master/simavr/sim/avr/avr_mcu_section.h
 *
 * Usage (file scope):
 *   SIMAVR_DECLARE_CMD_REG(&GPIOR0);
 *
 * Usage (in main, before sleep_cpu):
 *   GPIOR0 = failures ? SIMAVR_CMD_EXIT_CODE_1 : SIMAVR_CMD_EXIT_CODE_0;
 */

#ifndef SIMAVR_CMD_H
#define SIMAVR_CMD_H

#include <stdint.h>

/* Tags data into the .mmcu ELF section where simavr reads firmware metadata */
#define _MMCU_ __attribute__((section(".mmcu"))) __attribute__((used))

/* Anchor that prevents the linker discarding the .mmcu section entirely */
static const uint8_t _MMCU_ _mmcu_anchor[] = { 0, 0 };

/* Command IDs -- must match simavr's SIMAVR_CMD_* enum (values 4 and 5) */
#define SIMAVR_CMD_EXIT_CODE_0  4
#define SIMAVR_CMD_EXIT_CODE_1  5

/* Tag ID for the command register declaration (AVR_MMCU_TAG_SIMAVR_COMMAND = 10) */
#define SIMAVR_TAG_COMMAND  10

/* Struct layout must match simavr's avr_mmcu_addr_t exactly */
struct _simavr_mmcu_addr_t {
    uint8_t tag;
    uint8_t len;
    void   *what;
} __attribute__((__packed__));

/*
 * Declare `_reg` as the simavr command bridge register.
 * Place once at file scope.  A favourite is GPIOR0 (unused general-purpose IO).
 */
#define SIMAVR_DECLARE_CMD_REG(_reg) \
    static const struct _simavr_mmcu_addr_t _MMCU_ _simavr_cmd_reg = { \
        .tag  = SIMAVR_TAG_COMMAND,   \
        .len  = sizeof(void *),       \
        .what = (void *)(_reg),       \
    }

#endif /* SIMAVR_CMD_H */
