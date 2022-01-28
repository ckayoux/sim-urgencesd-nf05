//Temps : à peu près 2h
#include <stdbool.h>
#include "queue.h"
/*Ce fichier source définit les fonctions permettant la gestion des queues/files.
En effet, l'entrée et la sortie de l'hôpital sont modélisées sous la forme de files, et les patients des salles d'attente sont également rangés sous la forme d'une file.
Référence : https://www.thelearningpoint.net/computer-science/data-structures-queues--with-c-program-source-code*/

/** \brief Instancie une queue de capacité maximale donnée, pouvant être utilisée dans la simulation.
 *
 * \param La capacité maximale de la queue (entier non signé).
 * \return Un pointeur vers la queue nouvellement créée (Queue*).
 *
 * Réservation d'espace mémoire pour un nouvel élément de type Queue, récupération du pointeur vers cet élément.
 * Réservation d'espace mémoire pour un tableau d'adresses de patients de taille la capacité maximale passée en paramétre d'entrée. Affectation de l'adresse de ce tableau au champ prévu à cet effet de la Queue.
 * Initialisation des champs de la Queue, en partant du principe qu'elle est initialement vide.
 */
Queue *creerQueue(unsigned int maxElements) {
        Queue *Q;
        Q = (Queue *)malloc(sizeof(Queue));
        //initialisation des champs
        Q->elements = (patient **)malloc(sizeof(patient*)*maxElements);
        Q->taille = 0; //Initialement, la queue comporte 0 éléments
        Q->capacite = maxElements; //La queue peut au maximum comporter maxElements
        Q->debut = 0; //Elle commencera à l'indice 0 de maxElements
        Q->fin = -1; //Elle doit se finir à l'indice 0 une fois qu'on aura rajouté un élément, donc le champ fin est initialisé à -1
        //retour du pointeur
        return Q;
    }

/** \brief Test de vacuité d'une Queue.
 *
 * \param Un pointeur vers la Queue testée (Queue*).
 * \return Une valeur booléenne : vrai si la file est vide, faux sinon.
 *
 * On considère que la file est vide si le champ "taille" est nul.
 */
bool estVide(Queue *Q) {
    /*Permet de tester si la queue passée en paramètres contient ou non des éléments.
    Retourne un booléen vrai si elle est vide, faux sinon*/
        if(Q->taille<=0) //si la queue est vide, on renvoie vraie
                return true;
        return false; //sinon, on renvoie faux.
}

/** \brief Fournit l'élément en tête de file.
 *
 * \param Un pointeur Q vers la Queue dont on souhaite obtenir l'élément.
 * \return Un pointeur vers le patient situé en tête de file (à l'indice "Q->debut" du tableau elements), ou NULL si la file est vide.
 *
 * L'élément en tête de file est l'élément Q->elements[Q->debut]
 */
patient* element_debut_queue(Queue *Q) {
    /*Cette fonction donne accès à l'élément placé en tête de file.*/
        if(Q->taille==0) //si la queue est vide, on renvoie un pointeur NULL
        {
                return NULL;
        }
        return Q->elements[Q->debut]; //Sinon, on renvoie un pointeur vers le premier patient de la file, càd le patient présent à l'indice queue->debut de queue->elements
}

/** \brief Ajoute l'adresse d'un patient en bout de file dans la Queue souhaitée.
 *
 * \param Un pointeur Q vers la Queue à laquelle on souhaite ajouter un patient.
 * \param Un pointeur vers le patient dont on veut ajouter l'adresse dans la Queue.
 *
 * Si la file n'a pas atteint sa capacité maximale, incrémentation de l'indice "Q->fin", du compteur "Q->taille" et insertion de l'adresse du nouveau patient dans "Q->elements[Q->fin]".
 */
void enqueue(Queue *Q,patient* element)
/*Cette fonction permet d'ajouter un patient en fin de file. Cela revient à :
    - incrémenter la taille de la queue
    - incrémenter l'indice de fin
    - enregistrer dans le tableau elements, à l'indice de fin, l'élément passé en paramètre d'entrée */
{
        if(Q->taille == Q->capacite)
        {
                //la queue est pleine, on ne peut rien ajouter
                return;
        }
        else //il reste de la place dans la queue
        {
                Q->taille++; //On incrémente la taille
                Q->fin = Q->fin + 1; //On incrémente l'indice de fin
               if(Q->fin==Q->capacite) {
                    //Si on déborde la capacité max de la file, on remet Q->fin à 0 pour éviter la segmentation fault en tentant d'accéder à elements
                    Q->fin=0;
                }
                 Q->elements[Q->fin] = element;//on enregistre element dans elements à l'indice fin

        }
        return;
}

