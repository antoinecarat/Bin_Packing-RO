/* NOM1 Prénom1 
   NOM2 Prénom2	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glpk.h> 
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

/* Fonctions à ajouter
	.
	.
	.
 */
 
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
	
	lecture_data(argv[1],&p);
	
	/* Lancement du chronomètre! */
	
	crono_start(); 
	
	/* A compléter
	 .
	 .
	 .
	 */
	
	
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
