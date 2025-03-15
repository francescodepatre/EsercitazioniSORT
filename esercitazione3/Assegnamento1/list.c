#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"

/************* Funzioni locali *********************************/

/* Confronta due elementi della lista:
   - ritorna un valore >0 se item1 > item2;
   - ritorna un valore <0 se item1 < item2;
   - ritorna un valore ==0 se item1 == item2.
  NB: puo' essere utilizzata nelle funzioni di ricerca e/o ordinamento
*/
int itemCompare(int id1, int id2) {
    if (id1 == id2) /*** esempio ***/
        return 1;
    else
        return 0;
}

/* alloca un nuovo nodo per la lista, copiandone all'interno l'elemento item */
NODE* createNode(ItemType item) {
    NODE* p = (NODE*)malloc(sizeof(NODE));
    assert(p != NULL);

    p->item = item;
    p->next = NULL;
    return p;
}

/* dealloca il nodo p */
void deleteNode(NODE* p) {
    free(p);
}

/********** Funzioni standard *******************************/

/* Inizializza una nuova lista */
LIST NewList() {
    return NULL;
}

/* Azzera la lista */
LIST DeleteList(LIST l) {
    LIST tmp = l;

    /* dealloca tutti i nodi della lista */
    while (!isEmpty(tmp)) {
        NODE* todel = tmp;
        tmp = tmp->next;
        deleteNode(todel);
    }

    /* la lista e' ora vuota, e puo' essere utilizzata di nuovo */
    return NewList();
}

/* ritorna TRUE se la lista e' vuota */
BOOL isEmpty(LIST l) {
    return l == NULL;
}

/* ritorna il numero di elementi presenti nella lista */
int getLength(LIST l) {
    int size = 0;
    LIST tmp = l;

    while (!isEmpty(tmp)) {
        ++size;
        tmp = tmp->next;
    }

    return size;
}

/* Restituisce l'elemento in testa alla lista, senza modificarla */
ItemType getHead(LIST l) {
    assert(!isEmpty(l));
    return l->item;
}

/* Restituisce l'elemento in coda alla lista, senza modificarla */
ItemType getTail(LIST l) {
    NODE* tmp = l;
    assert(!isEmpty(l));

    while (!isEmpty(tmp->next))
        tmp = tmp->next;

    return tmp->item;
}

/* Cerca l'elemento item nella lista e ne restituisce il puntatore
 ritorna NULL se non lo trova */
ItemType* Find(LIST l, int id) {
    LIST tmp = l;
    /*if (tmp == NULL) {
      return NULL;
    }*/
    while (tmp != NULL && itemCompare(tmp->item.playerId, id) != 0) {
        tmp = tmp->next;
    }
    if (tmp == NULL) {
        return NULL;
    }
    return &(tmp->item);
}

/* Inserisce un elemento nella prima posizione della lista */
LIST EnqueueFirst(LIST l, ItemType item) {
    assert(FALSE);

    /* TODO */

    return l;
}

/* Inserisce un elemento nell'ultima posizione della lista */
LIST EnqueueLast(LIST l, ItemType item) {
    NODE* new_node = createNode(item);

    if (isEmpty(l)) {
        /* Lista vuota: inserimento in testa */
        l = new_node;
    } else {
        LIST tmp = l;
        while (!isEmpty(tmp->next))
            tmp = tmp->next;

        tmp->next = new_node;
    }

    return l;
}

/* Inserisce un elemento mantenendo la lista ordinata */
LIST EnqueueOrdered(LIST l, ItemType item) {
    assert(FALSE);
    
    /* TODO */

    return l;
}

/* Toglie il primo elemento della lista (se non e' vuota) */
LIST DequeueFirst(LIST l) {
    if (!isEmpty(l)) {
        NODE* todel = l;
        l = l->next;
        deleteNode(todel);
    }
    return l;
}

/* Toglie l'ultimo elemento della lista (se non e' vuota) */
LIST DequeueLast(LIST l) {
    if (!isEmpty(l)) {
        assert(FALSE);

        /* TODO */
    }

    return l;
}

/* Toglie l'elemento item dalla lista (se e' presente) */
LIST Dequeue(LIST l, ItemType item) {
    if (!isEmpty(l)) {
        if (itemCompare(l->item.playerId, item.playerId) == 0) {
            /* eliminazione dell'elemento in testa */
            NODE* todel = l;
            l = l->next;
            deleteNode(todel);
        } else {
            LIST tmp = l;

            while (!isEmpty(tmp->next) &&
                   itemCompare(tmp->next->item.playerId, item.playerId) != 0)
                tmp = tmp->next;

            if (!isEmpty(tmp->next)) {
                /* l'elemento e' stato trovato nella lista (viene eliminato) */
                NODE* todel = tmp->next;
                tmp->next = tmp->next->next;
                deleteNode(todel);
            }
        }
    }
    return l;
}

/* Stampa a video un elemento della lista */
void PrintItem(ItemType item) {
    /*** esempio ***/
    printf("ID: %d \nOverall score: %d \nPlayed matches: %d \n", item.playerId,item.playerScore,item.playerMatch);
}

/* Stampa a video la lista (esegue PrintItem() su tutti gli elementi) */
void PrintList(LIST l) {
    LIST tmp = l;
    while (!isEmpty(tmp)) {
        PrintItem(tmp->item);
        tmp = tmp->next;
    }
}

void shufflePoints(int *pointsArray, int PointSize) {
    for (int i = PointSize - 1; i > 0; i--) {
        int j = rand() % (i + 1); 
        int temp = pointsArray[i];
        pointsArray[i] = pointsArray[j];
        pointsArray[j] = temp;
    }
}

void assignPoints(LIST l, int *points){
    LIST tmp = l;
    for(int i = 0; i < 3; i ++){
        tmp->item.playerMatch += 1;
        tmp->item.playerScore += points[i];
        printf("%d Points assigned to player %d\n", points[i], tmp->item.playerId);
        tmp = tmp->next;
    }
}
