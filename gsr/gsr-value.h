
#ifndef GSR_VALUE_H
#define GSR_VALUE_H 1

/**
 ** @file
 **
 ** @section GSR Value
 **
 ** GSR Value is read-only multi-type value structure.
 ** Structure is similar to glib GVariant but it supports dynamic
 ** pluggable types, their coercion and validation (via GSR Type)
 **/

#include <glib.h>
#include <gsr/gsr-type.h>

typedef struct _GSR_Value GSR_Value;
/**
 **< @brief Opaque data structure to represent a multi-type value
 **/

GSR_Value *     gsr_value_new (
    GSR_Type *type,
    gpointer  data);
/**
 **<@brief Create new GSR_Value with data of primary type
 **
 ** Calls type validation.
 **
 ** @param type type of 'data'
 ** @param data value data
 **
 ** @return NULL on error, not-NULL otherwise
 **/

GSR_Value *     gsr_value_from_gint32 (
    gint32 value);
/**
 **<@brief Create new GSR_Value from gint32 value
 **
 ** @param value gint32 value
 **
 ** @return NULL ont error, not-NULL otherwise
 **/

GSR_Value *     gsr_value_from_gint64 (
    gint64 value);
/**
 **<@brief Create new GSR_Value from gint64 value
 **
 ** @param value gint64 value
 **
 ** @return NULL ont error, not-NULL otherwise
 **/

GSR_Value *     gsr_value_from_gboolean (
    gboolean value);
/**
 **<@brief Create new GSR_Value from gboolean value
 **
 ** @param value gboolean value
 **
 ** @return NULL ont error, not-NULL otherwise
 **/

GSR_Value *     gsr_value_from_string (
    const char *value);
/**
 **<@brief Create new GSR_Value from string value
 **
 ** @param value string value, not NULL
 **
 ** @return NULL ont error, not-NULL otherwise
 **/

GSR_Value *     gsr_value_ref (
    GSR_Value *self);
/**
 **<Atomically increment reference counter of object by one.
 **
 ** Function is thread safe and is not affected by any other GSR_Value function
 ** except gsr_value_unref ()
 **
 ** Function is NULL safe.
 **
 ** @param self valid GSR_Value object
 **
 ** @returns 'self' parameter itself
 **/

void            gsr_value_unref (
    GSR_Value *self);
/**
 **<@brief Atomically decrement reference counter of object by one.
 **
 ** If it drops to zero all allocated memory is released.
 **
 ** Function is thread safe and is not affected by any other GSR_Value function
 ** except gsr_value_ref ()
 **
 ** Function is NULL safe.
 **
 ** @param self valid GSR_Value object
 **/

gpointer        gsr_value_get (
    GSR_Value *self,
    GSR_Type  *type);
/**
 **<@brief Get typed value of GSR_Value
 **
 ** Function will coerce value if necessary
 **
 ** @param self GSR_Value instance
 ** @param type requested type
 **
 ** @return Value of 'type'
 **/

gint32          gsr_value_get_gint32 (
    GSR_Value *self);
/**
 **<@brief Get gint32 value of GSR_Value
 **/

gint64          gsr_value_get_gint64 (
    GSR_Value *self);
/**
 **<@brief Get gint64 value of GSR_Value
 **/

gboolean        gsr_value_get_gboolean (
    GSR_Value *self);
/**
 **<@brief Get gboolean value of GSR_Value
 **/

const char *    gsr_value_get_string (
    GSR_Value *self);
/**
 **<@brief Get string value of GSR_Value
 **/

#endif /* header guard */


