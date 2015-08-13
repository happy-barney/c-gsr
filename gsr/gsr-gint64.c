
#include <gsr/gsr.h>

#if SIZEOF_GPOINTER < SIZEOF_GINT64

gpointer    gsr_gint64_slice_alloc (
    guint64 value)
{
    return g_slice_copy (sizeof (value), &value);
}

void        gsr_gint64_slice_free  (
    guint64 *ptr)
{
    if (NULL != ptr) g_slice_free1 (sizeof (*ptr), ptr);
}

#endif
