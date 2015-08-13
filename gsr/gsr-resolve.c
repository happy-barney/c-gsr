
#include "config.h"

#include <gsr/gsr.h>
#include <cmx/cmx.h>

#include <stdio.h>

typedef struct _GSR_Rule GSR_Rule;

struct _GSR {
    CMX_STRUCT_REFS_DEFINE;
    CMX_STRUCT_SHAREABLE_DEFINE;

    GList      *rules;
    GHashTable *cache;
};

struct _GSR_Rule {
    GSR_Resolve_Function callback;
    gpointer             data;
};

static GSR_Rule *   gsr_rule_new (
    GSR_Resolve_Function callback,
    gpointer             data)
{
    GSR_Rule *retval = g_slice_alloc (sizeof (*retval));
    retval->callback = callback;
    retval->data     = data;
    return retval;
}

static void         gsr_rule_free (
    gpointer ptr)
{
    GSR_Rule *self = ptr;
    if (NULL != self)
        g_slice_free1 (sizeof (*self), self);
}

GSR *       gsr_new (
    GSR_Static *def)
{
    GSR *retval = g_slice_alloc0 (sizeof (*retval));

    CMX_STRUCT_REFS_INIT      (retval);
    CMX_STRUCT_SHAREABLE_INIT (retval);

    if (NULL != def)
        gsr_add_rule_static (retval, def);

    retval->cache = g_hash_table_new_full (
        g_direct_hash,
        g_direct_equal,
        NULL,
        gsr_value_destroy
    );

    return retval;
}

GSR *       gsr_ref (
    GSR *self)
{
    CMX_STRUCT_REFS_REF (self);
}

void        gsr_unref (
    GSR *self)
{
    CMX_STRUCT_REFS_UNREF (self) {
        if (NULL != self->rules)
            g_list_free_full (self->rules, gsr_rule_free);
        if (NULL != self->cache)
            g_hash_table_unref (self->cache);
        g_slice_free1 (sizeof (*self), self);
    }
}

GSR *       gsr_share (
    GSR *self)
{
    CMX_STRUCT_SHAREABLE_SHARE (self);
}

GSR *       gsr_add_rule (
    GSR                  *self,
    GSR_Symbol           *symbol,
    GSR_Resolve_Function  callback,
    gpointer              data)
{
    if (NULL == self) return NULL;
    if ((NULL == symbol) && (NULL == callback)) return NULL;

    GSR_Rule *rule = gsr_rule_new (callback, data);
    self->rules = g_list_append (self->rules, rule);

    return self;
}

GSR *       gsr_add_rule_static (
    GSR        *self,
    GSR_Static *def)
{
    if (NULL == def) return NULL;

    return gsr_add_rule (self, NULL, gsr_resolve_from_static, def);
}

GSR *       gsr_add_rule_gint32 (
    GSR        *self,
    GSR_Symbol *symbol,
    gint32      value)
{
    if (NULL == symbol) return NULL;
    return gsr_add_rule (self, symbol, gsr_resolve_from_gint32, GSR_DATA_FROM_GINT32 (value));
}

GSR *       gsr_add_rule_gint64 (
    GSR        *self,
    GSR_Symbol *symbol,
    gint64      value)
{
    if (NULL == symbol) return NULL;
    return gsr_add_rule (self, symbol, gsr_resolve_from_gint64, GSR_DATA_FROM_GINT64 (value));
}


GSR *       gsr_add_rule_gboolean (
    GSR        *self,
    GSR_Symbol *symbol,
    gboolean    value)
{
    if (NULL == symbol) return NULL;
    return gsr_add_rule (self, symbol, gsr_resolve_from_gboolean, GSR_DATA_FROM_GBOOLEAN (value));
}

GSR *       gsr_add_rule_string (
    GSR        *self,
    GSR_Symbol *symbol,
    const char *value)
{
    if (NULL == symbol) return NULL;
    return gsr_add_rule (self, symbol, gsr_resolve_from_string, (gpointer) value);
}

gboolean    gsr_set (
    GSR        *self,
    GSR_Symbol *symbol,
    GSR_Type   *type,
    gpointer    data)
{
    GSR_Value *value = gsr_value_new (type, data);
    return gsr_set_value (self, symbol, value);
}

gboolean    gsr_set_value (
    GSR        *self,
    GSR_Symbol *symbol,
    GSR_Value  *value)
{
    if (   (NULL == self)
        || (NULL == symbol)
        || (NULL == value)
        || (! gsr_symbol_validate (symbol, value))
    ) {
        gsr_value_unref (value);
        return FALSE;
    }

    g_hash_table_insert (self->cache, symbol, value);
    return TRUE;
}

