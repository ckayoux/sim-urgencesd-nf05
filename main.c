//Temps pour concevoir le main : à peu près 25h (2h sur les options, 5h sur la création de patients,  6h sur le flux, 5h sur les statistiques, 2h sur l'historique, 2h sur l'optimisation de l'affichage, 3h de debuggage et peaufinage )
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "salles.h"/*Les différents fichiers headers sont imbriqués les uns dans les autres pour éviter les définitions multiples des fonctions.
L'ordre est le suivant : salles.h > queue.h > patient.h > aleatoire.h > temps.h. Headerception*/

bool yesno_question(const char *chaine);
char convertir_minuscules(char c);
char convertir_majuscules(char c);
void saisirstring(char* chaine) ;
bool sauterligne(bool sauter_une_ligne) ;
bool notifier_evenement(bool sauter,temps t,char *descriptif,char *message_salle,FILE *f);

/** \brief Fonction main : assemble toutes les fonctionnalités du programme.
 *
 * \return Retourne 0 (EXIT_SUCCESS) si le programme s'est exécuté correctement.
 *Cette partie du programme permet de lancer plusieurs simulations indépendantes les unes des autres.
 *Elle remplit les fonctionnalités suivantes :
 *- Initialisation d'une nouvelle simulation
 *- Affichage des différents menus, prise en compte des options choisies et des commandes saisies.
 *- Création des patients
 *- Détermination du circuit emprunté par les patients
 *- Gestion du flux de patients. Affichage des événements et enregistrement de l'historique.
 *- Affichage des résultats et statistiques construites à partir des données de la simulation.
 *
 *On y manipulera un tableau liste_patients (patient**) contenant la liste exhaustive des adresses des patients de la simulation.
 *On y instanciera également les différentes salles et boxs de l'hôpital, et on conservera leurs adresses dans des tableaux.
 */
