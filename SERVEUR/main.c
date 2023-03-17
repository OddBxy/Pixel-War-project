#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "header.h"
int main(int nbArgs, char *args[]){
	
	/*recupère les paramètres pour initialiser le serveur*/
	Parametre parametres;
	parametres = parametrage(nbArgs, args);
	/*--------------------------------------------------*/
	
	//on initialise la matrice avec les parametres
	char ***matrice = (char ***) malloc(parametres.dimensions[0] * LG_MESSAGE);
	for(int i=0; i<parametres.dimensions[0]; i++){
		matrice[i] = (char **) malloc(parametres.dimensions[1] * LG_MESSAGE);
		for(int j=0; j<parametres.dimensions[1]; j++){
			matrice[i][j] = (char*) malloc(LG_MESSAGE);
			strcpy(matrice[i][j],  "////");
		}
	}
	
	int socketEcoute;
	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;

	char messageEnvoi[LG_MESSAGE]; /* le message de la couche Application ! */
	char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */
	int ecrits, lus; /* nb d’octets ecrits et lus */


	// Crée un socket de communication
	socketEcoute = socket(PF_INET, SOCK_STREAM, 0); /* 0 indique que l’on utilisera le protocole par défaut associé à SOCK_STREAM soit TCP */
	// Teste la valeur renvoyée par l’appel système socket()
	if(socketEcoute < 0) /* échec ? */
	{
		perror("socket"); // Affiche le message d’erreur
		exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", socketEcoute);
	
	
	// On prépare l’adresse d’attachement locale
	longueurAdresse = sizeof(struct sockaddr_in);
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse);
	pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // toutes les interfaces locales disponibles
	pointDeRencontreLocal.sin_port = htons(parametres.PORT);
	
	// On demande l’attachement local de la socket
	if((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0)
	{
		perror("bind");
		exit(-2);
	}
	printf("Socket attachée avec succès !\n");
	// On fixe la taille de la file d’attente à 5 (pour les demandes de connexion non encore
	
	
	if(listen(socketEcoute, 5) < 0)
	{
		perror("listen");
		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");
	
	
	/*creation d'un pollfd pour l'acceuil de plusieur clients*, et d'un tableau de poll pour la reception des différents message*/
	struct pollfd poll_accept;
	memset(&poll_accept, 0, sizeof(poll_accept));
	poll_accept.fd = socketEcoute;
	poll_accept.events = POLLIN; 
	
	struct pollfd *poll_message = calloc(0, sizeof(struct pollfd));
	/*-------------------------------------------------------------------------------------------------------------------------*/
	
	
	memset(messageEnvoi, 0x00, LG_MESSAGE*sizeof(char));
	memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
	printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");
	
	printf("PORT : %d\n", parametres.PORT);
	printf("LARGEUR : %d\n", parametres.dimensions[0]);
	printf("HAUTEUR : %d\n", parametres.dimensions[1]);
	printf("LIMITE DE PIXELS : %d\n\n", parametres.limite);
	
	
	User *tete_liste = NULL;
	int nbClient = 0;
	int *pt_nbClient = &nbClient;
	
	while(1)
	{	
		/*----------------------------poll---------------------------------------------------------*/
		
		if( poll(&poll_accept, 1, 100) == 1){

			//socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
			tete_liste = ajout_Client(tete_liste, socketEcoute, &longueurAdresse);
			User *parcour = tete_liste;
			while(parcour->suivant != NULL){
				parcour = parcour->suivant;
			}
			
			if (parcour->socketClient < 0)
			{
				perror("accept");
				close(parcour->socketClient);
				close(socketEcoute);
				exit(-4);
			}
			// On réceptionne les données du client et on affiche ses informations
			printf("Connection client de : %s:%d\n", inet_ntoa(parcour->sockin->sin_addr),ntohs(parcour->sockin->sin_port));
			
			nbClient +=1 ;
			poll_message[nbClient].fd = parcour->socketClient;
			poll_message[nbClient].events = POLLIN;
			
		}
		
		
		if(nbClient > 0){
			reception_message(poll_message, pt_nbClient, &tete_liste, parametres, matrice);	
		}

		/*-----------------------------------------------------------------------------------*/
	}
	close(socketEcoute);
	return 0;
}
