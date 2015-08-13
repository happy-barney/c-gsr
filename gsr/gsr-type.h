
#ifndef GSR_TYPE_H
#define GSR_TYPE_H 1

/**
 ** @file
 **
 ** @section GSR dynamic types
 **
 ** GSR dynamic types let GSR know how to ref/unref values of custom types
 ** as well as validate their values and how to (programmatically) coerce them.
 **
 ** @subsection How to create your own GSR type
 **
 ** You have to registry your data type using one of following functions
 ** - gsr_type_registry
 **
 ** You can use type builder macros which creates "type function" (function returning type)
 ** as expected by gsr-symbol.h
 **
 ** - Declare your type function
 **   GSR_TYPE_FUNCTION (my_type_function_name);
 **
 ** - define your type function
 **     GSR_TYPE_FUNCTION_DEFINE (my_type_function_name
 **         , GSR_TYPE_NAME      ("this is my type")
 **         , GSR_TYPE_REF       (my_type_ref)
 **         , GSR_TYPE_UNREF     (my_type_unref)
 **     );
 **/

#include <glib.h>

#define GSR_TYPE_FUNCTION(Type)                                         \
    GSR_Type * Type (void)
/**
 **<@brief Declare GSR_Type function
 **
 ** @param Type C-token, will be used as function name
 **/

