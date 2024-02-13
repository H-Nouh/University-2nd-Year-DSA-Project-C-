#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

//partitions
    typedef struct partition {  //déclaration d'une partition
        int ID, adresse, taille, etat; //l'ID est l'id du processus chargé, il prend 0 si il n'y a aucun processus chargé dans la partition, sinon il prend l'id du processus 
                                       //l'état est un entier, il prend 1 pour occupé et 0 pour libre
        } partition; 

    typedef struct cellule {  //déclaration d'une cellule de la liste, qui contient une partie information de type partition, et une partie suivant de type pointeur vers cellule
        struct partition info;
        struct cellule *suivant;
    } cellule; 

    typedef cellule* liste;  //déclaration d'une liste, qui est un pointeur vers une cellule

    liste CreerListe(liste T){  //fonction de creation d'une liste à partir des partitions stockés en fichier.txt
        liste P =(cellule*)malloc(sizeof(cellule));
        FILE* fichier=NULL;
        fichier=fopen("Partitions.txt", "r");
        fscanf(fichier, "%d %d %d %d", &P->info.ID,&P->info.adresse, &P->info.taille, &P->info.etat);
        T=P;
        while (!feof(fichier)){
            liste Q =(cellule*)malloc(sizeof(cellule));
            fscanf(fichier, "%d %d %d %d", &Q->info.ID, &Q->info.adresse, &Q->info.taille, &Q->info.etat);    
            P->suivant = Q;
            P = Q; 
        }
        fclose(fichier);
        P->suivant = NULL;
        printf("L'etat initial de memoire a ete creer avec succes!\n");
        return T;
    }

    void AfficheListe (liste T){  //fonction qui affiche le contenue d'une liste de partition
    if (T==NULL)
        printf("La liste est vide!\n");
    else{
        printf("\nContenu de la liste de partitions:\n");
        while(!T==NULL){
            printf("ID de processus chargee: %d, adresse de partition: %d, taille de partition: %d, etat de partition: %d\n", 
                            T->info.ID, T->info.adresse, T->info.taille, T->info.etat);
            T=T->suivant;
        }
    }
    }

//processus
    typedef struct processus {
        int id, taille;
        float temps, duree; //temps d'arrivé et durée d'execution.
    }processus;

    typedef struct Element{ //déclaration d'un element de la file
        processus P;
        struct Element* suiv; 
    }Element;

    typedef struct File{
        Element* tete;
        Element* queue; //déclaration du type file
    }File;


    //Primitives de manipulation de notre file
        void InitFile(File* F) { 
                F->tete = NULL; 
                F->queue = NULL; 
            } 

        int FileVide(File F) { 
                return (F.tete == NULL); 
            } 

        void Enfiler(File* F, processus Pr) {  //pour ajouter un nouveau élement en tete de file
                Element* NVP =(Element*)malloc(sizeof(Element)); //NVP: nouveau processus qu'on veut enfiler.
                (NVP->P).id = Pr.id; 
                (NVP->P).taille = Pr.taille; 
                (NVP->P).temps = Pr.temps;
                (NVP->P).duree = Pr.duree;
                NVP->suiv = NULL; 

                if (FileVide(*F)) { 
                    F->tete = NVP; 
                } else { 
                    F->queue->suiv = NVP; 
                } 
                F->queue = NVP; 
            } 

        processus Defiler(File* F) { //pour retirer un élement en queue de file
                if (!FileVide(*F)) { 
                    Element* Temp = F->tete;  
                    F->tete = F->tete->suiv; 
                    if(F->tete==NULL) 
                    { 
                    F->queue=NULL; 
                    } 
                    processus x = Temp->P;
                    free(Temp); 
                    return(x);
                } 
                processus y;
                return (y);
            } 

        processus TeteFile(File F) { //pour retourner l'information dans le premier élement de la file
                return (F.tete)->P; 
            }    


        File CreerFile(File F){  //Fonction de creation d'une file à partir des partitions stockéss en fichier.txt

                InitFile(&F);
                FILE* fichier=NULL;
                fichier=fopen("Processus.txt", "r");

                while (!feof(fichier)){
                    processus Temp;
                    fscanf(fichier, "%d %d %f %f", &Temp.id, &Temp.taille, &Temp.temps, &Temp.duree);
                    Enfiler (&F, Temp);
                }

                fclose(fichier);
                printf("La file de processus a ete creer avec succes!\n");
                return (F);
            }

        
        void AfficheFile(File F) {  //Fonction qui affiche le contenu d'une File de processus
            if (FileVide(F)) { 
                printf("La file de processus est vide.\n");
                return; 
                } 

            processus Pr;
            printf("\nContenu de la file de processus:\n"); 
            File Temp; 
            InitFile(&Temp); 
            
            while(!FileVide(F)) 
            {                                         
                Pr=Defiler(&F);
                Enfiler(&Temp,Pr); 
                printf("id: %d, taille de processus: %d, Le temps d'arrive du processus: %f, duree d'execution du prossesus: %f\n", 
                            Pr.id, Pr.taille, Pr.temps, Pr.duree);
                 
            } 
                            
            while(!FileVide(Temp)) 
            { 
                Pr = Defiler(&Temp);  
                Enfiler (&F, Pr);
            }
            
                    
                
        }