int main(int argc, char *argv[])
{
    srand(time(NULL));//initialisation de rand avec pour seed l'heure système pour avoir des générations différentes à chaque lancement

    bool reafficher_menu=false; //booléen utilisé pour réafficher ou quitter des menus.
    bool saisie_correcte = false; //booléen utilisé pour gérer les erreurs de saisie de l'utilisateur.
    char requete; //caractère utilisé pour lire les commandes saisies par l'utilisateur.

    patient* *liste_patients; //déclaration d'un tableau de (patient*)

    temps temps_simulation;

    //Declaration des différentes salles et boxs. Ils seront instanciés avant le flux de patients.
    Queue *ENTREE;Queue *SORTIE;
    salle_dattente *SDA1,*SDA2,*SDA3,*SDA4;
    salle_even *S_ENREG,*S_EXAMEN_IOA,*S_EXAMEN_LONG,*S_EXAMEN_COURT,*S_EXAMEN_CPT;
    salle_even *BOX1,*BOX2,*BOX3, *BOX4, *BOX5,*BOX_CPT1,*BOX_CPT2;
    //Declaration de tableaux groupant les salles de même type.
    salle_dattente* tableau_salles_attente[4];
    salle_even* tableau_salles_even[5];

    //DEBUT DE L'INTERACTION
    puts("Bienvenue dans ce logiciel de simulation de service des urgences.\n");
    bool quitter=!yesno_question("Souhaitez-vous demarrer une simulation ?"); //invite l'utilisateur à commencer une première simulation ou à quitter directement le programme.

while(!quitter) {//Boucle dont chaque tour correspond à une nouvelle simulation indépendante des autres.
    int nombre_patients=0; //initialisation du nombres de patients à 0.
    liste_patients=(patient**)malloc(sizeof(patient*)); //allocation d'un espace mémoire pouvant stocker un ptr vers un patient. On utilisera realloc à pour s'adapter au nombre de patients simulés et stocker autant de pointeurs vers des patients qu'il le faudra.

    //REGLAGES
    bool options; //conditionne l'affichage ou non du menu des options
        bool modifier_t=false; //permet la modification des paramètres temporels
            bool un_seul_mois=true; //fixe le mois de la simulation. Activé par défaut.
                unsigned short int annee_fixe; //L'année et le mois fixés devront alors être renseignés au moins une fois par l'utilisateur.
                unsigned char mois_fixe;
                bool un_seul_jour=true;//le mode jour fixe est activé par défaut, mais on peut le désactiver dans le menu des options. Il permet d'éviter d'avoir à saisir plusieurs fois la date si on veut faire arriver tous les patients le même jour
                unsigned char jour_fixe;
        bool choisir_circuit=false; //par défaut : circuit déterminé aléatoirement. Modifiable dans le menu des options pour que l'utilisateur puisse choisir le circuit de chaque patient.
        bool generation_patients_auto = true; //par défaut : patients générés automatiquement. Modifiable dans le menu des options pour que l'utilisateur puisse créer ses propres patients, choisir leur nom et leur heure d'arrivée.
        bool generation_durees_auto = true; //par défaut : durées générées automatiquement. Modifiable dans le menu des options pour que l'utilisateur puisse choisir des durées opératoires à chaque événement du flux.
        char beep='\a'; //par défaut : bip sonore dépendant de l'OS de l'utilisateur joué à chaque étape de la simulation. Comme ça peut être casse-...pieds, surtout sur Windows, on peut le désactiver dans les options en le remplaçant par '\0'
        char msg_options[300]=""; //Message correspondant à la liste des options choisies pour informer l'utilisateur des modalités de sa simulation.


        {


        //PARAMETRES DE LA SIMULATION
        options=!yesno_question("Souhaitez-vous utiliser les parametres par defaut ?"); //Si non, on va entrer dans le menu des options (vrai). Si non, on garde les options par défaut et on passe le menu (faux).
        if (options) {
        //Permet a l'utilisateur de regler les parametres de la simulation : mois fixé (par défaut), jour fixé, probabilites ou temps saisi a la main ...
        do {//Cette première boucle do{instructions...}while(reafficher_menu); permet d'inviter l'utilisateur à saisir de nouvelles commandes tant qu'il n'a pas lancé sa simulation. Il peut ainsi modifier ses options à volonté et revenir sur ses choix.
            printf("\nMENU DES OPTIONS :\n_____________________________________________________________________________________\n%c",beep);
            puts("Saisissez l'une des commandes suivantes :");
                puts("\t- 't' pour modifier les parametres temporels de la simulation");
                puts("\t- 'p' pour modifier le mode de generation des patients");
                puts("\t- 'd' pour modifier le mode de generation des durees des taches");
                puts("\t- 'c' pour modifier le mode de determination des circuits empruntes par les patients");
                puts("\t- 'b' pour activer ou desactiver les tonalites sonores");
                puts("\t- 's' pour lancer la simulation");
            saisie_correcte=false;
            //On traitera chaque saisie de l'utilisateur dans une structure de contrôle de type switch(...){case:...} avec un cas par défaut correspondant à la saisie d'une commande inconnue.
            do {//On invitera à saisir de nouveau sa commande en cas de saisie incorrecte. La boucle de type do{instructions...}while(!saisie_correcte) éxécute une première fois ses instructions avant de vérifier si la saisie est correcte.
                /*Les différents booléens correspondant aux options choisies sont affectés grâce à la fonction yesno_question définie dans le main.
                Elle affiche une question, invite à la saisie sécurisé d'un caractère 'o' ou 'n' et le convertit en booléen 'vrai' ou 'faux', dont la valeur peut être inversée si besoin est.*/
                fflush(stdin); //fflush(stdin) permet de 'nettoyer' la console pour éviter de consommer des caractères excédentaires à la prochaine saisie.
                scanf(" %c",&requete);
                requete = convertir_minuscules(requete);//On convertit requete en caractère minuscule (grâce à une fonction définie dans main.c) pour rendre la saisie insensible à la casse.
                switch(requete) {
                    case 't' :
                    un_seul_mois=yesno_question("\tVoulez-vous que la simulation couvre un seul mois ?");
                    //cette option permet d'éviter d'avoir à taper la date entière lorsque tous les patients arrivent le même mois.  ACTIVEE PAR DEFAUT
                    un_seul_jour=yesno_question("\tVoulez-vous que la simulation couvre un seul jour ?");
                    //avec cette option, on ne demandera que l'heure d'arrivée pour chaque patient.
                    modifier_t=true;
                    saisie_correcte=true;
                    reafficher_menu=true;
                    break;

                    case 'p' :
                    generation_patients_auto=!yesno_question("\tSouhaitez-vous creer les patients manuellement ?");
                    saisie_correcte=true;
                    reafficher_menu=true;
                    break;

                    case 'd' :
                    generation_durees_auto=!yesno_question("\tSouhaitez-vous choisir manuellement les durees des taches pour chaque patient ?");
                    saisie_correcte=true;
                    reafficher_menu=true;
                    break;

                    case 'c' :
                    choisir_circuit=yesno_question("\tVoulez-vous choisir manuellement le circuit emprunte par chaque patient ?");
                    saisie_correcte=true;
                    reafficher_menu=true;
                    break;

                    case 'b':
                    beep=(yesno_question("\tSouhaitez-vous couper le son du programme ?"))?'\0':'\a';//Si l'utilisateur répond 'o', le caractère beep prend la valeur '\0' au lieu de '\a' et le programme devient silencieux.
                    saisie_correcte=true;
                    reafficher_menu=true;
                    break;

                    case 's' :
                    saisie_correcte=true;
                    reafficher_menu=false; //On arrête de reafficher le menu des options : on passe à la suite.
                    break;

                    default : //gestion du cas où l'utilisateur saisit une commande inconnue
                    puts("Saisie incorrecte. Veuillez reessayer :");
                    saisie_correcte=false;
                    break;
                }

            }while(!saisie_correcte);

        }while(reafficher_menu);
        reafficher_menu=false;
        }



}



        printf("_____________________________________________________________________________________\n");
        //Remplissage de la chaîne msg_options en fonction des options choisies pour informer l'utilisateur des modalités de la simulation.
        strcat(msg_options,"\nCONFIGURATION : ");
        if(un_seul_jour&&!choisir_circuit&&generation_durees_auto&&generation_patients_auto&&beep=='\a') {
            strcat(msg_options,"DEFAUT\n");
        }
        else {
            strcat(msg_options,"PERSONNALISEE\n");
        }


        strcat(msg_options,"\tGeneration des patients : ");
        if(generation_patients_auto) {
            strcat(msg_options,"AUTO\n");
        }
        else {
            strcat(msg_options,"MANUELLE\n");
        }

        strcat(msg_options,"\tGeneration des durees : ");
        if(generation_durees_auto) {
            strcat(msg_options,"AUTO\n");
        }
        else {
            strcat(msg_options,"MANUELLE\n");
        }

        strcat(msg_options,"\tDetermination du circuit : ");
        if(choisir_circuit) {
            strcat(msg_options,"MANUELLE\n");
        }
        else {
            strcat(msg_options,"AUTO\n");
        }

        strcat(msg_options,"\tLimitations temporelles : ");
        if(un_seul_jour) {
            strcat(msg_options,"JOUR\n");
        }
        else if(un_seul_mois) {
            strcat(msg_options,"MOIS\n");
        }
        else {
            strcat(msg_options,"SANS\n");
        }

        strcat(msg_options,"\tBip sonore horripilant : ");
        if(beep=='\a') {
            strcat(msg_options,"AVEC\n");
        }
        else{
            strcat(msg_options,"SANS\n");
        }
        printf("%s",msg_options); //Affichage de la chaîne msg_options
        putchar('\n');


        int nb_patients_cc=0; int nb_patients_cl=0;int nb_patients_cl_cpt=0; //initialisation des nombres de patients passant par chaque circuit.
        //AJOUT DES PATIENTS
        printf("\nGENERATION DES PATIENTS :\n_____________________________________________________________________________________\n%c",beep);

            temps temps_temporaire; //détermination des données temporelles fixes qui ne seront pas redemandées à l'utilisateur.
            if(un_seul_mois&&!un_seul_jour) {//Si on est en mode mois fixé (sans que le jour soit aussi fixé), on demande le mois et l'année à l'utilisateur.
                    puts("Mois et annee d'arrivee des patients :");
                    temps_temporaire=demander_annee_mois();
                    annee_fixe = temps_temporaire.annee;
                    mois_fixe = temps_temporaire.mois;
                }
            if(un_seul_jour) {//activé par défaut : mode jour fixé
                    puts("Date d'arrivee des patients :");
                    temps temps_temporaire;
                    temps_temporaire=demander_date(); //On demande la date d'arrivée des patients à l'utilisateur. (la simulation pourra tout de même s'étendre au delà de cette date).
                    annee_fixe = temps_temporaire.annee;
                    mois_fixe = temps_temporaire.mois;
                    jour_fixe = temps_temporaire.jour;
                }



        if(!generation_patients_auto){//création des patients en mode manuel
            puts("\nVeuillez entrer a la suite les informations demandees pour chaque patient\n\n");
            do {
                //proposition de débloquage du mode mois fixé si on arrive à la fin du mois (ex : 31/12)
                if((nombre_patients>0)&&estDernierJourMois(liste_patients[nombre_patients-1]->arrivee)&&(un_seul_mois||un_seul_jour)) {//Si un patient est arrivé tard le dernier jour du mois, on propose à l'utilisateur de désactiver les modes jour fixe et mois fixe pour ajouter des patients le lendemain.
                    if(yesno_question("\nLe dernier patient arrive le dernier jour du mois.\nVoulez-vous permettre a la simulation de s'etendre sur plus d'un mois ?"))
                        un_seul_mois=false;
                        un_seul_jour=false;
                    puts("\n");
                }
                liste_patients=realloc(liste_patients,(nombre_patients+1)*sizeof(patient*)); //Réallocation dynamique de la mémoire : à chaque nouveau patient ajouté par l'utilisateur, on augmente de (sizeof(patient*)) la taille de l'espace mémoire.

                patient *patient_temporaire = creerPatient(); //La fonction creerPatient (patient.c) instancie un patient en lui réservant un espace mémoire. On utilise une variable intermédiaire patient_temporaire pour le manipuler.

                printf("\tNouveau patient (%d au total) :\n",nombre_patients);

                //on demande le nom et le prenom du patient
                printf("Prenom : ");saisirstring(patient_temporaire->prenom);//La fonction saisirstring permet la saisie sécurisée d'une chaîne de caractères contenant des espaces
                printf("Nom : "); saisirstring(patient_temporaire->nom);

                /*on demande la date et l'heure d'arrivée selon les paramètres de la simulation (en mode génération manuelle des patients, c'est l'utilisateur qui choisit leur date d'arrivée).
                Les fonctions de saisie de temps associées sont définies dans temps.c*/
                if(!un_seul_mois&&!un_seul_jour) {
                    printf("Date et heure d'arrivee au service des urgences : \n");
                    patient_temporaire->arrivee=demander_date_heure();
                }
                else if(un_seul_mois&&!un_seul_jour) {
                    printf("Jour et heure d'arrivee au service des urgences : \n");
                    patient_temporaire->arrivee=demander_jour_heure(annee_fixe,mois_fixe);

                    patient_temporaire->arrivee.annee=annee_fixe;
                    patient_temporaire->arrivee.mois=mois_fixe;
                }
                else if(un_seul_jour) {
                    printf("Heure d'arrivee au service des urgences : \n");
                    patient_temporaire->arrivee=demander_heure();

                    patient_temporaire->arrivee.annee=annee_fixe;
                    patient_temporaire->arrivee.mois=mois_fixe;
                    patient_temporaire->arrivee.jour=jour_fixe;
                }

                if(!choisir_circuit) {//Si le mode "choix automatique du circuit est activé...
                    if(!generation_durees_auto) {//et le mode "génération manuelle des durées opératoires" est aussi activé, on demande la durée de l'examen complémentaire pour chaque patient au cas où il y serait admis (c'est dans les consignes du sujet).
                        printf("Duree de l'examen complementaire : \n");
                        patient_temporaire->duree_examen_complementaire=demander_duree();
                    }
                }
                else {//Sinon, le mode "choix manuel du circuit" est activé, on demande à l'utilisateur de choisir un circuit pour son patient.
                    printf("\tCircuit emprunte par le patient (c/l) : ");
                    saisie_correcte=false;
                    requete=0;
                    do{
                        fflush(stdin);
                        scanf(" %c",&requete);
                        requete=convertir_minuscules(requete);
                        switch(requete) {
                            case 'c'://L'utilisateur a choisi le circuit court pour son patient.
                            patient_temporaire->circuit=COURT;
                            saisie_correcte=true;
                            nb_patients_cc++;//On incrémente le nombre de patients passant en circuit court.
                            break;

                            case 'l'://L'utilisateur a choisi le circuit long pour son patient.
                            patient_temporaire->circuit=(yesno_question("\tLe patient necessite-t-il un examen complementaire ?"))?LONG_EXAM_CPT:LONG_SIMPLE;//On demande à l'utilisateur s'il veut diriger son utilisateur vers la salle d'examens complémentaires ou non.
                            if(patient_temporaire->circuit==LONG_EXAM_CPT) {//L'utilisateur a choisi le circuit long avec examen complémentaire.
                                if(!generation_durees_auto) {//Si le mode "génération manuelle des durées" est activé, on demande de renseigner la durée de l'examen complémentaire.
                                    printf("\tDuree de l'examen complementaire : \n");
                                    patient_temporaire->duree_examen_complementaire=demander_duree();
                                }

                                nb_patients_cl_cpt++;//On incrémente le nombre de patients admis en examens complémentaires
                            }
                            saisie_correcte=true;
                            nb_patients_cl++;//On incrémente le nombre de patients passant par le circuit long.
                            break;

                            default: //cas où le caractère saisi est inattendu. On demande alors une nouvelle saisie en rebouclant.
                            saisie_correcte=false;
                        }
                    }while(!saisie_correcte);
                }

                liste_patients[nombre_patients]=patient_temporaire;//Ajout du patient au tableau contenant tous les patients de la simulation.
                nombre_patients++;//Incrémentation du nombre de patients.
            }
            while(yesno_question("\nVoulez-vous ajouter un nouveau patient ?"));//On invite l'utilisateur à saisir ou non un nouveau patient. Cette manière de faire évite de demander un nombre de patients à l'utilisateur.

        }
        else { //si les patients sont génerés automatiquement et non manuellement
            /*La génération des instants d'arrivée nécessite la définition d'une plage horaire avec un début et une fin et d'un nombre de patients.
            On va créer deux variables temps correspondant aux limites de la plage horaire et les affecter en fonction des options de la simulation.
            */
            temps debut_plage_horaire_arrivee;
            temps fin_plage_horaire_arrivee;

                if(!un_seul_mois&&!un_seul_jour) { //Aucun paramètre temporels fixés, on demande la date et l'heure de début et de fin d'arrivée des patients.
                    printf("Date et heure de debut de la plage horaire d'arrivee des patients : \n");
                    debut_plage_horaire_arrivee=demander_date_heure();
                    printf("Fin de la plage horaire d'arrivee des patients : \n");
                    fin_plage_horaire_arrivee=demander_date_heure();
                    while(comparerTemps(fin_plage_horaire_arrivee,debut_plage_horaire_arrivee)!=1){ //Comparaison de l'instant de fin et de début. Si l'instant de fin d'arrivée des patients est antérieur ou égal à l'instant de début, il faut le saisir de nouveau.
                        printf("Plage horaire incorrecte. Veuillez saisir la date fin de la plage horaire d'arrivee des patients : \n");
                        fin_plage_horaire_arrivee=demander_date_heure();
                    }
                }
                else if(un_seul_mois&&!un_seul_jour) {
                    printf("Jour et heure de debut de la plage horaire d'arrivee des patients : \n");
                    debut_plage_horaire_arrivee=demander_jour_heure(annee_fixe,mois_fixe);
                    debut_plage_horaire_arrivee.annee=annee_fixe;
                    debut_plage_horaire_arrivee.mois=mois_fixe;
                    printf("Fin de la plage horaire d'arrivee des patients : \n");
                     if(estDernierJourMois(debut_plage_horaire_arrivee)) {//permet de désactiver le mode "mois fixé" si l'arrivée des patients commence tard le dernier jour du mois.
                            if(yesno_question("\nLa simulation commence a la fin du mois. Souhaitez-vous desactiver le mode \"mois fixe\" ?")) {
                                un_seul_mois=false;
                                un_seul_jour=false;
                                printf("Fin de la plage horaire d'arrivee des patients : \n");
                                fin_plage_horaire_arrivee=demander_date_heure(); //On demandera alors la date et l'heure au complet.
                                puts("\n");
                            }
                        }
                    else{
                        fin_plage_horaire_arrivee=demander_jour_heure(annee_fixe,mois_fixe);//On ne demande que le jour et l'heure d'arrivée des patients en mode "mois fixé"
                        fin_plage_horaire_arrivee.annee=annee_fixe;
                        fin_plage_horaire_arrivee.mois=mois_fixe;
                    }

                    while(comparerTemps(fin_plage_horaire_arrivee,debut_plage_horaire_arrivee)!=1) {//On vérifie ensuite que la plage horaire est correcte et on demande un nouvel instant de fin s'il y a un problème, selon les paramètres temporels.
                            if(debut_plage_horaire_arrivee.jour==fin_plage_horaire_arrivee.jour&&(un_seul_mois)){
                                printf("Plage horaire incorrecte.Saisir l'heure de fin de la plage horaire d'arrivee des patients, le %02d/%02d/%04d :\n",fin_plage_horaire_arrivee.jour,fin_plage_horaire_arrivee.mois,fin_plage_horaire_arrivee.annee);
                                fin_plage_horaire_arrivee=demander_heure();
                                fin_plage_horaire_arrivee.annee=annee_fixe;
                                fin_plage_horaire_arrivee.mois=mois_fixe;
                                fin_plage_horaire_arrivee.jour=jour_fixe;
                            }
                            else if(debut_plage_horaire_arrivee.jour>=fin_plage_horaire_arrivee.jour&&(un_seul_mois)){
                                printf("Plage horaire incorrecte. Le jour de fin doit etre superieur au jour d'arrivee.");
                                fin_plage_horaire_arrivee=demander_jour_heure(annee_fixe,mois_fixe);
                                fin_plage_horaire_arrivee.annee=annee_fixe;
                                fin_plage_horaire_arrivee.mois=mois_fixe;
                            }
                            else if(!un_seul_mois){
                                printf("Plage horaire incorrecte. La fin doit etre posterieure au debut. Veuillez reessayer : \n");
                                fin_plage_horaire_arrivee=demander_date_heure();
                            }
                        }
                }
                else if(un_seul_jour) {
                    temps okazou;//Si l'heure est supérieure à 23:00, on désactivera le mode "jour fixe" pour permettre à l'utilisateur de saisir une plage horaire potentiellement plus étendue au prix de la simplicité de saisie.

                    printf("Heure de debut de la plage horaire d'arrivee des patients : \n");
                    debut_plage_horaire_arrivee=demander_heure();
                    debut_plage_horaire_arrivee.annee=annee_fixe;
                    debut_plage_horaire_arrivee.mois=mois_fixe;
                    debut_plage_horaire_arrivee.jour=jour_fixe;

                    okazou = debut_plage_horaire_arrivee; okazou.h=23;okazou.m=00;
                    printf("Fin de la plage horaire d'arrivee des patients : \n");
                    fin_plage_horaire_arrivee=demander_heure();
                    fin_plage_horaire_arrivee.annee=annee_fixe;
                    fin_plage_horaire_arrivee.mois=mois_fixe;
                    fin_plage_horaire_arrivee.jour=jour_fixe;

                    while(comparerTemps(fin_plage_horaire_arrivee,debut_plage_horaire_arrivee)!=1) {
                        if(comparerTemps(okazou,debut_plage_horaire_arrivee)==0) { //pour ne pas rester bloqué si on est en mode jour fixe et que l'utilisateur a choisi l'arrivee au delà de 23:00
                            printf("\nLa simulation commence a la fin du jour. Desactivation du mode \"jour fixe\".\n");
                            un_seul_mois=false;
                            un_seul_jour=false;
                            okazou.h=0;okazou.m=0;
                            printf("Fin de la plage horaire d'arrivee des patients : \n");
                            fin_plage_horaire_arrivee=demander_date_heure();

                            puts("\n");
                        }
                        else {
                            if(un_seul_jour){
                                printf("Plage horaire incorrecte. L'heure de fin doit etre superieure a l'heure d'arrivee. Veuillez reessayer : \n");
                                fin_plage_horaire_arrivee=demander_heure();
                                fin_plage_horaire_arrivee.annee=annee_fixe;
                                fin_plage_horaire_arrivee.mois=mois_fixe;
                                fin_plage_horaire_arrivee.jour=jour_fixe;
                            }
                            else{
                                printf("Plage horaire incorrecte. La fin doit etre posterieure au debut. Veuillez reessayer : \n");
                                fin_plage_horaire_arrivee=demander_date_heure();
                            }
                        }
                    }

                }


            unsigned int diff = diffTemps_minutes(debut_plage_horaire_arrivee,fin_plage_horaire_arrivee);//Calcul de la plage horaire comme instant fin - instant début à l'aide de la fonction diffTemps(temps t1, temps t2) définie dans temps.c.
            unsigned int inf_recommande=ceil(diff/50+sqrt(diff)/4); inf_recommande = (inf_recommande>1000)?500:inf_recommande;
            unsigned int sup_recommande=ceil(5*sqrt(diff)+diff/50); sup_recommande = (sup_recommande>1000)?1000:sup_recommande;
            printf("\nNombre de patients a simuler (recommande : %d < nb < %d) : ",inf_recommande,sup_recommande);
            /*les durées recommandées sont choisies de manière à permettre une simulation plus ou moins réaliste en permettant l'arrivée d'assez de patients pour créer des files d'attente tout en
            évitant les trop grands nombres pour avoir des patients qui attendent 3 jours en salle d'attente. La fonction racine carrée permet d'avoir des nombres relativement grands pour des petites plages horaires
            et relativement petits pour les grandes plages horaires, puisqu'on suppose que l'utilisateur préférera simuler soit une petite plage horaire avec beaucoup de patients (heure de pointe)
            Soit une grande plage horaire avec un flux de patient normal, et il n'a pas non plus envie que la simulation prenne trop de temps et soit trop longue.
            */

            scanf("%d",&nombre_patients);
            bool confirme = false;
            while(nombre_patients>1000&&!confirme || nombre_patients<1) {
                if(nombre_patients>1000) {
                    confirme = yesno_question("C'est beaucoup pour la memoire de l'ordinateur, le programme risque de crasher. Continuer ? "); /*On avertit l'utilisateur que la simulation n'a pas été conçue pour traiter de très grands nombres de patients.
                    Mais on peut saisir n'importe quel nombre dans les limites de l'unsigned short int, du moment que l'on a assez de mémoire ça fonctionne.*/
                }
                else if(nombre_patients<1) {
                    printf("Il faut au minimum un patient pour lancer la simulation. Veuillez de nouveau saisir le nombre de patinets a simuler : ");
                }

                if(!confirme||nombre_patients<1){
                    scanf("%d",&nombre_patients);
                }
            }
            confirme = false;
            liste_patients = genererPatients(nombre_patients,debut_plage_horaire_arrivee,fin_plage_horaire_arrivee); /*Utilisation de la fonction genererPatients (définie dans patient.c) pour :
            - instancier nombre_patients patients et les ajouter à un tableau de (patient*) que l'on retourne
            - leur attribuer un id
            - leur attribuer une heure d'arrivée en se basant sur un processus de poisson (appliqué par paquets de 22 patients - car factorielle(23) est un trop grand nombre pour l'ordinateur) sélectionnant l'instant avec la plus forte probabilité d'arrivée du patient,
            influencé par une composante aléatoire pour éviter une répartition trop parfaite des patients (on souhaite que des patients puissent arriver en même temps et on ne veut pas qu'il y ait presque toujours le même écart entre deux heures d'arrivée)*/
            nommerPatients(nombre_patients,liste_patients); /*Utilisation de la fonction nommerPatients (définie dans patient.c) pour attribuer des noms indiqués dans un fichier ("liste_noms.txt") à chaque *patient de liste_patients.
            La fonction distribue aléatoirement ces noms et évite dans la mesure du possible la création de doublons.*/

            patient *patient_temporaire;
            //Gestion des modes automatiques/manuels déterminés dans les options
            if(!(generation_durees_auto&&!choisir_circuit)){//Dans tous les cas, sauf celui où les durées sont générées automatiquement et où les circuits sont déterminés automatiquements aussi
                for(int i = 0 ; i<nombre_patients ; i++) {//On parcourt l'ensemble de la liste de patients pour leur appliquer un traitement
                    patient_temporaire=liste_patients[i];
                    printf("Patient %d (%s %s) : \n",patient_temporaire->id,patient_temporaire->prenom,patient_temporaire->nom);
                    if(!choisir_circuit) {//Si on est en mode détermination automatique des circuits...
                            if(!generation_durees_auto) {//...avec en plus un choix manuel des durées des tâches
                                printf("Duree de l'examen complementaire : \n");
                                patient_temporaire->duree_examen_complementaire=demander_duree(); //On demande la durée de l'examen complémentaire de chaque patient généré (demandé dans les consignes du sujet)
                            }
                        }
                    else {//choix manuel du circuit
                        /*Détermination du circuit de chaque patient à l'aide d'une structure switch(...){case:...} imbriquée dans une boucle.
                        Orientation des patients vers le circuit court, long avec ou sans examen complémentaire, et incrémentation du compteur correspondant.*/
                            printf("\tCircuit emprunte par le patient (c/l) : ");
                            saisie_correcte=false;
                            requete=0;
                            do{
                                fflush(stdin);
                                scanf(" %c",&requete);
                                requete=convertir_minuscules(requete);
                                switch(requete) {
                                    case 'c':
                                    patient_temporaire->circuit=COURT;
                                    saisie_correcte=true;
                                    nb_patients_cc++;
                                    break;

                                    case 'l':
                                    patient_temporaire->circuit=(yesno_question("\tLe patient necessite-t-il un examen complementaire ?"))?LONG_EXAM_CPT:LONG_SIMPLE;
                                    if(patient_temporaire->circuit==LONG_EXAM_CPT) {
                                        if(!generation_durees_auto) {
                                            printf("\tDuree de l'examen complementaire : \r");
                                            patient_temporaire->duree_examen_complementaire=demander_duree();
                                        }
                                        nb_patients_cl_cpt++;
                                    }
                                    saisie_correcte=true;
                                    nb_patients_cl++;
                                    break;

                                    default:
                                    saisie_correcte=false;
                                }
                            }while(!saisie_correcte);
                        }
                }
            }

        }
        //DETERMINATION ALEATOIRE DES CIRCUITS (activé par défaut)
        if(!choisir_circuit) { //Si le mode "choix manuel des circuits" est désactivé (par défaut)
            int p;//On va générer un entier entre 0 et 99.
            for(int i=0; i<nombre_patients; i++) {
                p = rand() % 100; /*à l'aide du générateur de nombres pseudo-aléatoires rand(), on génère pour chaque patient un entier compris entre 0 et 99 sur la base de la seed correspondant au temps machine au lancement du programme.
                S'il est inférieur à 40, on attribuera au patient le circuit court (40% de chance pour un patient d'être orienté en CC).
                Sinon, il est orienté en circuit long. */
                if(p<40)  { //p est inférieur à 40 donc compris entre 0 et 39 : 40 valeurs possibles sur 100 au total soit 40% de chance que cette condition soit élue : le patients ira en circuit court
                    liste_patients[i]->circuit=COURT; //Attribution de la valeur COURT (valeur possible parmi l'enum circuit) au champ circuit(caractère) du patient.
                    nb_patients_cc++;//Incrémentation du compteur de patients entrés en circuit court.
                }
                else {//Si p>= 40, alors p est compris entre 40 et 99 (60 valeurs possibles sur 100 au total) : le patient sera orienté vers le circuit long.(60% de chance)
                    p = rand() % 100;//On génère un autre nombre pseudo aléatoire entre 0 et 99 déterminant l'admission ou non à des examens complémentaires.
                    if(p<80) { //80% de chance de valider cette condition. Patient orienté vers la salle d'examens complémentaires.
                        liste_patients[i]->circuit=LONG_EXAM_CPT;//On attribue à liste_patient[i]->circuit la valeur LONG_EXAM_CPT de l'enum circuit.
                        nb_patients_cl_cpt++; //On incrémente le nombre de patients allant en examen complémentaire.
                    }
                    else {//20% de chance de valider cette condition. Patient libéré après son premier examen.
                        liste_patients[i]->circuit=LONG_SIMPLE;//On attribue à liste_patient[i]->circuit la valeur LONG_EXAM_SIMPLE de l'enum circuit.
                    }
                    nb_patients_cl++; //Dans les deux cas, le compteur de patients du circuit long est incrémenté.

                }
            }
        }
         printf("\nDEBUT DE LA SIMULATION :\n_____________________________________________________________________________________\n%c",beep);

        //CREATION DES SALLES (à l'aide de fonctions d'instanciation définies dans queue.c et salles.c). Les variables suivantes contiennent des POINTEURS vers des structures de type Queue, salle_dattente et salle_even
        ENTREE = trierPatients(nombre_patients,liste_patients); //on créée une queue qui contient tous les patients dans l'ordre où ils arrivent. La fonction trierPatients de queue.c trie un tableau (patient**) par heures d'arrivée croissantes et créée une queue correspondante
        SORTIE = creerQueue(nombre_patients); //La queue de sortie est simplement instanciée (fonction créerQueue de queue.c). Son tableau SORTIE->elements (patient**) sera de taille nombre_patients car tous les patients vont sortir.

        SDA1 = creerSalleDattente(nombre_patients,"salle d'attente 1");//instanciation d'une salle d'attente. SDA1->QUEUE contiendra un ptr vers une queue de taille nombre_patients. Son message sera "salle d'attente1". Le temps d'atttente sera initialisé à un temps nul.
        SDA2 = creerSalleDattente(nombre_patients,"salle d'attente 2");
        S_ENREG=creerSalleEven("salle d'enregistrement",SDA1,SDA2,(unsigned int)nombre_patients,3,2,NULL,0);
        /*Explication : création d'une salle_even à l'aide d'une fonction définie dans salles.c : descriptif ="salle d'enregistrement",salle_prec=SDA1,salle_suiv=SDA2,nb patients = nombre_patients,
        duree(esperance) : 3, duree(ecart) : 2 (donc durées comprises entre 1 et 5 minutes), groupement de boxs : NULL, nombre de boxs : 0*/
        S_EXAMEN_IOA=creerSalleEven("salle d'examen IOA",SDA2,NULL,(unsigned int)nombre_patients,6,3,NULL,0);/*on choisit une durée moyenne de 6 minutes, +- 3 minutes pour l'examen ioa.
        La salle suivante n'est pas prédéterminée, elle sera modifiée selon le circuit emprunté par les patients*/

        SDA3 = creerSalleDattente(nb_patients_cl,"salle d'attente 3"); //la taille des queues de SDA3 et SDA4 correspond au nombre de patients empruntant chaque circuit.
        SDA4 = creerSalleDattente(nb_patients_cc,"salle d'attente 4");

        BOX1 = creerBox("box 1") ; BOX2 = creerBox("box 2") ; BOX3 = creerBox("box 3") ; BOX4 = creerBox("box 4"); BOX5 = creerBox("box 5");//instanciation des boxs. (creerBox(const char* message) est définie dans salles.c)
        S_EXAMEN_LONG=creerSalleEven("salle d'examens (circuit long)",SDA3,NULL,(unsigned int)nb_patients_cl,45,20,grouperBoxs(3,BOX1,BOX2,BOX3),3);
        /*Explications : il n'y a pas de SDA suivante, on met NULL à la place : les patients de cette salle_even iront soit à la queue de sortie, soit en salle d'examens complémentaires.
        Le nombre de patients correspond au nombre de patients du circuit long.
        Les durées des tâches sont principalement de l'ordre de 1h +-35 minutes, mais cela peut être légèrement plus ou moins grâce à la composante aléatoire.
        Le groupement de boxs est généré par la fonction grouperBoxs de salles.c. Elle prend un nombre d'arguments variables et renvoie un tableau de ptr vers des box (salle_even**) qui est assigné au champ boxs de la salle instanciée.
        On indique également le nombre de box pour délimiter l'accès au tableau de box.*/
        S_EXAMEN_COURT=creerSalleEven("salle d'examens (circuit court)",SDA4,NULL,(unsigned int)nb_patients_cc,45,20,grouperBoxs(2,BOX4,BOX5),2);

        BOX_CPT1 = creerBox("box d'examen complementaire 1") ; BOX_CPT2 = creerBox("box d'examen complementaire 2") ;
        S_EXAMEN_CPT=creerSalleEven("salle d'examens complementaires",NULL,NULL,(unsigned int)nb_patients_cl_cpt,60,30,grouperBoxs(2,BOX_CPT1,BOX_CPT2),2); //Pour les durées d'examen complémentaire, on choisit une espérance de 1h20min et un écart de 40 min.

        tableau_salles_attente[0] = SDA1;tableau_salles_attente[1] = SDA2;tableau_salles_attente[2] = SDA3;tableau_salles_attente[3] = SDA4; //on créée un tableau de ptr vers des salles d'attente pour des traitements groupés.
        tableau_salles_even[0] = S_ENREG;tableau_salles_even[1] = S_EXAMEN_IOA;tableau_salles_even[2] = S_EXAMEN_LONG;tableau_salles_even[3]=S_EXAMEN_CPT;tableau_salles_even[4] = S_EXAMEN_COURT;//on créée un tableau de ptr vers des salles_even pour les parcourir pendant le flux et pour des traitements de groupe

        //DEBUT DU FLUX
        bool modification=true; //permet de relancer le flux avant d'incrémenter le temps de la simulation si une modification a eu lieu
        bool changementminute=false;
        //changementminute servira à sauter des lignes entre les événements ne se produisant pas au même moment
        bool changementdate=true;
        //changement date permettra d'afficher la date lorsque l'on changera de jour et qu'un événement devra se produire




        //Ouverture du fichier de l'historique en écriture
        FILE *fhistorique;
        bool acces_fichier = true;
        if(!(fhistorique = fopen("historique.txt","w"))) {
            acces_fichier=false;
           }

        //VARIABLES TEMPORAIRES POUR MANIPULER LES PATIENTS, BOXS ... DE CHAQUE SALLE
        /*Utiliser des variables intermédiaires pour désigner les patients manipulés, les box d'une salle en possédant, les queues des salles d'attente concernées ... s'avèrera très pratique,
         étant donné le nombre d'opérations à réaliser sur ces différents éléments.*/
        unsigned char jour_prochainevenement;//Caractère prédisant le jour auquel doit se produire le prochain événement. Permet d'afficher la date uniquement lorsqu'elle change.
        char evenement[130];//message qui stockera le descriptif d'un événement, sans l'heure et le nom de la salle de destination.
        char msg_sortie[35]="quitte le service des urgences";//chaîne contenant un message utilisé à plusieurs reprises, affiché lorsque les patients quittent le service des urgences.
        char msg_salle_avec_boxs[60]; //les salles contenant des box utiliseront un message permettant de mentionner le numéro du box concerné par un événement.
        Queue *q; //Désignera la queue d'une salle_dattente d'intérêt.
        salle_even* psalle; //Désignera la salle_even contrôlant le flux de patients.
        salle_even* pbox; //Désignera un box de psalle choisi. Les boxs sont les cases du champ boxs (salle_even**) de la salle_even (voir salles.h)
        salle_even* pbox2; //Désignera les boxs de la salle précédant psalle. Utilisée dans le cas de la salle d'examens complémentaires, qui est précédée par une salle avec des boxs et doit donc les gérer individuellement
        patient* ppatient; //Désignera le patient manipulé par psalle.
        temps duree;    //Désignera une durée d'événement, utilisée pour de nombreux traitements et assignations au sein du flux.


        /*On va définir temps_simulation, la date et l'heure de la simulation.
        Ce temps sera "incrémenté" (fonction incrementerTemps(temps* t) de temps.c) à chaque fois qu'il ne restera plus aucun traitement possible sur les salles et tant que tous les patients ne seront pas sortis.
        On définit aussi temps_simulation_prec qui enregistrera la date et l'heure précédent chaque incrémentation de temps_simulation. On aura besoin de cette variable pour notifier les changements de date.*/
        temps_simulation = element_debut_queue(ENTREE)->arrivee; //on initialise l'heure de la simulation à l'heure d'arrivée du premier patient.
        temps temps_simulation_prec;temps_simulation_prec.annee=0;temps_simulation_prec.mois=0;temps_simulation_prec.jour=0;temps_simulation_prec.h=0;temps_simulation_prec.m=0;//initialisée à un temps nul pour pouvoir l'utiliser dans une comparaison la première fois
        jour_prochainevenement=temps_simulation.jour;//On sait que le premier événement surviendra le jour d'arrivée du premier patient.

        while(SORTIE->taille<nombre_patients) { //Tant que tous les patients créés ne sont pas sortis, incrémentation du temps simulation et contrôle du flux. Esperons qu'aucun ne meure avant ...
            if(changementdate==true) {
                //Si on a changé de jour
                printf("\n\tLe : ");afficher_date(temps_simulation);//AFFICHAGE DE LA DATE
                fprintf(fhistorique,"\n\tLe : %02d/%02d/%04d",temps_simulation.jour,temps_simulation.mois,temps_simulation.annee);//ECRITURE DANS LE FICHIER HISTORIQUE EGALEMENT
                changementdate=false;//on indique qu'on n'a plus à afficher la date jusqu'à ce qu'elle change. On ne pourra pas revenir dans ce if tant que changementdate restera à false.
                changementminute=true;//Il faut toujours sauter une ligne et afficher l'heure après avoir affiché une nouvelle date.

            }
            //On commence par remplir SDA1 lorsque la simulation atteint l'heure d'arrivée du patient en tête de la file
            do {//cette boucle se poursuit tant que des modifications ont eu lieu. Plus de détails à l'accolade de fin.
                modification=false;//modification est initialisé à faux à chaque tour de boucle, et devient vrai à la moindre modification.

                //Première étape du flux : on commence, à chaque fois, par faire entrer en salle d'attente 1 les patients en tête de file de la queue ENTREE lorsque leur instant d'arrivée atteint temps_simulation.
                if(!estVide(ENTREE)) {
                    if(comparerTemps(element_debut_queue(ENTREE)->arrivee,temps_simulation)<=0) {//Si le temps_simulation atteint l'heure d'arrivee du patient en tête de file
                        modification=true;//On enregistre une modification. Ainsi, si le deuxième patient dans la file est arrivé à la même heure, il sera lui aussi mis en SDA1, et ainsi de suite.

                        ppatient=element_debut_queue(ENTREE);//on prend le patient en tête de file (element_debut_queue(Queue Q*) est définie dans queue.c)
                        ajouterPatientSalleDattente(SDA1,ppatient);//on le met dans la queue de SDA1 ( ajouterPatientSalleDattente(salle_dattente* Salle_Dattente,patient *Patient) est définie dans salles.c)
                        //AFFICHAGE DE L'HEURE ET DE L'EVENEMENT, ENREGISTREMENT DANS LE FICHIER HISTORIQUE (fonction notifier_evenement)
                        sprintf(evenement,"Le patient %3d (%s %s) arrive aux urgences et entre en ",ppatient->id,ppatient->prenom,ppatient->nom);//enregistrement du descriptif de l'événement dans la chaîne evenement.
                        changementminute = notifier_evenement(changementminute,temps_simulation,evenement,SDA1->message,fhistorique);/*
                        La fonction notifier_evenement est définie dans main.c. Elle affiche un événement (heure + descriptif + salle) dans la console
                        et en enregistre une version légèrement différente dans le fichier historique.
                        Une ligne est sautée et l'heure est affichée si changementminute est vrai. La fonction retourne toujours faux, que l'on assigne à changementminute pour notifier qu'on a déjà "consommé" le changement de minute.
                        (on a fait le choix d'écrire l'heure seulement quand elle change et de garder groupés les événements survenus à la même heure.*/
                        ppatient=NULL;
                        dequeue(ENTREE);//il faut ensuite enlever le patient (la tête de file) de la queue d'entree pour que le suivant devienne la nouvelle tête de file, prêt à entrer en SDA1. dequeue(Queue Q*) est définie dans queue.c.

                    }
                    if(!estVide(ENTREE)) { /*si la queue d'entrée n'est toujours pas vie même après l'utilisation de de dequeue, on prédit qu'un événement va arriver le jour d'arrivée de la nouvelle tête de file.
                        On conserve ce jour dans jour_prochainevenement s'il est plus proche que le prochain événement alors prédit, ou si on a fait aucune prédiction (jour_prochainevenement==0).*/
                                if(jour_prochainevenement>element_debut_queue(ENTREE)->arrivee.jour||jour_prochainevenement==0) {
                                    //on donne à jour_prochainevenement la valeur du jour de l'événement qui arrivera en premier
                                    jour_prochainevenement=element_debut_queue(ENTREE)->arrivee.jour;
                                }
                            }
                }

                for(char k=4;k>=0;k--) {//On va parcourir tableau_salles_even, un tableau de (salle_even *) qui les contient toutes, en commençant par la fin (pour vider les salles occupées avant de regarder si elles sont libres)
                    //Nous avons fait le choix de définir une gestion du flux générale, plutôt que de gérer chaque salle individuellement. IL faut cependant distinguer les différents cas qui se présentent.
                    psalle=tableau_salles_even[k]; //pour chaque salle du tableau de salle_even

                    if (psalle->nombre_boxs==0) { //si la salle N'A PAS DE BOX (S_ENREG, S_EXAM_IOA)

                        if(psalle->libre==false) { //si la salle est OCCUPEE, elle doit peut-être être libérée. Le champ booléen libre de (salle_even) indique que la salle ou ses boxs contiennent des patients.
                            /*La libération d'une telle salle se déroule comme suit :
                            Si la salle est occupée, et si la salle a atteint son heure de libération, le patient est ajouté à la queue de la sda suivante et est supprimé de la salle actuelle.
                            On affiche entre temps un message correspondant à la tâche et on en enregistre une copie dans le fichier historique.
                            Enfin, on indique que la salle est libre*/
                            if(comparerTemps(psalle->heure_liberation,temps_simulation)<=0) { /*si la simulation a atteint l'heure de libération de la salle, il faut la libérer. comparerTemps(temps t1,temps t2) est définie dans temps.c.
                                Cette fonction renvoie -1 si t1 < t2, 0 si t1 = t2, et 1 si t1 > t2.*/
                                modification=true; //On enregistre une modification
                                ppatient = psalle->Patient; //On prend son patient
                                /*Les salle_even ont deux champs : salle_prec et salle_suiv, pointeurs vers une salle d'attente précédente et une salle d'attente suivante.
                                C'est elles qui se chargent de prendre un patient dans la sda précédente et de l'envoyer dans la sda suivante quand la tâche est terminée.
                                Les liens vers les sda préc et suiv sont attribués à la création des salles, avant la simulation.

                                Dans le cas de la salle d'examen IOA, il peut y avoir deux salle_suiv différentes selon le circuit attribué au patient.
                                Il faut donc gérer ce cas particulier et modifier salle_prec et salle_suiv en conséquence.*/
                                if(psalle==S_EXAMEN_IOA) {//Gestion du cas particulier
                                        //Si la salle_even est IOA, sa salle_suiv est soit SDA4, soit SDA3, selon le circuit attribué au patient.
                                    if(ppatient->circuit==COURT)
                                        psalle->salle_suiv=SDA4;
                                    else
                                        psalle->salle_suiv=SDA3; //TODO : POINTEURS DE STRUCTURES

                                }
                                ajouterPatientSalleDattente(psalle->salle_suiv,ppatient); //ajouterPatientSalleDattente est définie dans salles.c. Elle ajoute un patient au bout de la queue d'une sda.

                                //AFFICHAGE DE L'HEURE ET DE l'EVENEMENT, ENREGISTREMENT DANS LE FICHIER HISTORIQUE
                                sprintf(evenement,"Le patient %3d (%s %s) entre dans la ",ppatient->id,ppatient->prenom,ppatient->nom);
                                changementminute = notifier_evenement(changementminute,temps_simulation,evenement,psalle->salle_suiv->message,fhistorique);//saut de ligne et affichage de l'heure si on a changé d'heure. Actualisation du booléen changementminute


                                psalle->libre = true; //on indique que la salle est maintenant libre.
                                psalle->Patient=NULL; //Le pointeur vers le patient contenu par la salle passe à NULL.
                                ppatient=NULL;  //La variable intermédiaire oublie également l'adresse du patient pour éviter les erreurs.
                            }
                        }

                        if(psalle->libre==true) { //Maintenant, si la salle est LIBRE (/vient d'être libérée : nous ne sommes pas dans un else, on regarde si la salle est libre juste après l'avoir potentiellement libérée.)
                            /*Le remplissage d'une salle_even de ce type (ne possédant pas de box) s'effectue de la manière suivante :
                            On prend l'adresse du patient en tête de la queue de la  salle d'attente précédente, on la stocke dans le champ patient de la salle_even actuelle, et on le supprime de la queue de la salle précédente.
                            On affiche et on grave dans le fichier historique le message correspondant.
                            On demande/génère (selon les options choisies) la durée de la tâche et on calcule la valeur à stocker dans le champ heure_liberation de la salle comme : duree + temps_simulation.
                            Enfin, on indique que la salle est maintenant occupée.*/

                            q = psalle->salle_prec->QUEUE;
                            if(!estVide(q)) { //si la queue de la salle précédente (forcément une salle d'attente) n'est pas vide
                                ppatient=element_debut_queue(q); //on prend le patient au début de la queue de la salle d'attente précédente
                                psalle->Patient = ppatient;//et on le fait entrer dans la salle actuelle
                                modification=true;//On notifie une modification
                                //AFFICHAGE DU MESSAGE
                                //AFFICHAGE DE L'HEURE ET DE l'EVENEMENT, ENREGISTREMENT DANS LE FICHIER HISTORIQUE
                                sprintf(evenement,"Le patient %3d (%s %s) entre en ",ppatient->id,ppatient->prenom,ppatient->nom);
                                changementminute = notifier_evenement(changementminute,temps_simulation,evenement,psalle->message,fhistorique);//saut de ligne si on a changé d'heure

                                psalle->libre=false;//on notifie que la salle est occupée
                                psalle->heure_liberation=temps_simulation;//On commence par affecter à psalle->heure_liberation la valeure de temps_simulation
                                if(!generation_durees_auto) {//Si on est en mode durees manuelles :
                                    duree = demander_duree(); //saisie manuelle de la durée au format "hh:mm" ou "hh" par l'utilisateur
                                }
                                else{//Sinon, on est en mode durées générées automatiquement :
                                    duree=genererDureeTache(psalle->d_esperance,psalle->d_ecart);/*La fonction générerDureeTache(unsigned int esperance, unsigned int ecart) est définie dans aleatoire.c.
                                    Elle génère, à l'aide de la loi normale et d'une légère composante aléatoire, une durée opératoire le plus souvent comprise entre espérance-écart et espérance+écart.
                                    La probabilité d'obtenir des valeurs proches de l'espérance est beaucoup plus forte, on a donc une durée proche de la "normale" et des durées "inhabituelles" plus rarement attribuées.
                                    Pour les petites durées (<10 minutes), la composante aléatoire a plus de poids (choix arbitraire pour avoir des durées moins semblables pour les opérations telles que l'enregistrement et l'examen ioa).
                                    L'espérance et l'écart des durées est attribuée arbitrairement à chaque salle, à leur création, avant le début de la simulation.*/
                                }
                                    /*Gestion des éléments destinés à servir pour l'établissement des statistiques : les (salle_even) ont
                                    - un champ ordre (usi*) qui retient l'id des patients dans leur ordre d'arrivée
                                    - un champ tab_id_patients (usi*) qui retient l'id des patients passés par la salle et qui sera trié par ordre de durée opératoire croissante
                                    - un champ tab_durees (temps*) qui retient les durees de séjour des patients passés par la salle et qui sera trié pour correspondre à tab_id_patients (à même indice, l'id et la durée correspondront à un même patient)
                                    - un champ indice qui permet de contrôler l'insertion dans les tableaux (de taille max le nombre de patients qui va passer par la salle).
                                    Ces champs servent au calcul des statistiques à l'issue de la simulation et subiront des traitements (tri, opérations ...) en fin de flux*/
                                    psalle->ordre[psalle->indice]=ppatient->id;
                                    psalle->tab_durees[psalle->indice]=duree;
                                    psalle->tab_id_patients[psalle->indice]=(unsigned short int)ppatient->id;;
                                    psalle->indice++; //On incrémente le champ indice pour la prochaine insertion dans les tableaux de la salle. A l'issue de la simulation, indice contient le nombre d'éléments des tableaux.

                                ajouterTemps(&(psalle->heure_liberation),duree); //mise à jour de l'heure de libération de la salle : psalle->heure_liberation vaut maintenant temps_simulation + duree. ajouterTemps est définie dans temps.c.
                                    if(jour_prochainevenement > psalle->heure_liberation.jour||jour_prochainevenement==0) {//on enregistre le jour de libération comme jour du prochain événement si on a pas une prédiction plus proche ou si on a aucune prédiction.
                                            jour_prochainevenement=psalle->heure_liberation.jour;
                                        }
                                dequeue(q);//on enlève le patient de la salle d'attente précédente
                            }
                            q = NULL; //les variables intermédiaires pour la queue de la sda préc. et le patient manipulé oublient leurs valeurs.
                            ppatient=NULL;
                        }

                }
                else{    //si la salle A DES BOXS (exam courts, exam longs, exam cpt) : les traitements doivent manipuler les patients à l'intérieur des box.
                /*Comme on peut le voir dans salles.h, les box sont contenus dans le champ boxs d'une (salle_even). Il s'agit d'un tableau de (salle_even*). Beaucoup de leurs champs sont inutilisés, mais leur fonctionnement est le même.
                Il faudra parcourir chaque box de la salle et les remplir, libérer ou ignorer selon la situation.
                De plus, dans le cas de la salle_exam_cpt, la salle précédente n'est pas une salle d'attente avec une queue de patients mais une salle_even avec des boxs : il faudra traiter ce cas séparément pour prendre les patients dans les boxs de S_EXAMEN_LONG*/

                    if(psalle==S_EXAMEN_CPT) { //Si la salle est la salle d'examens complémentaires (la seule précédée par une salle_even), le traitement est différent. Il n'y a pas de sda précédente avec une queue de patients mais une salle_even avec des box.
                        for(int l=0;l < psalle->nombre_boxs;l++) {//le champ nombre_boxs permet de délimiter l'indice d'accès au tableau de box (on sait qu'il y en a 2 dans ce cas, mais on préfère garder du dynamisme si on devait ajouter une nouvelle salle du même type par exemple).

                            pbox=psalle->boxs[l]; //Pour chaque box de la salle d'examen complémentaire ...
                            if(pbox->libre==false) { //si le box est occupé...
                                if(comparerTemps(pbox->heure_liberation,temps_simulation)<=0) { //si la simulation a atteint l'heure de sa libération ...
                                    modification=true;//on enregistre une modification pour reboucler avant d'incrémenter le temps de la simulation
                                    ppatient=pbox->Patient;//on prend le patient du box choisi
                                    enqueue(SORTIE,ppatient); //on envoie le patient au bout de la queue de sortie
                                    ppatient->sortie=temps_simulation;//on enregistre l'heure de sortie du patient dans le champ (temps) sortie du patient

                                    if(temps_simulation_prec.m!=temps_simulation.m&&!changementminute){
                                        changementminute=true; temps_simulation_prec.m=temps_simulation.m;
                                    }
                                    //AFFICHAGE DU MESSAGE (HEURE ET DE l'EVENEMENT), ENREGISTREMENT DANS LE FICHIER HISTORIQUE

                                    sprintf(evenement,"Le patient %3d (%s %s) ",ppatient->id,ppatient->prenom,ppatient->nom);
                                    changementminute = notifier_evenement(changementminute,temps_simulation,evenement,msg_sortie,fhistorique);//le message utilisé à la place du nom de la salle est msg_sortie - fixé avant le début du flux.

                                    jour_prochainevenement=0; //on efface la prédiciton du prochain événement, parce que la simulation est potentiellement terminée et on ne veut pas réafficher la date après le dernier événement.
                                    pbox->Patient=NULL; //on vide le box (il ne contient plus de patient)
                                    ppatient=NULL;
                                    pbox->libre=true; //on notifie que le box est désormais libre
                                }
                            }

                            if(pbox->libre==true) {//si le box est libre/vient d'être libéré
                                for(int b=0;b < S_EXAMEN_LONG->nombre_boxs;b++) {//La salle précédente est une salle_even dont les patients sont contenus dans des boxs. Il faut donc parcourir les box de la S_EXAMEN_LONG pour prendre leurs patients
                                    pbox2 = S_EXAMEN_LONG->boxs[b]; //pbox2 prend la valeur du box choisi dans la salle précédente. (pbox est toujours le box choisi dans la salle d'examens complémentaires)
                                    ppatient = pbox2->Patient;
                                    if(ppatient&&pbox2->libre==false&&pbox->libre==true) { //si le box choisi dans la salle précédente est occupée et que le box choisi dans la salle actuelle est libre ...
                                        if(ppatient->circuit==LONG_EXAM_CPT) { //si ce patient est admis en examens complémentaires (sinon, il ne faut pas s'en occuper, il sera libéré par sa propre salle)
                                            if(comparerTemps(pbox2->heure_liberation,temps_simulation)<=0) { //si la simulation a atteint l'heure de libération du box de la salle précédente
                                                modification=true;//on notifie une modification

                                                //AFFICHAGE DE L'HEURE ET DE l'EVENEMENT, ENREGISTREMENT DANS LE FICHIER HISTORIQUE
                                                sprintf(evenement,"Le patient %3d (%s %s) est pris en charge au ",ppatient->id,ppatient->prenom,ppatient->nom);
                                                changementminute = notifier_evenement(changementminute,temps_simulation,evenement,pbox->message,fhistorique);//le message affiché indique le box accueillant un patient

                                                if(generation_durees_auto) {
                                                    ppatient->duree_examen_complementaire=genererDureeTache(psalle->d_esperance,psalle->d_ecart);//génération auto de la durée de la tâche
                                                }
                                                //enregistrement des éléments qui serviront à la production des statistiques
                                                psalle->ordre[psalle->indice]=ppatient->id;
                                                psalle->tab_durees[psalle->indice]=ppatient->duree_examen_complementaire;
                                                psalle->tab_id_patients[psalle->indice]=(unsigned short int)ppatient->id;
                                                psalle->indice++;

                                                pbox->heure_liberation=temps_simulation;
                                                ajouterTemps(&(pbox->heure_liberation),ppatient->duree_examen_complementaire); //enregistrement de l'heure de libération de la salle
                                                    if(jour_prochainevenement > pbox->heure_liberation.jour||jour_prochainevenement==0) {
                                                        jour_prochainevenement=pbox->heure_liberation.jour; //si on a aucune prédiction pour le prochain événement, ou une prédiction pour un jour plus éloigné proche, prédiction d'un événement à l'heure de libération du box.
                                                    }
                                                pbox->Patient=ppatient;//On met l'adresse du patient dans le box de la salle actuelle
                                                pbox2->Patient=NULL;//On la supprime du box de la salle précédente
                                                pbox->libre=false;//le box de la salle actuelle devient occupé
                                                pbox2->libre=true;//le box de la salle précédente devient libre

                                                ppatient=NULL;
                                            }
                                        }
                                    }
                                }
                                pbox2=NULL;
                            }
                        }
                        pbox=NULL;

                    }
                    else { /*si la salle n'est pas la salle d'examens complémentaires : donc la salle d'examens principale du circuit long ou court
                    Les salles de cette classe sont précédées par une salle d'attente où elles puisent leurs patients, possèdent des box, et n'ont pas de sda suivante.*/
                        for(int l=0;l < psalle->nombre_boxs;l++) {
                            pbox=psalle->boxs[l]; //pour chaque box de la salle choisie...

                            if(pbox->libre==false && comparerTemps(pbox->heure_liberation,temps_simulation)<=0) { //si le box est occupe et qu'on a atteint l'heure de liberation ...
                                ppatient = pbox->Patient; //on prend le patient du box choisi de notre salle
                                if(ppatient->circuit!=LONG_EXAM_CPT) { //s'il n'est pas admis en salle d'examens complémentaires, on l'enverra à la sortie
                                    modification=true; //enregistrement d'une modification
                                    enqueue(SORTIE,ppatient); //le patient quitte le flux de la simulation. Il est ajouté au bout de la queue de sortie.
                                    ppatient->sortie=temps_simulation;//on enregistre l'heure de sortie du patient dans son champ (temps) sortie.
                                    //AFFICHAGE DE L'HEURE ET DE l'EVENEMENT, ENREGISTREMENT DANS LE FICHIER HISTORIQUE
                                    sprintf(evenement,"Le patient %3d (%s %s) ",ppatient->id,ppatient->prenom,ppatient->nom);
                                    changementminute = notifier_evenement(changementminute,temps_simulation,evenement,msg_sortie,fhistorique);//message notifiant la sortie du patient.

                                    jour_prochainevenement=0; //effacement des prédictions : la simulation est peut-être terminée et on ne veut pas reafficher la date s'il n'y a plus aucun événement.
                                    pbox->Patient = NULL; //on supprime l'adresse du patient du box libéré.
                                    pbox->libre = true; //on indique que le box n'est plus occupé.
                                    ppatient = NULL;
                                }

                            }
                            if(pbox->libre) {//Si le box est libre, on le remplit avec les patients présents dans la salle d'attente précédente.
                                q = psalle->salle_prec->QUEUE; //on prend la queue de la salle_dattente précédente (ici, SDA3 ou SDA4).
                                if(!estVide(q)) { //si la queue de la salle précédente n'est pas vide
                                    modification=true;//On enregistre une modification
                                    ppatient = element_debut_queue(q);//On prend le patient en tête de file
                                    pbox->Patient = ppatient;//on ajoute le patient au box de notre salle
                                    //AFFICHAGE DE L'HEURE ET DE l'EVENEMENT, ENREGISTREMENT DANS LE FICHIER HISTORIQUE
                                    sprintf(msg_salle_avec_boxs,"%s, %s",psalle->message,pbox->message);
                                    sprintf(evenement,"Le patient %3d (%s %s) entre en ",ppatient->id,ppatient->prenom,ppatient->nom);
                                    changementminute = notifier_evenement(changementminute,temps_simulation,evenement,msg_salle_avec_boxs,fhistorique);//affichage du message correspondant à l'événement et à notre box, saut de ligne et affichage de l'heure si ce n'est pas déjà fait

                                    //CALCUL ET ATTRIBUTION DE L'HEURE DE LIBERATION DU BOX
                                    pbox->heure_liberation=temps_simulation;
                                    if(!generation_durees_auto) {
                                        duree = demander_duree(); //on demande la duree a l'utilisateur
                                    }
                                    else{
                                        duree=genererDureeTache(psalle->d_esperance,psalle->d_ecart);
                                    }
                                    ajouterTemps(&(pbox->heure_liberation),duree);

                                    //ENREGISTREMENT DES ELEMENTS NECESSAIRES A L'ETABLISSEMENT ULTERIEUR DES STATISTIQUES
                                    psalle->ordre[psalle->indice]=ppatient->id;
                                    psalle->tab_durees[psalle->indice]=duree;
                                    psalle->tab_id_patients[psalle->indice]=(unsigned short int)ppatient->id;
                                    psalle->indice++;

                                    pbox->libre=false;//Le box passe au statut "occupé"
                                    dequeue(q);//on supprime l'adresse du patient pris de la tête de file de la salle d'attente précédente.
                                        if(jour_prochainevenement > pbox->heure_liberation.jour||jour_prochainevenement==0) {
                                            jour_prochainevenement=pbox->heure_liberation.jour;//on enregistre éventuellement la prédiction d'un événement le jour de libération du box - on affichera alors éventuellement la date en début de boucle le jour venu.
                                        }
                                }
                                q = NULL;
                            }
                         }
                         pbox=NULL;
                    }
                }

            }
            }while(modification); /*S'il y a eu une modification dans le flux, on relance le flux à la même heure. Des places ont pu se libérer !
            Exemple : on essaie d'abord de faire entrer des patients de SDA2 dans salle_examen_IOA alors qu'elle est occupée. Ensuite, on libère salle_examen_IOA.
            Il faut alors retourner en arrière pour retenter de faire entrer un patient en salle d'examen IOA.
            C'est pourquoi, à chaque fois qu'un traitement est effectué sur les salles, on enregistre une modification, et on reboucle à chaque fois qu'une modification est notifiée.
            modification est remise à "faux" au début de cette boucle.
            Il s'agit d'une boucle do{...}while(...);, donc elle s'éxécutera toujours une première fois et continuera uniquement si des modifications ont été enregistrées*/

            temps_simulation_prec=temps_simulation;
            incrementerTemps(&temps_simulation); //On fait passer une minute dans la simulation.
            changementminute=true;
            //si on a changé de jour et qu'un événement va se produire ce jour-ci, affichage de la date.
            if(jour_prochainevenement==temps_simulation.jour && jour_prochainevenement!=0 && temps_simulation_prec.jour!=temps_simulation.jour) {
                /*Afin d'afficher la date seulement quand elle change, on prédit le jour où se produira le prochain événement et on l'enregistre dans jour_prochainevenement
                (en calculant les heures de libération des salles et en choisissant la plus proche de l'instant simulé)
                et on attend que le temps de la simulation atteigne ou dépasse ce jour.
                Cas particulier : si jour_prochainevenement vaut 0 (assignation faite dans le but d'éviter un réaffichage de la date dans certains cas), le traitement n'est pas réalisé.*/
                changementdate=true;//l'activation de ce booléen va provoquer l'affichage de la date en début de boucle, après quoi il repassera à "faux".
                jour_prochainevenement=0; //en passant jour_prochainevenement à 0, on s'assure qu'aucun saut de ligne sera provoqué à la prochaine itération. de la boucle
            }

            for(int i=0; i<4 ; i++) {
                if(!estVide(tableau_salles_attente[i]->QUEUE)) {
                    patientsAttendent(tableau_salles_attente[i]); //On fait passer une minute dans le temps d'attente des patients.
                    /*A chaque fois que le temps de la simulation est incrémenté d'une minute, on ajoute une minute par patient présent dans chaque salle d'attente au champ "temps_attente" de cette dernière.
                    La fonction patientAttendent se charge de ce traitement ; elle est définie dans salle.c*/
                }
            }
        }//fin de la boucle faisant avancer le flux de patients (Tant qu'ils n'étaient pas tous sortis. Désormais, ils le sont tous.)


        printf("\n_____________________________________________________________________________________\nFIN DU FLUX DE PATIENTS\n\n%c",beep);
        fclose(fhistorique); //Fermeture du fichier fhistorique, dans lequel on écrivait l'historique de la simulation.

        //CALCUL DES TEMPS D'ATTENTE MOYENS DANS LES SALLES D'ATTENTE
        for(int i=0; i<4 ; i++) {
               if(tableau_salles_attente[i]->temps_attente!=0) {
                //le champ temps_attente contient, à ce moment, la somme des durees de séjour de chaque patient passé dans la salle
               tableau_salles_attente[i]->temps_attente/=tableau_salles_attente[i]->QUEUE->capacite;
               }
               //on divise le temps d'attente total de chaque salle par le nombre de patients qui y ont attendu
            }
        //TRI DES TABLEAUX DE DUREES DES SALLES_EVEN
        for(int i=0; i<5; i++) {
            trierStats(tableau_salles_even[i]);//la fonction trierStats est définie dans salles.c
            //on range les durees par ordre croissant, de même que les id des patients correspondant
        }


        //AFFICHAGE DU MENU PRINCIPAL DE FIN DE SIMULATION
        requete=0;//variable stockant le caractère saisi par l'utilisateur
        bool confirmation;
        reafficher_menu=false;//booléen permettant de réafficher ou non le menu principal après des commandes. déjà utilisé pour le menu des options
        bool reafficher_menu_stats = false; //booléen permettant de réafficher ou non le menu des statistiques après des commandes
        do {
            printf("\n\nDONNEES DE LA SIMULATION\n_____________________________________________________________________________________\n%c",beep);
            puts("Saisissez l'une des commandes suivantes :");
                puts("\t- 's' pour afficher les statistiques de la simulation");
                puts("\t- 'h' pour afficher l'historique des evenements");
                puts("\t- 'p' pour afficher les horaires de passage d'un patient dans les differents services");
                puts("\t- 'q' pour mettre fin a la simulation");
            saisie_correcte=false;
            do {
                fflush(stdin);
                scanf(" %c",&requete);
                requete = convertir_minuscules(requete);
                switch(requete) { //on utilise la structure de contrôle switch (...){case ...} pour gérer le menu. On définit un cas pour chaque caractère correspondant à une commande proposée, et un cas par défaut pour les caractères inattendus
                    case 'q' :
                    confirmation = yesno_question("Etes-vous sur(e) de vouloir quitter la simulation ?");//Permet d'éviter de perdre les données de la simulation si l'utilisateur s'est trompé de touche
                    saisie_correcte=true;
                    if(confirmation==true) {
                        reafficher_menu=false;//si l'utilisateur confirme sa volonté de quitter la simulation, on ne réaffiche pas le menu principal et on annonce la fin de la simulation.
                         printf("\n\nFIN DE LA SIMULATION\n_____________________________________________________________________________________\n%c",beep);
                    }
                    else {
                        reafficher_menu=true;//si l'utilisateur ne confirme pas, on réaffiche le menu principal.
                    }
                    break;

                    case 's' :
                    saisie_correcte=true;
                    reafficher_menu=true;
                    reafficher_menu_stats=true;//On va afficher le menu des statistiques (le traitement se fait en dehors de ce switch).
                    break;

                    case 'h' :
                    printf("\n\nHISTORIQUE\n_____________________________________________________________________________________\n%c",beep);
                    /*Cette commande permet d'afficher l'historique de la simulation (un affichage en tout point identique à celui qui a été fait "en direct" pendant le flux).

                    Pendant le flux, à chaque fois qu'on affichait un message dans la console, on écrivait également un message similaire dans le fichier "historique.txt".
                    Il s'agit maintenant d'ouvrir ce fichier en mode lecture et d'en afficher le contenu en filtrant certains éléments, comme la date présente au début de chaque ligne.*/
                    char heure[6];//cette chaîne stockera l'heure associée à une ligne (écrite entre le dernier '!' et le premier '$' de la ligne)
                    char heure_prec[6]="ooxoo";//cette chaîne stockera la dernière heure affichée, ce qui permettra de l'afficher uniquement quand elle change
                    if((fhistorique=fopen("historique.txt","r"))) { //Test d'ouverture du fichier en mode lecture. Il faudra gérer le cas où le fichier ne peut être ouvert.
                        char c=fgetc(fhistorique);//on consomme le premier caractère du fichier, un saut de ligne.
                        while(!feof(fhistorique)){//tant que le curseur de lecture n'a pas atteint la fin du fichier ...
                            fscanf(fhistorique,"%c",&c);//lecture caractère par caractère du contenu du fichier.
                            //Disjonction de cas selon le caractère lu :
                            if(c=='!') {/*
                                Entre les deux caractères '!' se trouve la date associée à l'évènement.
                                Elle est nécessaire pour l'affichage de l'historique d'un patient unique mais on ne l'affiche pas dans l'historique complet : on lira plutôt les dates présentes au début de chaque paragraphe.
                                On va donc consommer tous les caractères situés entre deux caractères '!' de la même ligne, ainsi que ces caractères eux-mêmes.
                                De plus, après le deuxième caractère est indiquée l'heure de la tâche que l'on souhaite afficher uniquement la première fois.
                                Elle s'étend jusqu'au premier caractère '$'. Ainsi, tous les caractères situés entre le deuxième '!' et '$' seront copiés dans la chaîne heure, que l'on traitera ensuite.*/

                                do{ //si on rencontre un '!', on parcourt le fichier jusqu'à trouver le prochain sans rien ecrire. (on ignore la date écrite au début de chaque ligne, elle sert pour l'historique d'un patient unique)
                                  fscanf(fhistorique,"%c",&c);//on consomme le caractère mais on ne l'utilise pas. Le curseur de lecture avance quand-même.
                                }while(c!='!');//On a rencontré le deuxième '!'.
                                int i=0;
                                c=fgetc(fhistorique);//On lit le premier caractère après le deuxième '!'
                                while(c!='$'){//On lit un caractère. Tant que ce n'est pas '$', on le copie dans la chaîne heure.
                                    heure[i] = c;//ensuite, on lit jusqu'au prochain caractere '$' : c'est l'heure de début de la tâche.
                                    i++;
                                    c=fgetc(fhistorique);
                                };
                                heure[6]=0;//On termine heure par le car
                                if(strcmp(heure,heure_prec)!=0){ //si l'heure n'a pas déjà été affichée, on l'affiche.
                                    printf("%s",heure);
                                    strcpy(heure_prec,heure); //on enregistre l'heure affichée dans l'heure précédente pour ne pas la réafficher.
                                }
                                else{
                                    printf("     ");//sinon, on l'ignore et on affiche du vide à la place.
                                }
                            }
                            else if (c!='$'){ //on ignore les caractères '$'
                                putchar(c); //Tous les autres caractères sont affichés dans l'ordre où ils se trouvent dans le fichier.
                            }
                        }
                        fclose(fhistorique);//Le fichier historique est fermé à la fin de la lecture.
                    }
                    else {
                        puts("Erreur lors de l'ouverture du fichier contenant l'historique de la simulation."); //en cas d'échec d'ouverture de historique.txt, on prévient l'utilisateur.
                    }
                    saisie_correcte=true;
                    reafficher_menu=true;
                    break;

                    case 'p' :
                    fflush(stdin);
                    bool encore=true; //on utilisera le booléen "encore" pour reboucler pour un autre patient, afin d'éviter à l'utilisateur d'avoir à saisir une nouvelle fois les commandes.
                    do {
                        puts("\nListe des patients de la simulation : \n");
                        for(int i=0;i<nombre_patients;i++) {
                            ppatient = id_getPatient(i+1,liste_patients,nombre_patients);
                            printf("Patient %3d (%s %s)\n",ppatient->id,ppatient->prenom,ppatient->nom);
                        }

                        printf("\nVeuillez entrer le numero correspondant au patient choisi dans la liste : "); //affichage de la liste des patients pour sélection de l'un d'entre eux
                        int ID;
                        scanf("%d",&ID); //saisie de l'id d'un patient de la liste pour afficher son historique
                        while(!id_getPatient(ID,liste_patients,nombre_patients)) {
                            printf("\nAucun patient correspondant. Assurez-vous de bien saisir un nombre compris entre 1 et %d.\n",nombre_patients);
                            fflush(stdin);
                            scanf("%d",&ID);//gestion d'une saisie incorrecte
                        }
                        fflush(stdin);
                        printf("\nHORAIRES DE PASSAGE DANS LES DIFFERENTS SERVICES : \n_____________________________________________________________________________________\n%c",beep);
                        /*Cette commande permet de sélectionner un patient de la simulation pour afficher son historique.

                        L'historique de toute la simulation est enregistré dans un fichier texte créé lors de la simulation.
                        Pour lire l'historique d'un seul patient, on va lire toutes les lignes de l'historique en repérant uniquement celles qui concernent le patient choisi.
                        La date de passage sera également indiquee a chaque ligne du fichier, et affichee si la derniere date affichee etait differente.
                        Ainsi, on gère le cas des patients qui entre un jour et sortent le lendemain.*/

                        if((fhistorique=fopen("historique.txt","r"))) { //si le fichier historique.txt a pu être ouvert en lecture...
                            char c;
                            char chaine_patient_choisi[31]; //Cette chaine contiendra "Le patient ID" avec l'id du patient correspondant. Elle sert à repérer les lignes correspondant au patient choisi
                            char *occurence; //Cette chaine contiendra tour à tour chaque ligne du fichier historique.
                            char *curseur; //ce pointeur vers un caractere sera utilise pour localiser le caractere de debut et de fin des differentes parties d'une ligne du fichier.
                            char heure[6]; //cette chaine stockera les chiffres composant l'heure a laquelle la tache est effectuee. exemple : "10:03\0"
                            char heure_prec[6]="ooxoo";//permet de stocker l'heure précédente pour mettre un évidence un possible changement d'heure
                            char date_prec[11]="00/00/0000"; //permet de stocker la date précédente pour mettre en évidence un possible changement de date
                            char date[11];//cette chaine stocke la date indiquee au debut de la ligne, correspondant a la date à laquelle la tâche commence
                            ppatient = id_getPatient(ID,liste_patients,nombre_patients);//présentation du patient choisi.
                            printf("Parcours du patient %d (%s %s) :\n",ppatient->id,ppatient->prenom,ppatient->nom);
                            while(!feof(fhistorique)){//tant que la fin de l'historique n'est pas atteinte
                                fgets(evenement,sizeof(evenement),fhistorique);//lire une ligne du fichier et l'enregistrer dans la chaine "evenement"
                                sprintf(chaine_patient_choisi,"Le patient %3d (",ID); //on crée une chaine : Le patient ID ...
                                occurence = strstr(evenement,chaine_patient_choisi);//on sélectionne les lignes contenant : Le patient ID...
                                if(occurence!=NULL) { //si la ligne ne contient pas "chaine_patient_choisi" càd s'il n'y a aucune occurence de cette chaîne dans evenement ...
                                    curseur=strchr(evenement,'!')+1*sizeof(char);//on va chercher la date (qui commence après le premier caractère !). et regarder si il faut la reafficher. curseur pointe vers le premier caractère de la date dans la ligne.
                                    /*on a fait le choix d'utiliser des fonctions de recherche de caractere et non directement des indices correspondant à la position des caractères dans la chaîne au cas où il serait nécessaire de modifier
                                    , à l'avenir, le contenu du fichier historique*/
                                    int i = 0;
                                    do{
                                        c=curseur[i];
                                        if(c!='!') date[i]=c;//on écrit dans date tous les caractères à partir de curseur, jusqu'à rencontrer "!", que l'on ignore
                                            i++;
                                        }while(c!='!');
                                        date[10]='\0';
                                        if(strcmp(date_prec,date)!=0) { //si la date de la tache a change par rapport a la derniere date affichee
                                            printf("\n\tLe %s :",date); //afficher la nouvelle date
                                            strcpy(date_prec,date); //mettre a jour date_prec pour se rappeler qu'on a deja affiché la nouvelle date.
                                        }

                                        i=0;
                                        curseur = strrchr(evenement,'!')+1*sizeof(char); //on va chercher le second '!'
                                        do{//on va recuperer et afficher l'heure, de '!' jusqu'à '$' :
                                            c=curseur[i];
                                            if(c!='$'){heure[i]=c;
                                            i++;}
                                        }while(c!='$');
                                        heure[i]='\0';
                                        curseur = strrchr(evenement,'$')+1*sizeof(char);//on prend tout ce qu'il reste après le second $ (donc la salle)
                                        curseur[0] = convertir_majuscules(curseur[0]);//conversion en majuscules de la première lettre de la salle pour faire joli.

                                        if(strcmp(heure_prec,heure)!=0) { //on affiche l'heure uniquement quand elle change
                                            strcpy(heure_prec,heure);
                                            printf("\n%s : %s",heure,curseur); //affichage de l'heure et de tout le reste de "événement" à partir du deuxième $, donc la salle.
                                        }
                                        else {
                                            printf("      : %s",curseur);//sinon, on ne reaffiche pas l'heure
                                        }
                                    }
                                }
                                fclose(fhistorique);//fermeture du fichier historique
                            }
                            else {
                                puts("Erreur lors de l'ouverture du fichier contenant l'historique de la simulation.");//gestion du cas où le fichier historique n'aurait pas été produit ou échouerait à s'ouvrir
                            }
                            encore=yesno_question("\n\nVoulez-vous afficher les horaires de passage d'un autre patient ?"); //proposition de reboucler pour éviter à l'utilisateur d'avoir à naviguer dans les menus
                        }while(encore);

                        saisie_correcte=true;
                        reafficher_menu=true;
                        reafficher_menu_stats=false;
                        break;

                    default :
                    puts("Saisie incorrecte. Veuillez reessayer :");
                    saisie_correcte=false;
                    break;
                }

            }while(!saisie_correcte);

            do { //MENU SECONDAIRE (STATISTIQUES)
            /*
                Menu utilisé pour afficher des statistiques à la demande de l'utilisateur.
                Séparé du reste pour ne pas surcharger l'affichage, car il y a beaucoup de commandes possibles.
                Comme le menu est réaffiché instantanément après chaque commande (pour limiter le nombre de commandes à entrer par l'utilisateur qui voudra sûrement afficher plusieurs stats différentes), il faut remonter pour voir les résultats.

                Nous avons choisi d'intégrer ces fonctionnalités directement dans le main car elles sont très spécifiques et on n'y a recours qu'à cet endroit du programme - utiliser des fonctions aurait pu libérer de la place
                dans le main, mais aurait compliqué la compréhension et le déboggage du code.
            */
            if(reafficher_menu_stats) {
                do{
                    printf("\n\nSTATISTIQUES :\n_____________________________________________________________________________________\n%c",beep);
                    puts("Saisissez l'une des commandes suivantes :");
                        puts("\t- 'm' pour retourner au menu principal.");
                        puts("\t- 'a' pour afficher l'ordre d'arrivee et de sortie des patients ainsi que le temps passe aux urgences.");
                        puts("\t- 's' pour afficher le temps d'attente moyen pour chaque salle d'attente");
                        puts("\t- 'o' pour afficher l'ordre de passage des patients dans chaque service");
                        puts("\t- 't' pour calculer le temps total, moyen et median passe aux urgences");
                        puts("\t- 'p' pour afficher le pourcentage et la liste de patients ayant emprunte chaque circuit");
                        puts("\t- 'd' pour afficher des statistiques relatives a la duree de chaque tache");

                    fflush(stdin);
                    scanf(" %c",&requete);
                    requete = convertir_minuscules(requete);
                    //le menu prend encore une fois la forme d'une saisie de caractère suivie d'un switch...case..., avec saisie sécurisée.
                    switch(requete) {
                        case 's' :; //Le compilateur couine parfois quand le case est suivi d'une affectation, donc dans le doute (si on est amenés à modifier la structure du cas) nous mettons un ';' - empty statement - après le ':' pour avoir la paix
                        printf("\n\nTEMPS D'ATTENTE MOYEN DANS CHAQUE SALLE D'ATTENTE\n_____________________________________________________________________________________\n%c",beep);
                        /*Permet d'afficher le temps d'attente moyen en jours, heures, minutes et secondes passé dans chaque salle d'attente.

                        Celui-ci peut très bien être nul s'il y a peu de patients sur une grande plage horaire - surtout dans les salles 3 et 4 qui ne recoivent pas tous les patients,
                        ou astronomique s'il y a un grand nombre de patients sur une petite plage horaire - surtout dans les salles 1 et 2 où tous les patients doivent passer.

                        Les traitements sont réalisés au moyen du champ "(double) temps_attente" de chaque salle_dattente, qui contient le temps total attendu par les patients d'une salle, en minutes.
                        A la fin du flux, cette durée est divisee par le nombre de patients passe par la salle - il s'agit donc du temps d'attente moyen. Il n'y a plus qu'à l'afficher.
                        */
                        saisie_correcte=true; //la saisie correspond bien à une commande disponible - pas besoin de redemander à l'utilisateur de saisir une commande après le switch.
                        double temps_moy;
                        for (int i=0;i<4;i++) {
                            unsigned char jours = 0;
                            unsigned char heures=0;
                            unsigned char minutes=0;
                            unsigned char secondes=0;
                            temps_moy=tableau_salles_attente[i]->temps_attente; /*pendant la simulation, on a enregistré dans chaque salle d'attente le temps (en minutes, réel) attendu par chaque patient quittant la salle et on l'a sommé.
                            On disposait donc du temps total d'attente dans la salle d'attente.
                            A la fin du flux, nous avons divisé ce temps total d'attente par le nombre de patients passés par là et avons écrasé sda->temps_attente.
                            Nous avons donc déjà le temps moyen d'attente, qu'il suffit d'affecter à temps_moy puis de convertir en heures, minutes et secondes pour affichage dans la console.*/

                            //temps moy : temps moyen d'attente dans une sda EN MINUTES (nombre REEL (format avec des secondes adaptés pour une moyenne))
                            jours = ((int)temps_moy)/1440; //1440 minutes dans un jour
                            temps_moy-=1440*jours; //on retire ensuite tout ce qui est passé dans le compteur de jours
                            heures = ((int)temps_moy)/60; //60 minutes dans une heure
                            temps_moy-=60*heures;
                            minutes = (int)temps_moy;//il reste entre 0 et 59.999... minutes
                            temps_moy-=minutes;
                            secondes=(int)(temps_moy*60); /*il ne reste que les chiffres après la virgule. On multiplie par 60 pour avoir le nombre de secondes. 0.5 minute --> 0.5*60 = 30 secondes*/
                            printf("\tSalle d'attente %d : ",i+1);
                            char jplur[3]= {'s',',','\0'};
                            char hplur[3]= {'s',',','\0'};
                            char mplur[3]= {'s','\0','\0'};
                            char splur[3]= {'s','.','\0'}; /*gestion des pluriels / singuliers pour éviter d'afficher "1 jours, 1 heures, 1 minutes et 1 secondes"
                            on créée une chaîne "s" à mettre à la fin de chaque unité; si il y a exactement une unité, on remplace le 's' par ','.
                            Donc, si singulier, plur contient "," Sinon, plur contient "s,".
                            On met '\0' au lieu de ',' après les minutes (car suivies d'un "et").
                            Cas particulier : on met '.' au lieu de ',' après les secondes.
                            Comme ça, on reste en de bons termes avec Molière.*/
                            hplur[0]=(heures==1)?',':'s'; hplur[1]=(heures==1)?'\0':',';
                            mplur[0]=(minutes==1)?'\0':'s'; mplur[1]=(minutes==1)?'\0':'\0';
                            splur[0]=(secondes==1)?'.':'s'; splur[1]=(secondes==1)?'\0':'.';
                            //Affichage d'un message dépendant des unités de valeur non nulles : on ne veut pas afficher de 0 avant la première unité significative.
                            if(jours >0 ){
                                jplur[0]=(jours==1)?',':'s';jplur[1]=(jours==1)?'\0':',';
                                printf("%d jour%s %d heure%s %d minute%s et %d seconde%s",jours,jplur,heures,hplur,minutes,mplur,secondes,splur);
                            }
                            else if(heures>0) {
                                printf("%d heure%s %d minute%s et %d seconde%s",heures,hplur,minutes,mplur,secondes,splur);}
                            else if(minutes>0){
                                printf("%d minute%s et %d seconde%s",minutes,mplur,secondes,splur);}
                            else if(secondes>0){
                                printf("%d seconde%s",secondes,splur);}
                            else{
                                printf("aucune attente");}
                            putchar('\n');
                        }
                        reafficher_menu_stats=true; //et après, on réaffiche le menu des stats.
                        break;

                        case 't' :;
                    {
                        printf("\n\nTEMPS PASSE AUX URGENCES :\n_____________________________________________________________________________________\n%c",beep);
                        /*Statistiques relatives au temps total/moyen/médian passé aux urgences.
                        Il est souvent pertinent d'indiquer le temps moyen, très sensible aux valeurs extrêmes, et le temps médian (la moitié des patients a attendu moins longtemps que ce temps, l'autre moitié plus)
                        L'écart-type est également une donnée pertinente lorsqu'on commence à avoir beaucoup de patients : il permet de voir s'il existe de grandes disparités entre le temps de traitement des patients.

                        Les traitements sont faits à partir des heures d'arrivée et de sortie des patients de liste_patients.
                        Les fonctions utilisées pour décomposer, recomposer, afficher, et calculer des durées de type "temps" sont définies dans temps.c.
                        */

                        saisie_correcte=true;
                        temps t_total; t_total.annee=t_total.mois=t_total.jour=t_total.h=t_total.m=0; //on créée un "temps nul" auquel on va "additionner" le temps passé par chaque patient.
                        temps durees[nombre_patients];
                        /*On créée des pointeurs ppatientmin et ppatientmax vers les patients ayant respectivement passé le moins et le plus de temps aux urgences.
                        On les fait correspondre aux temps tmin et tmax qui auront pris leur valeur finale à la fin de la boucle qui parcourra l'ensemble des patients.*/
                        patient *ppatientmin,*ppatientmax;
                        ppatientmin=liste_patients[0]; ppatientmax=liste_patients[nombre_patients-1];
                        temps tmin, tmax; tmax=t_total; //on profite du temps total initialisé à des valeurs nulles pour initialiser tmax à des valeurs nulles - même si ce n'est pas très élégant.
                        tmin = diffTemps(liste_patients[0]->arrivee,liste_patients[0]->sortie);//on initialise tmin à la durée correspondant au premier patient de notre liste - on retiendra ensuite les patients pour lesquels la durée de séjour est inférieure.
                        for(int g=0; g<nombre_patients;g++) {
                            ppatient=liste_patients[g];
                            duree=diffTemps(ppatient->arrivee,ppatient->sortie); //le temps total passé aux urgences vaut heure de sortie - heure d'arrivée. On calcule cette différence avec la fonction diffTemps et on l'affecte au temps "duree"
                            if(comparerTemps(duree,tmin)==-1) { //on a trouvé un temps inférieur à tmin ?
                                    tmin = duree;               //on enregistre le nouveau minimum dans tmin
                                    ppatientmin=ppatient;       //et on retient l'adresse du nouveau patient ayant attendu le moins longtemps dans ppatientmin
                            }
                            if(comparerTemps(duree,tmax)==1) { //on a trouvé un temps supérieur à tmax ?
                                    tmax = duree;               //on enregistre le nouveau maximum dans tmax
                                    ppatientmax=ppatient;       //et on retient l'adresse du nouveau patient ayant attendu le plus longtemps dans ppatientmax
                            }
                            ajouterTemps(&t_total,duree); //puis on ajoute ce temps au  temps total grâce à la fonction ajouterTemps qui incrémente la valeur du premier paramètre et décrémente le deuxième paramètre jusqu'à ce qu'il soit nul (temps.c)
                            durees[g]=duree; //on met également "duree" dans un tableau de durees pour chaque patient - c'est nécessaire pour retrouver les valeurs extrêmes, l'écart-type et la médiane.

                        }

                        trierDurees(durees,nombre_patients); //la fonction trierDurees de temps.c TRIE par ordre de durées croissante le tableau durees. Ce tri est nécessaire pour trouver les valeurs extrêmes et la médiane.
                        printf("\tTotal : ");dureeMoyenne(t_total,1); /*La fonction dureeMoyenne de temps.c décompose un temps en minutes, puis le divise par le second paramètre avant de recomposer un temps et de
                        l'afficher sous la forme j jours, h heures, m minutes et s secondes - de manière optimisée (pas de 0 avant les valeurs significatives etc). Ici on divise le temps total par 1 - c'est un raccourci.*/
                        printf("\tMoyen : ");dureeMoyenne(t_total,nombre_patients);//duree moyenne : temps_total(minutes)/nombre_total_patients
                        printf("\tEcart-type (minutes) : %.2lf\n",ecart_type(durees,nombre_patients));/*la fonction ecart-type de temps.c recalcule la moyenne, puis fait la somme des carrés des écarts de chaque élément de durees[] à la moyenne.
                        Elle renvoie un nombre réel de minutes.*/
                        printf("\tMedian : ");dureeMoyenne(durees[(nombre_patients+1)/2-1],1); /* Le temps médian est celui contenu par la case du milieu - ou celle d'avant la moitié du tableau trié dans le cas d'un nombre pair de patients.
                        Exemple : Pour 10 patients, la case choisie sera la cinquieme (durees[4]) : 11/2-1 --> int(5,5)-1 = 4. Un peu plus de la moitié des patients aura attendu plus longtemps que le temps médian.
                        Pour 9 patients, on tombe pile sur la cinquième case : 10/2-1--> 4. il y a 4 patients qui auront attendu moins longtemps, et 4 patients qui auront attendu plus longtemps.*/
                        if(nombre_patients>1){//on décide d'afficher les valeurs extrêmes uniquement s'il y a plus d'un patient. Sinon, le minimum et le maximum sont les mêmes.
                            printf("\tMinimal : Patient %d (%s %s) : ",ppatientmin->id,ppatientmin->prenom,ppatientmin->nom);dureeMoyenne(durees[0],1);//Le temps min sera celui qui se trouvera à la première case.
                            printf("\tMaximal : Patient %d (%s %s) : ",ppatientmax->id,ppatientmax->prenom,ppatientmax->nom);dureeMoyenne(durees[nombre_patients-1],1);//le temps max sera celui de la dernière case du tableau de durées trié.
                        }
                    }
                        bool afficher_temps_indiv;
                        if(nombre_patients>=7){ //s'il y a plus de 7 patients, on laisse le choix à l'utilisateur pour l'affichage des durees individuelles - sinon on pourrait encombrer la console.
                            afficher_temps_indiv = yesno_question("\nVoulez-vous afficher le temps passe aux urgences par chaque patient ?");
                        }
                        else{
                            afficher_temps_indiv=true;
                        }
                        if(afficher_temps_indiv){
                            printf("\nTemps passe aux urgences par chaque patient : \n");
                            for(int i=0; i<nombre_patients; i++){
                                ppatient=liste_patients[i];
                                printf("\tPatient %3d (%s %s) : ",ppatient->id,ppatient->prenom,ppatient->nom);dureeMoyenne(diffTemps(ppatient->arrivee,ppatient->sortie),1);
                            }
                        }
                        reafficher_menu_stats=true; //terminé, affichage du menu des stats.
                        break;

                        case 'p' :;
                        saisie_correcte=true;
                        printf("\n\nSTATISTIQUES RELATIVES AUX CIRCUITS\n_____________________________________________________________________________________\n%c",beep);
                        /*Cette commande affiche le pourcentage de patients ayant emprunté chaque circuit, ainsi que la liste des patients les ayant empruntés.

                        Les traitements sont réalisés à partir de liste_patients, un tableau de patients, et du nombre de patients passé par chaque circuit (nb_patients_cc, nb_patients_cl ...) enregistré avant le flux.*/
                        int pourcentage=0; //nous voulons afficher un pourcentage entier.
                        if(nb_patients_cc>0){
                            pourcentage=100*nb_patients_cc/nombre_patients; //nombre de patients enregistrés en circuit court sur nombre total de patients. On multiplie ce ratio par 100 et on obtient un pourcentage - arrondi a l'entier inferieur.
                            printf("\nCircuit COURT : %d%% des patients.\n",(int)pourcentage);
                            for (int i=0;i<nombre_patients;i++) {
                                if(liste_patients[i]->circuit==COURT) { //à chaque fois, on parcourt la liste de tous les patients de la simulation et on affiche uniquement ceux qui sont passés dans le circuit concerné.
                                    printf("\tPatient %3d : (%s %s)\n",liste_patients[i]->id,liste_patients[i]->prenom,liste_patients[i]->nom); /*Afin de garder un affichage esthétique (toutes les lignes au même niveau) on utilise tout au long du programme le %3d pour que les
                                    numéros de patients à 1 chiffres, 2 chiffres et 3 chiffres prennent la même place dans la chaîne. Nous n'allons pas jusqu'à 4 chiffres car on considère que l'utilisateur n'y aura probablement pas recours - et l'espace affiché serait trop grand*/
                                }
                            }
                        }
                        if(nb_patients_cl>0){
                            pourcentage=100-pourcentage; //tous les patients qui ne sont pas allés en circuit court sont allés en circuit long.
                            printf("\nCircuit LONG : %d%% des patients.\n",pourcentage);
                            for (int i=0;i<nombre_patients;i++) {
                                if(liste_patients[i]->circuit==LONG_SIMPLE) {
                                    printf("\tPatient %3d : (%s %s)\n",liste_patients[i]->id,liste_patients[i]->prenom,liste_patients[i]->nom);
                                }
                            }
                            if(nb_patients_cl_cpt>0) {
                                printf("\t%d%% ont subi des examens complementaires (%d%% du total).\n",100*nb_patients_cl_cpt/nb_patients_cl,100*nb_patients_cl_cpt/nombre_patients);
                                //On affiche le pourcentage de patients du CL ayant eu des examens complémentaires puis le pourcentage de patients ayant eu un examen complémentaire sur l'ensemble de la simulation.
                                for (int i=0;i<nombre_patients;i++) {
                                    if(liste_patients[i]->circuit==LONG_EXAM_CPT) {
                                        printf("\t\tPatient %3d : (%s %s)\n",liste_patients[i]->id,liste_patients[i]->prenom,liste_patients[i]->nom);
                                    }
                                }
                            }
                        }
                        reafficher_menu_stats=true;
                        break;

                        case 'd' :;
                        saisie_correcte=true;
                        printf("\n\nSTATISTIQUES DES TACHES REALISEES\n_____________________________________________________________________________________\n%c",beep);
                        /*Cette commande affiche, pour chaque salle_even, la durée de séjour des patients ainsi que des statistiques relatives à l'ensemble de ces durees.

                        Ces données sont traitées à partir de deux tableaux appartenant aux salle_even :
                        un tableau de durées trié par ordre de durées croissantes (tab_durees) et un tableau d'id de patients correspondant (tab_id_patients)*/
                        temps duree_totale;
                        char tache[30];
                        for(int g=0;g<5;g++) {
                            psalle=tableau_salles_even[g]; //on considère chaque salle_even parmi le tableau de salles utilisé pour contrôler le flux durant la simulation
                            if(psalle==S_ENREG) strcpy(tache,"Enregistrement"); //On donne un "titre" à chaque sous-partie, correspondant à la tâche réalisée.
                            else if(psalle==S_EXAMEN_IOA) strcpy(tache,"Examen IOA");
                            else if(psalle==S_EXAMEN_LONG) strcpy(tache,"Examen (circuit long)");
                            else if(psalle==S_EXAMEN_COURT) strcpy(tache,"Examen (circuit court)");
                            else strcpy(tache,"Examen complementaire");
                            printf("%s :\n",tache);

                            if(psalle->indice > 1) {
                                duree_totale=dureeTotale(tableau_salles_even[g]);
                                /*La fonction dureeTotale(salle_even *S) est définie dans salles.c.
                                Elle créée un temps "nul", lui additionne les durées de séjour de chaque patient entré dans la salle (stockées dans S->tab_durees) et le retourne.*/
                                printf("\tDuree totale : ");dureeMoyenne(duree_totale,1);
                                printf("\tDuree moyenne : ");dureeMoyenne(duree_totale,psalle->indice);
                                //on divise le temps total par le champ "indice" de la salle_even, qui a été initialisé à 0 et incrémenté à chaque fois qu'un nouveau patient l'occupait - il vaut donc le nombre de patients entrés dans la salle.
                                printf("\tEcart-type (minutes) : %3.2lf\n",ecart_type(psalle->tab_durees,(unsigned short int)psalle->indice));
                                //pour l'écart-type, on recalcule la moyenne à partir du tab_durees de la salle, puis la somme des écarts des carrés des valeurs du tableau à cette moyenne. (les durées sont converties en minutes (réelles))
                                printf("\tDuree mediane : ");dureeMoyenne(psalle->tab_durees[(psalle->indice+1)/2-1],1);
                                //La mediane correspond à l'indice de milieu du tableau tab_durees de la salle -- pour rappel, il a été trié par ordre de durées croissantes après la fin du flux.

                                if(comparerTemps(psalle->tab_durees[psalle->indice-1],psalle->tab_durees[0])!=0) { //si le temps max et le temps min sont différents
                                    ppatient = id_getPatient(psalle->tab_id_patients[0],liste_patients,nombre_patients);
                                    /*La fonction id_getPatient(unsigned short int ID,patient** liste_patients,int nombre_patients) de patient.c prend en paramètre l'id d'un patient, la liste de tous les patients et sa taille,
                                    Et trouve, à partir de cet id, le patient correspondant dans la liste.
                                    Ainsi on peut se contenter de stocker les id (int) des patients par ordre de durées de séjour croissantes dans la salle_even*/
                                    printf("\tDuree de la tache pour chaque patient : \n");
                                    //comme le tableau tab_durees de la salle est trié, le minimum correspond à la première case et le maximum à la dernière case.
                                    /*Les id de patients se trouvent, par ordre de durées croissantes, dans le champ tab_id_patients de la salle.
                                    Donc pour le même indice, on peut avoir l'id du patient (dans tab_id_patients) et sa durée de séjour dans la salle (tab_durees)*/
                                    printf("\t\tDuree minimale : Patient %3d : (%s %s) : ",ppatient->id,ppatient->prenom,ppatient->nom);dureeMoyenne(psalle->tab_durees[0],1);
                                    ppatient = id_getPatient(psalle->tab_id_patients[psalle->indice-1],liste_patients,nombre_patients);
                                    printf("\t\tDuree maximale : Patient %3d : (%s %s) : ",ppatient->id,ppatient->prenom,ppatient->nom);dureeMoyenne(psalle->tab_durees[psalle->indice-1],1);
                                }
                                for(int h=0;h< psalle->indice;h++){
                                    //affichage de tous les patients passés par la salle et de leur durée de séjour
                                    ppatient = id_getPatient(psalle->tab_id_patients[h],liste_patients,nombre_patients);
                                    printf("\t\tPatient %3d (%s %s) : ",ppatient->id,ppatient->prenom,ppatient->nom);dureeMoyenne(psalle->tab_durees[h],1);

                                }
                            }
                            else if(psalle->indice ==1) { //pas besoin d'afficher toutes ces stats pour un seul patient ! Ce n'est pas intéressant
                                ppatient = id_getPatient(psalle->tab_id_patients[0],liste_patients,nombre_patients);
                                //Message personnalisé si un seul patient est passé par là
                                printf("\tSeul le patient %3d (%s %s) est passe par la %s : ",ppatient->id,ppatient->prenom,ppatient->nom,psalle->message);dureeMoyenne(psalle->tab_durees[0],1);
                            }
                            else {
                                //message personnalisé si la salle n'a accueilli aucun patient.
                                printf("\tLa %s n'a pas ete utilisee.\n",psalle->message);
                            }


                        }
                        reafficher_menu_stats=true;
                        break;

                        case 'o' :
                        printf("\n\nORDRE DE PASSAGE DANS CHAQUE SERVICE\n_____________________________________________________________________________________\n%c",beep);
                        /*Cette commande affiche, pour chaque salle_even, les patients entrés dans la salle dans l'ordre. Elle est utile pour faire des vérifications du bon fonctionnement du programme.

                        Les patients qui entrent dans une salle_even sont conservés dans son champ "ordre", tableau de (patient*), qui contient les id des patients qui y sont entres, classe par ordre d'insertion.
                        A partir de leur ID et de la liste de tous les patients de la simulation, on peut ensuite recuperer des pointeurs vers les patients correspondants pour afficher leurs données.*/

                        char service[30];//chaine qui stockera un message personnalisé correspondant à une salle_even
                        for(int g=0;g<5;g++) {
                            putchar('\n');
                            psalle=tableau_salles_even[g]; //pour chaque salle du tableau de salles_even ...
                            if(psalle==S_ENREG) strcpy(service,"Enregistrement");
                            else if(psalle==S_EXAMEN_IOA) strcpy(service,"Examen IOA");
                            else if(psalle==S_EXAMEN_LONG) strcpy(service,"Examen (circuit long)");
                            else if(psalle==S_EXAMEN_COURT) strcpy(service,"Examen (circuit court)");
                            else strcpy(service,"Examen complementaire");
                            printf("%s :\n",service);

                            if(psalle->indice > 0) { /*Le champ indice valait initialement 0 et a été incrémenté à chaque entrée d'un patient. Il correspond donc à la taille de "ordre".
                                Si plus d'un patient est entré dans la salle ...*/
                                for(int i=0; i < psalle->indice ;i++) { //on parcourt "ordres" pour récupérer l'id de tous les patients qui sont passés par ce service
                                    ppatient=id_getPatient(psalle->ordre[i],liste_patients,nombre_patients); //la fonction id_getPatient de patient.c permet de récupérer un pointeur vers un patient à partir de son ID et de la liste des patients.
                                    printf("\tPatient %3d (%s %s)\n",ppatient->id,ppatient->prenom,ppatient->nom); //affichage du patient
                                }
                            }
                            else { //Sinon, càd. si aucun patient n'est entré dans la salle ...
                                printf("\tAucun patient n'est entre en %s.\n",psalle->message);//Message personnalisé
                            }

                        }
                        saisie_correcte=true;
                        reafficher_menu=true;

                        reafficher_menu_stats=true;
                        break;

                        case 'a' :
                        printf("\n\nORDRE D'ARRIVEE ET DE SORTIE\n_____________________________________________________________________________________\n%c",beep);
                        /*Cette commande affiche l'ordre dans lequel les patients sont arrivés et l'heure heure d'arrivée,
                        l'ordre dans lequel ils sont sortis et leur heure de sortie, le temps total passé aux urgences pour chaque patient,
                        ainsi qu'un message personnalisé si les patients ne sont pas entrés et sortis dans le même ordre.

                        */
                        puts("Ordre d'arrivee :");//Affichage de l'ordre d'arrivée des patients
                        temps t;
                        char date_prec[11]="00/00/0000"; //chaîne qui retiendra la date d'arrivee ou de sortie précédente pour éviter de l'afficher plusieurs fois
                        char date[11];//chaine qui contiendra la date d'arrivee ou de sortie d'un patient
                        char heure[6];//chaine qui contient l'heure d'arrivee ou de sortie d'un patient.
                        bool meme_ordre = true;
                        for(int g=0;g<nombre_patients;g++) {
                            ppatient=liste_patients[g]; //liste_patients contient des ptr vers les patients triés par ordre d'arrivée (triés au prealable par la fonction trierPatients de patient.c).
                            //nb : on ne peut pas utiliser les éléments de la queue ENTREE car elle est vidée de ses patients au fur et à mesure qu'ils entrent à l'hôpital
                            t=ppatient->arrivee; //ppatient et t sont des variables intermédiaires servant à simplifier l'affichage des informations concernant les patients et leurs heures d'arrivée et de sortie.
                            sprintf(date,"%02d/%02d/%04d",t.jour,t.mois,t.annee);//ecriture de la date d'arrivee dans la chaine date
                            if(strcmp(date,date_prec)!=0) {
                                printf("\n\t\tLe %s :\n",date); //affichage de la date si changement de date.
                                strcpy(date_prec,date);         //enregistrement de la date affichee dans date_prec pour ne pas la reafficher
                            }
                            sprintf(heure,"%02d:%02d",t.h,t.m);//ecriture de l'heure d'arrivee du patient *ppatient dans la chaine heure
                            printf("\t%3d - %s : Patient %3d (%s %s)\n",g+1,heure,ppatient->id,ppatient->prenom,ppatient->nom);//affichage du rang du patient dans la queue d'entrée, de son heure d'arrivee et de son id + prenom + nom
                            if(meme_ordre&&ppatient != SORTIE->elements[g]) {// si un des patients pris dans l'ordre d'arrivee ne correspond pas au patient de même indice dans la queue de sortie, meme_ordre devient faux et ne peut plus changer.
                                meme_ordre=false;
                            }
                        }
                        putchar('\n');
                        puts("Ordre de sortie :");//affichage de l'ordre de sortie
                        strcpy(date_prec,"00/00/0000");
                        for(int g=0;g<nombre_patients;g++) {
                            ppatient=SORTIE->elements[g]; //Le champ elements de la queue SORTIE contient des ptr vers les patients classés dans l'ordre où ils ont quitté l'hôpital. On parcourt le tableau dans l'ordre de sortie :
                            t=ppatient->sortie;//l'heure de sortie a été sauvegardée dans le champ "sortie" du patient.
                            sprintf(date,"%02d/%02d/%04d",t.jour,t.mois,t.annee);//Comme pour l'ordre d'arrivée, affichage de la date seulement lorsqu'elle ne change pas.
                            if(strcmp(date,date_prec)!=0) {
                                printf("\n\t\tLe %s :\n",date);
                                strcpy(date_prec,date);
                            }

                            sprintf(heure,"%02d:%02d",t.h,t.m);
                            printf("\t%3d - %s : Patient %3d (%s %s).",g+1,heure,ppatient->id,ppatient->prenom,ppatient->nom);
                            printf(" Temps passe aux urgences : ");//Affichage, avec l'ordre de sortie, duu temps passé aux urgences
                            t=diffTemps(ppatient->arrivee,ppatient->sortie);// La fonction diffTemps de temps.c calcule la différence entre deux instants : dans cet appel, la différence entre l'heure de sortie et l'heure d'arrivée.
                            //t correspond donc au temps total passé aux urgences par le patient, de son entrée dans la salle d'enregistrement à son ajout dans la queue SORTIE.
                            if(t.mois>0) printf("%dm%dj et %dh%02dmin.",t.mois,t.jour,t.h,t.m); //affichage personnalise selon les differentes unites non nulles de cette duree
                            else if(t.jour>0) printf("%dj et %dh%02dmin.",t.jour,t.h,t.m);
                            else if(t.h>0) printf("%dh%02dmin.",t.h,t.m);
                            else if(t.m>1) printf("%d minutes.",t.m);
                            else if(t.m==1) printf("1 minute.");
                            putchar('\n');
                        }
                        putchar('\n');
                        if(nombre_patients>1) {
                            if(meme_ordre)//Notification de la modification ou non de l'ordre d'arrivée et de sortie des patients.
                                puts("Les patients sont arrives et repartis des urgences dans le meme ordre.");
                            else
                                puts("Les patients ne sont pas arrives et repartis des urgences dans le meme ordre.");
                        }
                        saisie_correcte=true;
                        reafficher_menu=true;
                        reafficher_menu_stats=true;
                        break;

                        case 'm'://Cette commande permet de retourner au menu principal.
                        saisie_correcte=true;
                        reafficher_menu=true;//On reaffiche le menu principal,
                        reafficher_menu_stats=false;//on ne reaffiche pas le menu des statistiques
                        break;

                        default :
                        puts("Saisie incorrecte. Veuillez reessayer :");//gestion des erreurs de saisie.
                        saisie_correcte=false;
                        break;
                        }
                }while(!saisie_correcte);//si saisie_correcte est faux, le programme invite l'utilisateur à saisir de nouveau.
            }
            }while(reafficher_menu_stats);//reaffichage du menu des statistiques apres chaque commande tant que le caractere 'm' n'est pas entree.

        }while(reafficher_menu);//reaffichage du menu principal apres chaque commande ou en sortant du menu des statistiques tant que le caractere 'q' n'est pas entre

        //LIBERATIONS
         if(nombre_patients!=0) {
        /*On effectue la libération de la majorité des pointeurs vers des segments de taille dynamiquement allouée.
        Il faut à chaque fois effectuer des vérifications afin d'éviter de libérer un pointeur de valeur NULL ou un pointeur déjà libéré.*/
        for(int i=0;i<5;i++) {
            if(tableau_salles_even[i]){

                if(tableau_salles_even[i]->nombre_boxs>0) {
                    for(int k=0;k<tableau_salles_even[i]->nombre_boxs;k++) {
                                if(tableau_salles_even[i]->boxs[k]){
                                    free(tableau_salles_even[i]->boxs[k]); //on libère les box des salle_even
                                }
                    }
                }
                if(tableau_salles_even[i]->message){
                    free(tableau_salles_even[i]->message); //ainsi que les messages des salle_even
                }
                if(tableau_salles_even[i]->indice>0) {
                    free(tableau_salles_even[i]->tab_id_patients); //les différents tableaux utilisés pour les statistiques de fin de simulation aussi - si au moins un patient est entré dans la salle, bien sûr
                    free(tableau_salles_even[i]->tab_durees);
                    free(tableau_salles_even[i]->ordre);
                }
                free(tableau_salles_even[i]); //puis on libère la salle elle-même
                }
            }


        for(int i=0;i<4;i++) {
            if(tableau_salles_attente[i]){
                free(tableau_salles_attente[i]->message);free(tableau_salles_attente[i]->QUEUE->elements);free(tableau_salles_attente[i]->QUEUE);free(tableau_salles_attente[i]);
                //pour chaque salle d'attente : on libère le message, les patients de la queue, la queue puis la salle d'attente elle-même.
            }
        }
        if(ENTREE){
            if(ENTREE->elements){
                free(ENTREE->elements);//on libère ensuite les queues d'entrée et de sortie des patients
                }
        free(ENTREE);}
        if(SORTIE){
            if(SORTIE->elements){
                free(SORTIE->elements);
                }
        free(SORTIE);}
        if(liste_patients){
            for(int i = 0 ; i < nombre_patients ; i++){
                free(liste_patients[i]);//Liberation des espaces memoire réservés pour les patients
            }
            free(liste_patients);//libération de la liste de patients elle-même
        }
    }

    quitter=!yesno_question("Voulez-vous commencer une nouvelle simulation ?");//Invite l'utilisateur à lancer une nouvelle simulation sans avoir à relancer le programme.
    if(!quitter){
        printf("\n\nNOUVELLE SIMULATION\n_____________________________________________________________________________________\n%c",beep);
        }
    }

    char beep='\a'; //caractère bip sonore
    putchar(*(&(*(&(*(&(*(&(*(&(*(&(*(&*(&beep)))))))))))))) );//Bip sonore pour saluer l'utilisateur et rallonger le code
    printf("\nMerci d'avoir utilise ce logiciel. Au revoir.\n");
    return 0;
}//FIN DU MAIN