#define GSR_TYPE_FUNCTION_DEFINE(Type, ...)                             \
    GSR_TYPE_FUNCTION (Type) {                                          \
        GSR_TYPE_FUNCTION_BODY (Type, ##__VA_ARGS__ );                  \
    }
/**
 **<@brief Creates implementation of GSR_Type function
 **
 ** @param Type C-token, generated function name
 **             Its string value will be default type name
 ** @param ...  GSR_Template static initializers
 **
 ** @see
 ** - GSR_TYPE_WITH_NAME()
 ** - GSR_TYPE_WITH_PARENT()
 ** - GSR_TYPE_WITH_REF()
 ** - GSR_TYPE_WITH_UNREF()
 ** - GSR_TYPE_WITH_VALIDATE()
 **/

#define GSR_TYPE_FUNCTION_BODY(Type, ...)                               \
    {                                                                   \
        static GSR_Type          *type     = NULL;                      \
        static GSR_Type_Template  template = {                          \
            GSR_TYPE_WITH_NAME (#Type),                                 \
            ##__VA_ARGS__                                               \
        };                                                              \
        gsr_type_registry_into (&type, &template);                      \
        return type;                                                    \
    }
/**
 **<@brief Default implementation of GSR_Type_Function
 **
 ** Generated function is multithread safe.
 **
 ** @param Type C-token, function name
 **             Its string value will be default type name
 ** @param ...  GSR_Template static initializers
 **
 ** @see
 ** - GSR_TYPE_WITH_NAME()
 ** - GSR_TYPE_WITH_PARENT()
 ** - GSR_TYPE_WITH_REF()
 ** - GSR_TYPE_WITH_UNREF()
 ** - GSR_TYPE_WITH_VALIDATE()
 **/

#define GSR_TYPE_FUNCTION_DEFINE_AS(Type, ...)                          \
    GSR_TYPE_FUNCTION_DEFINE_AS_TRAN (                                  \
        CMX_UNIQUE_TOKEN (Type),                                        \
        Type, ##__VA_ARGS__                                             \
    )
/**
 **<@brief Creates implementation of GSR_Type function along with its validation
 ** function.
 **
 ** Macro must be followed by block of GSR_Type_Validate_Function with argument
 ** names 'value' and 'data'.
 ** Generated function name looks like Type_LINE_COUNTER_validate
 **
 ** Macro accepts same arguments as GSR_TYPE_FUNCTION_DEFINE() except
 ** it uses GSR_TYPE_WITH_VALIDATE() as well
 **
 ** Macro increments __COUNTER__
 **
 ** @param Type C-token, function name
 **             Its string value will be default type name
 ** @param ...  GSR_Template static initializers
 **
 **/

#define GSR_TYPE_FUNCTION_DEFINE_AS_TRAN(Prefix, Type, ...)             \
    GSR_TYPE_FUNCTION_DEFINE_AS_IMPL (                                  \
        CMX_TOKEN (Prefix, validate),                                   \
        Type, ##__VA_ARGS__                                             \
    )
/**
 **<@brief Transition macro to prepare arguments for implementation macro
 **/

#define GSR_TYPE_FUNCTION_DEFINE_AS_IMPL(Name, Type, ...)               \
    GSR_TYPE_VALIDATE_FUNCTION (Name);                                  \
    GSR_TYPE_FUNCTION_DEFINE (                                          \
        Type, ##__VA_ARGS__,                                            \
        GSR_TYPE_WITH_VALIDATE (Name, NULL)                             \
    );                                                                  \
    GSR_TYPE_VALIDATE_FUNCTION (Name)
/**
 **<@brief Implementation macro of GSR_TYPE_FUNCTION_DEFINE_AS()
 **/

#define GSR_TYPE_VALIDATE_FUNCTION(Name)                                \
    gboolean Name (gconstpointer value, gpointer data)
/**
 **<@brief Expand as prototype of GSR_Type_Validate_Function
 **
 ** Function prototype will have 'value' and 'data' arguments
 **
 ** @param Name C-Token, function name
 **/

#define GSR_TYPE_WITH_NAME(Name)                                        \
    .name = (Name)
/**
 **<@brief Use Name as type name (lookup value)
 **
 ** @param Name C-string
 **
 ** @see
 ** - gsr_type_lookup()
 ** - gsr_type_name()
 **/

#define GSR_TYPE_WITH_PARENT(Parent)                                    \
    .subtype_of = (Parent)
/**
 **<@brief Type is subtype of another type
 **
 ** TODO: RFC
 **
 ** Subtype means that Ref and Unref functions are shared (if not provided)
 ** and parent validation is applied as well.
 **
 ** Subtyping also automatically creates bidirectional coercion.
 **
 ** @param Parent GSR_Type_Function of parent type
 **/

#define GSR_TYPE_WITH_REF(Function)                                     \
    .ref = (Function)
/**
 **<@brief Use given function as type's ref function
 **
 ** @param Function GSR_Type_Ref_Function
 **
 ** @see
 ** - gsr_type_value_ref
 **/

#define GSR_TYPE_WITH_UNREF(Function)                                   \
    .unref = (Function)
/**
 **<@brief Use given function as type's unref function
 **
 ** @param Function GSR_Type_Unref_Function
 **
 ** @see
 ** - gsr_type_value_unref
 **/

#define GSR_TYPE_WITH_VALIDATE(Function, Data)                          \
    .validate = (Function), .validate_data = (Data)
/**
 **<@brief Type validation function with its custom data
 **
 ** @param Function GSR_Type_Validate_Function
 ** @param Data     gpointer, custom data for validate function
 **/

typedef struct _GSR_Type GSR_Type;
/**
 **<@brief Opague GSR_Type handler
 **/

typedef struct _GSR_Type_Template GSR_Type_Template;
/**
 **/

typedef GSR_Type * (*GSR_Type_Function)          (void);
/**
 **<@brief Function returning GSR_Type handler
 **
 ** GSR works with functions returning handler as their pointers
 ** can be used in static context whereas handlers are dynamic.
 **
 ** GSR also defines "best practice" macros to easily and in uniform way
 ** define them, see GSR_TYPE() and GSR_TYPE_FUNCTION()
 **
 ** Well, you know, I'm macro positive guy :-)
 **/

typedef gpointer   (*GSR_Type_Ref_Function)      (gpointer value);
/**
 **<@brief Ref function of type
 **
 ** @see
 ** - gsr_type_value_ref()
 ** - GSR_Type_Unref_Function()
 ** - gsr_value_get()
 **/

typedef void       (*GSR_Type_Unref_Function)    (gpointer value);
/**
 **<@brief Unref function of type
 **
 ** @see
 ** - gsr_type_value_unref()
 ** - GSR_Type_Ref_Function()
 ** - gsr_value_get()
 **/

typedef GSR_TYPE_VALIDATE_FUNCTION ((*GSR_Type_Validate_Function));
/**
 **<@brief Validate function of type
 **
 ** Implementation function should return TRUE if 'value' is valid, FALSE otherwise
 ** Validation function is called by gsr_value functions after value of type is already
 ** created.
 **
 ** @param value Value to validate.
 ** @param data  Additional validation data
 **/

typedef gpointer (*GSR_Type_Coerce_Function) (GSR_Type *type, GSR_Type *to, gconstpointer value, gpointer data);
/**
 **/

struct _GSR_Type_Template {
    const char                 *name;
    GSR_Type_Function          *subtype_of;
    GSR_Type_Ref_Function       ref;
    GSR_Type_Unref_Function     unref;
    GSR_Type_Validate_Function  validate;
    gpointer                    validate_data;
};
/**
 **<@brief Describe GSR Type
 **
 ** @var name         stringified type name
 ** @var subtype_of   if this type is restriction of another type
 ** @var ref          type ref function
 ** @var unref        type unref function
 ** @var validate     type value validation function
 ** @var data         data required by validate function
 **/

void            gsr_type_init (
    void);
/**
 **<@brief Initialize GSR_Type global variables.
 **
 ** Function must be called before first usage.
 ** Function detects its state and can be called multiple times.
 ** Function is thread safe and can be called from any thread without any locks.
 ** Function is called by gsr_init().
 **
 ** @see
 ** - gsr_init()
 ** - gsr_cleanup()
 ** - gsr_type_clenaup()
 **/

void            gsr_type_cleanup (
    void);
/**
 **<@brief Clean-up global memory used by GSR_Type.
 **
 ** Subsequent call of any gsr_type_* function is unpredictable and may
 ** lead into invalid memory access.
 **
 ** Function should be called at program exit.
 ** Function detects its state and can be called multiple times.
 ** Function is thread safe and can be called from any thread without any locks.
 ** Function is called by gsr_cleanup().
 **/

GSR_Type *      gsr_type_ref (
    GSR_Type *type);
/**
 **<@brief Atomically increase type handler's reference counter
 **
 ** Function is thread-safe and can be called from any thread.
 ** Function doesn't collide with any other function except gsr_type_unref()
 ** Function is NULL safe.
 **
 ** @param type type handler
 **
 ** @returns its argument
 **/

void            gsr_type_unref (
    GSR_Type *type);
/**
 **<@brief Atomically decrease type handler's reference count
 **
 ** Frees all used memory when reference counter drops to zero.
 **
 ** Function is thread-safe and can be called from any thread.
 ** Function doesn't collide with any other function except gsr_type_ref()
 ** Function is NULL-safe
 **
 ** @param type type handler
 **/

gpointer        gsr_type_value_ref (
    GSR_Type *type,
    gpointer  value);
/**
 **<@brief Call type's ref function on value
 **
 ** If ref function is not registered returns value directly
 ** If ref function is registered and 'data' equals to  NULL
 ** ref function is not called and NULL is returned
 **
 ** @param type type handler
 ** @param value value to ref
 **/

void            gsr_type_value_unref (
    GSR_Type *type,
    gpointer  value);
/**
 **<@brief Call type's unref function on value
 **
 ** Function is not called if 'value' is NULL.
 **
 ** @param type type handler
 ** @param value value to unref
 **/

GSR_Type *      gsr_type_lookup (
    const char *name);
/**
 **<@brief Search for GSR_Type handler identified by string name
 **
 ** Returns NULL if such type is not registered yet
 ** Function is thread safe and can be called from any thread.
 **
 ** @param name type name
 **/

GSR_Type *      gsr_type_registry_into (
    GSR_Type                **ptr,
    const GSR_Type_Template  *template);
/**
 **<@brief Registry new data type
 **
 ** Registry of already existing type (identified by name)
 ** does not modify it if templates are different.
 **
 ** If 'ptr' is not NULL, return value is stored at memory it points to
 ** and it is also registered for cleanup.
 **
 ** Function is thread safe and can be called from any thread.
 ** Function uses atomic access to *ptr.
 **
 ** Returned value is valid until call of gsr_type_cleanup().
 **
 ** @param ptr      if not NULL, return value is also stored in memory pointed by
 ** @param template type template
 **/

gboolean        gsr_type_value_validate (
    GSR_Type      *type,
    gconstpointer  value);
/**
 **<@brief Validate value using type validation function
 **
 ** Returns TRUE if validate function is registered
 ** Calls parent's validate function as well.
 **
 ** @param type verify data of type
 ** @param value value to verify
 **/

void            gsr_type_coerce_add (
    GSR_Type                 *type,
    GSR_Type                 *to,
    GSR_Type_Coerce_Function  coerce,
    gpointer                  data);
/**
 **<@brief Registry new coercion rule
 **
 ** @param type    From type
 ** @param to      produce other type
 ** @param coerce  using coercion function
 ** @param data    with custom configuration
 **/

gboolean        gsr_type_coerce_exists (
    GSR_Type *type,
    GSR_Type *to);
/**
 **<@brief verify whether coercion exists
 **
 ** @param type from type
 ** @param to   to type
 **
 ** @return TRUE / FALSE
 **/

gboolean        gsr_type_coerce_value (
    GSR_Type      *type,
    GSR_Type      *to,
    gconstpointer  value,
    gpointer      *place);
/**
 **<@brief verify whether coercion exists
 **
 ** @param type from type
 ** @param to   to type
 ** @param value value to coerce
 ** @param place where to store coerced value
 **
 ** Returns FALSE if coercion failed, TRUE otherwise
 **/

const char *    gsr_type_name (
    GSR_Type *type);
/**
 **<@brief Get type name
 **/

GSR_TYPE_FUNCTION (gsr_type_gint32);
GSR_TYPE_FUNCTION (gsr_type_gint64);
GSR_TYPE_FUNCTION (gsr_type_gboolean);
GSR_TYPE_FUNCTION (gsr_type_string);
GSR_TYPE_FUNCTION (gsr_type_gstring);

#endif /* header guard */

