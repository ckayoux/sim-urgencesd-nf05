//Temps : à peu près 5h
#include "temps.h"

/** \brief Demande à l'utilisateur de saisir une durée au format hh:mm ou mm.
 *
 * \return La durée entrée (temps).
 *
 * Fonction utilisée pour obtenir la durée d'une tâche de l'utilisateur.
 * Réalise la saisie sécurisée d'une chaîne contenant l'un des formats autorisés. La lit d'une manière adaptée au format utilisé.
 * Vérifie la correction des valeurs entrées pour les heures et les minutes.
 * En cas d'erreur, réitération de la demande de saisie.
 */
temps demander_duree() { //cette fonction permet d'obtenir une durée en heures+minutes de l'utilisateur, avec une saisie sécurisée.
    temps output;
    output.annee=0;output.mois=0;output.jour=0; //la durée se compte en heures et minutes seulement. On fait l'hypothèse qu'aucune opération dure plus de 23h59.
    bool heure_correcte=true; //Booléens de gestion d'erreurs.
    bool minutes_correctes=true;
    bool format_correct=true;
    char *chaine; //Pointeur permettant de parcourir les caractères de saisie
    char saisie[20]; //chaîne saisie par l'utilisateur
    unsigned char heures;
    unsigned char minutes;
    do {
        printf("\tEntrez la duree de la tache au format hh:mm ou mm :  "); scanf(" %s",saisie); //On invite l'utilisateur à entrer l'heure au format hh:mm ou mm pour des minutes uniquement
        if(strrchr(saisie,':')) chaine = strrchr(saisie,':')-2; //Si la chaîne contient ':', localise le séparateur ':' et marque le début de chaine deux caractères avant
        else if(atoi(saisie)>0&&atoi(saisie)<60) {//Sinon, si la chaîne ne contient pas ':' mais qu'elle correspond à un nombre de minutes valide :
            heures=0;
            minutes=atoi(saisie); //permet à un utilisateur paresseux de saisir uniquement mm ou m. minutes prend la valeur du nombre entre, converti en entier.
            break;
        }
        else {format_correct=false; puts("\tFormat incorrect.") ; continue;} //Si aucune des deux assertions précédentes n'est vraie, notification d'erreur de saisie.

        if(isdigit(chaine[0]) && isdigit(chaine[1]) && chaine[2]==':' && isdigit(chaine[3]) && isdigit(chaine[4])) { //test de la correction du format temporel entré si format hh:mm
            format_correct=true;
            heures = (digit_en_int(chaine,0))*10+(digit_en_int(chaine,1)) ; //conversion de deux caractères en un nombre décimal
            minutes = (digit_en_int(chaine,3))*10+(digit_en_int(chaine,4));
            }
        else if(isdigit(chaine[1]) && chaine[2]==':' && isdigit(chaine[3]) && !isdigit(chaine[4])) { //acceptation du format h:m
            //permet aux utilisateurs fainéants de saisir uniquement h:m lorsqu'il n'y a pas de chiffre des dizaines
            format_correct=true;
            heures = (digit_en_int(chaine,1)) ;
            minutes = (digit_en_int(chaine,3));
            }
        else if(isdigit(chaine[1]) && chaine[2]==':' && isdigit(chaine[3]) && isdigit(chaine[4])) {//acceptation du format h:mm
            //permet aux utilisateurs tire-au-flanc de saisir uniquement h:mm lorsqu'il n'y a pas de chiffre des dizaines à l'heure
            format_correct=true;
            heures = (digit_en_int(chaine,1)) ;
            minutes = (digit_en_int(chaine,3))*10+(digit_en_int(chaine,4));
            }
        else {//rejet de la saisie.
            puts("\tFormat incorrect.");
            format_correct=false;
            }


        if (heures>23) { //test de correction des valeurs entrées
                heure_correcte=false;
                puts("\tErreur : le nombre d'heures ne peut exceder 23.");
            }
            else heure_correcte=true;
            if (minutes>59) {
                minutes_correctes=false;
                puts("\tErreur : le nombre de minutes ne peut exceder 59.");
            }
            else minutes_correctes=true;

    }//Nouvelle saisie requise si erreurs de saisie détectées
    while(!heure_correcte||!minutes_correctes||!format_correct);
    output.h=heures;
    output.m=minutes;
    return output; //retour d'une durée valide
}

/** \brief Demande à l'utilisateur de saisir une date au format jj/mm/aaaa, puis une heure au format hh:mm ou hh.
 *
 * \return Un temps correspondant à l'instant saisi par l'utilisateur (temps).
 *
 * Fonction utilisée pour obtenir de l'utilisateur une date, puis une heure. Exemple d'utilisation : quand l'option "aucun paramètre temporel fixé" activée, pour demander l'instant d'arrivée de chaque patient.
 * Réalise la saisie sécurisée d'une chaîne contenant l'un des formats autorisés. La lit d'une manière adaptée au format utilisé.
 * Vérifie la correction des valeurs entrées pour chaque composante de la date et de l'heure.
 * En cas d'erreur, réitération de la demande de saisie et affichage du problème détecté si possible.
 */
