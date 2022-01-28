//Temps : à peu près 2h
#include "salles.h"

/** \brief Instancie une salle d'attente d'intitulé donné, capable d'accueillir un nombre donné de patients.
 *
 * \param Le nombre de patients à accueillir dans la salle d'attente (entier non signés).
 * \param Un message décrivant la salle (chaîne de caractères).
 * \return Un pointeur vers la salle d'attente nouvellement créée (salle_dattente*).
 *
 * Réservation d'espace mémoire pour une nouvelle salle d'attente et conservation d'un pointeur vers celle-ci.
 * Création d'une nouvelle Queue de capacité le nombre de patients à accueillir, qui représentera la file de patients de la salle d'attente.
 * Affectation du message passé en paramètre d'entrée et de la Queue nouvellement créée aux champs prévus à cet effet.
 * Initialisation des champs de la nouvelle salle d'attente.
 * Retour d'un pointeur vers la nouvelle salle d'attente.
 */
salle_dattente* creerSalleDattente(unsigned int maxElements,const char *message) {
    /*Permet d'instancier une salle d'attente conçue pour accueillir maxElements patients, avec un intitulé message */
    salle_dattente *nouvelleSalleDattente = malloc(sizeof(salle_dattente)); //Réservation d'espace mémoire pour un élément de type salle_d'attente, acquisition du pointeur vers le début de l'espace réservé.

    nouvelleSalleDattente->QUEUE=creerQueue(maxElements); //Instanciation d'une queue (creerQueue def. dans queue.c) correspondant à la file de patients de la salle d'attente.

    nouvelleSalleDattente->temps_attente=0; //initialisation du temps d'attente à 0.0 minutes.
    char *msg = malloc(strlen(message)+1); //réservation de l'espace mémoire nécessaire pour stocker le message
    strcpy(msg,message); //gravure du message dans msg
    nouvelleSalleDattente->message=msg;//affectation du champ message de la salle d'attente avec la chaîne msg

    return nouvelleSalleDattente; //Retour du pointeur vers la nouvelle salle d'attente
}

/** \brief Instancie une salle d'examen ou la salle d'enregistrement.
 *
 * \param Un message décrivant la salle (chaîne de caractères).
 * \param L'adresse de la salle d'attente précédente s'il y en a une. (salle_dattente*).
 * \param L'adresse de la salle d'attente suivante s'il y en a une. (salle_dattente*).
 * \param Le nombre de patients qui entreront dans la salle. (entier non signé).
 * \param L'espérance, en minutes, pour la génération aléatoire de la durée de la tâche associée à cette salle (entier non signé).
 * \param L'écart-type, en minutes, pour la génération aléatoire de la durée de la tâche associée à cette salle (entier non signé).
 * \param Un tableau "Boxs" contenant l'adresse d'autres salles - qui seront les boxs de la salle nouvellement créée, si elle en a (salle_even**).
 * \param Le nombre de box que contiendra la salle nouvellement créée (caractère non signé).
 * \return Un pointeur vers la salle "d'événement" nouvellement créée (salle_even*).
 *
 * Réservation d'espace mémoire pour une nouvelle salle "d'événement" et conservation d'un pointeur vers celle-ci.
 * Initialisation des champs de la salle.
 * Réservation d'espace mémoire pour les différents tableaux servant à l'établissement des statistiques associées à la salle
 * Affectation des valeurs passées en paramètres d'entrée aux champs de la salle prévus à cet effet
 * Retour de l'adresse de la salle "d'événement" nouvellement créée.
 */