//Politiques D'allocation:

    void Residu (liste P, int taille){  //Fonction qui crée une nouvelle partition avec le residu de la partition précédente après le chargement d'un processus
        liste Q =(cellule*)malloc(sizeof(cellule));
        Q->info.taille=P->info.taille-taille;
        P->info.taille=taille;
        Q->info.ID=0; Q->info.adresse=P->info.adresse+taille; Q->info.etat=0; 
        Q->suivant=P->suivant;
        P->suivant=Q;
    }

    
    void FirstFit (File* F, liste* T){
        if (FileVide(*F)) printf("\nLa file des processus est vide!\n");
        if (*T==NULL) printf("\nLa liste des partition est vide!\n");
        if (!FileVide(*F) && *T!=NULL) {
            processus Pr=Defiler(&*F);
            liste P=*T;
            while (P!=NULL && (((P->info).taille<Pr.taille) || ((P->info).etat==1))) {
                P=P->suivant;
            }
            if (P==NULL) {
                printf("\nAucune partition ne convient pour charger le processus, donc on va le reculer vers la queue de la file.\n");
                Enfiler (&*F, Pr);
            }
            else{
                (P->info).ID=Pr.id;
                (P->info).etat=1;
                Sleep(Pr.duree*1000);
                (P->info).etat=0;
                printf("\nLe processus est charge avec succes!\n");
                if (Pr.taille!=P->info.taille){
                    Residu (P, Pr.taille);
                    printf("\nIl y a un residu, une nouvelle partition a ete cree.\n");
                }
                else printf("\nIl n'y a pas un residu.\n");
                
            }
        }
    }


    void BestFit (File* F, liste* T){
        if (FileVide(*F)) printf("\nLa file des processus est vide!\n");
        if (*T==NULL) printf("\nLa liste des partition est vide!\n");
        if (!FileVide(*F) && *T!=NULL) {
            processus Pr=Defiler(&*F);
            liste P=*T;
            liste min=NULL;
            while (P!=NULL){
                if ((P->info.etat==0) && (Pr.taille<P->info.taille)){
                    if (min==NULL) min=P;
                    else{
                        if (P->info.taille<min->info.taille) min=P;
                    } 
                } 
                P=P->suivant;
            }
            if (min==NULL) {
                printf("\nAucune partition ne convient pour charger le processus, donc on va le reculer vers la queue de la file.\n");
                Enfiler (&*F, Pr);
            }
            else{
                (min->info).ID=Pr.id;
                (min->info).etat=1;
                printf("\nLe processus est charge avec succes!\n");
                Sleep(Pr.duree*1000);
                (min->info).etat=0;
                if (Pr.taille!=min->info.taille){
                    Residu (min, Pr.taille);
                    printf("\nIl y a un residu, une nouvelle partition a ete cree.\n");
                }
                else printf("\nIl n'y a pas un residu.\n");
            }
        }
    }

    void WorstFit (File* F, liste* T){
        if (FileVide(*F)) printf("\nLa file des processus est vide!\n");
        if (*T==NULL) printf("\nLa liste des partition est vide!\n");
        if (!FileVide(*F) && *T!=NULL) {
            processus Pr=Defiler(&*F);
            liste P=*T;
            liste max=NULL;
            while (P!=NULL){
                if ((P->info.etat==0) && (Pr.taille<P->info.taille)){
                    if (max==NULL) max=P;
                    else{
                        if (P->info.taille>max->info.taille) max=P;
                    } 
                } 
                P=P->suivant;
            }
            if (max==NULL) {
                printf("\nAucune partition ne convient pour charger le processus, donc on va le reculer vers la queue de la file.\n");
                Enfiler (&*F, Pr);
            }
            else{
                (max->info).ID=Pr.id;
                (max->info).etat=1;
                Sleep(Pr.duree*1000);
                (max->info).etat=0;
                printf("\nLe processus est charge avec succes!\n");
                if (Pr.taille!=max->info.taille){
                    Residu (max, Pr.taille);
                    printf("\nIl y a un residu, une nouvelle partition a ete cree.\n");
                }
                else printf("\nIl n'y a pas un residu.\n");
            }
        }
    }

    void Supprimer(liste* T){  //Fonction qui libère touts les processus après leur exécution
        liste P=*T;
        while (P!=NULL){
            if((P->info.etat==0) && (P->info.ID!=0)) P->info.ID=0;
            P=P->suivant;
        }
    }



