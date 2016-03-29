#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct maillon maillon;

struct maillon{
    maillon *prec;
    maillon *suiv;
    int elt;
};

typedef struct{
    maillon *tete;
    maillon *queue;
    int taille;
} liste;

liste* creerListe(){
    liste* l = malloc(sizeof(liste));
    l->tete = NULL;
    l->queue = NULL;
    l->taille = 0;
    return l;
}

maillon* creerMaillon(int e){
    maillon* m = malloc(sizeof(maillon));
    m->prec = NULL;
    m->suiv = NULL;
    m->elt = e;
    
    return m;
}

void ajoutEnDebut(liste *l, int e){
    maillon *m = creerMaillon(e);
    maillon* tmp = l->tete;
    m->prec = NULL;
    m->suiv = tmp;
    l->tete = m;
    if(l->taille==0){
        l->queue = l->tete;
    }else{
        l->queue = tmp;   
    }
    l->taille++;
}

void ajoutEnFin(liste *l, int e){
    maillon *m = creerMaillon(e);
    m->prec = l->queue;
    m->suiv = NULL;
    l->queue = m;
    if(l->taille==0){
        l->tete = l->queue;
    }
    l->taille++;
}

void afficherListe(liste *l){
    
    if (l->taille==0){
        printf("Liste Vide.\n");
    } else {
        maillon *ptr = l->tete;
        while(ptr != NULL){
            printf("%d ;",ptr->elt);
            ptr = ptr->suiv;
        }
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    /* Déclarons 3 listes chaînées de façons différentes mais équivalentes */
    liste *l = creerListe();
    afficherListe(l);
    ajoutEnDebut(l, 4);
    afficherListe(l);
    ajoutEnDebut(l, 2);
    afficherListe(l);
    ajoutEnFin(l, 8);
    afficherListe(l);
 
    return 0;
}