temps demander_date_heure() { //cete fonction permet d'obtenir une date une heure de l'utilisateur
temps output;
bool annee_correcte=true;
bool mois_correct=true;
bool jour_correct=true;
bool heure_correcte=true;
bool minutes_correctes=true;
bool format_correct=true;
char *chaine;
char saisie[20];

unsigned char jour;
unsigned char mois;
unsigned short int annee;
do {
        annee_correcte=true;
        mois_correct=true;
        jour_correct=true;

        printf("\tEntrez la date au format jj/mm/aaaa :  "); scanf(" %s",saisie);
        if(strchr(saisie,'/')) chaine = strchr(saisie,'/')-2; //localise le premier slash de la chaîne et débute la chaine deux char avant.
        else {format_correct=false; puts("\tFormat incorrect.") ; continue;} //s'il n'y a pas de slash, on redemande la saisie.
        format_correct = ( isdigit(chaine[0]) && isdigit(chaine[1]) && chaine[2]=='/'
                          && isdigit(chaine[3]) && isdigit(chaine[4]) && chaine[5]=='/'
                          && isdigit(chaine[6]) && isdigit(chaine[7]) && isdigit(chaine[8]) && isdigit(chaine[9])); //test de la corrcetion du format.
        if(!format_correct) {puts("\tFormat incorrect.");continue;}
        jour = (digit_en_int(chaine,0))*10+(digit_en_int(chaine,1)) ; //conversion de deux caractères en un nombre décimal
        mois = (digit_en_int(chaine,3))*10+(digit_en_int(chaine,4));
        annee = (digit_en_int(chaine,6))*1000+(digit_en_int(chaine,7))*100+(digit_en_int(chaine,8))*10+(digit_en_int(chaine,9));
    }
    while(!jmSontCorrects(jour,mois,annee));
    output.annee=annee; output.mois=mois; output.jour=jour;

    fflush(stdin);
    temps heureminutes = demander_heure();
    output.h=heureminutes.h;
    output.m=heureminutes.m;
    return output;
}

/** \brief Demande à l'utilisateur de saisir une date au format jj/mm/aaaa.
 *
 * \return La date entrée (temps).
 *
 * Fonction utilisée pour obtenir de l'utilisateur une date uniquement.
 * Réalise la saisie sécurisée d'une chaîne contenant une date de la forme jj/mm/aaaa avec j,m et a des chiffres.
 * Vérifie la validité de la date.
 * En cas d'erreur, réitération de la demande de saisie.
 * Exemple d'utilisation : quand l'option "jour fixé" est activée, pour demander le jour d'arrivée de tous les patients.
 */
temps demander_date() { //cete fonction permet d'obtenir une date de l'utilisateur (utile pour mode jour fixé)
temps output;
bool annee_correcte=true;
bool mois_correct=true;
bool jour_correct=true;
bool format_correct=true;
char *chaine;
char saisie[20];

unsigned char jour;
unsigned char mois;
unsigned short int annee;
do {
        annee_correcte=true;
        mois_correct=true;
        jour_correct=true;

        printf("\tEntrez la date au format jj/mm/aaaa :  "); scanf(" %s",saisie);
        if(strchr(saisie,'/')) chaine = strchr(saisie,'/')-2; //localise le premier slash de la chaîne et débute la chaine deux char avant.
        else {format_correct=false; puts("\tFormat incorrect.") ; continue;} //s'il n'y a pas de slash, on redemande la saisie.
        format_correct = ( isdigit(chaine[0]) && isdigit(chaine[1]) && chaine[2]=='/'
                          && isdigit(chaine[3]) && isdigit(chaine[4]) && chaine[5]=='/'
                          && isdigit(chaine[6]) && isdigit(chaine[7]) && isdigit(chaine[8]) && isdigit(chaine[9])); //test de la corrcetion du format.
        if(!format_correct) {puts("\tFormat incorrect.");continue;}
        jour = (digit_en_int(chaine,0))*10+(digit_en_int(chaine,1)) ; //conversion de deux caractères en un nombre décimal
        mois = (digit_en_int(chaine,3))*10+(digit_en_int(chaine,4));
        annee = (digit_en_int(chaine,6))*1000+(digit_en_int(chaine,7))*100+(digit_en_int(chaine,8))*10+(digit_en_int(chaine,9));
    }
    while(!jmSontCorrects(jour,mois,annee));
    output.annee=annee; output.mois=mois; output.jour=jour;
    return output;
}

/** \brief Demande à l'utilisateur de saisir une heure au format hh:mm ou hh.
 *
 * \return L'heure entrée (temps).
 *
 * Fonction utilisée pour obtenir de l'utilisateur une heure.
 * Réalise la saisie sécurisée d'une chaîne contenant l'un des formats autorisés. La lit d'une manière adaptée au format utilisé.
 * Vérifie la correction des valeurs entrées pour les heures et les minutes.
 * En cas d'erreur, réitération de la demande de saisie.
 * Exemple d'utilisation : pour obtenir l'heure d'arrivée de chaque patient un jour fixé.
 */
temps demander_heure() { //cette fonction permet d'obtenir une heure de l'utilisateur (utile pour mode jour fixé)
temps output;

bool heure_correcte=true;
bool minutes_correctes=true;
bool format_correct=true;
char *chaine;
char saisie[20];
unsigned char heures;
unsigned char minutes;

    do {

        printf("\tEntrez l'heure au format hh:mm :  "); scanf(" %s",saisie);

        if(strrchr(saisie,':')) chaine = strrchr(saisie,':')-2; //localise le séparateur : et marque le début de la chaîne deux caractères avant le :
        else if(atoi(saisie)<24&&atoi(saisie)>0) { //permet à un utilisateur fainéant d'entrer uniquement hh et d'obtenir hh:00;
            heures=atoi(saisie);
            minutes=0; break;
        }
        else {format_correct=false; puts("\tFormat incorrect.") ; continue;}

        if(isdigit(chaine[0]) && isdigit(chaine[1]) && chaine[2]==':' && isdigit(chaine[3]) && isdigit(chaine[4])) { //test de la correction du format temporel entré
            format_correct=true;
            heures = (digit_en_int(chaine,0))*10+(digit_en_int(chaine,1)) ; //conversion de deux caractères en un nombre décimal
            minutes = (digit_en_int(chaine,3))*10+(digit_en_int(chaine,4));
            }
        else if(isdigit(chaine[1]) && chaine[2]==':' && isdigit(chaine[3]) && !isdigit(chaine[4])) {
            //permet aux utilisateurs fainéants de saisir uniquement h:m lorsqu'il n'y a pas de chiffre des dizaines
            format_correct=true;
            heures = (digit_en_int(chaine,1)) ;
            minutes = (digit_en_int(chaine,3));
            }
        else if(isdigit(chaine[1]) && chaine[2]==':' && isdigit(chaine[3]) && isdigit(chaine[4])) {
            //permet aux utilisateurs tire-au-flanc de saisir uniquement h:mm lorsqu'il n'y a pas de chiffre des dizaines à l'heure
            format_correct=true;
            heures = (digit_en_int(chaine,1)) ;
            minutes = (digit_en_int(chaine,3))*10+(digit_en_int(chaine,4));
            }
        else {
            puts("\tFormat incorrect.");
            format_correct=false;
            }


        if (heures>23) { //test de correction des valeurs entrées
                heure_correcte=false;
                puts("\tErreur : le nombre d'heures ne peut exceder 23.");
            }
            else heure_correcte=true;
            if (minutes>59) {
                minutes_correctes=false;
                puts("\tErreur : le nombre de minutes ne peut exceder 59.");
            }
            else minutes_correctes=true;

    }
    while(!heure_correcte||!minutes_correctes||!format_correct);
    fflush(stdin);
    output.h=heures;
    output.m=minutes;
    return output;
}

