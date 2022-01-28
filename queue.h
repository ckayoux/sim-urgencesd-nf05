#include <stdio.h>
#include "patient.h"

/**Creation d'une structure queue pour gerer les files d'attente. Capacite est le nombre max d'éléments que l'on peut mettre dans la queue.
*Taille est le nombre d'éléments contenus par la queue. Debut et fin sont les index des premier et dernier éléments.
*/
typedef struct {
    unsigned int capacite;/**<Ce champ désigne la taille du tableau "elements". C'est le nombre maximal d'éléments que peut contenir la file. Il n'a pas vocation à change une fois la queue créée.*/
    unsigned int taille;/**<Ce champ désigne le nombre d'éléments contenus dans la file. Il évolue lorsqu'on ajoute ou retire un élément.*/
    patient* *elements;/**<Il s'agit d'un tableau d'adresses de patients. La file peut être vue comme la liste des adresses contenues par ce tableau de l'indice "debut" à l'indice "fin", rangée dans l'ordre.*/
    int debut;/**<Il s'agit de l'indice du tableau "elements" correspondant à l'élément en tête de file. Pour enlever un patient de la tête de la file, on incrémentera ce champ.*/
    int fin;/**<Il s'agit de l'indice du tableau "elements" correspondant à l'élément en bout de file. Pour ajouter un patient en bout de file, on incrémentera ce champ.*/
} Queue;

void enqueue(Queue *Q,patient* element);
void dequeue(Queue *Q);
Queue *creerQueue(unsigned int maxElements);
patient* element_debut_queue(Queue *Q);
void afficherQueue(Queue *Q);
bool estVide(Queue *Q);
Queue* trierPatients(int Nombre_Patients,patient** Liste_Patients);
