#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <glpk.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "pile.h"

struct timeval start_utime, stop_utime;

typedef struct {
	int t; // Taille de l'objet
	int nb; // Nombre d'objets de la même taille 
} objets;

typedef struct {
	int T; // Taille du bin 
	int nb; // Nombre de tailles différentes pour les objets
	objets *tab; // Tableau des objets (taille + nombre dans une même taille) 
} donnees;

typedef struct {
	int nbvar; 
	int nbcontr; 
	int *coeff;  
	int **contr; 
	int *sizeContr; 
	int *droite;
} probleme;

typedef struct motifs motifs;

struct motifs {
    motifs *suiv;
    motifs *prec;
    objets *tab;
    int taille;
};

//Liste chainee pour stoker les motifs
typedef struct{
    motifs* tete;
    motifs* queue;
    int nbMotifs;
} listeMotifs;

listeMotifs *lMotifs;

/**
 * Prodecure crono_start : Demarre le chrono
*/
void crono_start()
{
	struct rusage rusage;
	
	getrusage(RUSAGE_SELF, &rusage);
	start_utime = rusage.ru_utime;
}

/**
 * Prodecure crono_stop : Arrete le chrono
*/
void crono_stop()
{
	struct rusage rusage;
	
	getrusage(RUSAGE_SELF, &rusage);
	stop_utime = rusage.ru_utime;
}

/**
 * Prodecure crono_ms
*/
double crono_ms()
{
	return (stop_utime.tv_sec - start_utime.tv_sec) * 1000 +
    (stop_utime.tv_usec - start_utime.tv_usec) / 1000 ;
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
	
	for(i = 0;i < p->nb;++i) // Pour chaque format d'objet...
	{
		// ... on lit les informations correspondantes
		fscanf(fin,"%d",&val);
		(p->tab[i]).t = val;
		fscanf(fin,"%d",&val);
		(p->tab[i]).nb = val;
	}
	
	fclose(fin); // Fermeture du fichier
}

void fusion(objets *tab, int d1, int f1, int d2, int f2){
	objets *tmp;
	int cmp1=d1;
	int cmp2=d2;

	tmp=malloc((f1-d1+1)*sizeof(objets));

	for(int i=d1;i<=f1;++i){
	    tmp[i-d1].t=tab[i].t;
	    tmp[i-d1].nb=tab[i].nb;
	}    
	for(int i=d1;i<=f2;++i){
	    if (cmp1==d2){
	        break;
	    }
	    else if ((cmp2==(f2+1)) || (tmp[cmp1-d1].t>tab[cmp2].t)){
	        tab[i].t=tmp[cmp1-d1].t;
	        tab[i].nb=tmp[cmp1-d1].nb;
	        cmp1++;
	    }
	    else{
	        tab[i].t=tab[cmp2].t;
	        tab[i].nb=tab[cmp2].nb;
	        cmp2++;
	    }
	}
	free(tmp);
}

void triFusion_rec(objets *tab, int d, int f){
	int pivot = ((f+d)/2);
	if (d != f) {
		triFusion_rec(tab,d,pivot);
		triFusion_rec(tab,pivot+1,f);
		fusion(tab,d,pivot,pivot+1,f);
	}
}

void triFusion(objets *tab, int nb){
	int pivot = (nb/2) - 1;

	triFusion_rec(tab, 0, pivot);
	triFusion_rec(tab, pivot+1, nb-1);
	fusion(tab, 0, pivot, pivot+1, nb-1);
}


listeMotifs* creerListeMotifs(){
    listeMotifs *l = malloc(sizeof(listeMotifs));
    l->tete=NULL;
    l->queue=NULL;
    l->nbMotifs=0;
    
    return l;
}

motifs* convertirMotif(pile *p, donnees *d){
    motifs *m = malloc(sizeof(motifs));
    m->prec = NULL;
    m->suiv=NULL;
    m->tab = malloc (d->nb*sizeof(objets));
    m->taille = 0;

    for (int i=0; i<d->nb; ++i){
        m->tab[i].t=d->tab[i].t;
        m->tab[i].nb=0;
    }

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

motifs creerMotif(donnees *d){
    motifs m;
    m.prec = NULL;
    m.suiv=NULL;
    m.tab = malloc (d->nb*sizeof(objets));
    m.taille = 0;

    for (int i=0; i<d->nb; ++i){
        m.tab[i].t=d->tab[i].t;
        m.tab[i].nb=0;
    }

    return m;
}

void ajouterMotif(motifs *m, listeMotifs *l){
    if(l->nbMotifs>0){
        l->queue->suiv = m;
        m->prec = l->queue;
        l->queue = m;
    } else {
        l->queue = m;
        l->tete = l->queue;
    }
    l->nbMotifs++;
}

void afficherDonnees(donnees *d){
    printf("Taille des bins : %d\n",d->T);
    printf("%d Pieces :\n",d->nb);
    for(int i=0; i<d->nb; ++i){
        printf("%d :: %d\n",d->tab[i].t,d->tab[i].nb);
    }
    printf("\n");
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
        printf("%d Motifs :\n",l->nbMotifs);
        motifs *ptr = l->tete;
        while(ptr != NULL){
            afficherMotifs(ptr, nb);
            ptr = ptr->suiv;
        }
        printf("\n");
    }
}

