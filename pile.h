#ifndef PILE_H
#define PILE_H

typedef struct maillon maillon;

struct maillon{
    maillon *suiv;
    int elt;
};

typedef struct{
    maillon *sommet;
    int taille;
    int poids;
} pile;

maillon* creerMaillon(int e);

pile* creerPile();

void empiler(pile *p, int e);

void depiler(pile *p);

void afficherPile(pile *p);

#endif