int main (){

liste T, P;
File F;
int choix, i;
char politique;

printf("                                              MENU PRINCIPAL\n");
printf("--------------------------------------------------------------------------------------------------------------\n");

do{
    printf("--------------------------------------------------------------------------------------------------------\nACTIONS POSSIBLES:\n");
    printf("1-Creer l'etat initial de la memoire\n");
    printf("2-Afficher l'etat de la memoire de maniere textuelle\n");
    printf("3-Creer un ensemble de processus en attente dans la file\n");
    printf("4-Afficher l'etat initial de la file des processus\n");
    printf("5-Choisir une politique d'allocation\n");
    printf("6-Charger un processus en memoire\n");
    printf("7-Supprimer un processus termine de la memoire\n");
    printf("8-Exit\n");
    printf("Veuillez choisir une action a partir du menu au-dessus:");
    scanf("%d",&choix);
    printf("--------------------------------------------------------------------------------------------------------\n");


switch(choix){

    case 1:
        T=CreerListe(T);
        break;
    
    case 2:
        printf("+----------+--------------+------------------+---------+\n");
        printf("| Partion  | Adresse      | Taille (en Bits) | etat    |\n");
        printf("+----------+--------------+------------------+---------+\n");
        P=T; i=1;
        while(P!=NULL){
        printf("+----------+--------------+------------------+---------+\n");
        printf("| P %d     | %d           | %d               | %d      |\n", i,(P->info).adresse,(P->info).taille,(P->info).etat);
        printf("+----------+--------------+------------------+---------+\n");
        P=P->suivant;
        i=i+1;
        }
        break;

    case 3:
        F=CreerFile(F);
        break;

    case 4:
        AfficheFile(F);
        break;

    case 5:
        printf("POLITIQUES D'ALLOCATION:\n");
        printf("A-FirstFit\n");
        printf("B-BestFit\n");
        printf("C-WorstFit\n");
        getchar();
        printf("Veuillez introduisez votre choix:");
        do {                                
            scanf("%c",&politique); 
            printf("Choix invalide, veuillez reintroduisez votre choix.");
            getchar();
         } while ((politique!='A') && (politique!='B') && (politique!='C'));
        break;
        
    case 6:
            if (politique=='A'){
            printf("politique: FirstFit");
            FirstFit(&F, &T);
            }

            if (politique=='B'){
            printf("politique: BestFit");
            BestFit(&F, &T);
            }

            if (politique=='C'){
            printf("politique: WorstFit");
            WorstFit(&F, &T);
            }
            AfficheListe(T);
            AfficheFile(F);
            break;

    case 7:
    Supprimer (&T);
    AfficheListe(T);
    AfficheFile(F);
    case 8:
        break;
    default:
        printf("invalid input");
            break;
    }
    } while(choix!=8);
}



