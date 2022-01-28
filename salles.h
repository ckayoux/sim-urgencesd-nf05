#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include "queue.h" //queue contient patient.h et temps.h, deux headers nécessaires pour salles.c
/**Le type (salle_dattente) sera utilisé pour modéliser SDA1, SDA2, SDA3 et SDA4.
*Ces salles ont un rôle passif dans le flux, ce sont les éléments de type (salle_even) qui contrôlent la simulation
*/
typedef struct {
    Queue *QUEUE; /**<ptr vers une file représentant la file de patients dans la salle d'attente : le premier patient arrivé sera le premier à entrer dans la prochaine salle.*/
    double temps_attente;      /**<temps moyen d'attente (minutes - un reel car on s'en servira pour calculer la moyenne, on sera donc intéressés par la possibilité de recuperer les secondes.)*/
    char *message;/**<message correspondant a la salle d'attente, defini a la création de la salle d'attente*/
} salle_dattente;

/**<Le type (salle_even) représente les salles où se produit un événement : salle d'enregistrement, d'examen ioa, d'examens ...
    *Elles ont un rôle actif dans la simulation, ce sont elles qui contrôlent le flux de patient en fonction de leur état.

    *On établira dans le main une communication directe entre chaque salle_even et la salle précédente/suivante (contrôle de l'état des salles adjacentes, manipulation de leurs données ...)
    *Et une communication indirecte entre les différentes salle_even (réalisation d'une tâche, puis, passage à une autre salle_even affectée par le résultat de la tâche).
    */
typedef struct salle_even{
    bool libre; /**<notifie le statut de la salle : libre/occupée*/
    salle_dattente* salle_prec;   /**<pointe vers la salle d'attente précédente. Si la salle précédente n'est pas une salle d'attente (ENTREE,S_EXAMEN_LONG), on affectera NULL et on traitera le cas à part.*/
    salle_dattente* salle_suiv;   /**<pointe vers la salle d'attente suivante.Si la salle suivante n'est pas une salle d'attente(SORTIE,S_EXAMEN_CPT), on affectera NULL et on traitera le cas à part.*/
    struct salle_even **boxs; /**<tableau de pointeurs vers salles_even (on considère chaque box comme des sous-salles).
    *La plupart de leurs champs resteront inutilisés mais le principe reste le même.
    *Ils jouent un rôle passif dans le contrôle du flux - ce sont les éléments de type (salle_even) qui gèrent le statut des box.*/
    unsigned char nombre_boxs; /**<Nombre de boxs contrôlant l'accès au tableau boxs de la structure.*/
    temps heure_liberation; /**<instant auquel on libère le patient. Affecté à chaque fois qu'un nouveau patient arrive dans la salle.*/
    patient *Patient; /**<ptr vers le patient occupant la salle. On mettra NULL si la salle ne contient aucun patient.*/
    char *message; /**<message identifiant la salle*/

    /**<
    *champs nécessaires a l'établissement des statistiques
    */
    temps *tab_durees; /**<on stockera les durees dans un tableau,*/
    unsigned short int *tab_id_patients;/**<pareil pour les id des patients. tab_durees et tab_id_patients seront triés conjointement par ordre de durées croissantes pour que la durée et l'id correspondent à un même patient pour le même indice.*/
    unsigned short int *ordre; /**<tableau qui contiendra egalement des id de patients mais qui restera classé par ordre d'insertion, ne sera pas trié.*/
    unsigned short int indice;/**<entier indiquant la prochaine position d'insertion des données dans ces trois tableaux. A la fin de la simulation, indique la taille des tableaux.*/

    /**<
    *champs nécessaires à la génération aléatoire de durées par la loi normale :
    */
    unsigned int d_esperance; /**<La plupart des valeurs seront très proches de l'espérance (abscisse du sommet de la courbe gaussienne).*/
    unsigned int d_ecart; /**<Quelques valeurs se trouveront entre espérance-écart et espérance+écart, voire même, rarement, un peu plus éloignées.*/
} salle_even;

salle_dattente* creerSalleDattente(unsigned int maxElements,const char * message);
salle_even* creerBox(const char* message);
salle_even** grouperBoxs(int nombre_boxs,...);
salle_even* creerSalleEven(const char* message,salle_dattente* salle_prec,salle_dattente* salle_suiv,unsigned int nombre_patients,unsigned int D_Esperance,unsigned int D_Ecart,salle_even **boxs,unsigned char Nombre_Boxs);

void patientsAttendent(salle_dattente* Salle_Dattente);

void afficher_salle_dattente(salle_dattente* Salle_Dattente);
void afficher_salle_even(salle_even* Salle_Even);

void ajouterPatientSalleDattente(salle_dattente* Salle_Dattente,patient *Patient);
void enleverPatientSalleDattente(salle_dattente* Salle_Dattente);
void ajouterPatientSalleEven(salle_even* Salle_Even,temps duree);
void libererSalleEven(salle_even* Salle_Even);

void trierStats(salle_even *S);
temps dureeTotale(salle_even *S);
