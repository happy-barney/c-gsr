
#include <gsr.h>
#include <cmx/cmx.h>

struct _GSR_Value {
    CMX_STRUCT_REFS_DEFINE;
    CMX_STRUCT_SHAREABLE_DEFINE;

    GSR_Type   *type;           /**< primary value type */
    gpointer    data;           /**< primary value data */
    GHashTable *coerce;         /**< cache values in different types*/
};

static gboolean gsr_value_cache_foreach_unref (
    gpointer key,
    gpointer value,
    gpointer user_data)
{
    gsr_type_value_unref (key, value);
    return TRUE;
}

GSR_Value *     gsr_value_new (
    GSR_Type *type,
    gpointer  data)
{
    if (NULL == type) return NULL;
    if (! gsr_type_validate (type, data)) return NULL;

    GSR_Value *retval = g_slice_alloc (sizeof (*retval));

    if (NULL == retval) return NULL;

    CMX_STRUCT_REFS_INIT (retval);
    CMX_STRUCT_SHAREABLE_INIT (retval);

    retval->type   = type;
    retval->data   = data;
    retval->coerce = g_hash_table_new (g_direct_hash, g_direct_equal);

    g_hash_table_insert (retval->coerce, type, data);

    CMX_STRUCT_SHAREABLE_SHARE (retval);
}

GSR_Value *     gsr_value_from_gint32 (
    gint32 value)
{
    return gsr_value_new (gsr_type_gint32 (), GSR_DATA_FROM_GINT32 (value));
}

GSR_Value *     gsr_value_from_gint64 (
    gint64 value)
{
    return gsr_value_new (gsr_type_gint64 (), GSR_DATA_FROM_GINT64 (value));
}

GSR_Value *     gsr_value_from_gboolean (
    gboolean value)
{
    return gsr_value_new (gsr_type_gboolean (), GSR_DATA_FROM_GBOOLEAN (value));
}

GSR_Value *     gsr_value_from_string (
    const char *value)
{
    return gsr_value_new (gsr_type_string (), (gpointer) (value));
}

GSR_Value *     gsr_value_ref (
    GSR_Value *self)
{
    CMX_STRUCT_REFS_REF (self);
}

void            gsr_value_unref (
    GSR_Value *self)
{
    CMX_STRUCT_REFS_UNREF (self) {
        g_hash_table_foreach_remove (self->coerce, gsr_value_cache_foreach_unref);
        g_hash_table_unref (self->coerce);
    }
}

gpointer        gsr_value_get (
    GSR_Value *self,
    GSR_Type  *type)
{
    gpointer retval = NULL;

    if (NULL == self) return NULL;
    if (NULL == type) return NULL;

    CMX_STRUCT_SHAREABLE_SYNCHRONIZE (self) {
        if (! g_hash_table_lookup_extended (self->cache, type, NULL, &retval)) {
            if (gsr_type_coerce_exists (self->type, type)) {
                retval = gsr_type_coerce (self->type, type, self->data);
                g_hash_table_insert (self->cache, type, retval);
            }
        }
    }

    return gsr_type_value_ref (type, retval);
}

gint32          gsr_value_get_gint32 (
    GSR_Value *self)
{
    GSR_Type *type   = gsr_type_gint32 ();
    gpointer  value  = gsr_value_get (self, type);
    gint32    retval = GSR_DATA_TO_GINT32 (value);

    gsr_type_value_unref (type, value);

    return retval;
}

gint64          gsr_value_get_gint64 (
    GSR_Value *self)
{
    GSR_Type *type   = gsr_type_gint64 ();
    gpointer  value  = gsr_value_get (self, type);
    gint64    retval = GSR_DATA_TO_GINT64 (value);

    gsr_type_value_unref (type, value);

    return retval;
}

gboolean          gsr_value_get_gboolean (
    GSR_Value *self)
{
    GSR_Type *type   = gsr_type_gboolean ();
    gpointer  value  = gsr_value_get (self, type);
    gboolean  retval = GSR_DATA_TO_GBOOLEAN (value);

    gsr_type_value_unref (type, value);

    return retval;
}

const char *      gsr_value_get_string (
    GSR_Value *self)
{
    GSR_Type   *type   = gsr_type_string ();
    gpointer    value  = gsr_value_get (self, type);
    const char *retval = GSR_DATA_TO_STRING (value);

    gsr_type_value_unref (type, value);

    return retval;
}
