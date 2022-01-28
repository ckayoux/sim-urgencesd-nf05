//temps passé : à peu près 5h
#include "patient.h"

/** \brief Instancie un nouveau patient dans la simulation.
 *
 * \return Pointeur vers le patient nouvellement créé.
 *Cette fonction permet d'instancier un patient en lui réservant suffisamment d'espace en mémoire, et retourne un pointeur vers ce patient.
 */
patient* creerPatient() {
    patient *p = (patient *)malloc(sizeof(patient));
    return p;
}

/** \brief Trouve un patient dans une liste à partir de son ID et renvoie un pointeur vers celui-ci.
 *
 * \param L'identifiant ID du patient recherché (entier non signé).
 * \param La liste exhaustive des patients liste_patients manipulés dans la simulation (patient**).
 * \param Le nombre de patients contenus dans la liste (entier).
 * \return Pointeur vers le patient de liste_patients dont le champ id est égal à ID. NULL si aucun patient correspondant n'est trouvé.
 *
 *Cette fonction permet de récupérer un pointeur vers un patient à partir de son ID, d'un tableau de (patient*) et de sa taille.
 *Utile pour l'affichage des statistiques, puisque les salle_even conservent l'id des patients qui y séjournent dans les champs ordre (ids dans l'ordre d'arrivée dans la salle) et tab_id_patients (id classées par durée de séjour).
 */
patient* id_getPatient(unsigned short int ID,patient** liste_patients,int nombre_patients){
    for(int k=0;k<nombre_patients;k++) {
        if(liste_patients[k]->id==ID) return liste_patients[k]; //Si un patient* de liste_patients ayant l'ID recherché est trouvé, on retourne ce pointeur.
    }
    return NULL;//Si aucun patient correspondant à ID n'a été trouvé, on retourne NULL.
}

/** \brief Nomme tous les patients d'une liste à partir des noms et prénoms contenus dans un fichier texte.
 *
 * \param La liste exhaustive des patients liste_patients manipulés dans la simulation (patient**).
 * \param Le nombre de patients contenus dans la liste (entier non signé).
 *
 *Procédure utilisée pour attribuer un nom et un prénom aux patients quand l'option "génération automatique des patients" est activée (par défaut).
 *Les combinaisons "numéro_combinaison prénom nom" et leur nombre total sont consignées dans le fichier "liste_noms.txt".
 *Les noms sont tirés au hasard en évitant les doublons - il y en aura uniquement si on génère plus de patients qu'il n'existe de noms disponibles.
 *Si un même nom est attribué plusieurs fois à des patients différents, un numéro est ajouté pour différencier les patients entre eux.
 *Si le fichier "liste_noms.txt" est inaccessible ou ne peut pas être lu, tous les patients seront nommés "John Doe".
 */
