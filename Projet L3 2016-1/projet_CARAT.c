//CARAT Antoine

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <glpk.h> 
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>


/* Structure contenant les données du problème */

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
	int t; //Taille du motif (somme des taille des objets dans le motif)
	objets *tab; 
}motif;

motif motifTab;

/* lecture des donnees */

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

/* Quelques fonctions utiles pour la mesure du temps CPU */

struct timeval start_utime, stop_utime;

void crono_start()
{
	struct rusage rusage;
	
	getrusage(RUSAGE_SELF, &rusage);
	start_utime = rusage.ru_utime;
}

void crono_stop()
{
	struct rusage rusage;
	
	getrusage(RUSAGE_SELF, &rusage);
	stop_utime = rusage.ru_utime;
}

double crono_ms()
{
	return (stop_utime.tv_sec - start_utime.tv_sec) * 1000 +
    (stop_utime.tv_usec - start_utime.tv_usec) / 1000 ;
}


void fusion(objets* tab, int d1, int f1, int d2, int f2){
	
	objets *tmp = (objets *) malloc((f2 * sizeof(objets))+1);
	int cmp1 = d1;
	int cmp2 = d2;

	for(int i=d1; i<=f2; ++i){
		
		if (cmp1>f1 && cmp2>f2){
			break;
		} else if (cmp1>f1) {
	        tmp[i]=tab[cmp2];
            ++cmp2;
    	} else if (cmp2>f2) {
            tmp[i]=tab[cmp1];
            ++cmp1;
        } else if (tab[cmp1].t > tab[cmp2].t){
			tmp[i].t = tab[cmp1].t;
			tmp[i].nb = tab[cmp1].nb;
			++cmp1;
		} else {
			tmp[i].t = tab[cmp2].t;
			tmp[i].nb = tab[cmp2].nb;
			++cmp2;
		}
	}
	
	for(int i=d1; i<=f2; ++i){
		tab[i].t = tmp[i].t;
		tab[i].nb = tmp[i].nb;
	}
	
    free(tmp);
}

void triFusion(objets* tab, int d, int f){
	int pivot = ((f-d)/2)+d;
	
	if (f-d>2) {
		triFusion(tab,d,pivot);
		triFusion(tab,pivot+1,f);
	} else {
		fusion(tab,d,pivot,pivot+1,f);
	}
}

//trier des objets par ordre decroissant ; last est le dernier indice de tab
void triObjets(objets *tab, int last){
	int pivot = (last/2);
	
	triFusion(tab,0,pivot);
	triFusion(tab,pivot+1,last);
	fusion(tab,0,pivot,pivot+1,last);
}

/*//ajouter un objet o dans le motif i
void ajoutObjet(int o, int i){
	
}

int tailleMotif(int i){
	return motifTab[i].t;
}

//Fonction qui calcule les différents motifs pour les donnees p
int motifs(donnees p, int objetCourant, int motifCourant){
	int nb = 0;
	if (objetCourant < p.nb) {
		for (int i=objetCourant; i< p.nb; ++i){
			if ((tailleMotif(motifCourant) + p.tab[objetCourant].t) < p.T) {	//Si on a la place de stocker l'objet courant
				ajoutObjet(p.tab[objetCourant],motifCourant);	//On l'ajoute au motif et on essaie d'en mettre un autre du même type (on boucle dans le for).

			} else if ((tailleMotif(motifCourant) + p.tab[objetCourant+1].t) < p.T){ //Sinon on a la place de de mettre l'objet suivant.
				ajoutObjet(p.tab[objetCourant+1],motifCourant);	//On l'ajoute au motif et on essaie d'en mettre un autre du même type.
				nb = motifs(p,objetCourant+1,motifCourant);
			} else {
				nb = 1 + motifs(p,objetCourant+1,motifCourant+1);
			}
		}
		nb = motifs(p,objetCourant,motifCourant+1);
	}
	return nb;
}*/
 
int main(int argc, char *argv[])
{	
	/* Données du problème */

	donnees p; 
	
	/* autres déclarations 
		.
		.
		.
	 */ 
	
	
	/* Chargement des données à partir d'un fichier */
	
	//lecture_data(argv[1],&p);
	
	//motifTab = (motif *) malloc (1*p.nb * sizeof(motif));
	
	
	/* Test du tri par fusion.
	p.T = 10;
	p.nb = 5;
	p.tab = (objets *) malloc (p.nb * sizeof(objets));
	
	int valT[5] = {4,3,1,24,2};
	int valNb[5] = {2,4,1,5,3};
	
	for(int i = 0;i < p.nb; ++i) // Pour chaque format d'objet...
	{
		p.tab[i].t = valT[i];
		p.tab[i].nb = valNb[i];
	}

	printf("AVANT : ");
	for (int i=0; i< p.nb; ++i){
		printf("%d ; ",p.tab[i].t);	
	}
	printf("\n");
	triObjets(p.tab, p.nb-1);
	printf("APRES : ");
	for (int i=0; i< p.nb; ++i){
		printf("%d ; ",p.tab[i].t);	
	}*/
	
	/* Lancement du chronomètre! */
	
	crono_start(); 
	
	triObjets(p.tab, p.nb-1);
	//resoudre le probleme

	
	
	/* Problème résolu, arrêt du chrono et affichage des résultats */

	double temps;
	crono_stop();
	temps = crono_ms()/1000,0;

	printf("Temps : %f\n",temps);	
	
	/* Libération mémoire */
	
	free(p.tab);
											
	/* J'adore qu'un plan se déroule sans accroc! */
	
	return 0;
}
