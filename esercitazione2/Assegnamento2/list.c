#include "list.h"

//Da list.c fornita dal professore
typedef struct 
{
    double value;
}itemType;

//Da list.c fornita dal professore
struct LINKED_LIST_NODE{
    itemType item;
    struct LINKED_LIST_NODE *next;
};

//Da list.c fornita dal professore
NODE* createNode(itemType item) {
    NODE* p = (NODE*)malloc(sizeof(NODE));
    assert(p != NULL);

    p->item = item;
    p->next = NULL;
    return p;
}

LIST EnqueueFirst(LIST l, itemType item){
    NODE* new_node = createNode(item);
    new_node->next = l;
    return l;
}

LIST DequeueLast(LIST l){
    NODE* tmp = l;
    assert(!isEmpty(l));

    while(!isEmpty(tmp->next)){
        tmp = tmp->next;
    }

    tmp->next = 0;

    return l;
}

LIST EnqueueOrdered(LIST l, itemType item){
    return l;
}

//Da list.c fornita dal professore
int itemCompare(itemType item1, itemType item2) {
    if (item1.value > item2.value) /*** esempio ***/
        return 1;
    else if (item1.value < item2.value) /*** esempio ***/
        return -1;
    else
        return 0;
}

itemType* Find(LIST l, itemType item){
    NODE* tmp = l;
    assert(!isEmpty(l));

    while(!isEmpty(tmp->next)){
        if(itemCompare(tmp->item, item)){
            return &(tmp->item);
        }
        tmp = tmp->next;
    }
}