void nommerPatients(unsigned int nb_patients,patient** liste_patients) {
    /*Pour le mode génération auto des patients, nous avons décidé que les patients prendraient un nom aléatoire parmi une liste de noms fournie dans le projet : "liste_noms.txt"
    Cette fonction attribue aux patients de liste_patients un nom et un prénom choisis au hasard dans le fichier "liste_noms.txt".
    */

    FILE* fnoms ;
    if((fnoms=fopen("liste_noms.txt","r"))) { /*Pour un fonctionnement optimal, merci de vous assurer que ce fichier se trouve bien dans le répertoire
        d'éxécution.
        Si le programme compilé est éxécuté depuis un shell, il faut que le répertoire de travail du shell contienne l'annexe (cd repertoireousetrouvelefichier)
        Si le programme compilé est éxécuté directement, il faut que le répertoire contenant le programme contienne également l'annexe.
        Si on éxécute le programme depuis un IDE, il faudra en général de placer le fichier dans le dossier où se trouvent les sources.*/

        unsigned int doublons_noms=0;//indice égal au nombre de nouvelles fois où on aura épuisé la liste de noms
        char chaine_doublons[10]; //chaîne permettant de différencier le nom de deux patients s'il a été choisi plusieurs fois (on aura par exemple John Doe et John Doe (1) si la liste de noms est épuisée)
        unsigned int nb_noms_dispo;//entier égal au nombre de noms différents disponibles dans le fichier
        unsigned int id_nom_choisi;//entier égal au numéro associé à un nom choisi(permet de retenir les id des noms déjà pris sans avoir à stocker les noms eux-mêmes)
        unsigned int id_scanne; //entier égal au numéro écrit au début d'une ligne lue depuis le fichier
        unsigned int indice_random; //indice correspondant à un numéro associé à un nom du fichier, choisi aléatoirement
        char prenom_scane[40];//chaîne recevant le prenom choisi
        char nom_scane[40];//chaîne recevant le nom choisi

        patient *ppatient; //variable intermédiaire désignant le patient dont on choisit le nom

        fscanf(fnoms,"%d",&nb_noms_dispo); //lecture du nombre de noms différents disponibles, toute première ligne du fichier
        unsigned int tab_id_noms_dispo[nb_noms_dispo];//Création d'un tableau pouvant contenir autant d'entiers qu'il y a de noms différents disponibles dans le tableau. Servira à indiquer qu'un nom est encore disponible

        for(int i=1;i<=nb_noms_dispo;i++) {
                tab_id_noms_dispo[i-1]=i; //remplissage du tableau avec tous les numéros des noms disponibles. (tab_id_noms_dispo[0] = 1)
        }

        for(int p=0;p<nb_patients;p++) { //pour chaque patient à nommer ...
            ppatient=liste_patients[p];
            fseek(fnoms,0,SEEK_SET); //retour au debut du fichier
            fscanf(fnoms,"%d",&id_nom_choisi); // on se débarrasse de la premiere ligne qui indique le nombre de noms dispos

                indice_random=(unsigned int)rand()%nb_noms_dispo +1; //On trouve un indice aléatoire pour le nom qu'on donne au patient avec la fonction rand(). Le résultat est compris entre 1 et nb_noms_dispo inclus
                id_nom_choisi=0;//On initialise id_nom_choisi avec un id invalide et introuvable dans le fichier : 0
                for(int i=indice_random-1;i<nb_noms_dispo;i++) {//on parcourt tab_id_noms_dispo à partir de indice_random jusqu'à la fin.
                    if(tab_id_noms_dispo[i]!=0) {//Si la case contient une id disponible (différente de 0) :
                        id_nom_choisi=tab_id_noms_dispo[i];//...on la choisit...
                        tab_id_noms_dispo[i]=0;//...et on remplace l'id contenue dans la case par 0 pour que le nom correspondant ne puisse pas être choisi à nouveau.
                        break;//on quitte la boucle si on a trouvé un id valide.
                    }
                }

                if(id_nom_choisi==0) { //si aucun id valide n'a été trouvé, on cherche dans l'autre sens : de indice_random-1 à 0
                   for(int i=indice_random-2;i>=0;i--) {
                      if(tab_id_noms_dispo[i]!=0) { //Si on trouve un numéro valide (différent de 0)
                        id_nom_choisi=tab_id_noms_dispo[i]; //on l'affecte à id_nom_choisi
                        tab_id_noms_dispo[i]=0;//On le remplace par 0 dans tab_id_noms_dispo
                        break; //On arrête de chercher un numéro valide
                        }
                   }
                    if(id_nom_choisi==0){ //Si à la fin des deux recherches, on a toujours pas trouvé de numéro valide, c'est que la liste de noms est épuisée.
                        for(int i=1;i<=nb_noms_dispo;i++) {//si plus aucun nom n'est disponible, on remplit à nouveau le tableau avec les noms déjà pris. Tant pis.
                            tab_id_noms_dispo[i-1]=i;
                        }
                        doublons_noms++; //on incrémente doublons_noms à chaque nouvelle fois qu'on épuise tous les noms disponibles. Les prochains patients qui prennent ces noms auront un numéro de doublons pour être différenciés.
                        id_nom_choisi=tab_id_noms_dispo[indice_random-1];//Une fois que le tableau est rempli avec uniquement des indices valides, on peut directement choisir le numéro tiré aléatoirement.
                        tab_id_noms_dispo[indice_random-1]=0; //et on enlève directement ce numéro du tableau
                    }
                }
            //on va chercher le nom-prénom correspondant à l'id choisi dans le fichier
            do {
                fscanf(fnoms,"%d %s %s",&id_scanne,prenom_scane,nom_scane); //lecture du numéro, prénom et nom dans les variables correspondantes.
            }
            while((unsigned int)id_scanne!=id_nom_choisi); //On lit une ligne du fichier. Tant que l'id lu est différente de l'id choisi, on lit une nouvelle ligne
            //à la fin de la boucle, prenom_scane correspond au prénom associé au numéro id_nom_choisi. Il en va de même pour le nom.

            if(doublons_noms>0) { //notification des doublons lorsqu'on a épuisé la liste de noms
                sprintf(chaine_doublons," (%d)",doublons_noms);
                strcat(nom_scane,chaine_doublons);//on ajoute au nom lu le numéro de doublon pour différencier le patient de son jumeau
            }
            //On attribue au patient choisi le nom et le prénom choisis
            strcpy(ppatient->prenom,prenom_scane);
            strcpy(ppatient->nom,nom_scane);

        }
        fclose(fnoms);
        return;
    }
    else { //Gestion du cas de l'erreur d'ouverture du fichier de noms.
        puts("Erreur lors de l'ouverture du fichier de noms. Vos patients s'appelleront tous John Doe.");//on avertit l'utilisateur de l'erreur.
        patient *ppatient;
        char nom_generique[14];//chaîne qui contiendra le nom "Doe" + un numéro de doublon
        strcpy(liste_patients[0]->prenom,"John");
        strcpy(liste_patients[0]->nom,"Doe");//le premier patient n'a pas de numéro de doublon.
        for(int p=1;p<nb_patients;p++) {
            ppatient=liste_patients[p];
            strcpy(ppatient->prenom,"John");
            sprintf(nom_generique,"Doe (%d)",p);//écriture de nom_generique : "Doe ($numerodedoublon)"
            strcpy(ppatient->nom,nom_generique);//copie de nom_generique dans ppatient->nom
        }
        return;
    }
}

