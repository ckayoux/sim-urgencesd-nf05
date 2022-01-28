#include <stdio.h>
#include "aleatoire.h"
/**<On définit 3 entiers correspondant chacun à un parcours différent : circuit court, circuit long sans examen complémentaire, circuit long avec examen complémentaire*/
enum circuit {
    COURT,/**<Correspond au circuit court. Les patients subissent un examen dans la salle d'examens du circuit court et peuvent quitter le service d'urgences.*/
    LONG_SIMPLE,/**<Correspond à un parcours "circuit long SANS examen complémentaire". Une fois le premier examen du circuit long terminé, les patients peuvent quitter le service d'urgences.*/
    LONG_EXAM_CPT /**<Correspond à un parcours "circuit long AVEC examen complémentaire". Une fois le premier examen du circuit long terminé, les patients doivent subir un examen complémentaire avant de quitter l'hôpital.*/
};

/**<Structure désignant un patient. On manipulera principalement des pointeurs vers les patients.
*Les patients ont un rôle passif dans la gestion du flux de la simulation. Ce sont les salle_even (exemple : salle d'enregistrement) qui les manipulent.
*/
typedef struct {
    char nom[40]; /**<Chaîne contenant le nom du patient, entré par l'utilisateur ou choisi dans un dictionnaire.*/
    char prenom[40];/**<Chaîne contenant le prénom du patient, entré par l'utilisateur ou choisi dans un dictionnaire.*/
    unsigned int id; /**<Identifiant associé à un patient et correspondant à son rang dans l'ordre d'arrivée. Deux patients ne peuvent pas avoir le même id.*/
    unsigned char circuit; /**<0 pour le circuit court. 1 pour le circuit long. 2 pour le circuit long avec examen complémentaire. Correspond à l'enum circuit.*/
    temps arrivee; /**<Date et heure d'arrivée au service des urgences. Généré automatiquement ou choisi par l'utilisateur, selon les options choisies.*/
    temps sortie; /**<date et heure de sortie du service des urgences. Déterminé par le déroulement du flux de patients*/
    temps duree_examen_complementaire; /**<Durée d'examen complémentaire (on doit la demander pour tous les patients en mode génération manuelle des durées - c'est dans les consignes du sujet).*/
} patient;

patient *creerPatient();
patient* id_getPatient(unsigned short int ID,patient** liste_patients,int nombre_patients);
patient** genererPatients(unsigned int nb_patients,temps debut_plage_horaire, temps fin_plage_horaire);
void nommerPatients(unsigned int nb_patients,patient** liste_patients) ;