void enumerationMotifs(donnees *d, pile *pile, int start){
    for (int i=start; i<d->nb; ++i){
        if (d->T >= pile->poids + d->tab[i].t){
            empiler(pile, d->tab[i].t);
            enumerationMotifs(d, pile, i);
        }
    }
    if(pile->poids + d->tab[d->nb-1].t > d->T){
        motifs *m = convertirMotif(pile, d);
        ajouterMotif(m,lMotifs);
    }
    depiler(pile);
}

void chargerProbleme(donnees *d, probleme *p){
	p->nbvar = lMotifs->nbMotifs;
	p->nbcontr = d->nb;
		
	p->coeff = (int *) malloc (p->nbvar * sizeof(int));
	p->droite = (int *) malloc (p->nbcontr * sizeof(int));
	p->sizeContr = (int *) malloc (p->nbcontr * sizeof(int));
	p->contr = (int **) malloc (p->nbcontr * sizeof(int *));
	
	for(int i = 0;i < p->nbvar;++i){
			p->coeff[i] = 1;
	}

	for(int i = 0; i < p->nbcontr; ++i){
		p->sizeContr[i] = p->nbvar;
		p->contr[i] = (int *) malloc (p->sizeContr[i] * sizeof(int));
		
		for(int j = 0; j < p->sizeContr[i];++j){
			p->contr[i][j] = j+1;
		}
		p->droite[i] = d->tab[i].nb; 
	}
}

/*void resoudreGLPK(probleme *p){
	glp_prob *prob;
	int *ia;
	int *ja;
	double *ar;
	
	char **nomcontr;
	char **numero;	
	char **nomvar; 

    int z; 		 
	double *x; 
	
	int nbcreux;
	
	//Transfert de ces données dans les structures utilisées par la bibliothèque GLPK

	prob = glp_create_prob(); // allocation mémoire pour le problème  
	glp_set_prob_name(prob, "Bin-Packing"); //affectation d'un nom (on pourrait mettre NULL) 
	glp_set_obj_dir(prob, GLP_MIN); // Il s'agit d'un problème de minimisation, on utiliserait la constante GLP_MAX dans le cas contraire
	
	// Déclaration du nombre de contraintes (nombre de lignes de la matrice des contraintes) : p->nbcontr
	glp_add_rows(prob, p->nbcontr); 
	nomcontr = (char **) malloc (p->nbcontr * sizeof(char *));
	numero = (char **) malloc (p->nbcontr * sizeof(char *)); 

	for(int i=1;i<=p->nbcontr;++i){
		nomcontr[i - 1] = (char *) malloc (8 * sizeof(char)); 
		numero[i - 1] = (char *) malloc (3  * sizeof(char));
		strcpy(nomcontr[i-1], "Contrainte piece");
		sprintf(numero[i-1], "%d", i);
		strcat(nomcontr[i-1], numero[i-1]); 	
		glp_set_row_name(prob, i, nomcontr[i-1]);
		
		glp_set_row_bnds(prob, i, GLP_LO, p->droite[i-1], 0.0); 
	}	

	glp_add_cols(prob, p->nbvar); 
	nomvar = (char **) malloc (p->nbvar * sizeof(char *));
	for(int i=1;i<=p->nbvar;++i){
		nomvar[i - 1] = (char *) malloc (3 * sizeof(char));
		sprintf(nomvar[i-1],"m%d",i-1);
		glp_set_col_name(prob, i , nomvar[i-1]);

		glp_set_col_bnds(prob, i, GLP_LO,0.0, 0.0);
		glp_set_col_kind(prob, i, GLP_IV);
	}

	for(int i = 1;i <= p->nbvar;++i){
	    glp_set_obj_coef(prob,i,p->coeff[i - 1]);
	}
	
	nbcreux = 0;
	for(int i = 0;i < p->nbcontr;++i){
		nbcreux += p->sizeContr[i];
	}
	
	ia = (int *) malloc ((1 + nbcreux) * sizeof(int));
	ja = (int *) malloc ((1 + nbcreux) * sizeof(int));
	ar = (double *) malloc ((1 + nbcreux) * sizeof(double));
	
	motifs *m;
	int pos=1;
	for(int i = 0;i < p->nbcontr;++i){
		m = lMotifs->tete;
		for(int j = 0;j < p->sizeContr[i];++j){
			ia[pos] = i + 1;
			ja[pos] = p->contr[i][j];
			ar[pos] = m->tab[i].nb;
			pos++;
			if (j+1<p->sizeContr[i]){
				m = m->suiv;
			}
		}
	}
	
	glp_load_matrix(prob,nbcreux,ia,ja,ar); 
	glp_write_lp(prob,NULL,"projet_CARAT.lp");

    // Résolution
	glp_simplex(prob,NULL);	glp_intopt(prob,NULL);
	z = glp_mip_obj_val(prob);
	x = (double *) malloc (p->nbvar * sizeof(double));
	for(int i = 0;i < p->nbvar; ++i) x[i] = glp_mip_col_val(prob,i+1); // Récupération de la valeur des variables, Appel différent dans le cas d'un problème en variables continues : for(i = 0;i < p->nbvar;++i) x[i] = glp_get_col_prim(prob,i+1);

	printf("\nz = %d\n\n",z);
	for(int i = 0;i < p->nbvar;++i) printf("x%d = %d, ",i,(int)(x[i] + 0.5)); // un cast est ajouté, x[i] pourrait être égal à 0.99999...
	puts("");

	glp_delete_prob(prob); 
	free(p->coeff);
	free(p->sizeContr);
	free(p->droite);
	for(int i = 0;i < p->nbcontr;++i) free(p->contr[i]);
	free(p->contr);
	free(ia);
	free(ja);
	free(ar);
	free(x);
}*/