/** \brief Demande à l'utilisateur de saisir une année et un mois.
 *
 * \return L'instant saisi (temps).
 *
 * Fonction utilisée pour obtenir la durée d'une tâche de l'utilisateur.
 * Réalise la saisie sécurisée d'une chaîne contenant l'un des formats autorisés. La lit d'une manière adaptée au format utilisé.
 * Vérifie la correction des valeurs entrées pour le mois.
 * En cas d'erreur, réitération de la demande de saisie.
 * Exemple d'utilisation : pour demander le mois d'arrivée de tous les patients lorsque l'option "mois fixé" est activée.
 */
//Pour mode mois fixé
temps demander_annee_mois() { //Cette fonction permet de demander l'annee et le mois à l'utilisateur dans le cas où on fixerait le mois.
temps output;
bool mois_correct=true;
bool format_correct=true;
char *chaine;
char saisie[20];

unsigned char mois;
unsigned short int annee;
do {
        mois_correct=true;
        printf("\tEntrez le mois et l'annee auxquels se limitera la simulation au format mm/aaaa :  "); scanf(" %s",saisie);
        if(strrchr(saisie,'/')) chaine = strchr(saisie,'/')-2; //localise le premier slash de la chaîne et débute la chaine deux char avant.
        else {format_correct=false; puts("\tFormat incorrect.") ; continue;} //s'il n'y a pas de slash, on redemande la saisie.
        format_correct = ( isdigit(chaine[0]) && isdigit(chaine[1]) && chaine[2]=='/'
                          && isdigit(chaine[3]) && isdigit(chaine[4]) && isdigit(chaine[5]) && isdigit(chaine[6])); //test de la correction du format.
        if(!format_correct) {puts("\tFormat incorrect.");continue;}
        mois = (digit_en_int(chaine,0))*10+(digit_en_int(chaine,1));//conversion de deux caractères en un nombre décimal
        annee = (digit_en_int(chaine,3))*1000+(digit_en_int(chaine,4))*100+(digit_en_int(chaine,5))*10+(digit_en_int(chaine,6));

        if (mois<0||mois>12) {
            puts("Mois incorrect.");
            mois_correct=false;
        }
        else {
            mois_correct=true;
        }
    }
    while(!mois_correct||!format_correct);
    fflush(stdin);
    output.annee=annee;
    output.mois=mois;
    return output;
}

/** \brief Demande à l'utilisateur de saisir le jour d'un mois donné et une heure
 *
 * \param L'année donnée (entier court non signé)
 * \param Le mois donné (caractère non signé).
 * \return L'instant saisi (temps).
 *
 * Fonction utilisée pour obtenir le jour d'un mois donné et une heure de ce jour.
 * Réalise la saisie sécurisée d'une chaîne contenant l'un des formats autorisés. La lit d'une manière adaptée au format utilisé.
 * Vérifie la correction des valeurs entrées pour le jour, les heures et les minutes.
 * En cas d'erreur, réitération de la demande de saisie.
 * Exemple d'utilisation : pour demander l'instant d'arrivée de chaque patient en mode "mois fixé".
 */
temps demander_jour_heure(unsigned short int annee,unsigned char mois) { //Cette fonction permet de demander le jour à l'utilisateur dans le cas où le mois serait fixé.
    temps output;
    unsigned char jour;
    do {
        printf("\tEntrez le jour au format jj (/%02d/%04d) :  ",mois,annee);
        scanf(" %d",&jour);
    }
    while(!jmSontCorrects(jour,mois,annee));
    fflush(stdin);
    output = demander_heure();
    output.jour = jour;
    return output;
}

//PROCEDURES D'AFFICHAGE
void afficher_heure(temps t) { //cette procédure permet d'afficher les champs h et m d'un temps au format hh:mm
    printf("%02d:%02d",t.h,t.m);
    return;
}

/** \brief Convertit une heure en une chaîne de caractères de la forme "hh:mm"
 *
 * \param Un instant t (temps).
 *
 * Procédure utilisée pour l'affichage des heures d'occurrence des événements durant la gestion du flux de patients
 * Il faut penser à libérer l'espace mémoire réservé pour la chaîne créée dès qu'elle ne sert plus
 */
char* h_to_string(temps t) { //cette fonction permetde convertir les champs h et m d'un temps en une chaîne "hh:mm"
    char *string = calloc(6,sizeof(char));
    sprintf(string,"%02d:%02d",t.h,t.m);
    return string;
}