salle_even* creerSalleEven(const char* message,salle_dattente* Salle_Prec,salle_dattente* Salle_Suiv,unsigned int nombre_patients,unsigned int D_Esperance,unsigned int D_Ecart,salle_even **Boxs,unsigned char Nombre_Boxs) {
    //cette fonction permet d'instancier un élément de type salle_even et de renvoyer un pointeur vers cet élément.
    salle_even *nouvelleSalleEven = malloc(sizeof(salle_even)); //réservation de l'espace mémoire occupé par l'élément de type (salle_even)
    nouvelleSalleEven->libre = true; //On initialise les salles avec le statut "libre" afin qu'elles soient prêtes à accueillir un premier patient.

    nouvelleSalleEven->salle_prec=Salle_Prec; //La salle d'attente précédente passée en paramètre d'entrée est assignée au champ prévu à cet effet.
    nouvelleSalleEven->salle_suiv=Salle_Suiv;//La salle d'attente suivante passée en paramètre d'entrée est assignée au champ prévu à cet effet.

    nouvelleSalleEven->boxs=Boxs; //un pointeur vers un tableau de salle_even*, ou NULL si pas de boxs. On génèrera les groupements de boxs avec la fonction grouperBoxs.
    nouvelleSalleEven->nombre_boxs=Nombre_Boxs;//Le nombre de box passé en paramètre d'entrée est assigné au champ prévu à cet effet - il servira à indiquer le dernier indice du tableau boxs.
    nouvelleSalleEven->Patient=NULL;//Initialement, la salle ne contient aucun patient : on initialise le champ Patient à NULL.

    temps temps_nul; temps_nul.annee=temps_nul.mois=temps_nul.jour=temps_nul.h=temps_nul.m=0;

    nouvelleSalleEven->indice=0; //l'indice des tableaux de statistiques est initialisé à 0 (première position d'insertion)
    nouvelleSalleEven->tab_id_patients=calloc(nombre_patients,sizeof(unsigned short int)); //Réservation de l'espace mémoire nécessaire au stockage des id de tous les patients passant par la salle. Remplissage avec des valeurs nulles
    nouvelleSalleEven->tab_durees=calloc(nombre_patients,sizeof(temps)); //Réservation de l'espace mémoire nécessaire au stockage des durées de séjour de tous les patients passant par la salle. Initialisation avec des valeurs nulles.
    nouvelleSalleEven->ordre=calloc(nombre_patients,sizeof(unsigned short int)); //Réservation de l'espace mémoire nécessaire au stockage des id de tous les patients passant par la salle. Remplissage avec des valeurs nulles

    nouvelleSalleEven->heure_liberation=temps_nul;

    nouvelleSalleEven->d_esperance=D_Esperance; //Assignation de l'espérance passée en paramètre d'entrée au champ prévu à cet effet
    nouvelleSalleEven->d_ecart=D_Ecart;//Assignation de l'écart passé en paramètre d'entrée au champ prévu à cet effet
    char *msg = malloc(strlen(message)+1); //Réservation de l'espace nécessaire au stockage du message passé en paramètre d'entrée
    strcpy(msg,message); //Gravure du message dans la chaîne msg
    nouvelleSalleEven->message=msg; //attribution de la chaîne msg au champ message de la salle crée

    return nouvelleSalleEven; //Retour d'un pointeur sur la salle créée.
}

/** \brief Instancie un nouveau box d'examen.
 *
 * \param Un message décrivant le nouveau box (chaîne de caractères).
 * \return Un pointeur vers le box nouvellement créée (salle_even*).
 *
 * Réservation d'espace mémoire pour une nouvelle salle "d'événement" et conservation d'un pointeur vers celle-ci.
 * Initialisation des champs - beaucoup de champs de la structure (salle_even) seront inutilisés pour les box.
 * Retour de l'adresse du box nouvellement créé.
 */
salle_even* creerBox(const char* message) {
     //cette fonction permet d'instancier un box - élément de type salle_even - et de renvoyer un pointeur vers cet élément.
    salle_even* nouveauBox = malloc(sizeof(salle_even)); //Réservation de la mémoire nécessaire, récupération du pointeur vers le bloc mémoire réservée
    nouveauBox->libre = true; //initialisation au statut "libre", prêt à accueillir un premier patient.
    nouveauBox->salle_prec=NULL; //On ne considère pas de salle_prec pour les box : la salle_even contenant les box se chargera de faire le lien avec les salles adjacentes.
    nouveauBox->salle_suiv=NULL; //On ne considère pas non plus de salle_suiv pour les box.
    nouveauBox->Patient=NULL;//champ Patient initialisé à NULL
    nouveauBox->boxs=NULL; //Les box n'ont pas de sous-box. boxs initialisé à NULL
    nouveauBox->nombre_boxs=0;//Les box n'ont pas de sous-box. nombre_boxs initialisé à 0.
    temps temps_nul; temps_nul.annee=temps_nul.mois=temps_nul.jour=temps_nul.h=temps_nul.m=0;
    nouveauBox->heure_liberation=temps_nul;

     char *msg = malloc(strlen(message)+1);
    strcpy(msg,message);
    nouveauBox->message=msg; //Attribution d'un message correspondant au box (passé en paramètre d'entrée).
    return nouveauBox; //Retour d'un pointeur vers le box créé.
}