gboolean    gsr_set_gint32 (
    GSR        *gsr,
    GSR_Symbol *symbol,
    gint32      value)
{
    return gsr_set_value (gsr, symbol, gsr_value_from_gint32 (value));
}

gboolean    gsr_set_gint64 (
    GSR        *gsr,
    GSR_Symbol *symbol,
    gint64      value)
{
    return gsr_set_value (gsr, symbol, gsr_value_from_gint64 (value));
}

gboolean    gsr_set_gboolean (
    GSR        *gsr,
    GSR_Symbol *symbol,
    gboolean    value)
{
    return gsr_set_value (gsr, symbol, gsr_value_from_gboolean (value));
}

gboolean    gsr_set_string (
    GSR        *gsr,
    GSR_Symbol *symbol,
    const char *value)
{
    return gsr_set_value (gsr, symbol, gsr_value_from_string ((gpointer) value));
}

gpointer    gsr_lookup (
    GSR        *gsr,
    GSR_Symbol *symbol,
    GSR_Type   *type)
{
    GSR_Value *value = gsr_lookup_value (gsr, symbol);
    gpointer retval = gsr_value_get (value, type);
    gsr_value_unref (value);

    return retval;
}

GSR_Value * gsr_lookup_value (
    GSR        *gsr,
    GSR_Symbol *symbol)
{
    if (NULL == gsr) return NULL;
    if (NULL == symbol) return NULL;

    GSR_Value *retval = gsr_value_ref (g_hash_table_lookup (gsr->cache, symbol));
    if (NULL == retval) {
        GList *current = gsr->rules;

        while ((NULL == retval) && (NULL != current)) {
            GSR_Rule *rule = current->data;
            retval = rule->callback (gsr, symbol, rule->data);
            current = current->next;
        }

        // TODO: how to provide "no cache" ?
        gsr_set_value (gsr, symbol, retval);
    }

    // TODO: signal error
    // errno = NULL == retval ? ENOENT : 0;
    return retval;
}

gint32      gsr_lookup_gint32 (
    GSR        *gsr,
    GSR_Symbol *symbol)
{
    GSR_Value *value = gsr_lookup_value (gsr, symbol);
    gint32 retval = 0;

    if (NULL != value) {
        retval = gsr_value_get_gint32 (value);
        gsr_value_unref (value);
    }

    return retval;
}

GSR_RESOLVE_FUNCTION (gsr_resolve_from_static) {
    GSR_Static *current = data;
    GSR_Value  *retval  = NULL;

    GSR_RESOLVE_FUNCTION_SANITY_CHECK ();
    if (NULL == data) return NULL;

    for (; (NULL != current->symbol) || (NULL != current->callback); ++current) {
        if ((NULL == current->symbol) || (symbol == current->symbol ())) {
            retval = current->callback (gsr, symbol, current->data);
            if (NULL != retval) break;
        }
    }

    return retval;
}

GSR_RESOLVE_FUNCTION (gsr_resolve_from_gint32) {
    GSR_RESOLVE_FUNCTION_SANITY_CHECK ();

    return gsr_value_from_gint32 (GSR_DATA_TO_GINT32 (data));
}

GSR_RESOLVE_FUNCTION (gsr_resolve_from_gint64) {
    GSR_RESOLVE_FUNCTION_SANITY_CHECK ();

    return gsr_value_from_gint64 (GSR_DATA_TO_GINT64 (data));
}

GSR_RESOLVE_FUNCTION (gsr_resolve_from_gboolean) {
    GSR_RESOLVE_FUNCTION_SANITY_CHECK ();

    return gsr_value_from_gboolean (GSR_DATA_TO_GBOOLEAN (data));
}

GSR_RESOLVE_FUNCTION (gsr_resolve_from_string) {
    GSR_RESOLVE_FUNCTION_SANITY_CHECK ();

    return gsr_value_from_gboolean (GSR_DATA_TO_GBOOLEAN (data));
}

GSR_RESOLVE_FUNCTION (gsr_resolve_if) {
    GSR_Static *def   = data;
    GSR_Value  *value = NULL;

    GSR_RESOLVE_FUNCTION_SANITY_CHECK ();
    if (NULL == def) return NULL;
    if (NULL == def->callback) return NULL;
    if (NULL == def->symbol)   return NULL;

    value = gsr_lookup_value (gsr, def->symbol ());
    if (NULL == value) return NULL;

    gsr_value_unref (value);
    return def->callback (gsr, symbol, def->data);
}

GSR_RESOLVE_FUNCTION (gsr_resolve_alias) {
    GSR_RESOLVE_FUNCTION_SANITY_CHECK ();
    if (symbol == data) return NULL;

    return gsr_lookup_value (gsr, data);
}
