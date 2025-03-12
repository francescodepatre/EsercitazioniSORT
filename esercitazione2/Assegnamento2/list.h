typedef struct LINKED_LIST_NODE NODE;
typedef struct NODE *LIST;

LIST EnqueueFirst(LIST l, itemType item);

LIST DequeueLast(LIST l);

LIST EnqueueOrdered(LIST l, itemType item);

itemType* Find(LIST l, itemType itemType);
