/* Utilisation de GLPK en mode bibliothèque */
/* Il s'agit de l'exercice 2.2 des feuilles de TD, qui a servi à illustrer l'utilisation d'une matrice creuse avec GNUMathProg */
/* Ici, les données sont séparées de la modélisation, et sont lues dans un fichier */
/* Comme nous ne connaissons pas a priori la taille du problème, des allocations dynamiques sont nécessaires */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <glpk.h> /* Nous allons utiliser la bibliothèque de fonctions de GLPK */
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>


/* Structure contenant les données du problème */
/**
 * @struct Structure objets : 
 *          int t : taille d'une piece
 *          int nb : nombre de piece de cette taille
 */
typedef struct {
	int t; 
	int nb; 
} objets;


/**
 * @struct Structure donnees : contenant les donnees du probleme
 *          int T : taille du bin
 *          int nb : nombre de tailles differentes d'objets
 *          objets *tab : tableau d'objet (voir struct objets)
 */ 
typedef struct {
	int T;  
	int nb; 
	objets *tab;  
} donnees;

typedef struct {
	int nbvar; // Nombre de variables [nbMotif]
	int nbcontr; // Nombre de contraintes [p.nb]
	int *coeff; // Matrice des coeff de la fonction objectif "1" 
	int **contr; // Matrice creuse des contraintes
	int *sizeContr; // Tableau des nombres de variables dans chacune des contraintes
	int *droite; // Tableau des valeurs des membres de droites des contraintes [di]
} probleme;

struct timeval start_utime, stop_utime;


