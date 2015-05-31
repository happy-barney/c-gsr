
#ifndef GSR_RESOLVE_H
#define GSR_RESOLVE_H 1

#include <glib.h>
#include <gsr/gsr-type.h>
#include <gsr/gsr-symbol.h>
#include <gsr/gsr-value.h>

#define GSR_RESOLVE_FUNCTION(Name)                                      \
    GSR_Value * Name (GSR *gsr, GSR_Symbol *symbol, gpointer data)
/**
 **<@brief Signature of GSR_Resolve_Function
 **
 ** refer GSR_Resolve_Function
 **
 ** I prefer to use such macros to quickly determine purpose of function
 ** regardless of fact that return type and arguments are hidden.
 **
 ** Usage:
 **     GSR_RESOLVE_FUNCTION (name);
 **     GSR_RESOLVE_FUNCTION (name) {
 **         return gsr_value_new (gsr_type_gint32, GSR_DATA_TO_GINT32 (data));
 **     }
 **/

#define GSR_RESOLVE_FUNCTION_SANITY_CHECK()                             \
    if (NULL == gsr) return NULL;                                       \
    if (NULL == symbol) return NULL;
/**
 **<@brief Common sanity check for GSR resolve functions
 **/

#define END_OF_GSR_STATIC                                               \
    { .symbol = NULL, .callback = NULL }
/**
 **<@brief Convenient macro to end GSR_Static definition
 **
 ** Usage:
 **     GSR_Static def[...] = {
 **         ...,
 **         END_OF_GSR_STATIC
 **     };
 **/

#define gsr_destroy                                                     \
    ((GDestroyNotify) gsr_unref)

#define GSR_VALUE_OF_TYPE(Type, Value)                                  \
    ( ( (struct { Type x; }) { Value } ).x )

#define GSR_STATIC_GINT32(Value)                                        \
    .callback = gsr_resolve_from_gint32,                                \
    .data     = GSR_DATA_FROM_GINT32 (GSR_VALUE_OF_TYPE (gint32, (Value)))

#define GSR_STATIC_GINT64(Value)                                        \
    .callback = gsr_resolve_from_gint64,                                \
    .data     = GSR_DATA_FROM_GINT64_STATIC (GSR_VALUE_OF_TYPE (gint64, (Value)))

#define GSR_STATIC_GBOOLEAN(Value)                                      \
    .callback = gsr_resolve_from_gboolean,                              \
    .data     = GSR_DATA_FROM_GBOOLEAN (GSR_VALUE_OF_TYPE (gboolean, (Value)))