int bestFit(donnees *d){
    int best=1;
    int a;
    motifs *bins = malloc(sizeof(motifs));
    bins[0] = creerMotif(d);
    
    for(int i=0; i<d->nb; ++i){
    	for(int j=0; j<d->tab[i].nb; ++j){
    		a = 0;
            for(int k=0; (k<best && a==0) ; ++k){
                if(bins[k].taille + d->tab[i].t <= d->T){
                    bins[k].tab[i].nb++;
                    bins[k].taille += d->tab[i].t;
                    a=1;
                }
            }
            if(a==0){
            	bins = realloc(bins, (best+1)*sizeof(motifs));
            	bins[best] = creerMotif(d);
            	bins[best].tab[i].nb++;
                bins[best].taille += d->tab[i].t;
            	best++;
            }
        }
    }
    for (int i=0; i<best; ++i){
    	printf("[ ");
    	for (int j=0; j<d->nb; ++j){
    		printf("%d ,",bins[i].tab[j].nb);
    	}
    	printf(" ] ,");
    }
    printf("\n\n");
    return best;
}

int entierSup(double x){
	if(x - ((int)x)>0){
		return ((int)x)+1;
	} else {
		return (int)x;
	}
}

int tailleTotale(donnees *d){
	int s=0;
	for (int i=0; i<d->nb; ++i){
		s += d->tab[i].t * d->tab[i].nb;
	}
	
	return s;
}

int resoudreALGO(donnees *d, int b, int *solution){
	if (entierSup(tailleTotale(d)/(double)d->T) == b){
		return b;
	} else {
		//Algo recursif ici
		return 0;
	}
}

int main(int argc, char **argv){
    pile *pMotifs=creerPile();
    lMotifs = creerListeMotifs();
    donnees d;
    probleme pr;
	
	crono_start();
	
	lecture_data(argv[1],&d);
	
	triFusion(d.tab,d.nb);
	afficherDonnees(&d);
	
	enumerationMotifs(&d,pMotifs,0);
    afficherListeMotifs(lMotifs,d.nb);
    
    chargerProbleme(&d, &pr);
    printf("======================================\n\tRESOLUTION AVEC GLPK :\n======================================\n\n\n");
    //resoudreGLPK(&pr);
    
    
    printf("===========================\n\tBEST-FIT :\n===========================\n\n\n");
    
    int bf = bestFit(&d);
    printf("bf = %d\n",bf);
    
    int *solution = malloc(lMotifs->nbMotifs * sizeof(int));
    for (int i=0; i<lMotifs->nbMotifs;++i){
    	solution[i]=0;
    }
    int z = resoudreALGO(&d, bf, solution);

    printf("\n\n\n======================================\n\tRESOLUTION PAR ALGORITHME :\n======================================\n\n");
    printf("z = %d\n\n", z);
    for (int i=0; i<lMotifs->nbMotifs;++i){
    	printf("x%d = %d ,",i, solution[i]);
    }

    crono_stop();
	double temps = crono_ms()/1000.0;

	printf("\n\nTemps : %f\n",temps);

	free(pMotifs);
	free(lMotifs);

    return 0;
}
