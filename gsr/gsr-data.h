
#ifndef GSR_DATA_H
#define GSR_DATA_H 1

#include <glib.h>
#include <gsr/gsr-gint64.h>

#define GSR_DATA_TO_GBOOLEAN(Data)                                      \
    GPOINTER_TO_INT (Data)

#define GSR_DATA_TO_GINT32(Data)                                        \
    GPOINTER_TO_INT (Data)

#define GSR_DATA_TO_GINT64(Data)                                        \
    GPOINTER_TO_INT64 (Data)

#define GSR_DATA_FROM_GBOOLEAN(Value)                                   \
    GINT_TO_POINTER(Value)

#define GSR_DATA_FROM_GINT32(Value)                                     \
    GINT_TO_POINTER(Value)

#define GSR_DATA_FROM_GINT64(Value)                                     \
    GINT64_TO_POINTER(Value)

#endif /* header guard */

