#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* ---------- Structure Pile ---------- */

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
 /* --------------------------------- */


typedef struct {
	int t; // Taille de l'objet
	int nb; // Nombre d'objets de la même taille 
} objets;

typedef struct {
	int T; // Taille du bin 
	int nb; // Nombre de tailles différentes pour les objets
	objets *tab; // Tableau des objets (taille + nombre dans une même taille) 
} donnees;

typedef struct motifs motifs;

struct motifs {
    motifs *suiv;
    motifs *prec;
    objets *tab;//de taille donnes->nb
    int taille;
};

//Liste chainee pour stoker les motifs
typedef struct{
    motifs* tete;
    motifs* queue;
    int nbMotifs;
} listeMotifs;


listeMotifs lMotifs;

listeMotifs* creerListeMotifs(){
    listeMotifs *l = malloc(sizeof(listeMotifs));
    l->tete=NULL;
    l->queue=NULL;
    l->nbMotifs=0;
}

void afficherMotifs(motifs *m, int nb){
    if (m->taille==0){
        printf("Motif Vide._n");
    } else {
        printf("============\nTAILLE : %d\n",m->taille);
        for (int i=0; i<nb; ++i){
            printf("%d :: %d\n",m->tab[i].t,m->tab[i].nb);
        }
        printf("============\n");
    }
}

void afficherListeMotifs(listeMotifs *l, int nb){
    if (l->nbMotifs==0){
        printf("ListeMotifs Vide.\n");
    } else {
        motifs *ptr = l->tete;
        while(ptr != NULL){
            afficherMotifs(ptr, nb);
            ptr = ptr->suiv;
        }
        printf("\n");
    }
}

void lecture_data(char *file, donnees *p)
{
	
	FILE *fin; // Pointeur sur un fichier
	int i;	
	int val;
	
	fin = fopen(file,"r"); // Ouverture du fichier en lecture
	
	/* Première ligne du fichier, on lit la taille du bin, et le nombre de tailles différentes pour les objets à ranger */
	
	fscanf(fin,"%d",&val);
	p->T = val;
	fscanf(fin,"%d",&val);
	p->nb = val;
	
	/* On peut maintenant faire l'allocation dynamique concernant le nombre de tailles différentes d'objets */
	
	p->tab = (objets *) malloc (p->nb * sizeof(objets));
	
	/* On lit ensuite les infos sur les (taille d'objets + nombres) */
	
	for(i = 0;i < p->nb;i++) // Pour chaque format d'objet...
	{
		// ... on lit les informations correspondantes
		fscanf(fin,"%d",&val);
		(p->tab[i]).t = val;
		fscanf(fin,"%d",&val);
		(p->tab[i]).nb = val;
	}
	
	fclose(fin); // Fermeture du fichier
}

motifs* creerMotif(pile *p, donnees *d){
    motifs *m = malloc(sizeof(motifs));
    m->prec = NULL;
    m->suiv=NULL;
    m->tab = malloc (d->nb*sizeof(objets));
    m->taille = 0;
    
    //Initialisation
    for (int i=0; i<d->nb; ++i){
        m->tab[i].t=d->tab[i].t;
        m->tab[i].nb=0;
    }
    
    //On converti le contenu de la pile en motif
    maillon *ptr = p->sommet;
    for(int j=d->nb; j>0 ; --j){
        while (ptr->elt == d->tab[j-1].t){
            m->tab[j-1].nb++;
            m->taille += m->tab[j-1].t;
            if (ptr->suiv != NULL){
                ptr = ptr->suiv;
            } else {
                break;
            }
        }
    }
    
    
    return m;
}

void ajouterMotif(motifs *m){
    if(lMotifs.nbMotifs>0){
        lMotifs.queue->suiv = m;
        m->prec = lMotifs.queue;
        lMotifs.queue = m;
    } else {
        lMotifs.queue = m;
        lMotifs.tete = lMotifs.queue;
    }
    lMotifs.nbMotifs++;//On incrémente nbMotifs
}

void enumeration_Motifs(donnees *d, pile *pile, int start){
    for (int i=start; i<d->nb; ++i){
        if (d->T >= pile->poids + d->tab[i].t){
            empiler(pile, d->tab[i].t);
            enumeration_Motifs(d, pile, i);
        }
    }
    if(pile->poids + d->tab[d->nb-1].t > d->T){
        motifs *m = creerMotif(pile, d);
        ajouterMotif(m);
    }
    depiler(pile);
}

int main(int argc, char **argv)
{
    pile *p=creerPile();
    donnees d; 
	
	/* autres déclarations 
		.
		.
		.
	 */ 
	
	
	/* Chargement des données à partir d'un fichier */
	
	lecture_data(argv[1],&d);
	
	enumeration_Motifs(&d,p,0);
    afficherListeMotifs(&lMotifs,d.nb);
    return 0;
}