/** \brief Créée un tableau contenant l'adresse de tous les box passés en paramètres d'entrée.
 *
 * \param Le nombre nombre_boxs de box à assembler (entier).
 * \param L'adresse de chaque box à intégrer à l'ensemble créé. La fonction est à nombre d'arguments variable, on peut donc passer autant de box qu'on le souhaite.
 * \return Un tableau regroupant les adresses des box passés en arguments d'entrée (salle_even**).
 *
 * Réservation d'espace mémoire pour un tableau pouvant contenir nombre_boxs pointeurs vers des box.
 * Insertion des box donnés en arguments dans le tableau.
 * Retour du tableau. Cette fonction est utilisée pour affecter le champ "boxs" des salles d'examen du circuit court et du circuit long
 */
salle_even** grouperBoxs(int nombre_boxs,...){
    /*Cette fonction prend un nombre indéterminé de box en argument et renvoie un tableau contenant ces box.

    On aurait très bien pu créer des tableaux de box dans le main, mais c'était une manière plus rigide de programmer (qui n'aurait pas été pratique avec une simulation contenant beaucoup plus de salles)
    Et cela nous a permis de nous faire la main en ce qui concerne les fonctions à nombre d'arguments variables*/
    salle_even** tableauBoxs = malloc(sizeof(salle_even*)*nombre_boxs);//réservation de l'espace mémoire nécessaire au stockage de nombre_boxs (salle_even*) = pointeurs vers des box .
    va_list args;//récupération de la liste des arguments.
    va_start(args,nombre_boxs);//Préparation à la lecture de nombre_boxs arguments.

    for(int i = 0 ; i < nombre_boxs;i++) {
        tableauBoxs[i] = va_arg(args,salle_even*);//Remplissage des cases du tableau avec les (salle_even*) = ptr vers des box passés en arguments.
    }
    va_end(args);//fin du traitement des arguments de nombre variable.
    return tableauBoxs; //retour du tableau contenant les box pour assignation au champ boxs d'une salle_even.
}

/** \brief Ajoute une minute (par patient dans sa file) au temps d'attente total d'une salle d'attente.
 *
 * \param L'adresse d'une salle d'attente (salle_dattente*).
 *
 * Ïncrémentation du compteur de minutes attendues de la salle_dattente pour chaque patient présent dans sa Queue.
 * Procédure utilisée pour mettre à jour le temps d'attente total des salles d'attente à chaque fois que le temps virtuel de la simulation est incrémenté, pour permettre le calcul du temps d'attente moyen par salle d'attente.
 */
void patientsAttendent(salle_dattente* Salle_Dattente) {
    /*Cette procédure permet d'ajouter une minute au temps d'attente total de la salle d'attente dont l'adresse est passée en argument, par patient dans sa file.
    Appelée à chaque changement de minute dans la simulation.*/
    Queue *Q=Salle_Dattente->QUEUE;
    for(int i=Q->debut; i<=Q->fin ; i++) { //on incrémente le champ temps_attente de la salle d'attente pour chaque patient présent dans la file de la salle d'attente.
        Salle_Dattente->temps_attente++;
    }
    return;
}

void afficher_salle_dattente(salle_dattente* Salle_Dattente) {
    //Procédure utilisée par les concepteurs du programme pour vérifier le contenu d'une salle d'attente. Inutilisée dans la version finale.
    printf("\nMessage :");
    puts(Salle_Dattente->message); puts("\n");
    printf("\nQueue : ");
    afficherQueue(Salle_Dattente->QUEUE); puts("");
    return;
}

/** \brief Ajoute l'adresse d'un patient la queue d'une salle d'attente (en bout de file).
 *
 * \param L'adresse d'une salle d'attente (salle_dattente*).
 * \param L'adresse du patient transféré (patient*).
 *
 * Ajout du patient en bout de file du champ QUEUE de la salle d'attente à l'aide de la fonction enqueue de queue.c.
 */
