/*
 * openfpgaOS Slot Demo
 * Displays registered file slots with a box-drawn table.
 */

#include "of.h"
#include <stdio.h>

static void draw_hline(int w, int sep, char left, char mid, char right) {
    printf("  %c", left);
    for (int i = 1; i < w - 1; i++)
        printf("%c", (i == sep) ? mid : ACS_HLINE);
    printf("%c\n", right);
}

/* Chip32 slot table in CRAM (0x30000000, written by loader via bridge 0x20000000)
 * Format per entry: [0] magic 0x534C4F54 ("SLOT")
 *                   [4] slot_id
 *                   [8] status (0=missing, other=file size)
 *                   [12] reserved
 * End marker: 0xFFFFFFFF */
#define CRAM_TABLE_BASE  0x30000000
#define SLOT_MAGIC       0x534C4F54

typedef struct {
    uint32_t magic;
    uint32_t slot_id;
    uint32_t status;
    uint32_t reserved;
} chip32_slot_entry_t;

int main(void) {
    of_file_slot_register(1, "os.bin");
    of_file_slot_register(2, "slotdemo.elf");

    printf("\033[2J\033[H");
    printf("\n CRAM Test\n\n");

    /* CPU write test */
    volatile uint32_t *ct = (volatile uint32_t *)0x30000400;
    ct[0] = 0xDEADBEEF;
    ct[1] = 0xCAFEBABE;
    __asm__ volatile("fence" ::: "memory");
    printf(" CPU: %08X %08X %s\n",
           (unsigned)ct[0], (unsigned)ct[1],
           (ct[0] == 0xDEADBEEF) ? "OK" : "FAIL");

    /* Chip32 slot table */
    volatile chip32_slot_entry_t *t = (volatile chip32_slot_entry_t *)CRAM_TABLE_BASE;
    printf(" C32:");
    int n = 0;
    for (int i = 0; i < 20 && t[i].magic != 0xFFFFFFFF; i++) {
        if (i < 4)
            printf(" %08X", (unsigned)t[i].magic);
        n++;
    }
    printf("\n %d entries %s\n",
           n, (n > 0 && t[0].magic == SLOT_MAGIC) ? "OK" : "FAIL");

    while (1)
        of_delay_ms(100);

    return 0;
}