/** \brief Retire l'adresse d'un patient en tête de file d'une Queue.
 *
 * \param Un pointeur Q vers la Queue dont on souhaite retirer l'élément en tête de file.
 *
 * Si la file n'est pas vide, incrémentation de l'indice de début et décrémentation du champ indiquant sa taille.
 */
void dequeue(Queue *Q)
/*Cette fonction permet de retirer un patient présent en tête de file. Cela revient à :
    - décrémenter la taille de la queue
    - incrémenter l'indice de début (pour ignorer l'élément présent à l'ancien indice de début comme s'il n'etait plus dans la file)*/
{
        if(Q->taille==0)
        {
                // la queue est vide, on ne peut rien enlever
                return;
        }
        else//la queue n'est pas vide, on peut lui enlever un élément
        {
                Q->taille--; //on décrémente la taille de la queue, qui compte désormais un élément de moins
                Q->debut++; //retirer un element, ça revient à incrémenter l'indice de début pour ignorer cet élément
                if(Q->debut==Q->capacite)
                {
                    /*comme on "vide" la queue en incrémentant l'indice de debut, une fois qu'on est arrive à la capacité max on est hors des limites du tableau elements et il faut revenir à 0*/
                        Q->debut=0;
                }
        }
        return;
}

void afficherQueue(Queue *Q)
{
    //Fonction utilisée pour le deboggage, permet d'afficher le contenu d'une file. Jamais utilisée dans la version finale.
        for (int i=Q->debut;i<=Q->fin;i++) {
            printf("%s , ",(Q->elements[i])->prenom);
        }
        return;
}

/** \brief Créée une queue de patients ordonnée par ordre d'arrivée.
 *
 * \param Un tableau contenant des adresses de patients - (patient**).
 * \param Le nombre de patients contenus dans ce tableau - (entier).
 * \return Un pointeur vers la nouvelle Queue créée à partir des adresses des patients contenues dans la liste passée en paramètre d'entrée.
 *
 * La fonction trie par le même temps Liste_Patients par ordre d'arrivée (en utilisant la fonction comparerTemps de temps.c).
 * Elle attribue également l'id des patients en fonction de leur ordre d'arrivée (Le patient arrivé en premier aura pour id 1, le second aura pour id 2...).
 * Fonction utilisée pour créer la queue ENTREE contenant tous les patients au début de la simulation et pour trier liste_patients après la génération des patients.
 */
Queue* trierPatients(int Nombre_Patients,patient** Liste_Patients)
{
    patient* patient_temporaire; //variable intermédiaire désignant un patient
    Queue *Q = creerQueue(Nombre_Patients); //instanciation d'une queue de capacité Nombre_Patients.
    for (int i=0;i<Nombre_Patients;i++) {
        for(int j=i+1;j<Nombre_Patients;j++) { //on parcourt Liste_Patients à partir de l'indice suivant i pour trouver d'éventuels patients arrivés plus tôt que Liste_Patients[i]
            if(comparerTemps(Liste_Patients[i]->arrivee,Liste_Patients[j]->arrivee)==1) { //Si un patient en j est arrivé plus tôt que celui en i, on les échange de place et on continue jusqu'à la fin du tableau.
             patient_temporaire = Liste_Patients[j]; //on permute Liste_Patients[i] et Liste_Patients[j]
             Liste_Patients[j] = Liste_Patients[i];
             Liste_Patients[i] = patient_temporaire;
            }
        }
        Liste_Patients[i]->id=i+1; //On assigne un id correspondant au rang dans l'ordre d'arrivée au service des urgences. Le premier arrivé aura pour id 1, le dernier aura pour id Nombre_Patients
        enqueue(Q,Liste_Patients[i]);// on ajoute les patients en bout de file dans la queue Q par ordre d'arrivée
    }
    return Q;//Retour de Q.
}
