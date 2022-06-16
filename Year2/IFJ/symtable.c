/**
 * @file symtable.c
 * @authors Marek Micek
 * @date 24 OCT 2020
 * @brief Implementation of the table of symbols
 */

#include "symtable.h"

// extern array to remove cyclic dependency, not used in symtable
char *data_types[] = 
{
    "T_UNDEFINED",
    "T_INT",
    "T_STRING",
    "T_FLOAT64",
    "T_NIL"
};


TNode* init_symtable(TNode *root)
{
    root = NULL;
    return root;
}


TNode* search_symtable(TNode *root, char *k)
{
    if (root == NULL)
    {
        return NULL;
    }
    else if (!strcmp(root->key, k))
    {
        return root;
    }
    else if (strcmp(root->key, k) > 0)
    {
        return (search_symtable(root->lptr, k));    //< searching left side
    }
    else
    {
        return (search_symtable(root->rptr, k));    //< searching right side
    }
}

/**
 * @brief Supportive function which rewrites all data when incoming key is same as existing one
 *        Function is called also when creating new node
 * @return Node with rewrote data
 */
TNode *rewrite_data(TNode *node, TData new_data)
{
    node->data.type = new_data.type;
    node->data.defined = new_data.defined;
    node->data.is_var = new_data.is_var;
    node->data.is_function = new_data.is_function;
    node->data.in_block = new_data.in_block;
    node->data.param_counter = new_data.param_counter;
    node->data.ret_counter = new_data.ret_counter;
    node->data.line = new_data.line;
    node->data.id_counter = new_data.id_counter;
    node->data.is_param = new_data.is_param;
    node->data.param_num = new_data.param_num;

    if (node->data.is_function == true)
    {
        for (unsigned i = 0; i < new_data.ret_counter; i++)
            node->data.retval_arr[i] = new_data.retval_arr[i];

        for (unsigned j = 0; j < new_data.param_counter; j++)
            node->data.arg_arr[j] = new_data.arg_arr[j];
        
        for (unsigned k = 0; k < new_data.id_counter; k++)
            node->data.id_type[k] = new_data.id_type[k];
    }

    return node;
}


TNode* create_node(TData d, char *k)
{
    TNode *new_node = malloc(sizeof(TNode));
    new_node->key = malloc(sizeof(char)*(strlen(k)+1));
    if (new_node == NULL || new_node->key == NULL)
    {
        intern_error();
        return NULL;
    }

    strcpy(new_node->key, k);
    new_node->lptr = NULL;
    new_node->rptr = NULL;
    return (rewrite_data(new_node, d));

}


TNode* insert_symtable(TNode *root, TData d, char *k)
{
    if (root == NULL)   //< create new node cause key was not found
    {
         return create_node(d, k);
    }

    if (strcmp(root->key, k) > 0)
    {
        root->lptr = insert_symtable(root->lptr, d, k);     //< trying insert to left child
    }
    else if (strcmp(root->key, k) < 0)
    {
        root->rptr = insert_symtable(root->rptr, d, k);     //< trying insert to right child
    }
    else
    {
        root = rewrite_data(root, d);     //< key was found, rewrite data
    }
    return root;
}


void delete_symtable(TNode *root)
{
    if (root != NULL)
    {
        delete_symtable(root->lptr);
        delete_symtable(root->rptr);
        free(root->key);            //< must free heap
        free(root);
    }
    root = NULL;
}


TNode* most_left_node(TNode *root)
{
    if (root->lptr == NULL) //< the next left child does not exist
    {
        return root;
    }
    else
    {
        return (most_left_node(root->lptr));
    }
}


TNode *delete_node(TNode *root, char *k)
{
    if (root == NULL)
    {
        return NULL;
    }
    else if (strcmp(root->key, k) > 0)     //< searching key is on the left
    {
        root->lptr = delete_node(root->lptr, k);
        return root;
    }
    else if (strcmp(root->key, k) < 0)     //< searching key is on the right
    {
        root->rptr = delete_node(root->rptr, k);
        return root;
    }
    else                        //< key was found
    {
        if (root->lptr == NULL && root->rptr == NULL)   //< in case node has no children
        {
            free(root->key);    //< must free heap
            free(root);
            return NULL;
        }
        else if (root->lptr != NULL && root->rptr != NULL)  //< deleted node has 2 children
        {
            TNode *min = most_left_node(root->rptr);
            root->key = (char *) realloc(root->key, sizeof(strlen(min->key)+1));
            strcpy(root->key, min->key);
            root = rewrite_data(root, min->data);
            root->rptr = delete_node(root->rptr, min->key);
            return root;

        }
        else    //< deleted node has one child
        {
            TNode *only_child;
            if (root->lptr == NULL)     //< there is no left child
            {
                only_child = root->rptr;
            }
            else                        //< there is no right child
            {
                only_child = root->lptr;
            }
            free(root->key);
            free(root);
            return only_child;
        }
    }
}

