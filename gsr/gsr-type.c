
#include "config.h"

#include <gsr.h>
#include <cmx.h>

typedef struct _GSR_Type_Validate GSR_Type_Validate;
typedef struct _GSR_Type_Coerce GSR_Type_Coerce;

struct _GSR_Type_Validate_List {
    CMX_STRUCT_SHAREABLE_DEFINE;
    GList *list;
};

struct _GSR_Type_Coerce_Map {
    CMX_STRUCT_SHAREABLE_DEFINE;
    GHashTable *map;
};

struct _GSR_Type {
    CMX_STRUCT_REFS_DEFINE;
    const char                     *name;
    GSR_Type_Ref_Function           ref;
    GSR_Type_Unref_Function         unref;
    struct _GSR_Type_Validate_List  validate;
    struct _GSR_Type_Coerce_Map     coerce;
};

struct _GSR_Type_Validate {
    GSR_Type_Validate_Function callback;
    gpointer                   data;
};

struct _GSR_Type_Coerce {
    GSR_Type_Coerce_Function callback;
    gpointer                 data;
};

static GMutex     gsr_type_global_mutex = { 0 };
static GHashTable gsr_type_hash_table   = NULL;

static void         gsr_type_init (
    void)
{
    if (NULL == g_atomic_pointer_get (&( gsr_type_hash_table ))) {
        GHashTable *hash_table = g_hash_table_new (g_str_hash, g_str_equal);
        if (! g_atomic_pointer_compare_and_exchange (&( gsr_type_hash_table ), NULL, hash_table))
            g_hash_table_unref (hash_table);
    }
}

static GSR_Type   * gsr_type_alloc (
    void)
{
    GSR_Type *retval = g_slice_alloc0 (sizeof (*retval));
    CMX_STRUCT_REFS_INIT (retval);
    CMX_STRUCT_SHAREABLE_INIT (&( retval->validate ));
    CMX_STRUCT_SHAREABLE_INIT (&( retval->coerce ));

    return retval;
}

static GSR_Type   * gsr_type_lookup_unlocked (
    const char *name)
{
    return g_hash_table_lookup (gsr_type_hash_table, name);
}

GSR_Type *      gsr_type_ref (
    GSR_Type *self)
{
    CMX_STRUCT_REFS_REF (self);
}

void            gsr_type_unref (
    GSR_Type *self) {
    CMX_STRUCT_REFS_UNREF (type) {
        g_slice_free (sizeof (*type), type);
    }
}

gpointer        gsr_type_value_ref (
    GSR_Type *type,
    gpointer  value)
{
    if (NULL == type) return NULL;
    if (NULL == value) return NULL;
    if (NULL == type->ref) return value;

    return type->ref (value);
}

void            gsr_type_value_unref (
    GSR_Type *type,
    gpointer  value)
{
    if (NULL == type) return;
    if (NULL == value) return;
    if (NULL == type->unref) return;

    type->unref (value);
}

GSR_Type *      gsr_type_registry_static (
    const char               *name,
    GSR_Type_Ref_Function     ref,
    GSR_Type_Unref_Function   unref)
{
    GSR_Type *retval = NULL;

    gsr_type_init ();

    CMX_SYNCHRONIZE_WITH (gsr_type_global_mutex) {
        retval = gsr_type_lookup_unlocked (name);

        if (NULL == retval) {
            retval = gsr_type_alloc ();

            retval->name  = name; /* TODO: strdup ? */
            retval->ref   = ref;
            retval->unref = unref;

            g_hash_table_insert (gsr_type_hash_table, name, retval);
        }
    }

    return retval;
}

GSR_Type *      gsr_type_lookup (
    const char * name)
{
    GSR_Type *retval;

    CMX_SYNCHRONIZE_WITH (gsr_type_global_mutex) {
        retval = gsr_type_lookup_unlocked (name);
    }

    return retval;
}

void            gsr_type_validate_add (
    GSR_Type                   *type,
    GSR_Type_Validate_Function  validate,
    gpointer                    data)
{
    if (NULL == type) return;
    if (NULL == validate) return;

    GSR_Type_Validate *callback = g_slice_alloc (sizeof (*callback));

    callback->function     = validate;
    callback->data         = data;

    CMX_SYNCHRONIZE_WITH (type->validate_mutex)
        type->validate = g_list_append (type->validate, callback);
}

gboolean        gsr_type_validate (
    GSR_Type *type,
    gpointer  value)
{
    GList    *list   = NULL;;
    gboolean  retval = TRUE;

    if (NULL == type) return FALSE;

    CMX_STRUCT_SHAREABLE_SYNCHRONIZE (&( type->validate )) {
        for (list = type->validate->list; NULL != list; list = list->next) {
            GSR_Type_Validate *callback = list->data;
            retval = callback->function (value, callback->data);
            if (! retval) break;
        }
    }

    return retval;
}

void            gsr_type_coerce_add (
    GSR_Type                 *type,
    GSR_Type                 *to,
    GSR_Type_Coerce_Function  coerce,
    gpointer                  data)
{
    if (NULL == type) return;
    if (NULL == to)   return;
    if (NULL == coerce) return;

    CMX_STRUCT_SHAREABLE_SYNCHRONIZE (&( type->coerce)) {
        if (NULL == type->coerce->map)
            type->coerce->map = g_hash_table_new (g_direct_hash, g_direct_compare);
        if (NULL == g_hash_table_lookup (type->coerce->map, to)) {
            struct _GSR_Type_Coerce *callback = g_slice_alloc (sizeof (*callback));
            callback->callback = coerce;
            callback->data     = data;
            g_hash_table_insert (type->coerce->map, to, callback);
        }
    }
}

gint32          gsr_type_coerce_lookup (
    GSR_Type *type,
    GSR_Type *to,
    gint32    max_depth)
{
    if (NULL == type) return -1;
    if (NULL == to)   return -1;
    if (NULL == type->coerce->map) return -1;
    if (max_depth < 0) return -1;

    gint32 retval = -1;
    GList *keys   = NULL;

    CMX_STRUCT_SHAREABLE_SYNCHRONIZE (&( type->coerce->map)) {
        if (NULL != g_hash_table_lookup (type->coerce->map, to)) {
            retval = 0;
        } else {
            values = g_hash_table_get_keys (type->coerce->map);
        }
    }

    if (NULL != keys) {
        GList *current = keys;

        while (NULL != current) {
            GSR_Type *key = current->data;
            struct _GSR_Type_Coerce *callback = g_hash_table_lookup (type->coerce->map, key);
            guint32 value = gsr_type_coerce_lookup (key, to, max_depth -1);
            current = current->next;

            if (value < 0) continue;
            if (retval < 0) retval = value;
            if (value < retval) retval = value;
        }

        g_list_free (keys);
    }

    return retval;
}

const char *    gsr_type_name (
    GSR_Type *type)
{
    if (NULL == type) return NULL;
    return type->name;
}

GSR_TYPE_FUNCTION (gsr_type_gint32) {
    GSR_TYPE_FUNCTION_BODY (gint32);
}

GSR_TYPE_FUNCTION (gsr_type_gint64) {
    GSR_TYPE_FUNCTION_BODY (gint64);
}

GSR_TYPE_FUNCTION (gsr_type_gboolean) {
    GSR_TYPE_FUNCTION_BODY (gboolean);
}

GSR_TYPE_FUNCTION (gsr_type_string) {
    GSR_TYPE_FUNCTION_BODY (const char *);
}