void afficher_date(temps t) {  //cette procédure permet d'afficher les champs a,m et h d'un temps au format jj/mm/aaaa
    printf("%02d/%02d/%04d",t.jour,t.mois,t.annee);
    return;
}
void afficher_date_heure(temps t) {  //cette procédure permet d'afficher la date et l'heure correspondant aux champs d'un temps t.
    afficher_date(t);
    printf("  ");
    afficher_heure(t);
    return;
}

//FONCTIONS INTERMEDIAIRES
/** \brief Convertit un caractère représentant un chiffre en la valeur numérique de ce chiffre
 *
 * \param Une chaîne de caractères de taille > indice.
 * \param Un indice correspondant à l'emplacement du caractère à traiter dans la chaîne passée en entrée (entier non signé).
 * \return La valeur numérique d'un caractère représentant un chiffre, ou 0 si le caractère n'en représente pas un. (caractère)
 *
 * Fonction utilisée pour tirer des valeurs numériques de la saisie d'un instant sous la forme d'une chaîne de caractères (ex : demander_duree()).
 * par exemple, pour une heure au format "h1 h2 : m1 m2", l'heure vaudra VN(h1)*10+VN(h2) et le nombre de minutes VN(m1)*10 + VN(m2), avec VN(caractère) la valeur numérique du chiffre représenté par caractère.
 */
char digit_en_int(char *chaine,unsigned char indice) { //cette fonction permet de convertir un caractère représentant un chiffre dans une string, en le chiffre qu'il représente

    if(isdigit(chaine[indice]))//Si le caractère représente un chiffre
        return chaine[indice]-48;//Le caractère '0' est encodé par 48 dans la plupart des tables d'encodage, et les chiffres se suivent dans la table. En enlevant 48 à un caractère représentant un chiffre, on obtient donc sa valeur numérique.
    else
        return 0;
}

/** \brief Teste la bissextilité d'une année
 *
 * \param Une année (entier court non signé)
 * \return Un booléen vrai si l'année passée en entrée est bissextile, faux sinon.
 *
 * Test utilisé pour pour connaître le dernier jour du mois de février afin de vérifier la correction d'une date ou de réaliser un calcul sur des valeurs temporelles.
 * On considère une année bissextile si elle est divisible 400, ou par 4 mais pas par 100
 */
bool estBissextile(unsigned short int annee) { //cette fonction permet de tester si une année est bissextile (pour éviter d'entrer des dates incorrectes et tester les derniers jours du mois)

    if(annee%400 == 0)
        return true;
    else {
        if(annee%4==0 && annee%100!=0) return true;
        else return false;
    }
}

/** \brief Test de correction d'une date
 *
 * \param Un jour (caractère non signé)
 * \param Un mois (caractère non signé)
 * \param Une année (entier court non signé)
 * \return L'intersection d'un booléen indiquant si le mois est correct, et d'un autre indiquant si le jour est correct, ce qui revient à retourner un booléen vrai si la date jour/mois/année est valide selon le calendrier Grégorien, faux sinon.
 *
 * Vérification que le mois est compris entre 1 et 12.
 * Vérification que le jour est compris entre 1 et le dernier jour du mois donné pour l'année donnée.
 * Affichage des problèmes détectés en cas d'invalidité de la date.
 */
bool jmSontCorrects(unsigned char jour, unsigned char mois, unsigned short int annee) {
    //Cette fonction teste si le jour et le mois renseignés sont corrects (il faut aussi connaître l'année pour savoir s'il y a un 29 février). Elle renvoie true si c'est le cas, false sinon.
    bool jour_correct = true;
    bool mois_correct = true;
    if (mois==2) { //test de correction du mois
            if(estBissextile(annee)) {
                if(jour>29) {
                    puts("\tJour incorrect. Fevrier n'a que 29 jours les annees bissextiles.");
                    jour_correct=false;
                }
            }
            else {
                if(jour>28) {
                    puts("\tJour incorrect. Fevrier n'a que 28 jours les annees non bissextiles.");
                    jour_correct=false;
                }
            }
        }
        else if (mois==1) {
            if(jour>31) {
                puts("Jour incorrect. Il n'y a que 31 jours dans ce mois.");
                jour_correct=false;}
        }
        else if (mois>2&&mois<=12) {
            if(mois<=7) {
                    if(mois%2==1) {
                            if(jour>31) {
                                puts("\tJour incorrect. Il n'y a que 31 jours dans ce mois.");
                                jour_correct=false;
                                }
                    }
                    else {
                        if(jour>30) {
                                puts("\tJour incorrect. Il n'y a que 30 jours dans ce mois.");
                                jour_correct=false;
                                }
                    }
            }
            else { //à partir d'août, ce sont les mois pairs qui ont 31 jours.
                if(mois%2==1) {
                            if(jour>30) {
                                puts("\tJour incorrect. Il n'y a que 30 jours dans ce mois.");
                                jour_correct=false;
                                }
                    }
                    else {
                        if(jour>31) {
                                puts("\tJour incorrect. Il n'y a que 31 jours dans ce mois.");
                                jour_correct=false;
                                }
                    }
            }
        }
        else { //si le mois est 0 ou >12
            puts("\tMois incorrect.");
            mois_correct=false;
        }
    if(jour==0) { if(mois_correct){puts("\tjour incorrect.");};jour_correct=false;}

    return jour_correct&&mois_correct;
}

/** \brief Test de valeur maximale d'un jour pour un mois donné.
 *
 * \param Un instant t (temps).
 * \return Un booléen vrai si le jour de l'instant t est le dernier du mois.
 *
 * Vérification en fonction de la parité du mois. Traitement des cas particuliers de février et du changement de lien parité/valeur maximale en août.
 * Traitement des cas particuliers de février et du changement de lien parité/valeur maximale en août.
 * Fonction utilisée pour proposer à l'utilisateur de désactiver la fixation d'un paramètre temporel ou pour réaliser des sommes ou des différences de valeurs temporelles.
 */