/** \brief Instancie un certain nombre de patients et leur attribue une heure d'arrivée comprise dans une plage horaire précise.
 *
 * \param Le nombre de patients à générer (entier non signé).
 * \param L'instant de début de la plage horaire d'arrivée des patients (temps).
 * \param L'instant de fin de la plage horaire d'arrivée des aptients (temps).
 * \return Un tableau de ptr. vers chacun des patients créés.
 *
 *Les heures d'arrivées sont réparties sur la plage horaire à l'aide d'un processus de Poisson.
 *Il y a également une composante aléatoire, à l'influence adaptée en fonction de la situation.
 *La répartition est conçue pour être régulière dans l'ensemble avec quelques disparités par moments.
 *Une fois l'heure d'arrivée déterminée, le patient est correspondant est instancié, se voit attribuer cette heure d'arrivée et un id correspondant à son rang dans l'ordre d'arrivée.
 *Enfin, son adresse en mémoire est insérée dans un tableau de pointeurs vers des patients qui contiendra, à terme, un lien vers chaque patient créé.
 */
patient** genererPatients(unsigned int nb_patients,temps debut_plage_horaire, temps fin_plage_horaire) {
    /*Cette fonction permet d'instancier nb_patients patients et de leur attribuer un id et une heure d'arrivée.
    Les heures d'arrivée sont générées au moyen d'un processus de poisson avec une forte composante aléatoire :
    On découpe la plage horaire en paquets de 22 arrivées de patients maximum(car on devra calculer le factoriel d'un nombre, et factorielle(23) est trop grand pour le type (double))
    On calcule ensuite à chaque instant de la plage horaire la probabilité qu'un certain nombre de patients (jusqu'à 22) soit arrivés avec la loi de poisson, en ajoutant une composante aléatoire.
    On retient l'instant pour lequel la probabilité calculée est la plus grande.
    La composante aléatoire est intégrée arbitrairement de manière à permettre que des patients aient une chance d'arriver en même temps et d'éviter que l'écart entre deux arrivées soit trop régulier.
    On évite ainsi une distribution trop parfaite des patients sur la plage horaire.*/
    unsigned int ecart_plage_horaire = diffTemps_minutes(debut_plage_horaire,fin_plage_horaire); //on calcule le nombre de minutes entre l'instant de début de la plage horaire et l'instant de fin. diffTemps est définie dans temps.c
    patient* ppatient;//variable intermédiaire désignant le patient manipulé
    temps heure_arrivee_generee;//Variable (temps) destinée à contenir l'heure d'arrivée générée
    patient** liste_patients;//tableau d'éléments (patient*) qui sera renvoyé par la fonction
    liste_patients = malloc(nb_patients*sizeof(patient*));//on réserve assez d'espace mémoire pour stocker nb_patients pointeurs vers des patients

    /* Il y a une factorielle dans le calcul de probabilite, or à partir de factorielle 22 les nombres sont trop grands pour la précision du type (double).
    On va devoir créer des paquets de 22 patients.*/
    double proba_occur; //probabilité d'occurence de l'événement : "un patient arrive à une minute donnée de la plage horaire". vaut nombre de patients/nombre de minutes de la plage horaire.
    unsigned int instant_pg_proba=0;//entier qui contiendra la minute élue (plus grande probabilité calculée avec la loi de poisson)
    double pg_proba; //réel qui contiendra la plus grande probabilité trouvée (nécessaire pour comparer les probabilités et conserver le max)
    double proba_temp=0; //réel qui stockera les probabilités calculées avec la loi de poisson ((exp(-l*t)*(l*t)^n)/n!)
    double aleat; //Réel pseudo aléatoire modifiant légèrement la probabilité d'occurrence utilisée dans la loi de poisson, et s'ajoutant à la probabilité calculée avec la loi de poisson elle-même

    unsigned int nb_patients_restants = nb_patients; //Nombre de patients restants après formation d'un paquet de 22 patients max
    //toutes ces variables sont dues à la division par paquets de 22. On sépare les arrivées et la plage horaire par paquet de 22 patients pour se plier aux possibilités du C (factorielle(23) est trop grand pour le type (double)
    unsigned int paquet_22_patients_max; //contient entre 1 et 22 patients. Nombre de patients arrivant sur la plage horaire considérée
    unsigned int nb_paquets_22_p_max = ceil((float)nb_patients/22); //nombre de paquets formés.

    unsigned int debut_mini_plage_horaire;//La plage horaire sera subdivisée pour contenir 22 patients au maximum. On appliquera le processus de poisson à ces morceaux de plage horaires, indépendemment.
    unsigned int fin_mini_plage_horaire=0;
    unsigned int ecart_mini_plage_horaire;
    unsigned int tour = 1;

    while(nb_patients_restants > 0) {
        paquet_22_patients_max = (nb_patients_restants>22) ?22:nb_patients_restants;//à chaque tour de la boucle, on forme un nouveau paquet de 22 patients max parmis les patients restants.
        debut_mini_plage_horaire=fin_mini_plage_horaire;//On déplace le début de la plage horaire à son ancienne fin.
        //L'étendue de la mini plage horaire se calcule comme (((nombre patients dans le paquet)/(nombre total de patients)) * (étendue de la plage horaire complète)).
        if(tour%2==0&&tour!=nb_paquets_22_p_max||nb_patients<=22){
            fin_mini_plage_horaire+=ceil((float)(paquet_22_patients_max*ecart_plage_horaire)/nb_patients);//Un paquet sur deux, et pour le dernier paquet, on arrondit la fin de la plage horaire à l'entier supérieur.
        }
        else {
            fin_mini_plage_horaire+=floor((float)(paquet_22_patients_max*ecart_plage_horaire)/nb_patients);//Le reste du temps, on arrondit la fin de la plage horaire à l'entier inférieur.
        }

        ecart_mini_plage_horaire=fin_mini_plage_horaire-debut_mini_plage_horaire+1;
        for(int p=1;p<=paquet_22_patients_max;p++){ //Pour chaque patient du paquet de 22 patients max
            /**NB : Si on souhaite qu'un patient arrive toujours dès la première minute de la plage horaire, on peut faire commencer la boucle for à 0 et se terminer à paquet_22_patients_max
            (Il faudra alors ajouter 1 à l'id du patient et à son indice d'insertion dans liste_patients).
            Cette manière de faire est plus intuitive pour l'utilisateur mais elle fausse le calcul de probabilités.
            On pourrait également tirer au sort une valeur booléenne qui déterminerait laquelle des deux méthodes on appliquerait.*/

            pg_proba=0;//La plus grande probabilité est initialisée à 0. Elle est toujours inférieure à au moins une probabilité générée

            for(int instant=0;instant<fin_mini_plage_horaire-debut_mini_plage_horaire;instant++) { //On va calculer proba_temp à chaque instant de la subdivision de plage horaire, pour retenir la plus grande proba et l'instant correspondant
                //calcul de la répartition des arrivées avec une loi de poisson influencée par une composante pseudo-aléatoire
                if(nb_patients<=22){
                    aleat=rand()/(double)RAND_MAX*(((double)nb_patients_restants/ecart_plage_horaire)/4) + 0-((double)nb_patients_restants/(ecart_plage_horaire*4)); //Pour un petit nombre de patients, on favorise arbitrairement une meilleure répartition des heures d'arrivée en diminuant le poids de la composante aléatoire.
                }
                else{
                    aleat=rand()/(double)RAND_MAX*(((double)nb_patients_restants/ecart_plage_horaire)/2) + 0-((double)nb_patients_restants/(ecart_plage_horaire*4));
                }
                proba_occur = (double)paquet_22_patients_max/ecart_mini_plage_horaire + aleat;

                //le nombre aleatoire va légèrement modifier la proba d'occurence à chaque instant ainsi que la probabilité calculée par poisson elle-même
                proba_temp = aleat/5+(exp((double)-proba_occur*instant)*pow(((double)proba_occur*instant),p))/((double)factorielle(p)); //calcul de la probabilite que p patients soient arrives à instant avec une proba d'occurence d'arrivée de proba_occur
                //une composante aléatoire de faible poids est ajoutée directement au résultat pour avoir une petite chance de "mauvaise répartition" voire de "collision" (deux patients ayant des heures d'arrivées proches voire identiques).
                if(proba_temp > pg_proba) {
                    pg_proba=proba_temp;    //Conservation de la plus grande probabilite (+ composante aléatoire) et de l'instant pour lequel elle est atteinte
                    instant_pg_proba=instant; //Conservation de la minute pour laquelle on a trouvé la plus grande probabilité (avec incidence de la composante aléatoire).
                }
            }
            //à la fin de la boucle, instant_pg_proba est égal à la minute élue.

            heure_arrivee_generee=debut_plage_horaire; //On réinitialise l'heure d'arrivée générée au début de la plage horaire
            for(int i=0;i<debut_mini_plage_horaire+instant_pg_proba;i++) { //generation du temps d'arrivee comme : debut_mini_plage_horaire (la première minute de la subdivision de la plage horaire) + instant_pg_proba (la minute élue);
                incrementerTemps(&heure_arrivee_generee);
            } // à la fin de la boucle, heure_arrivee_generee vaut : debut_plage_horaire (temps) + début subdivision de plage horaire (minutes) + minute élue (minutes)

            ppatient=creerPatient(); //instanciation d'un patient
            ppatient->arrivee=heure_arrivee_generee;//Attribution de l'heure d'arrivée générée
            ppatient->id=p+(nb_patients-nb_patients_restants);//p est le rang du patient dans le paquet de 22 max. nb_patients-nb_patients_restants vaut le nombre de patients déjà traités avant ce paquet.

            liste_patients[p-1+(nb_patients-nb_patients_restants)]=ppatient; //On ajoute un pointeur vers le patient généré dans le tableau qui sera retourné à la fin
        }
        nb_patients_restants-=paquet_22_patients_max;//Une fois tout le paquet traité, on le soustrait au total de patients à traiter
        tour++; //On incrémente la variable tour. On reboucle et on traite un autre paquet de patients sur une autre subdivision de plage horaire.
    }


    return liste_patients;//Retour du tableau contenant les patients générés
}
