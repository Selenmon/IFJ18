
#include <stdlib.h>
#include <stdio.h>

#include "symtable.h"
#include <string.h>

void BST_symtable_init (tBSTNodePtr *RootPtr){
    if (RootPtr == NULL)
        return;
    *RootPtr = NULL;
}

tBSTNodePtr* BST_check_leafnodes(tBSTNodePtr *RootPtr){
    if (!RootPtr)
        return NULL;
    if(!(*RootPtr)->LPtr && !(*RootPtr)->RPtr)
    {
        return RootPtr;
    }
    if (!(*RootPtr)->LPtr)
    {
        BST_check_leafnodes((tBSTNodePtr*)(*RootPtr)->RPtr);
    }
    if (!(*RootPtr)->RPtr)
    {
        BST_check_leafnodes((tBSTNodePtr*)(*RootPtr)->LPtr);
    }
}

TData *BST_symtable_Insert (tBSTNodePtr *RootPtr, const char *K, bool* alloc_failed){

    *alloc_failed = false;

    if (*RootPtr == NULL || K == NULL)
    {
        *alloc_failed = true;
        return NULL;
    }
    tBSTNodePtr *tmp_last = NULL;
    tBSTNodePtr tmp =*RootPtr;
    tmp_last = BST_check_leafnodes(&tmp);
    tBSTNodePtr *new_item = (tBSTNodePtr *)malloc(sizeof(tBSTNodePtr));
    if (new_item == NULL)
    {
        *alloc_failed = true;
        return NULL;
    }

    if(!((*new_item)->Key = (char)malloc((strlen(K) + 1) * sizeof(char))))
    {
        free (new_item);
        *alloc_failed = true;
        return NULL;
    }

    if (!((*new_item)->data.params = (dynamic_string *)malloc(sizeof(dynamic_string))))
    {
        free(&(*new_item)->Key);
        free(new_item);
        *alloc_failed = true;
        return NULL;
    }
    if (!string_init((*new_item)->data.params))
    {
        free(&(*new_item)->Key);
        free(new_item);
        free((*new_item)->data.params);
        *alloc_failed = true;
        return NULL;
    }
    strcpy(&(*new_item)->Key,K);
    (*new_item)->data.identifier = &(*new_item)->Key;
    (*new_item)->data.type = TYPE_NIL;
    (*new_item)->data.defined = false;
    (*new_item)->data.globalvar = false;
    (*new_item)->RPtr = NULL;
    (*new_item)->LPtr = NULL;

    if (tmp_last == NULL)
        *RootPtr = (*new_item);
    else if ((*tmp_last)->LPtr == NULL)
        (*tmp_last)->RPtr = (struct tBSTNodePtr*) new_item;
    else if ((*tmp_last)->RPtr == NULL)
        (*tmp_last)->LPtr = (struct tBSTNodePtr*)(*new_item);
    return &(*new_item)->data;
}


TData *BST_symtable_Search (tBSTNodePtr *RootPtr,const char *K){
    if(RootPtr != NULL){
        if((*RootPtr)->Key == *K){
            return &(*RootPtr)->data;
        }
        else{
            if(!(*RootPtr)->RPtr){
                return BST_symtable_Search((tBSTNodePtr*)(*RootPtr)->LPtr, K);
            }
            if(!(*RootPtr)->LPtr){
                return BST_symtable_Search((tBSTNodePtr*)(*RootPtr)->RPtr, K);
            }
        }
    }
    else{
        return NULL;
    }
}


void ReplaceByRightmost (tBSTNodePtr *PtrReplaced, tBSTNodePtr *RootPtr){
    if(*RootPtr != NULL)
    {
        if((*RootPtr)->RPtr != NULL)
        {
            ReplaceByRightmost(PtrReplaced, (tBSTNodePtr*)(*RootPtr)->RPtr);
        }
        else
        {
            (*PtrReplaced)->Key = (*RootPtr)->Key;
            (*PtrReplaced)->data = (*RootPtr)->data;
            tBSTNodePtr tmp = *RootPtr;
            (*RootPtr) = (tBSTNodePtr)(*RootPtr)->LPtr;
            free(tmp);
        }
    }
}

bool BST_symtable_add_param(TData *data, int data_type)
{
    if (data == NULL)
        return false;

    switch (data_type)
    {
        case (TYPE_INT):
            if (!string_add(data->params, 'i'))
            {
                return false;
            }
            break;

        case (TYPE_FLOAT):
            if (!string_add(data->params, 'f'))
            {
                return false;
            }
            break;

        case (TYPE_STRING):
            if (!string_add(data->params, 's'))
            {
                return false;
            }
            break;

        default:
            break;
    }

    return true;
}

bool BST_symtable_Delete (tBSTNodePtr *RootPtr,const char *K){
    if(*RootPtr != NULL){
        if((*RootPtr)->Key < *K){
            BST_symtable_Delete((tBSTNodePtr*)(*RootPtr)->RPtr, K);
        }
        else if((*RootPtr)->Key > *K){
            BST_symtable_Delete((tBSTNodePtr*)(*RootPtr)->LPtr, K);
        }
        else{
            tBSTNodePtr tmp = *RootPtr;
            if(tmp->LPtr == NULL){
                *RootPtr = (tBSTNodePtr)tmp->RPtr;
                free(tmp);
            }
            else if(tmp->RPtr == NULL){
                *RootPtr = (tBSTNodePtr)tmp->LPtr;
                free(tmp);
            }
            else{
                ReplaceByRightmost ((tBSTNodePtr*)*RootPtr, (tBSTNodePtr*)(*RootPtr)->LPtr);
            }
        }
        return true;
    }
    else{
        return false;
    }
}


void BST_symtable_Free (tBSTNodePtr *RootPtr){
    if(*RootPtr != NULL)
    {
        BST_symtable_Free((tBSTNodePtr*)(*RootPtr)->LPtr);
        BST_symtable_Free((tBSTNodePtr*)(*RootPtr)->RPtr);
        free(*RootPtr);
        *RootPtr = NULL;
    }
}