bool estDernierJourMois(temps t) {
    /*Cette fonction teste si un temps correspond à une heure tardive d'un dernier jour du mois. Renvoie true si c'est le cas, false sinon.
    Utilisé pour les saisies et les fonctions de calcul de temps*/
    unsigned short int annee = t.annee;
    unsigned char mois = t.mois;
    unsigned char jour = t.jour;

    bool dernier_du_mois=false;
    if(t.h > 15) { //si un patient arrive le dernier jour du mois après 15 heures, on voudra proposer de débloquer l'éventuelle limitation pour ajouter plus de patients
        if (mois==2) {
                if(estBissextile(annee)) {
                    if(jour==29) {
                        dernier_du_mois=true;
                    }
                }
                else {
                    if(jour==28) {
                        dernier_du_mois=true;
                    }
                }
            }
            else if (mois==1) {
                if(jour==31) {
                    dernier_du_mois=true;
                }
            }
            else if (mois>2&&mois<=12) {
                if(mois<=7) {
                        if(mois%2==1) {
                                if(jour==31) {
                                    dernier_du_mois=true;
                                    }
                        }
                        else {
                            if(jour==30) {
                                    dernier_du_mois=true;
                                    }
                        }
                }
                else { //à partir d'août, ce sont les mois pairs qui ont 31 jours.
                    if(mois%2==1) {
                                if(jour==30) {
                                    dernier_du_mois=true;
                                    }
                    }
                        else {
                            if(jour==31) {
                                    dernier_du_mois=true;
                            }
                        }
                }
            }
    }
    return dernier_du_mois;
}

/** \brief Test de valeur maximale d'un mois
 *
 * \param Un instant t (temps).
 * \return Un booléen vrai si le mois de t est décembre.
 *
 * Fonction utilisée pour réaliser des sommes ou des différences de valeurs temporelles.
 */
bool estDernierMois(temps t) {//Cette fonction teste si un mois est le dernier mois possible (sera utilisée dans les fonctions de calcul)
    if(t.mois == 12) return true;
    else return false;
}

/** \brief Test de valeur maximale d'une heure
 *
 * \param Un instant t (temps).
 * \return Un booléen vrai si l'heure de t est 23.
 *
 * Fonction utilisée pour réaliser des sommes ou des différences de valeurs temporelles.
 */
bool estDerniereHeure(temps t) {//Cette fonction teste si une heure est la dernière possible (sera utilisée dans les fonctions de calcul)
    if(t.h == 23) return true;
    else return false;
}

/** \brief Test de valeur maximale d'un nombre de minutes
 *
 * \param Un instant t (temps).
 * \return Un booléen vrai si le nombre de minutes de t est 59.
 *
 * Fonction utilisée pour réaliser des sommes ou des différences de valeurs temporelles.
 */
bool estDerniereMinute(temps t) {//Cette fonction teste si une minute est la dernière possible (sera utilisée dans les fonctions de calcul)
    if(t.m == 59) return true;
    else return false;
}

/** \brief Ajout d'une minute à une valeur temporelle en conservant une expression de la date et de l'heure valide.
 *
 * \param L'adresse d'un instant t (temps*).
 *
 * Gestion des cas particuliers (fin du jour, du mois, de l'année ...) pour éviter d'avoir des dates incorrectes (comme 32/13/2020... ou 25:00... ou 63 min ...).
 * Utilisation des fonctions de test de valeurs maximales.
 * Incrémentation du champ correspondant à la plus petite unité n'ayant pas atteint sa valeur maximale, remise des champs correspondant à de plus petites unités à leur valeur minimale.
 * Fonction utilisée pour faire évoluer le flux de patients en incrémentant le temps virtuel de la simulation, et dans les fonctions de calcul de valeurs temporelles
 */
void incrementerTemps(temps *t) {
    temps resultat = *t;
    if(estDerniereMinute(resultat)) {
        if(estDerniereHeure(resultat)) {
            if(estDernierJourMois(resultat)) {
                if(estDernierMois(resultat)) {//Si on est au dernier jour du dernier mois, à la dernière minute de la dernière heure, on incrémente l'année, le mois et le jour sont remis à 1, l'heure et la minute à 0.
                    resultat.annee++;
                    resultat.mois=1;
                    resultat.jour=1;
                    resultat.h=0;
                    resultat.m=0;
                }
                else {
                resultat.mois++; //si on est pas encore en décembre, pas de changement d'année nécessaire.
                resultat.jour=1;
                resultat.h=0;
                resultat.m=0;
                }
            }
            else {
            resultat.jour++; //si on est pas encore le dernier du mois, pas de changement de mois.
            resultat.h=0;
            resultat.m=0;
            }
        }
        else {
            resultat.h++; // si on est pas encore à 23h, pas de changement de jour
            resultat.m=0;
        }
    }
    else {
        resultat.m++; //si on est pas encore à 59 minutes, pas de changement d'heure : incrémentation des minutes.
    }

    *t=resultat; //copie du résultat à l'adresse pointée par l'argument
    return;
}

/** \brief Retrait d'une minute à une valeur temporelle en conservant une expression de la date et de l'heure valide.
 *
 * \param Un instant t (temps).
 * \return Un temps t.
 *
 * Gestion des cas particuliers (début du jour, du mois, de l'année ...) pour éviter d'avoir des dates incorrectes (comme 32/13/2020... ou 25:00... ou 63 min ...).
 * Décrémentation du champ correspondant à la plus grande unité n'ayant pas atteint sa valeur minimale, remise des champs correspondant à de plus petites unités à leur valeur maximale.
 * Fonction utilisée dans les fonctions de calcul de valeurs temporelles.
 * Contrairement à la fonction d'incrémentation dont l'utilisation imposait un passage par adresse, il est ici plus pratique d'utiliser un passage par valeur.
 */
