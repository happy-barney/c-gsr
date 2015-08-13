
#include "config.h"
#include <gsr/gsr.h>

void gsr_init (void) {
    gsr_type_init ();
    gsr_symbol_init ();
}

void gsr_cleanup (void) {
    gsr_symbol_cleanup ();
    gsr_type_cleanup ();
}
