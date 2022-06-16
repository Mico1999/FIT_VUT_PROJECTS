/**
 * @file symtable-test.c
 * @authors Marek Micek, ...
 * @date 26 OCT 2020
 * @brief Test unit for table of symbols
 */

#include "symtable.h"


TNode *node, *actual_node;
TData data_1, data_2, data_3, data_4, data_5;
bool correct = 1; 

void print_inorder(TNode *node)
{
    if (node)
    {
        print_inorder(node->lptr);
        printf("%s ", node->key);
        print_inorder(node->rptr);
    }
}
 
int main() {

char *key1 = "ahojY";
char *key2 = "ahojZ";
char *key3 = "ahojW";
char *key4 = "ahojX";
char *key5 = "ahojV";


/****** first data unit ******/
data_1.type = T_INT;
data_1.defined = false;
data_1.is_var = false;
data_1.is_function = false;
data_1.param_counter = 0;

/***** second data unit ******/
data_2.type = T_INT;
data_2.defined = false;
data_2.is_var = false;
data_2.is_function = true;
data_2.param_counter = 1;

/***** third data unit ******/
data_3.type = T_FLOAT64;
data_3.defined = true;
data_3.is_var = false;
data_3.is_function = false;
data_3.param_counter = 0;

/***** fourth data unit ******/
data_4.type = T_STRING;
data_4.defined = true;
data_4.is_var = false;
data_4.is_function = false;
data_4.param_counter = 0;

/***** fifth data unit ******/
data_5.type = T_UNDEFINED;
data_5.defined = false;
data_5.is_var = false;
data_5.is_function = false;
data_5.param_counter = 0;


node = init_symtable(node);
node = insert_symtable(node, data_1, key1);    //< inserting first node == root
actual_node = insert_symtable(node, data_2, key3);
actual_node = insert_symtable(node, data_3, key2);
actual_node = insert_symtable(node, data_4, key4);
actual_node = insert_symtable(node, data_5, key5);


printf("Expected inorder output = ahojV ahojW ahojX ahojY ahojZ\n");
printf("Our inorder output : ");
print_inorder(node);
printf("\n***********************************************\n");

printf("Search table test 1: searching existing key\n");

actual_node = search_symtable(node, key2);
if (strcmp(actual_node->key, key2) != 0)    //< we should find node
{
    fprintf(stderr, "Keys not equal in node and in passing data!\n");
    correct = false;
}
if (actual_node->data.type != T_FLOAT64)
{
    fprintf(stderr, "Data type not same in node and in passing data!\n");
    correct = false;
}
if (actual_node->data.defined != true || actual_node->data.is_var != false || actual_node->data.is_function != false)
{
    fprintf(stderr, "One of boolean variables not same in node and in passing data!\n");
    correct = false;
}
if (actual_node->data.param_counter != 0)
{
    fprintf(stderr, "Param counter not same in node and in passing data!\n");
    correct = false;
}
if (correct)
{
    printf("Searching test 1 passed!!!\n");
}

printf("***********************************************\n");
printf("Search table test 2: searching not existing key\n");

actual_node = search_symtable(node, "NOT");        //< we should not find any node
if (actual_node != NULL)
{
    fprintf(stderr, "Search table error occured while searching not existing key!\n");
}
else
{
    printf("Searching test 2 passed!!!\n");
}

printf("***********************************************\n");
correct = true; //< in case it was set to false in first test unit

/* we are going to insert new data with same key*/
data_1.type = T_FLOAT64;
data_1.defined = true;
data_1.is_var = true;
data_1.is_function = true;
data_1.param_counter = 1;

insert_symtable(node, data_1, key5);           //<node with key5 should be overwrote with new data
actual_node = search_symtable(node, key5);     //< we check if data were overwrote

if (strcmp(actual_node->key, key5) != 0)                          //< we should find node
{
    fprintf(stderr, "Insert table error occured while inserting existing key!\n");
    correct = false;
}
if (actual_node->data.type != T_FLOAT64)
{
    fprintf(stderr, "Data type not same in node and in passing data!\n");
    correct = false;
}
if (actual_node->data.defined != true || actual_node->data.is_var != true || actual_node->data.is_function != true)
{
    fprintf(stderr, "One of boolean variables not same in node and in passing data!\n");
    correct = false;
}
if (actual_node->data.param_counter != 1)
{
    fprintf(stderr, "Param counter not same in node and in passing data!\n");
    correct = false;
}
if (correct)
{
    printf("Insertion of data with existing key passed!!!\n");
}

printf("***********************************************\n");
correct = true; //< in case it was set to false in last test unit

/* we gonna to delete one node with existing key without children */ 
delete_node(node, key2);
actual_node = search_symtable(node, key2);
if (actual_node != NULL)
{
    fprintf(stderr, "Failed to delete node with no children!\n");
}
else
{
    printf("Node with no child was deleted successfully!\n");
}


/* we gonna to delete one node with existing key with both children */
delete_node(node, key3);
actual_node = search_symtable(node, key3); //< node not exists, is overwrote by node witk key4 
if (actual_node != NULL)
{
    fprintf(stderr, "Failed to delete node with two children\n");
    correct = false;
}
actual_node =search_symtable(node, key4);
if (strcmp(actual_node->lptr->key, key5) != 0)    //< key4 now points to key5
{
    fprintf(stderr, "Failed to delete node with two children\n");
    correct = false;
}
if (correct)
{
    printf("Node with both children deleted successfully!\n");
}

correct = true; //< in case it was set to false in last test unit

/* we gonna to delete one node with existing key with one child */
delete_node(node, key4);
actual_node = search_symtable(node, key4); //< node six is deleted now
if (actual_node != NULL)
{
    fprintf(stderr, "Failed to delete node with one child!\n");
    correct = false;
}
if (strcmp(node->lptr->key, key5) != 0)   //< root must point to node 3 now
{
   fprintf(stderr, "Failed to delete node with one child!\n");
   correct = false; 
}
if (correct)
{
    printf("Node with one child was deleted successfully!\n");
}

printf("***********************************************\n");
printf("End of test unit\n");

delete_symtable(node);

return 0;
}