#define GSR_STATIC_IF(Symbol, ...)                                      \
    .callback = gsr_resolve_if,                                         \
    .data     = (GSR_Static []) { { .symbol = (Symbol), ##__VA_ARGS__ } }
/**
 **<@brief Resolve symbol if symbol 'Symbol' can be resolved
 **
 ** @param Symbol referenced symbol
 ** @param ...    callback/data
 **/

#define GSR_STATIC_ALIAS(Symbol)                                        \
    .callback = gsr_resolve_alias,                                      \
    .data     = (Symbol)
/**
 **<@brief Resolve symbol as alias of another symbol
 **
 ** @param Symbol referenced symbol
 **/

typedef struct _GSR GSR;
/**
 **<@brief Opaque data type representing symbol resolver
 **/

typedef struct _GSR_Static {
    GSR_Symbol_Function  symbol;
    GSR_Resolve_Function callback;
    gpointer             data;
} GSR_Static;
/**
 **<@brief Data type to define static resolve map
 **
 ** @var symbol   what symbol callback function can resolve
 ** @var callback how to resolve symbol value
 ** @var data     data passed to callback function
 **
 ** Both symbol and callback are NULL signals "end of list" entry.
 **
 ** symbol == NULL itself tells that callback can/may resolve any symbol
 ** callback == NULL itself tells that symbol cannot be resolved by following rules.
 **/

typedef GSR_RESOLVE_FUNCTION ((*GSR_Resolve_Function));
/**
 **<@brief Resolve symbol function
 **
 ** @param gsr    current resolver
 ** @param symbol symbol to resolve
 ** @param data   function private data
 **
 ** @return NULL if symbol cannot be resolved, valid GSR_Value * otherwise
 **
 ** @relates GSR_RESOLVE_FUNCTION()
 **/

GSR       * gsr_new (
    GSR_Static *def);
/**
 **<@brief Create new resolver
 **
 ** @param def Optional GSR_Static definition. Can be NULL.
 **
 ** @return new GSR resolver (or NULL in case of error)
 **/

GSR       * gsr_ref (
    GSR *gsr);
/**
 **<@brief Increate internal reference counter
 **
 ** Function is always thread safe.
 ** Function is NULL safe.
 **
 ** @return 'gsr' parameter itself
 **
 ** @see
 ** - cmx-struct-refs.h in cmx library
 **/

void        gsr_unref (
    GSR *gsr);
/**
 **<@brief Decrease internal reference counter and destroy object if needed
 **
 ** Function is always thread safe.
 ** Function is NULL safe.
 **
 ** @see
 ** - cmx-struct-refs.h in cmx library
 **/

GSR       * gsr_share (
    GSR *gsr);
/**
 **<@brief Enable internal locks to prevent unsafe concurrent access
 **
 ** After this function call every function declared in gsr-resolve.h
 ** working with same object will be thread safe.
 **
 ** Function is always thread safe.
 ** Function is NULL safe.
 **
 ** @param gsr object to share
 **
 ** @return 'gsr' parameter itself
 **
 ** @see
 ** - cmx-struct-shareable.h in cmx library
 **/

GSR       * gsr_add_rule (
    GSR                  *gsr,
    GSR_Symbol           *symbol,
    GSR_Resolve_Function  callback,
    gpointer              data);
/**
 **<@brief add resolve rule to resolver
 **
 ** Rules are evaluated in order as they were added into resolver.
 **
 ** Function is thread safe after calling gsr_share() on 'gsr'.
 ** Function is NULL safe.
 **
 ** @param gsr      Valid GSR resolver
 ** @param symbol   Symbol to resolve by callback (NULL = any)
 ** @param callback Function to resolve symbol (NULL = do not resolve in this resolver)
 ** @param data     Callback data
 **
 ** @return value of 'gsr' parameter itself
 **/

GSR       * gsr_add_rule_static (
    GSR        *gsr,
    GSR_Static *def);
/**
 **<@brief Add static definition to resolver
 **
 ** Function is thread safe after calling gsr_share() on 'gsr'.
 ** Function is NULL safe.
 **
 ** @param gsr      Valid GSR resolver
 ** @param def      pointer to GSR_Static definition list
 **
 ** @return value of 'gsr' parameter itself
 **/

GSR       * gsr_add_rule_gint32 (
    GSR        *gsr,
    GSR_Symbol *symbol,
    gint32      value);
/**
 **<@brief static rule to resolve symbol as gint32 value
 **/

GSR       * gsr_add_rule_gint64 (
    GSR        *gsr,
    GSR_Symbol *symbol,
    gint64      value);
/**
 **<@brief static rule to resolve symbol as gint64 value
 **/

GSR       * gsr_add_rule_gboolean (
    GSR        *gsr,
    GSR_Symbol *symbol,
    gboolean    value);
/**
 **<@brief static rule to resolve symbol as gboolean value
 **/

GSR       * gsr_add_rule_string (
    GSR        *gsr,
    GSR_Symbol *symbol,
    const char *value);
/**
 **<@brief static rule to resolve symbol as string value
 **/

gboolean    gsr_set (
    GSR        *gsr,
    GSR_Symbol *symbol,
    GSR_Type   *type,
    gpointer    data);
/**
 **<@brief Set value of symbol
 **
 ** Function populates value of symbol in current resolver if given
 ** value validates.
 **
 ** Function will coerce 'data' into value required by 'symbol' whenever necessary'
 **
 ** @param gsr    resolver
 ** @param symbol symbol to resolve
 ** @param type   type of value data
 ** @param data   value data
 **
 ** @return TRUE if symbol value was populated, FALSE otherwise
 **/

gboolean    gsr_set_value (
    GSR        *gsr,
    GSR_Symbol *symbol,
    GSR_Value  *value);

gboolean    gsr_set_gint32 (
    GSR        *gsr,
    GSR_Symbol *symbol,
    gint32      value);

gboolean    gsr_set_gint64 (
    GSR        *gsr,
    GSR_Symbol *symbol,
    gint64      value);

gboolean    gsr_set_gboolean (
    GSR        *gsr,
    GSR_Symbol *symbol,
    gboolean    value);

gboolean    gsr_set_string (
    GSR        *gsr,
    GSR_Symbol *symbol,
    const char *value);

GSR_Value * gsr_resolve
GSR_RESOLVE_FUNCTION (gsr_resolve_from_static);
GSR_RESOLVE_FUNCTION (gsr_resolve_from_gint32);
GSR_RESOLVE_FUNCTION (gsr_resolve_from_gint64);
GSR_RESOLVE_FUNCTION (gsr_resolve_from_gboolean);
GSR_RESOLVE_FUNCTION (gsr_resolve_from_string);
GSR_RESOLVE_FUNCTION (gsr_resolve_if);
GSR_RESOLVE_FUNCTION (gsr_resolve_alias);

#endif /* header guard */

