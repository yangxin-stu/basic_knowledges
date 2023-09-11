#include <stdio.h>
#include <stdlib.h>

/// @brief the definition of single list
typedef struct Node
{
    int data;
    Node *next;
} Node;

/// @brief insert an element into the head of linked list
/// @param list the linked list
/// @param d the `data` in the Node
void headInsert(Node *list, int d)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->data = d;
    node->next = list->next;
    list->next = node;
}
/// @brief insert an element into the tail of linked list
/// @param list the linked list
/// @param d the `data` in the Node
void tailInsert(Node *list, int d)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->data = d;
    node->next = NULL;
    while (list->next)
        ;
    list->next = node;
}

/// @brief delete the kth element of list. (from 0 index)
/// @param list
/// @param k the index of element
void deleteNode(Node *list, int k)
{
    if (k < 0)
    {
        // signal processing...
        printf("index %d must be greater than 0!", k);
    } // you can judge whether list is a NULL list too.
    Node *pre = list;
    // Node *cur = pre->next; // but wait. what if list is a `NULL` linked list?
    int ind = 0;
    while (ind < k && pre->next != NULL)
    {
        ind++;
        pre = pre->next;
    }
    if (pre->next != NULL && ind == k)
    {
        Node *temp = pre->next;
        pre->next = temp->next;
        free(temp);
    }
}

int main()
{

    return 0;
}