//Temps : à peu près 5h
#include "aleatoire.h"
//Référence : https://fr.wikipedia.org/wiki/Loi_normale

/** \brief Génération d'une probabilité et recherche dichotomique de son antécédent par la fonction de répartition P de la loi normale.
 *
 * \param L'espérance de la loi normale utilisée, un nombre de minutes (entier non signé).
 * \param Un écart (valant 3 fois l'écart-type de la loi normale utilisée, un nombre de minutes (entier non signé).)
 * \return La durée générée, comprise entre espérance-ecart et espérance+ecart (temps).
 *
 * Choix d'un réel aléatoire p compris entre 0 et P(esperance+ecart).
 * Recherche dichotomique de l'antécédent de p par P à epsilon près.
 * L'abscisse trouvée est la durée générée, exprimée en minutes.
 * Conversion en valeur de type (temps) et retour de la durée générée.
 *
 * Comme la courbe de la fonction de répartition de la loi normale est une sigmoïde centrée sur l'espérance, p a de fortes chances d'être l'image par P d'une abscisse proche de l'espérance.
 */
temps genererDureeTache(unsigned int esperance, unsigned int ecart)
{
    double eps = 0.005;//définition d'un epsilon pour majorer les erreurs maximales souhaitées.
    double pmax = integrale_LN(esperance,ecart,eps,esperance+ecart); //probabilité correspondant à la durée maximale esperance+ecart.
    double p;
    p = rand()/((double)RAND_MAX/pmax); //génération d'une probabilité comprise entre 0 et pmax.
    double a = esperance-ecart; //borne inférieure a  initialisée à la durée minimale
    double b = esperance+ecart;//borne supérieure b initialisée à la durée maximale
    double x; //abscisse recherchée
    double y; //ordonnée de P(x) (P est la fonction de répartition de la loi normale N(esperance,(ecart/3)²)
    if(p<0.5+eps && p>0.5-eps){
        x = esperance; //on choisira directement l'espérance si la probabilité est proche de 0,5
    }
    else{//calcul dichotomique de l'abscisse correspondant à la probabilité p.
        do{
            x = (a+b)/2; //milieu du segment ab
            y = integrale_LN(esperance,ecart,eps,x); //Integration de la fonction de densité de la loi normale sur [esperance-ecart ; x]
            if(y>p+eps){ //Tant que l'ordonnee est superieure a p+eps : on decale la borne superieure vers la gauche
                b=x;
            }
            else if(y<p-eps) { //Tant que l'ordonnee est inferieure a p-eps: on decale la borne inferieure vers la droite
                a=x;
            }
        }while(y>p+eps || y<p-eps); //Tant que P(x) est éloignée de plus d'epsilon de la probabilité générée :

    }
    temps t; t.annee=t.mois=t.jour=0;
    unsigned int minutes = round(x); //arrondissement à l'entier le plus proche
    unsigned int heures = minutes/60; //conversion en durée de type (temps)
    minutes-=heures*60;
    t.h=heures; t.m = minutes;
    return t; //retour

}


/** \brief Fonction permettant de calculer la densité de probabilité d'un instant suivant une loi normale N(éspérance, écart)
 *
 * \param L'espérance de la loi normale utilisée, un nombre de minutes (entier non signé).
 * \param 3 fois L'écart-type de la loi normale utilisée, un nombre de minutes (entier non signé).
 * \param Le réel x, instant tiré aléatoirement dans la plage des durées possibles
 * \return Le réel (1/((écart/3)*sqrt(2pi))) * exp(-0,5*((x-esperance)/(écart/3))²) , ordonnée du point d'abscisse x appartenant à la courbe de Gauss décrite par notre loi normale
 *
 * Calcul et retour de la densité de probabilité (ponctuelle) de x suivant la loi normale N de paramètres (espérance,écart)
 * Appelée depuis la fonction d'intégration
 */
double densite_LN(unsigned int esperance, unsigned int ecart,double x) {
    //fonction intermediaire utilisee dans la fonction integrale_LN. Calcule la densité de probabilité d'un événement suivant la loi normale - il faudra ensuite l'intégrer.
    double sigma = ((double)ecart)/3;
    double p = (1/((double)sigma * sqrt(2*PI)) )* ((double)exp(-(0.5) * pow((x-esperance) / ((double)sigma),2)));
    //La densité de probabilité de la loi normale est définie comme : (1/(sigma*sqrt(2pi))*exp(-0.5 * ((x-e)/sigma)²)) avec sigma l'ecart-type, e l'espérance et x l'abscisse.
    return p;
}