/** \brief Convertit une lettre majuscule en lettre majuscule.
 *
 * \param Un caractère.
 * \return Le même caractère, potentiellement modifié.
 *
 *Utilisée pour rendre les saisies de caractères insensibles à la casse. Tous les caractères saisis sont convertis en minuscules.
 *La conversion s'effectue en ajoutant 32 au caractère entré s'il s'agit d'une lettre majuscule de l'alphabet.
 */
char convertir_minuscules(char c) { //cette fonction permet de convertir un caractère en minuscules au cas où l'utilisateur l'aurait entrée en majuscules
    if(c>='A' && c<='Z')
        return c+32;
    else
        return c;
}

/** \brief Convertit une lettre minuscule en lettre majuscule.
 *
 * \param Un caractère.
 * \return Le même caractère, potentiellement modifié.
 *
 *Utilisée pour optimiser l'affichage de certains des résultats de la simulation.
 *La conversion s'effectue en soustrayant 32 au caractère entré s'il s'agit d'une lettre minuscule de l'alphabet.
 */
char convertir_majuscules(char c) { //cette fonction permet de convertir un caractère en majuscules au cas où l'utilisateur l'aurait entré en minuscules
    if(c>='a' && c<='z')
        return c-32;
    else
        return c;
}

/** \brief Fonction utilisée pour obtenir une réponse positive ou négative de l'utilisateur à une question indiquée en entrée.
 *
 * \param Une chaîne de caractères constituant la question posée à l'utilisateur.
 * \return Retourne un booléen valant true si l'utilisateur a saisi 'o', ou false s'il a saisi 'n'.
 *
 *Fonction d'interaction avec l'utilisateur utilisée régulièrement pour la saisie d'options et la navigation dans la simulation.
 *La fonction ajoute " (o/n) :\n" après la question posée pour indiquer le format attendu de la saisie de l'utilisateur.
 *La saisie est sécurisée et la console est nettoyée.
 *La saisie est également insensible à la casse grâce à l'utilisation de la fonction convertir_minuscules.
 *Une réponse inattendue entraînera une répétition de la saisie.
 */
