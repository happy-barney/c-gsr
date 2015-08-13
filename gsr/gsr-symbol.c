
#include "config.h"

#include <gsr/gsr.h>
#include <cmx/cmx.h>
#include <errno.h>

static struct {
    CMX_STRUCT_SHAREABLE_DEFINE;
    GHashTable *map;
} gsr_symbol_global;

struct _GSR_Symbol {
    const char *name;
    GSR_Type   *type;
};

static GSR_Symbol * gsr_symbol_new (
    const char *name,
    GSR_Type   *type)
{
    GSR_Symbol *retval = g_slice_alloc (sizeof (*retval));

    retval->name = name;
    retval->type = type;

    return retval;
}

static void         gsr_symbol_destroy (
    GSR_Symbol *symbol)
{
    if (NULL != symbol)
        g_slice_free1 (sizeof (*symbol), symbol);
}

static gpointer gsr_symbol_global_share (
    void)
{
    CMX_STRUCT_SHAREABLE_SHARE (&( gsr_symbol_global ));
}

void                gsr_symbol_init (
    void)
{
    CMX_RUN_ONCE {
        CMX_STRUCT_SHAREABLE_INIT  (&( gsr_symbol_global ));
        gsr_symbol_global_share ();

        gsr_symbol_global.map = g_hash_table_new_full (
            g_str_hash,
            g_str_equal,
            NULL,
            (GDestroyNotify) gsr_symbol_destroy
        );
    }
}

void                gsr_symbol_cleanup (
    void)
{
    g_hash_table_unref (gsr_symbol_global.map);
}

GSR_Symbol        * gsr_symbol_registry (
    const char *name,
    GSR_Type   *type)
{
    return gsr_symbol_registry_test_and_set (name, type, NULL);
}

GSR_Symbol        * gsr_symbol_registry_test_and_set (
    const char  *name,
    GSR_Type    *type,
    GSR_Symbol **place)
{
    GSR_Symbol *retval = NULL;
    if (NULL != place)
        retval = g_atomic_pointer_get (place);

    if (NULL == name) return NULL;

    gsr_symbol_init ();
    if ((NULL == retval)) {
        CMX_STRUCT_SHAREABLE_SYNCHRONIZE (&( gsr_symbol_global )) {
            retval = g_hash_table_lookup (gsr_symbol_global.map, name);
            if (NULL == retval) {
                retval = gsr_symbol_new (name, type);
                g_hash_table_insert (gsr_symbol_global.map, (gpointer) name, retval);
            }
        }
    }

    if (NULL != place)
        g_atomic_pointer_set (place, retval);

    return retval;
}

GSR_Symbol        * gsr_symbol_lookup (
    const char  *name)
{
    GSR_Symbol *retval = NULL;

    if (NULL == name) return NULL;

    gsr_symbol_init ();
    CMX_STRUCT_SHAREABLE_SYNCHRONIZE (&( gsr_symbol_global )) {
        retval = g_hash_table_lookup (gsr_symbol_global.map, name);
    }

    return retval;
}

const char        * gsr_symbol_name (
    GSR_Symbol *symbol)
{
    if (NULL == symbol) return NULL;
    return symbol->name;
}

GSR_Type          * gsr_symbol_type (
    GSR_Symbol *symbol)
{
    if (NULL == symbol) return NULL;
    return symbol->type;
}

gboolean            gsr_symbol_validate (
    GSR_Symbol *symbol,
    GSR_Value  *value)
{
    if (NULL == value)  return FALSE;
    if (NULL == symbol) return FALSE;
    if (NULL == symbol->type) return TRUE;

    gpointer data = gsr_value_get (value, symbol->type);
    if ((NULL == data) && (errno == ENOENT))
        return FALSE;

    gsr_type_value_unref (symbol->type, data);
    return TRUE;
}