/** \brief Intégration d'une fonction mathématique définissant une loi normale de paramètres données par la méthode des rectangles sur un intervalle donné
 *
 * \param L'espérance de la loi normale utilisée, un nombre de minutes (entier non signé).
 * \param L'écart-type de la loi normale utilisée, un nombre de minutes (entier non signé).
 * \param Un réel majorant l'erreur obtenue. Correspond à l'erreur maximale souhaitée.
 * \param bornesup, l'instant tiré parmi la plage des durées possibles, un réel.
 * \return Le réel égal à la moyenne de la somme de la sur-estimation et de la sous-estimation de la somme des aires des rectangles sous la courbe décrite par densite_LN, de espérance-écart à bornesup
 *
 * Subdivision de la plage des durées permises en N rectangles de largeur régulière et de hauteur densite_LN(abscisse d'un sommet du rectangle... sommets les plus à gauche pour la sous-estimation, les plus à droite pour la sur-estimation)
 * Calcul de la somme des aires des rectangles sous la courbe par sur-estimation et par sous-estimation.
 * Calcul de la valeur absolue différence sur-estimation - sous-estimation ... si elle est inférieure au majorant de l'erreur : retour de la moyenne des deux estimations. Sinon : incrémentation de N.
 * La valeur de retour est une probabilité comprise entre 0 et 1. Multipliée par 2*écart et ajoutée au début de la plage des durées possibles, elle donne la durée générée.
 * Appelée depuis genererDureeTache.
 */
double integrale_LN(unsigned int esperance,unsigned int ecart,double erreur,double bornesup) { /*fonction permettant d'intégrer la densité de probabilité calculée avec densite_LN
    On utilise la méthode des rectangles. x est la borne supérieure, (on intégre de borneinf à x) et borneinf vaut esperance-ecart*/
    double sur_eval = 0; //sur-évaluation de l'intégrale
    double sous_eval = 0;//sous-évaluation de l'intégrale
    double x = 0; //abscisse dont on calcule l'image pour former les rectangles
    double h = 0; //(largeur d'une subdivision de l'axe des abscisses.
    double borneinf = esperance - ecart;
    int N=2; //N est initialisé à 2 (2 subdivisions de l'axe des abscisses).
    unsigned short int evalErreur;//le booléen evalErreur permet de continuer le calcul tant que l'erreur n'est pas inférieur à celle indiquée en argument.

    do {//On calcule dans cette boucle la somme des aires des rectangles pour N subdivisions. On augmente le nombre de subdivisions à chaque tour.
        sous_eval=0;//à chaque tour de boucle, on remet les évaluations à 0.
        sur_eval=0;
        h=((double)(bornesup-borneinf))/N;
        for(int i=0;i<N;i++){//On va sommer les aires des N rectangles rectangles
            x = borneinf +i*h;
            sous_eval += densite_LN(esperance,ecart,x)*h;//L'aire de la sous-évaluation vaut la somme des h*LoiNormale(x) pour i allant de 0 à N-1
            sur_eval += densite_LN(esperance,ecart,(x+h))*h;//L'aire de la sous-évaluation vaut la somme des h*LoiNormale(x+h) pour i allant de 0 à N-1
        }
        N++; //On incrémente N pour réessayer avec plus de subdivisions pour amoindrir l'erreur.
        evalErreur = (abs(sur_eval - sous_eval)>erreur); //evalErrueur prend la valeur 1 si l'écart entre la sur-évaluation et la sous-évalutation est plus important que l'erreur maxmimale souhaitée.
    }
    while (evalErreur);
    double f_moy = (sous_eval+sur_eval)/2; //calcul de la moyenne de la sur-évalutation et de la sous-évaluation
    return f_moy;//On choisit de retourner la moyenne qui est généralement une bonne approximation
}


double factorielle(unsigned long int n){
    //Fonction permettant de calculer le factioriel d'un nombre : 5! = 5*4*3*2*1...
    double output=1;//initialisation du réel de sortie à 1
    for(unsigned long int i = 2;i<=n;i++) {
        output*=i;//multiplication du réel de sortie par chaque entier entre 2 et n inclus.
    }
    return output; // retour du réel de sortie.
}

