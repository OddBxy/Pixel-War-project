#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* pour close et sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

#include <poll.h>
#define PORT IPPORT_USERRESERVED // = 5000
#define LG_MESSAGE 256

#define parametre_PORT "-p"
#define parametre_SIZE "-s"

typedef struct Parametre{
	int PORT;
	int *dimensions;
}Parametre;



typedef struct User User;
struct User{
	int socketClient;
	struct sockaddr_in *sockin;
	User *suivant;
};

Parametre parametrage(int nbArgs, char *args[]){

	//si le parametre args est vide, le port et la matrice sont initialiser avec les valeurs suivantes :
	Parametre parametre;
	parametre.PORT = 5000;
	parametre.dimensions = (int*) calloc(2, sizeof(int));
	parametre.dimensions[0] = 80;
	parametre.dimensions[1] = 40;
	
	if(nbArgs > 5){
		printf("la ligne de commande contient trop de paramètres\n");
		exit(0);
	}
	for(int i=1; i < nbArgs; i++){
		//i commence a 1 car args[0] == ./serveur
		if( strcmp(args[i], parametre_PORT) == 0){
			i++;
			parametre.PORT = atoi(args[i]);
			//atoi permet de convertir des strings en integers
		}
		else if( strcmp(args[i], parametre_SIZE) == 0){
			char *dimensions_string[2] = {"", ""};
			i++;
			dimensions_string[0] = strtok(args[i], "x");
			dimensions_string[1] = strtok(NULL, "x");
			//strtok permet de separer une chaine de charactere selon un charactere donné
			for(int j=0; j<2; j++) parametre.dimensions[j] = atoi( dimensions_string[j] );
		}
	
	}
	
	return parametre;
}


User *ajout_Client(User *liste, int socketEcoute, socklen_t *address_len){
	struct sockaddr_in pointDeRencontreDistant;

	User *client_suivant = NULL;
	client_suivant = malloc(sizeof(User));
	client_suivant->suivant = NULL;
	client_suivant->socketClient = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, address_len);
	client_suivant->sockin = &pointDeRencontreDistant;
	
	
	if(liste == NULL) liste = client_suivant;
	else{
		User *parcour = liste;
		while(parcour != NULL) parcour = parcour->suivant;
		parcour = client_suivant;
	
	}
	return liste;
}

int main(int nbArgs, char *args[]){
	
	/*recupère les paramètres pour initialiser le serveur*/
	Parametre parametres;
	parametres = parametrage(nbArgs, args);
	printf("%d\n", parametres.PORT);
	printf("%d\n", parametres.dimensions[0]);
	/*--------------------------------------------------*/
	
	User *tete_liste = NULL;
	
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
	
	
	/*creation d'un pollfd pour l'acceuil de plusieur clients*/
	struct pollfd poll_accept;
	memset(&poll_accept, 0, sizeof(poll_accept));
	poll_accept.fd = socketEcoute;
	poll_accept.events = POLLIN; 
	/*------------------------------------------------------*/
	
	
	memset(messageEnvoi, 0x00, LG_MESSAGE*sizeof(char));
	memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
	printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");
	
	// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment !
	while(1)
	{	
		/*----------------------------poll---------------------------------------------------------*/
		if( poll(&poll_accept, 1, 100) == 1){
		
			//socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
			tete_liste = ajout_Client(tete_liste, socketEcoute, &longueurAdresse);
			
			User *parcour = tete_liste;
			while(parcour->suivant != NULL) parcour = parcour->suivant;
			
			if (parcour->socketClient < 0)
			{
				perror("accept");
				close(parcour->socketClient);
				close(socketEcoute);
				exit(-4);
			}
			// On réceptionne les données du client et on affiche ses informations
			printf("Connection client de : %s:%d\n", inet_ntoa(parcour->sockin->sin_addr),ntohs(parcour->sockin->sin_port));
			free(parcour);
			
		}
		
		/*creation d'un pollfd pour la reception de message de clients
		struct pollfd poll_message;
		memset(&poll_message, 0, sizeof(poll_message));
		poll_message.fd = socketDialogue;
		poll_message.events = POLLIN; 
		/------------------------------------------------------*/
		
		/*
		if(poll(&poll_message, 1, 100) == 1){			
			lus = read(socketDialogue, messageRecu, LG_MESSAGE*sizeof(char)); // ici appel bloquant
			switch(lus)
			{
				case -1 : // une erreur ! 
					perror("read");
					close(socketDialogue);
					exit(-5);
				case 0 : // la socket est fermée 
					fprintf(stderr, "La socket a été fermée par le client !\n\n");
					close(socketDialogue);
					return 0;
				default: printf("Message reçu du client : %s (%d octets)\n\n", messageRecu, lus);
					
			}
			// On envoie des données vers le client (cf. protocole)
			sprintf(messageEnvoi, "message reçu\n");
			ecrits = write(socketDialogue, messageEnvoi, strlen(messageEnvoi));
			switch(ecrits)
			{
				case -1 : // une erreur ! 
					perror("write");
					close(socketDialogue);
					
					exit(-6);
				case 0 : // la socket est fermée 
					fprintf(stderr, "La socket a été fermée par le client !\n\n");
					close(socketDialogue);
					return 0;
				default: // envoi de n octets 
					printf("Message %s envoyé (%d octets)\n\n", messageEnvoi, ecrits);
			}			
		}*/
		/*-----------------------------------------------------------------------------------*/
		
		// On ferme la socket de dialogue et on se replace en attente ..
		//close(socketDialogue); //pas forcement 
	//else printf("test\n");
	}
	//close(socketEcoute);
	return 0;
}
