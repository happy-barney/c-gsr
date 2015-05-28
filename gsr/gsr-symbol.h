
#ifndef GSR_SYMBOL_H
#define GSR_SYMBOL_H 1

#include <glib.h>
#include <gsr/gsr-type.h>

/**
 ** @file
 **
 ** @section GSR Symbol
 **
 ** GSR Symbol / GSR Symbol function represents unique (identified by string name)
 ** symbol as well as well defined C tokens to increase safety of your programs.
 **/

#define GSR_SYMBOL_FUNCTION(Function)                                   \
    GSR_Symbol * Function (void)
/**
 **<@brief Helper macro to declare GSR Symbol function
 **
 ** @param Function C-token, symbol function name
 **
 ** Usage:
 **     GSR_SYMBOL_FUNCTION (MY_SYMBOL);
 **     GSR_SYMBOL_FUNCTION (MY_SYMBOL) {
 **         ...;
 **     }
 **/

#define GSR_SYMBOL_FUNCTION_BODY(String, Type) {                        \
        static GSR_Symbol *handle = NULL;                               \
        return                                                          \
            gsr_symbol_registry_test_and_set (String, Type, &handle);   \
    }
/**
 **<@brief Default body of GSR_Symbol_Function
 **
 ** Macro generates thread safe code.
 **
 ** @param String GSR Symbol name
 ** @param Type GSR Symbol type
 **
 ** Usage:
 **    GSR_SYMBOL_FUNCTION (Name) {
 **        GSR_SYMBOL_FUNCTION_BODY ("Name", Type);
 **    }
 **
 ** @see
 ** - GSR_SYMBOL_FUNCTION_DEFINE()
 ** - gsr_symbol_registry_test_and_set()
 **/

#define GSR_SYMBOL_FUNCTION_DEFINE(Name, Type)                          \
    GSR_SYMBOL_FUNCTION (Name)                                          \
        GSR_SYMBOL_FUNCTION_BODY (#Name, Type)
/**
 **<@brief Helper macro to define GSR Symbol function
 **
 ** Generated function can be called from any thread without additional
 ** locks.
 **
 ** Uses string "Name" as symbol name.
 ** Combines GSR_SYMBOL_FUNCTION() and GSR_SYMBOL_FUNCTION_BODY()
 **
 ** @param Name C-string, symbol name
 ** @param Type GSR_Type_Handle
 **/

typedef struct _GSR_Symbol GSR_Symbol;
/**
 **<@brief Opaque structure representing symbol handle
 **
 ** Although GSR uses "GSR_Symbol *" as real symbol data type it is not
 ** guaranted that its value is valid pointer.
 **
 ** Pointer type is choosen only to ensure that value can be accepted
 ** by 'gpointer' data structures.
 **/

typedef GSR_SYMBOL_FUNCTION ((*GSR_Symbol_Function));
/**
 **<@brief Function returning symbol handle
 **
 ** GSR uses function pointer in static declarations as their values
 ** are static.
 **/

GSR_Symbol    * gsr_symbol_registry (
    const char *name,
    GSR_Type    type);
/**
 **<@brief create new symbol with name and expected type
 **
 ** Every call with same name returns same handle.
 ** Only first call type will be accepted
 **
 ** 'type' NULL means any type.
 **
 ** Function returns NULL in case of any unrecoverable error.
 **
 ** Function is thread safe.
 **
 ** @param name unique symbol name
 ** @param type its expected type (coercion must be possible)
 **
 ** @return Symbol handle
 **
 ** @see
 ** - gsr_symbol_validate()
 **/

GSR_Symbol    * gsr_symbol_registry_test_and_set (
    const char  *name,
    GSR_Type     type,
    GSR_Symbol **place);
/**
 **<@brief Create new symbol and atomically populate its value
 **
 ** If place is not NULL, symbol is registered only if its value
 ** is NULL and return value is atomically copied there as well.
 **
 ** 'type' NULL means any type.
 **
 ** Function returns NULL in case of any unrecoverable error.
 **
 ** Function is thread safe.
 **
 ** @param name unique symbol name
 ** @param type its expected type (coercion must be possible)
 ** @param place pointer to memory where to store return value
 **
 ** @return Symbol handler
 **
 ** @see
 ** - gsr_symbol_registry()
 ** - gsr_symbol_validate()
 **/

GSR_Symbol    * gsr_symbol_lookup (
    const char  *name);
/**
 **<@brief Search for symbol identified by given name
 **
 ** Function is thread safe.
 **
 ** @param name symbol name
 **
 ** @return Symbol handler or NULL if not found
 **/

const char    * gsr_symbol_name (
    GSR_Symbol  *symbol);
/**
 **<@brief Get symbol name
 **
 ** Function is thread safe.
 **/

GSR_Type      * gsr_symbol_type (
    GSR_Symbol symbol);
/**
 **<@brief Get expected type of symbol
 **
 ** Function is thread safe.
 **/

gboolean        gsr_symbol_validate (
    GSR_Symbol *symbol,
    GSR_Value  *value);
/**
 **<@brief Verify whether 'value' can resolve 'symbol'
 **
 ** @param symbol symbol to resolve
 ** @param value  potential symbol value
 **
 ** @return TRUE/FALSE whether it is possible
 **/

#endif /* header guard */