void ajouterPatientSalleDattente(salle_dattente* Salle_Dattente,patient *Patient) {
    /*Procédure permettant d'ajouter un patient (dont l'adresse est passée en argument) en bout de file de la queue de la salle d'attente (dont l'adresse est également passée en argument).*/
    enqueue(Salle_Dattente->QUEUE,Patient); //Utilisation de la fonction enqueue de queue.c.
    return;
    }


void afficher_salle_even(salle_even* Salle_Even) { //utilisee par le programmeur pour verifier l'etat d'une salle_even. Jamais appelée dans la version finale du programme.
    printf("\nMessage :");
    puts(Salle_Even->message); puts("\n");
    printf("\nLibre ? "); (Salle_Even->libre)?puts("Oui"):puts("Non");
    if(Salle_Even->boxs) {
        for(int i=0;i<Salle_Even->nombre_boxs;i++) {
            afficher_salle_even(Salle_Even->boxs[i]);
        }
    }
    else {
        puts("Salle depourvue de boxs.");
    }
}

/** \brief Trie conjointement deux tableaux servant à l'établissement de statistiques d'une salle "d'événement"
 *
 * \param L'adresse d'une salle d'événement S.
 *
 * Trie le tableau de durées "tab_durees" de la salle par ordre de valeurs temporelles croissantes en utilisant la fonction comparerTemps() définie dans temps.c.
 * Trie en même temps le tableau d'id des patients correspondants "tab_id_patients" en permutant les cases de même indice.
 * (Les durées de "tab_durees" correspondent au temps de séjour des patients dont les id sont contenus aux mêmes indices du tableau "tab_id_patients").
 * Procédure utilisée pour permettre l'établissement des statistiques liées au temps de séjour des patients dans les salles.
 */
void trierStats(salle_even *S) {
    /*Cette procédure sert à trier à la fois le tableau de durées et le tableau d'id de patients correspondant par ordre de durée croissante.
    à l'issue du tri, les durées de tab_durees sont rangées dans l'ordre croissant et les id de tab_id_patients sont rangées aux indices correspondant à leurs durées de séjour dans la salle *S.
    Fonction appelée une fois par salle après le flux de patients.*/
    temps inter;//Variable intermédiaire utilisée pour permuter deux temps
    unsigned short int inter2; //Variable intermédiaire utilisée pour permuter deux entiers
    for(int k=0; k<S->indice;k++){ //on compare chaque élément (en commençant par le début) avec tous les éléments suivants dans le tableau.
            for(int j=k+1; j<S->indice ;j++) {
                if(comparerTemps(S->tab_durees[k],S->tab_durees[j])==1) { //comparerTemps(temps t1, temps t2) est définie dans temps.c. Si t1 > t2, elle retourne 1. Il faut alors permuter les cases du tableau contenant t1 et t2.
                    //permutation des cases d'indice k et j dans les deux tableaux.
                        //tableau de durées
                    inter = S->tab_durees[k];
                    S->tab_durees[k] = S->tab_durees[j];
                    S->tab_durees[j] = inter;

                        //tri du tableau d'id de patients correspondant
                    inter2 = S->tab_id_patients[k];
                    S->tab_id_patients[k] = S->tab_id_patients[j];
                    S->tab_id_patients[j] = inter2;
                }
            }
    }
    return;
}

/** \brief Fait le total des durées contenues dans le tableau de durées d'une salle
 *
 * \param L'adresse d'une salle "d'événement" (salle_even*).
 * \return La durée totale calculée (temps).
 *
 * Additionne toutes les durées contenues dans le champ "tab_durees" de la salle.
 */
temps dureeTotale(salle_even *S) {
    //Fonction permettant d'obtenir la durée totale de séjour dans la salle en sommant les durées du tableau.
    temps total; total.annee=0;total.mois=0;total.jour=0;total.h=0;total.m=0; //initialisation du temps total à 0a0m0j 0h0min
    for(int k=0; k<S->indice;k++){//à l'issue de la simulation, la dernière position d'insertion est désignée par le champ indice de la salle *S.
        ajouterTemps(&total,S->tab_durees[k]); //ajout de chaque durée du tableau à temps total.
    }
    return total; //Retour de la durée totale calculée.
}
