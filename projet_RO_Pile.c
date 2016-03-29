#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct maillon maillon;

struct maillon{
    maillon *suiv;
    int elt;
};

typedef struct{
    maillon *sommet;
    int taille;
} pile;

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
    return p;
}

void empiler(pile *p, int e){
    maillon *m = creerMaillon(e);
    maillon* tmp = p->sommet;
    p->sommet = m;
    p->sommet->suiv = tmp;
    p->taille++;
}

void depiler(pile *p){
    if (p->taille!=0){
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


int main(int argc, char **argv)
{
    /* Déclarons 3 listes chaînées de façons différentes mais équivalentes */
    pile *p=creerPile();
    //p->sommet = NULL;
    afficherPile(p);
    depiler(p);
    afficherPile(p);
    empiler(p, 4);
    afficherPile(p);
    empiler(p, 2);
    afficherPile(p);
    depiler(p);
    afficherPile(p);
    return 0;
}
