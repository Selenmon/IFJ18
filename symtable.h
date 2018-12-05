
#ifndef  _BSTSYMTABLE_H
#define  _BSTSYMTABLE_H

#include <stdbool.h>
#include "dynamic_string.h"

typedef enum {
    TYPE_NIL,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
}Data_Type ;

typedef struct {
    Data_Type type;
    bool defined;
    dynamic_string *params;
    char *identifier;
    bool globalvar;
}TData ;

typedef struct {
    char Key;
    TData data;
    struct tBSTNodePtr * LPtr;
    struct tBSTNodePtr * RPtr;
} *tBSTNodePtr ;

/*functions*/

/**
 * @brief  Function initializes BStree
 *
 * @param tBSTNodePtr pointer to root of tree
 */
void BST_symtable_init (tBSTNodePtr *);

/**
 * @brief Function returns leaf node
 *
 * @param RootPtr pointer to root of tree
 * @return pointer to leaf node
 */

tBSTNodePtr* BST_check_leafnodes(tBSTNodePtr *RootPtr);

/**
 * @brief  Function finds symbol and returns its data node
 *
 * @param tBSTNodePtr pointer to root of tree
 * @param K Node Identifier
 * @return NULL if symbol doesnt exist or pointer to Data node
 */
TData *BST_symtable_Search (tBSTNodePtr *,const char *K);

/**
 * @brief Function Appends symbol to the BStree
 *
 * @param tBSTNodePtr pointer to root of tree
 * @param K Node Identifier
 * @param alloc_failed true if allocation failed
 * @return NULL if error or item is existing otherwise pointer to added item.
 */
TData* BST_symtable_Insert (tBSTNodePtr *,const char *K, bool*alloc_failed);

/**
 * @brief Function appends parameter to symbol
 *
 * @param token Pointer to output token
 * @param data_type Data type of parameter
 * @return true if successful false otherwise
 */
bool BST_symtable_add_param(TData *data, int data_type);

/**
 * @brief  Function Deletes symbol node from BStree
 * @param tBSTNodePtr pointer to root of tree
 * @param K Node Identifier
 * @return 1 if succesful 0 if not
 */
bool BST_symtable_Delete (tBSTNodePtr *, const char *K);

/**
 * @brief  Function frees all resources allocated for BStree
 *
 * @param tBSTNodePtr pointer to root of tree
 */
void BST_symtable_Free (tBSTNodePtr *);

#endif //_BSTSYMTABLE_H