bool yesno_question(const char *chaine) { /*cette fonction permet d'obtenir une reponse positive ou negative de l'utilisateur.
    La saisie est sécurisée (nettoyage de la console, répétition de la demande si saisie inattendue).
    La chaîne indiquée en paramètre est la question en elle-même.
    "Convertit" 'o/n' en 'true/false' et retourne la variable booléenne correspondante.*/
    printf("%s",chaine);
    printf(" (o/n) :\n"); //affichage de la question suivie d'un "(o/n)" précisant les caractères attendus.
    char yesno;
    while(1) {
        scanf(" %c",&yesno);
        yesno=convertir_minuscules(yesno);//rend la saisie insensible à la casse. Permet donc à l'utilisateur de saisir 'O' au lieu de 'o' par exemple.
        switch(yesno) {
            case 'o':
            fflush(stdin); //usage de fflush après scanf, au cas où l'utilisateur ait entré plus que prévu et remplisse le prochain scanf.
            return true;//on retourne directement le booléen correspondant à la saisie utilisateur.

            case 'n':
            fflush(stdin);
            return false;

            default:
            puts("Entree non valide. Veuillez entrer uniquement \'o\' ou \'n\'.");//si caractère inattendu, on réitère la demande.
        }
        fflush(stdin);
    }
}

