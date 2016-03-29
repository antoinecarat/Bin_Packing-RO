#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct arbre arbre;

struct arbre{
    int val;
    arbre *enfant;
    arbre *frere;
    int nbE;
};

arbre* creerArbre(int e){
    arbre* a = malloc(sizeof(arbre));
    a->val = e;
    a->enfant = NULL;
    a->frere = NULL;
    a->nbE=1;
    return a;
}

void ajouterEnfant(arbre *a, int e){
    arbre *ptr = creerArbre(e);
    ptr->frere = a->enfant;
    a->enfant = ptr;
    a->nbE++;
}

void embranche(arbre *a, arbre *b){
    b->frere = a->enfant;
    a->enfant = b;
    a->nbE+b->nbE;
}

void afficherArbre(arbre *a){
    printf("%d\n|\n ",a->val);
    if(a->nbE > 1){
        arbre *ptr=a->enfant;
        while (ptr!=NULL){
            afficherArbre(ptr);
            ptr=ptr->frere;
        }
    }
}


int main(int argc, char **argv)
{
    /* Déclarons 3 listes chaînées de façons différentes mais équivalentes */
   arbre *a = creerArbre(1);
   arbre *b = creerArbre(8);
   ajouterEnfant(a,2);
   ajouterEnfant(a,4);
   ajouterEnfant(b,16);
   embranche(a,b);
   afficherArbre(a);
   return 0;
}
