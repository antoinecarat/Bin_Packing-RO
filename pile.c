#include <stdio.h>
#include <stdlib.h>
#include "pile.h"

maillon* creerMaillon(int e){
    maillon* m = malloc(sizeof(maillon));
    m->suiv = NULL;
    m->elt = e;
    
    return m;
}

pile* creerPile(){
    pile *p = malloc(sizeof(pile));
    p->sommet=NULL;
    p->taille=0;
    p->poids=0;
    return p;
}

void empiler(pile *p, int e){
    maillon *m = creerMaillon(e);
    maillon* tmp = p->sommet;
    p->sommet = m;
    p->sommet->suiv = tmp;
    p->taille++;
    p->poids += m->elt;
}

void depiler(pile *p){
    if (p->taille!=0){
        p->poids -= p->sommet->elt;
        p->sommet = p->sommet->suiv;
        p->taille--;
    }
}

void afficherPile(pile *p){
    if (p->taille==0){
        printf("Pile Vide.\n");
    } else {
        maillon *ptr = p->sommet;
        while(ptr != NULL){
            printf("%d\n",ptr->elt);
            ptr = ptr->suiv;
        }
        printf("--\n");
    }
}