temps decrementerTemps(temps t) {
    //Cette fonction permet d'enlever une minute à un temps et de le récupérer décrémenté, suivant des principes similaires à la fonction incrémenterTemps.
    temps resultat = t;
    if(resultat.m==0) {
        if(resultat.h==0) {
            if(resultat.jour==0) {
                if(resultat.mois==0) {
                    if(resultat.annee==0) {
                        return resultat;
                    }
                    else {
                        resultat.annee--;
                        resultat.mois=12;
                        resultat.jour=31;
                        while(!estDernierJourMois(resultat)) {
                            resultat.jour--;
                        }
                        resultat.h=23;
                        resultat.m=59;
                        }
                    }

                else {
                resultat.mois--; //si on est pas encore en janvier
                resultat.jour=31;
                while(!estDernierJourMois(resultat)) {
                    resultat.jour--;
                }
                resultat.h=23;
                resultat.m=59;
                }
            }
            else {
            resultat.jour--; // si on est pas encore à00h
            resultat.h=23;
            resultat.m=59;
            }
        }
        else {
        // si on est pas encore à 00h
            resultat.h--;
            resultat.m=59;
            }
    }
    else {
        resultat.m--; //si on est pas encore à 00 minutes
    }

    return resultat;
}

/** \brief Test de "nullité" d'une valeur temporelle (0j0m0a 00h00min)
 *
 * \param Un instant t (temps).
 * \return Un booléen vrai si tous les champs de t valent 0, ou faux sinon.
 *
 * Fonction utilisée lors du calcul de valeurs temporelles (sommes, différences ...).
 */
bool estNul(temps t) {
    //Cette fonction teste si un temps est "nul", c'est-à-dire si tous ses champs sont à 0. Elle retourne true si le temps est "nul", false sinon.
    bool output = false;
    if(t.annee==0&&t.mois==0&&t.jour==0&&t.h==0&&t.m==0) output=true;
    return output;
}

/** \brief Calcul de la somme de deux valeurs temporelles et stockage du résultat à l'adresse de la première.
 *
 * \param L'adresse d'un instant/durée t1 (temps*) qui contiendra le résultat de la somme *t1 + t2 à l'issue.
 * \param Un instant/durée t2 (temps) à ajouter à *t1.
 *
 * Procédure permettant de sommer deux valeurs temporelles et de remplacer la première par le résultat.
 * Fait intervenir les fonctions d'incrémentation et de décrémentation, ainsi que le test de nullité.
 * Utilisée pour calculer l'instant de libération d'une salle ou pour sommer des durées pour établir des statistiques.
 */
void ajouterTemps(temps *t1,temps t2) {
    /*Cette procédure permet d'ajouter un temps t2 à un temps t1 passé par adresse. Seul t1 est modifié à l'issue.
    On utilise les fonctions incrementerTemps et decrementerTemps pour faire l'addition des temps sans obtenir des formats incorrects. (gestion des limites de chaque unité)
    Tant que la copie reste de t2 n'est pas nulle, on la décrémente et on incrémente *t1.
    On quitte la procédure quand reste est "nul".*/
    temps reste=t2;
    while(!estNul(reste)) { //Tant que reste n'est pas "nul"
        reste = decrementerTemps(reste);//On décrémente reste (on enlève une minute)
        incrementerTemps(t1);//On incrémente *t1 (on ajoute une minute)
    }//à l'issue de cette boucle, reste est "nul", et *t1 vaut sa valeur initiale + t2
    return;
}

/** \brief Calcul de la différence entre la plus grande et la plus petite de deux valeurs temporelles.
 *
 * \param Un instant t1 (temps).
 * \param Un instant t2 (temps).
 * \return Une valeur temporelle (temps).
 *
 * Détermination du plus petit et du plus grand des deux instants au moyen de la fonction de comparaison.
 * Calcul de la différence "plus grand instant - plus petit instant" à l'aide des fonctions d'incrémentation et de comparaison.
 * Fonction utilisée pour calculer l'étendue d'une plage horaire afin de générer l'heure d'arrivée de patients.
 */
temps diffTemps(temps t1,temps t2) {
    //Cette fonction permet de calculer la distance (positive) séparant deux instants. Elle retourne une valeur de type temps.
    temps pgt;//plus grand temps
    temps temps_nul; temps_nul.annee=temps_nul.mois=temps_nul.jour=temps_nul.h=temps_nul.m=0;
    temps ppt;//plus petit temps
    char c = comparerTemps(t1,t2); // On va comparer t1 et t2 pour savoir lequel des deux est le plus grand, et donc auquel des deux on va soustraire l'autre.
    if(c==0){
        return temps_nul;
    }
    else if(c==-1) { //on determine le plus petit temps et le plus grand temps
        ppt = t1;
        pgt = t2;
    }
    else {
        ppt = t2;
        pgt = t1;
    }
    while(comparerTemps(ppt,pgt)!=0) {//Tant que ppt et pgt sont différents ... on incrémente temps_nul (dont tous les champs étaient initialisés à 0) et on incrémente le plus ppt
        incrementerTemps(&temps_nul);
        incrementerTemps(&ppt);
    } //une fois que ppt est egal a pgt, on retourne temps_nul qui vaudra la difference pgt-ppt.
    return temps_nul;
}

/** \brief Calcul de la différence entre la plus grande et la plus petite de deux valeurs temporelles, retour du résultat sous la forme d'un nombre de minutes.
 *
 * \param Un instant t1 (temps).
 * \param Un instant t2 (temps).
 * \return Un nombre de minutes correspondant à la distance abs(t2-t1) (entier non signé).
 *
 * Détermination du plus petit et du plus grand des deux instants au moyen de la fonction de comparaison.
 * Calcul, sous forme de minutes, de la différence "plus grand instant - plus petit instant" à l'aide des fonctions d'incrémentation et de comparaison.
 * Fonction utilisée pour les calculs statistiques (notamment celui de l'écart-type).
 */