/** \brief F utilisée pour sauter une ligne sous certaines conditions.
 *
 * \param Un booléen. On sautera une ligne si et seulement s'il est vrai
 * \return Retourne systématiquement (bool) false;
 *
 *Fonction permettant de sauter une ligne si un booléen entré en paramètre est vrai, et de retourner faux (pour affectation au même booléen) pour indiquer qu'il n'y a plus besoin de sauter une ligne
 */
bool sauterligne(bool sauter_une_ligne) {
    if(sauter_une_ligne==true) {
        putchar('\n');
        sauter_une_ligne=false;
    }
    return sauter_une_ligne; //dans tous les cas, vaut faux. On l'affecte au booléen utilisé en paramètre.
}

/** \brief Procédure de saisie sécurisée.
 *
 * \param Une chaîne de caractères qui stockera les caractères lus dans les limites de sa taille.
 *Nous l'utilisons pour choisir le nom et le prénom des patients en mode génération manuelle.
 *
 *Lecture simple caractère par caractère façon NF04.
 *Nettoie également la console avant et après l'acquisition pour éviter que des caractères excédentaires soient lus.
 *Tous caractères autorisés.
 */
void saisirstring(char* chaine)
{
    fflush(stdin); //premier nettoyage pour éviter de corrompre la saisie
    int i = 0;
    chaine[0]=getchar();
    while(chaine[i]!='\n'&&i<40){//on remplit la chaine de tous les caractères jusqu'à '\n' inclus.
        i++;
        chaine[i]=getchar();
    }
    chaine[i]=0; //on remplace le dernier caractère saisi - ou le dernier de la chaîne - par '\0'.
    fflush(stdin); //on tire encore la chasse pour éviter de corrompre les prochaines saisies
    return;
}