void lecture_data(char *file, donnees *p){
	
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



/* On va charger les donnes du probleme dans un probleme pour glpk */
/**
 * Procedure : tri_rapide
 * @param tableau : tableau d'objet present dans la structure donnes 
 * @param taille : taille du tableau d'objet
 */
void tri_rapide (objets *tableau, int taille) 
{
    //privot : element qui est a sa place difinitive
    // tout deux inferieur a sa gauche tout ceux super a sa droite 
    int mur, courant, pivot, tmp;
    if (taille < 2) {
        return; 
    } else {
        // On prend comme pivot l element le plus a droite
        pivot = tableau[taille - 1].t;  
        
        mur  = courant = 0;
        while (courant<taille) {    
            if (tableau[courant].t >= pivot) {
                if (mur != courant) {
                    //echange les valeur de deux cases
                    tmp=tableau[courant].t;
                    tableau[courant].t =tableau[mur].t;
                    tableau[mur].t=tmp;              
                }
                mur ++;
            }
            courant ++;
        }
        //On refait l'algo sur le sous tableau de gauche
        tri_rapide(tableau, mur - 1);
        //On refait l'algo sur le sous tableu de droite 
        tri_rapide(tableau + mur - 1, taille - mur + 1);
    }
}    

/**
 * Proceudre : ajouterMotif - Ajoute une piece au tableau de motif 
 *                              (qui est un tableau d'objet)
 * @param tableauMotif : tableau de motif (tableau de tableau d'objets)
 * @param nbMotif : numero de motif dans le quel on doit rajouter la piece 
 * @param i : quel pièce a ete rajoute dans le motif nbMotif
 */
void ajouterMotif(objets** tableauMotif, int nbMotif, int i) {
    tableauMotif[nbMotif][i].nb  = tableauMotif[nbMotif][i].nb + 1;
}
/**
 * Procedure : motifs - Enumere les motifs 
 * @param p : donnees
 * @param tailleT : taille prise dans le bin
 * @param indO : indice
 * @param nbM : nombre de motif mit par reference pour recuperer a chaque tour
 *              combien de motif on a deja remplie
 * @param tableauMotif : tableau de tableau d'objet a remplie avec les motifs
 */ 
 
int motifs (donnees p, int tailleT, int indO, int nbM, objets** tableauMotif, int* tableau) {
    
    //tableau temporaire de coefficient d'objet
    int *tmp = (int* )malloc(p.nb * sizeof(int));
    // Parcours toutes les pieces 
    for (int i=indO ; i<(p.nb) ; i++) {
      //copie tableau dans tmp
      for (int k=0; k<p.nb;++k){
          tmp[k] = tableau[k];
      }
        //si il reste de la place dans le bin
        if ((tailleT + (p.tab[i]).t) <= p.T) {
            tmp[i] = tmp[i] +1;       //Ajoute la piece dans le tmp
            nbM = motifs(p, (tailleT + (p.tab[i]).t), i, nbM, tableauMotif, tmp); 
        //si l'il reste encore des pieces a tester
        } else if (indO+1 != p.nb) {
            nbM = motifs(p, tailleT, indO+1, nbM, tableauMotif, tableau);
            return nbM;
        } else {
        	//Ajout le motif terminé dans le tableau de motif
        	for (int j=0; j<p.nb;++j){
                if(tableau[j]>0){
                    for (int a=0; a<tableau[j]; ++a){
                        ajouterMotif(tableauMotif, nbM, j);
                    }
                }
            }
        	nbM = nbM +1;
        	return nbM;
        }
    }
    free(tmp);
    return nbM;
}

void load_data(donnees *d, probleme *p, int nbMotif)
{

	int i,j;  
	int val;
	
	//nb de variables correspond au nombre de motif 
	p->nbvar = nbMotif;
	//nb de contraintes correspond au nombre d'objet 
	p->nbcontr = d->nb;
		
	/* On peut maintenant faire les allocations dynamiques dépendant du nombre de variables et du nombre de contraintes */
	
	p->coeff = (int *) malloc (p->nbvar * sizeof(int));
	p->droite = (int *) malloc (p->nbcontr * sizeof(int));
	p->sizeContr = (int *) malloc (p->nbcontr * sizeof(int));
	p->contr = (int **) malloc (p->nbcontr * sizeof(int *)); // Il restera ensuite à dimensionner chaque "ligne" de la matrice creuse des contraintes
	
	//Les coefficients de la fonction objectif 
	//Fonction objectif : indique le nombre de bin 
	for(int i = 0;i < p->nbvar;i++)
	{
			p->coeff[i] = 1;
	}

	/*  les contraintes du problème */
	//chaque contraintes correspond à un type d'objet 
	
	for(int i = 0; i < p->nbcontr; i++) // Pour chaque contrainte,
	{
		
		p->sizeContr[i] = p->nbvar;
		p->contr[i] = (int *) malloc (p->sizeContr[i] * sizeof(int));
		

		// Lecture des indice des variables intervenant dans la contrainte
		for(int j = 0; j < p->sizeContr[i];j++)
		{
			//indices des variable intervanant dans la contraintes 
			p->contr[i][j] = j+1;
		}
		
		// Lecture du membre de droite de la contrainte
		p->droite[i] = d->tab[i].nb; 
	}
}


int main(int argc, char *argv[])
{	
	/* Données du problème */
	
	donnees d1;
	
	
	//char * fichier = "A8.dat";
	

	lecture_data(argv[1], &d1);
		
	/* Lancement du chronomètre! */
	
	crono_start(); 
	
    tri_rapide(d1.tab, d1.nb); 

    /**TODO  REALLOC */
    objets ** tableauMotif = (objets **) malloc (36 * sizeof(objets*));
	for (int i = 0; i < 36; i++)
    {
        tableauMotif[i] = (objets *) malloc(sizeof(objets) * d1.nb);
    }
    for (int i=0; i<36;i++) {
        for(int j=0; j<d1.nb; j++){
            tableauMotif[i][j].nb = 0;
        }
    }

	int * tableau = (int *)malloc(d1.nb * sizeof(int));
    for (int i=0; i<d1.nb;++i){
        tableau[i] = 0;
    }
    

    //calcul le nombre de motif pour le load_date et remplie tableau de motif 
	int nbMotif = motifs(d1,0,0,0,tableauMotif, tableau);//appel ta fonction motifs;
	probleme p; 
	
	load_data(&d1,&p, nbMotif);

	// /*/*/* structures de données propres à GLPK */
	
	// glp_prob *prob; // déclaration d'un pointeur sur le problème
	// int *ia;
	// int *ja;
	// double *ar; // déclaration des 3 tableaux servant à définir la matrice "creuse" des contraintes
	
	// /* Les déclarations suivantes sont optionnelles, leur but est de donner des noms aux variables et aux contraintes.
	//    Cela permet de lire plus facilement le modèle saisi si on en demande un affichage à GLPK, ce qui est souvent utile pour détecter une erreur! */
	
	// char **nomcontr;
	// char **numero;	
	// char **nomvar; 

 //    /* variables récupérant les résultats de la résolution du problème (fonction objectif et valeur des variables) */
	
	// int z; 
	// double *x; 
	
	// /* autres déclarations */ 
	
	// int i,j;
	// int nbcreux; // nombre d'éléments de la matrice creuse
	// int pos; // compteur utilisé dans le remplissage de la matrice creuse
		
	// /* Chargement des données à partir d'un fichier */
	
	// //lecture_data(argv[1],&p);
	
	// /* Transfert de ces données dans les structures utilisées par la bibliothèque GLPK */
	
	// prob = glp_create_prob(); /* allocation mémoire pour le problème */ 
	// glp_set_prob_name(prob, "bin packing"); /* affectation d'un nom (on pourrait mettre NULL) */
	// glp_set_obj_dir(prob, GLP_MIN); /* Il s'agit d'un problème de minimisation, on utiliserait la constante GLP_MAX dans le cas contraire */
	
	// /* Déclaration du nombre de contraintes (nombre de lignes de la matrice des contraintes) : p.nbcontr */
	
	// glp_add_rows(prob, p.nbcontr); 
	// nomcontr = (char **) malloc (p.nbcontr * sizeof(char *));
	// numero = (char **) malloc (p.nbcontr * sizeof(char *)); 

	// /* On commence par préciser les bornes sur les constrainte, les indices commencent à 1 (!) dans GLPK */

	// for(i=1;i<=p.nbcontr;i++)
	// {
	// 	/* partie optionnelle : donner un nom aux contraintes */
	// 	nomcontr[i - 1] = (char *) malloc (8 * sizeof(char)); // hypothèse simplificatrice : on a au plus 7 caractères, sinon il faudrait dimensionner plus large! 
	// 	numero[i - 1] = (char *) malloc (3  * sizeof(char)); // Même hypothèse sur la taille du problème
	// 	strcpy(nomcontr[i-1], "piece");
	// 	sprintf(numero[i-1], "%d", i);
	// 	strcat(nomcontr[i-1], numero[i-1]); /* Les contraintes sont nommés "piece1", "piece2"... */		
	// 	glp_set_row_name(prob, i, nomcontr[i-1]); /* Affectation du nom à la contrainte i */
		
	// 	/* partie indispensable : les bornes sur les contraintes */
	// 	glp_set_row_bnds(prob, i, GLP_LO, p.droite[i-1], 0.0); 
	// 	 Avec GLPK, on peut définir simultanément deux contraintes, si par exemple, on a pour une contrainte i : "\sum x_i >= 0" et "\sum x_i <= 5",
	// 	   on écrit alors : glp_set_row_bnds(prob, i, GLP_DB, 0.0, 5.0); la constante GLP_DB signifie qu'il y a deux bornes sur "\sum x_i" qui sont ensuite données
	// 	   Ici, nous n'avons qu'une seule contrainte du type "\sum x_i >= p.droite[i-1]" soit une borne inférieure sur "\sum x_i", on écrit donc glp_set_row_bnds(prob, i, GLP_LO, p.droite[i-1], 0.0); le paramètre "0.0" est ignoré. 
	// 	   Les autres constantes sont GLP_UP (borne supérieure sur le membre de gauche de la contrainte) et GLP_FX (contrainte d'égalité).   
	// 	 Remarque : les membres de gauches des contraintes "\sum x_i ne sont pas encore saisis, les variables n'étant pas encore déclarées dans GLPK  
	// }	

	// /* Déclaration du nombre de variables : p.nbvar */
	
	// glp_add_cols(prob, p.nbvar); 
	// nomvar = (char **) malloc (p.nbvar * sizeof(char *));
	
	// /* On précise le type des variables, les indices commencent à 1 également pour les variables! */
	
	// for(i=1;i<=p.nbvar;i++)
	// {
	// 	/* partie optionnelle : donner un nom aux variables */
	// 	nomvar[i - 1] = (char *) malloc (3 * sizeof(char));
	// 	sprintf(nomvar[i-1],"x%d",i-1);
	// 	glp_set_col_name(prob, i , nomvar[i-1]); /* Les variables sont nommées "x1", "x2"... */

	// 	/* partie obligatoire : bornes éventuelles sur les variables, et type */
	// 	glp_set_col_bnds(prob, i, GLP_LO,0.0, 0.0); /* bornes sur les variables, comme sur les contraintes */
	// 	glp_set_col_kind(prob, i, GLP_IV);	/* les variables sont par défaut continues, nous précisons ici qu'elles sont binaires avec la constante GLP_BV, on utiliserait GLP_IV pour des variables entières */	
	// } 

	// /* définition des coefficients des variables dans la fonction objectif */

	// for(i = 1;i <= p.nbvar;i++) glp_set_obj_coef(prob,i,p.coeff[i - 1]);  
	
	// /* Définition des coefficients non-nuls dans la matrice des contraintes, autrement dit les coefficients de la matrice creuse */
	// /* Les indices commencent également à 1 ! */

	// nbcreux = 0;
	// for(i = 0;i < p.nbcontr;i++) 
	// {
	// 	nbcreux += p.sizeContr[i];
	// }
	
	// ia = (int *) malloc ((1 + nbcreux) * sizeof(int));
	// ja = (int *) malloc ((1 + nbcreux) * sizeof(int));
	// ar = (double *) malloc ((1 + nbcreux) * sizeof(double));
	
	// pos = 1;
	// for(i = 0;i < p.nbcontr;i++)
	// {
	// 	for(j = 0;j < p.sizeContr[i];j++)
	// 	{
	// 		ia[pos] = i + 1;
	// 		ja[pos] = p.contr[i][j];
	// 		ar[pos] = tableauMotif[j][i].nb;
	// 		pos++;
	// 	}
	// }
	
	// /* chargement de la matrice dans le problème */
	
	// glp_load_matrix(prob,nbcreux,ia,ja,ar); 
	
	// /* Optionnel : écriture de la modélisation dans un fichier (utile pour debugger) */

	// glp_write_lp(prob,NULL,"projet_ro.lp");

	// /* Résolution, puis lecture des résultats */
	
	// glp_simplex(prob,NULL);	glp_intopt(prob,NULL); /* Résolution */
	// z = glp_mip_obj_val(prob); /* Récupération de la valeur optimale. Dans le cas d'un problème en variables continues, l'appel est différent : z = glp_get_obj_val(prob);*/
	// x = (double *) malloc (p.nbvar * sizeof(double));
	// for(i = 0;i < p.nbvar; i++) x[i] = glp_mip_col_val(prob,i+1); /* Récupération de la valeur des variables, Appel différent dans le cas d'un problème en variables continues : for(i = 0;i < p.nbvar;i++) x[i] = glp_get_col_prim(prob,i+1);	*/

	// printf("z = %d\n",z);
	// for(i = 0;i < p.nbvar;i++) printf("x%d = %d, ",i,(int)(x[i] + 0.5)); /* un cast est ajouté, x[i] pourrait être égal à 0.99999... */ 
	// puts("");

	// /* libération mémoire */
	// glp_delete_prob(prob); 
	// free(p.coeff);
	// free(p.sizeContr);
	// free(p.droite);
	// for(i = 0;i < p.nbcontr;i++) free(p.contr[i]);
	// free(p.contr);
	// free(ia);
	// free(ja);
	// free(ar);
	// free(x);*/*/

	/* Problème résolu, arrêt du chrono et affichage des résultats */

	double temps;
	crono_stop();
	temps = crono_ms()/1000,0;

	printf("Temps : %f\n",temps);	
	
	/* Libération mémoire */
	
	free(d1.tab);
											
	/* J'adore qu'un plan se déroule sans accroc! */	
	return 0;
}