unsigned int diffTemps_minutes(temps t1,temps t2) {
     /*Cette fonction permet de calculer la distance (positive) séparant deux instants.
     Elle retourne une valeur entière correspondant à un nombre de minutes.
     (utilisée pour les calculs statistiques, qui se font sur des minutes)*/
    temps pgt;
    temps ppt;
    char c = comparerTemps(t1,t2);
    if(c==0){
        return 0;
    }
    else if(c==-1) { //on determine le plus petit temps et le plus grand temps
        ppt = t1;
        pgt = t2;
    }
    else {
        ppt = t2;
        pgt = t1;
    }
    unsigned int difference=0;
    while(comparerTemps(ppt,pgt)!=0) {
        difference++;
        incrementerTemps(&ppt);
    } //une fois que ppt est egal a pgt, on retourne temps_nul qui vaudra la difference pgt-ppt.
    return difference;
}

/** \brief Compare deux valeurs temporelles.
 *
 * \param Un instant t1 (temps).
 * \param Un instant t2 (temps).
 * \return Un caractère valant 0 si t1=t2, 1 si t1>t2 et -1 si t1<t2
 *
 * Comparaison de tous champs des deux valeurs temporelles entre eux, en allant des plus grandes unités aux plus petites, jusqu'à détecter une différence entre deux champs ou à remarquer que même t1.minutes et t2.minutes sont égaux.
 * Fonction utilisée dans les calculs de valeurs temporelles, pour le tri de tableaux d'instants et de durées, et pour gérer le flux de patients (permettre la libération d'un patient lorsqu'une salle a atteint l'heure de libération par exemple).
 */
char comparerTemps(temps temps1,temps temps2) {
/*Cette fonction retourne 1 si le temps1 est supérieur au temps2, -1 s'il lui est inférieur et 0 en cas d'égalité.
Elle est utilisée notamment pour trier des durées pour l'établissement des statistiques,
Vérifier la validité de la saisie d'une plage horaire,
Et pour regarder si la simulation a atteint l'heure de libération d'une salle*/

//On procède par comparaison des plus grandes unités : si l'année de temps1 est supérieure à l'année de temps2, t1 est forcément inférieur à t2...
    if(temps1.annee>temps2.annee) {
        return 1;
    }
    else if (temps1.annee<temps2.annee) {
        return -1;
    }

    else { //années égales
        if(temps1.mois>temps2.mois) {
            return 1;
            }
        else if (temps1.mois<temps2.mois) {
            return -1;
            }

        else { //mois égaux
            if(temps1.jour>temps2.jour) {
                return 1;
            }
            else if (temps1.jour<temps2.jour) {
                return -1;
            }

            else  {//jours égaux
                if(temps1.h>temps2.h) {
                    return 1;
                }
                else if (temps1.h<temps2.h) {
                    return -1;
                }

                else { //heures égales
                    if(temps1.m>temps2.m) {
                        return 1;
                    }
                    else if (temps1.m<temps2.m) {
                        return -1;
                    }
                    else { //temps égaux;
                        return 0;
                    }
                }
            }
        }
    }

}

/** \brief Convertit une valeur temporelle en un nombre réel de minutes.
 *
 * \param Un instant t (temps).
 * \return Un nombre de minutes (réel encodé en précision double).
 *
 * Fonction utilisée pour l'établissement de statistiques temporelles.
 * En effet, dans cette situation, le nombre de secondes devient significatif même si on en a fait abstraction durant la simulation.
 * Par exemple, la durée moyenne entre 1min et 4min est de 2 minutes et 30 secondes soit de 2,5min.
 * Pour diviser une valeur temporelle par un effectif, on devra donc convertir une valeur de type (temps) en nombre réel de minutes.
 */
double convertir_temps_minutes(temps t) { //Cette fonction convertit une valeur de type temps en valeur réelle correspondant à un nombre de minutes.
    double converti=0;
    converti=t.m + t.h*60 + t.jour*1440 + 30.437525*1440*t.mois + 365.25*1440*t.annee; //Conversion de tous les champs-unités du temps en minutes.
    return converti;
}

/** \brief Divise un nombre réel de minutes par un effectif et affiche de manière optimisée la valeur temporelle moyenne obtenue.
 *
 * \param Un instant t (temps).
 * \param Un effectif non nul (entier court non signé).
 *
 * Fonction utilisée pour afficher différentes statistiques temporelles : instant/durée moyen.e, total.e, médian.e ...
 * L'affichage fait abstraction des unités pour lesquelles les valeurs sont nulles avant la première unité pour laquelle on a une valeur significative.
 * Il prend également en compte les cas où le nom de l'unité doit être écrit au singulier plutôt qu'au pluriel.
 */