/** \brief Cette fonction permet :
 *
 * \param Un booléen "sauter"; S'il est vrai, l'affichage contiendra un saut de ligne. On enregistrera le retour de la fonction dans cette variable à chaque appel.
 * \param Un temps "t" correspondant à l'instant d'occurrence de l'événement que l'on souhaite afficher.
 * \param Une chaîne de caractères "descriptif", qui constitue un message identifiant le patient concerné et faisant la liaison avec la fin du message
 * \param Une chaîne de caractères "message_salle" identifiant la salle concernée.
 * \param Un pointeur vers le fichier dans lequel on enregistrera les résultats.
 * \return La valeur de retour est systématiquement (bool) false : on l'affectera à la variable booléenne utilisée en paramètre d'entrée à chaque appel de la fonction pour notifier qu'il n'y a plus besoin de sauter une ligne.
 *
 *Cette fonction est utilisée pour :
 *   - sauter une ligne si on a changé de minute
 *   - notifier qu'il n'y a plus besoin de sauter une ligne en retournant systématiquement faux.
 *   - afficher l'heure et le descriptif d'un événement dans la console
 *   - enregistrer un message similaire dans le fichier historique, d'une manière plus facile à lire à la fin de la simulation

*/
bool notifier_evenement(bool sauter,temps t,char *descriptif,char *message_salle,FILE *f)
{
    char *heure = h_to_string(t); //conversion de l'heure en chaîne hh:mm
    char message[130]; //création d'une chaîne qui contiendra le message en entier pour affichage & écriture
    if(sauter){ //disjonction de cas : si on a changé d'heure et qu'on veut sauter une ligne, on ajoute un caractère '\n' au début du message.
       sprintf(message,"\n%s : %s%s.\n",heure,descriptif,message_salle);
    }
    else {
    sprintf(message,"      : %s%s.\n",descriptif,message_salle);/* dans la chaîne "message", on grave l'heure de l'événement,
    le descriptif (le patient ... entre en, entre dans la, est pris en charge au ...) et le nom de la salle/du box concerné*/

    }
    printf("%s",message); //ecriture du message directement dans la console

    if(sauter){
       sprintf(message,"\n!%02d/%02d/%04d!%s$ : %s$%s.\n",t.jour,t.mois,t.annee,heure,descriptif,message_salle); /*on réécrit une version différente de la chaîne, qui sera cette
       fois utilisée pour graver dans le fichier "historique" chaque événement. Il faut ajouter la date à chaque ligne, ainsi que des caractères '!' et '$' pour délimiter les différentes informations,
       afin de permettre la lecture des événements pour un seul patient.
       La date au début de chaque ligne sera ignorée, ainsi que les cractères '!' et '$' lors de l'affichage de l'historique au complet.
       Cependant, pour l'historique d'un patient unique, on regardera à chaque fois le numéro du patient contenu dans "descriptif", et la date contenue entre les deux '!' pour l'afficher si elle change d'un événement à l'autre.*/
    }
    else {
    sprintf(message,"!%02d/%02d/%04d!%s$ : %s$%s.\n",t.jour,t.mois,t.annee,heure,descriptif,message_salle);
    }
    if(f) {
        fprintf(f,"%s",message); //ecriture dans le fichier (avec des $ pour séparer les parties utiles pour la lecture, et des ! pour mettre la date à chaque ligne).
    }
    else {
        printf("ERREUR LORS DE L'ECRITURE DE L'HISTORIQUE.");
    }

    free(heure);
    return false; /*la fonction retourne systématiquement false, pour indiquer qu'il n'y a plus besoin de sauter une ligne (sauf si le paramètre sauter vaut true).
    En effet, nous avons choisi de sauter une ligne uniquement quand l'heure change. Un booléen "changementminute" est utilisé dans le main pour notifier un changement d'heure et appeler un premier saut de ligne.
    A chaque fois que l'on affiche le message, on affecte la valeur de retour (false) de cette fonction à "changementminute" pour indiquer qu'il ne faut plus sauter une ligne.
    C'est une solution peu élégante mais un bon raccourci pour éviter de compliquer le main. */
}