void dureeMoyenne(temps t,unsigned short int diviseur) {
    //Cette procédure divise un temps converti en minutes par un diviseur entier et l'affiche au format optimal.
    double minutes = convertir_temps_minutes(t);
    unsigned int m_temp;
    if(diviseur!=0) { //ne marche pas quand le diviseur est 1 apparemment
        minutes/=diviseur;
    }
    else {
        minutes = 0;
    }
    unsigned char jours = (int)minutes/1440; //Un jour = 1440 minutes.
    minutes-=jours*1440;//Les paquets de minutes formant des jours sont éliminés.
    unsigned int heures = (int)minutes/60;//Une heure = 60 minutes.
    minutes-=heures*60;//Les paquets de minutes formant des heures sont éliminés.
    m_temp=(unsigned int) minutes; //on affichera m_temp au lieu des minutes, c'est cette variable qui contient le nombre entier de minutes
    minutes-=m_temp;//Il ne reste que les chiffres après la virgule dans minutes après cette opération. Ils correspondent aux secondes (exprimées en minutes)
    unsigned char secondes = minutes*60;//Conversion des chiffres après la virgule en secondes : 0.5 minutes = 0.5*60 = 30 secondes.

    //Gestion du pluriel pour éviter d'afficher "une heures"
    char jplur[3]= {'s',',','\0'};
    char hplur[3]= {'s',',','\0'};
    char mplur[3]= {'s','\0','\0'};
    char splur[3]= {'s','.','\0'};
    hplur[0]=(heures==1)?',':'s'; hplur[1]=(heures==1)?'\0':',';
    mplur[0]=(m_temp==1)?'\0':'s'; mplur[1]=(m_temp==1)?'\0':'\0';
    splur[0]=(secondes==1)?'.':'s'; splur[1]=(secondes==1)?'\0':'.';
    //affichage optimisé (gestion du pluriel/singulier, évitement des valeurs non significatives ...)
    if(jours >0 ){
        jplur[0]=(jours==1)?',':'s';jplur[1]=(jours==1)?'\0':',';
        if(secondes>0){
            printf("%u jour%s %u heure%s %u minute%s et %u seconde%s",jours,jplur,heures,hplur,m_temp,mplur,secondes,splur);//affichage le plus souvent utilisé pour les statistiques style temps total
        }
        else{
            hplur[0]=(heures==1)?'\0':'s';hplur[1]='\0';
            mplur[0]=(m_temp==1)?'.':'s'; mplur[1]=(m_temp==1)?'\0':'.';
            printf("%u jour%s %u heure%s et %u minute%s",jours,jplur,heures,hplur,m_temp,mplur,secondes,splur);
        }
    }
    else if(heures>0) {
        if(secondes>0){
          printf("%u heure%s %u minute%s et %u seconde%s",heures,hplur,m_temp,mplur,secondes,splur);
        }
        else if(m_temp==0&&secondes==0){
            hplur[0]=(heures==1)?'.':'s';hplur[1]=(heures==1)?'\0':'.';
            printf("%u heure%s",heures,hplur);
        }
        else{
            hplur[0]=(heures==1)?'\0':'s';hplur[1]='\0';
            mplur[0]=(m_temp==1)?'.':'s'; mplur[1]=(m_temp==1)?'\0':'.';
            printf("%u heure%s et %u minute%s",heures,hplur,m_temp,mplur);//Le plus utilisé pour les durées des taches
        }
    }
    else if(m_temp>0){
        if(secondes>0){
            printf("%u minute%s et %u seconde%s",m_temp,mplur,secondes,splur);
        }
        else{
            mplur[0]=(m_temp==1)?'.':'s'; mplur[1]=(m_temp==1)?'\0':'.';
            printf("%u minute%s",m_temp,mplur);
        }
    }
    else if(secondes>0){
        printf("%u seconde%s",secondes,splur);
    }

        putchar('\n');
    return;
}

/** \brief Calcule l'écart-type d'une série statistique durée/individu.
 *
 * \param Un tableau de durées (temps*).
 * \param L'effectif de la série statistique (nb).
 * \return La somme des carrés des écarts de chaque durée du tableau à la moyenne, divisée par n, exprimée en minutes (réel encodé en précision double)
 *
 * Fonction utilisée pour le calcul de statistiques temporelles.
 */
double ecart_type(temps *tab_temps,unsigned short int nb) {
    //Cette fonction calcule un écart-type exprimé en minutes à partir d'un tableau de durées de taille connue.
    double sigma=0;//Sigma correspond à l'écart-type. On l'initialise à 0 pour construire une somme.
    double moyenne=0; //Il faut tout d'abord calculer la moyenne, avant de calculer le carré de l'écart à la moyenne de chaque durée.
    if(nb==0){ return 0;}
    for(int i=0;i<nb;i++) {
        moyenne+=convertir_temps_minutes(tab_temps[i]); //sommation des éléments du tableau pour former ultérieurement la moyenne (en minutes).
    }
    if (nb!=0)moyenne/=nb;//Division de la durée totale par le nombre d'éléments la constituant
    for(int i=0;i<nb;i++) {
        sigma+=pow(((convertir_temps_minutes(tab_temps[i]))-moyenne),2);//sigma prend la valeur de la somme des carrés de l'écart à la moyenne des valeurs du tableau.
    }
    if(nb!=0)sigma*=1/(double)nb;//sigma est divisé par nb : il contient la valeur de la variance.
    sigma=sqrt(sigma);//la racine carré de la variance est l'écart-type.
    return sigma; //retour de l'écart-type calculé.
}

/** \brief Trie un tableau contenant des valeurs temporelles dans l'ordre croissant.
 *
 * \param Un tableau contenant des valeurs temporelles (temps*).
 * \param Sa taille (un entier).
 *
 * Utilise la fonction de comparaison de valeurs temporelles.
 * Permute les cases du tableau pour obtenir, à l'issue, un tableau trié par ordre de valeurs temporelles croissantes.
 */
void trierDurees(temps *tab_temps,int nb)
{
    //Cette procédure permet de trier un tableau d'éléments (temps) dans l'ordre croissant.
    temps duree_temporaire;
    for (int i=0;i<nb;i++) { //On parcourt toute les cases du tableau à partir de l'indice 0
        for(int j=i+1;j<nb;j++) { //On va comparer chaque case i à toutes les cases j suivantes.
            if(comparerTemps(tab_temps[i],tab_temps[j])==1) { //Si un temps tab_temps[j] est inférieur à un temps tab_temps[i], les deux cases sont permutées. Comparaison effectuée à l'aide de comparerTemps.
             duree_temporaire = tab_temps[j];
             tab_temps[j] = tab_temps[i];
             tab_temps[i] = duree_temporaire;
            }
        }
    }
    return ; // à l'issu de ce tri, tab_temps est ordonné dans l'ordre croissant.
}
