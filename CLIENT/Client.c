#include "header.h"

int saisieInt(int* i, int min, int max, int plage)
{
    int invalide = 1;
    /* 
     * Il faut toujours vérifier la validiter d'un pointeur avant de s'en servir
     */
    if (NULL == i)
    {
        puts("Pointeur invalide dans la fonction saisieInt");
        /* Appel invalide */
        return EXIT_FAILURE;
    }

    /* Boucle de contrôle de la validité de la saisie de l'utilisateur */
    do
    {
        /*
         * Collecte de la saisie de l'utilisateur au format entier (%d), stocké
         * dans le pointeur fourni en argument. Comme c'est un pointeur, il est
         * inutile de faire précéder le nom de la variable par le caractère &.
         */
        if (0 == scanf("%d", i))
        {
            /* Elimination des caractères invalides */
            while (getc(stdin) != '\n');
            /* Indication à l'utilisateur */
            puts("Saisie invalide, caractère(s) non valide(s)");
            /* Nouvelle demande de saisie */
            continue;
        }

        /* Contrôle des bornes */
        if (plage && (*i < min || *i > max))
        {
            puts("Saisie invalide, non compris dans la page de valeurs");
            invalide = 1;
        }
        else if (!plage && *i != min && *i != max)
        {
            puts("Saisie invalide, doit être égal à l'une des deux valeurs");
            invalide = 1;
        }
        else
        {
            invalide = 0;
        }
    }
    while (invalide);

    /* Tout est OK */
    return EXIT_SUCCESS;
}

void ecritMess (char *messageEnvoi, int descripteurSocket) {
	int ecrits = write(descripteurSocket, messageEnvoi, strlen(messageEnvoi)); // message à TAILLE variable
	switch(ecrits)
	{
		case -1 : /* une erreur ! */
			perror("write");
			close(descripteurSocket);
			exit(-3);
		case 0 : /* la socket est fermée */
			fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
			close(descripteurSocket);
			exit(0);
		default: /* envoi de n octets */
			break;
	}
}


void recup_size(char **dimension, int descripteurSocket){
	ecritMess( "/getSize", descripteurSocket);
	
	char messageRecu[LG_MESSAGE];
	memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
	
	int lus = read(descripteurSocket, messageRecu, LG_MESSAGE*sizeof(char)); /* attend un message de TAILLE fixe */
	switch(lus)
	{
		case -1 : /* une erreur ! */
			perror("read");
			close(descripteurSocket);
			exit(-4);
		case 0 : /* la socket est fermée */
			fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
			close(descripteurSocket);
			exit(0);
		default: /* réception de n octets */
			dimension[0] = strtok(messageRecu, "x");
			dimension[1] = strtok(NULL, "x");
			dimension[1] = strtok(dimension[1], "\n");
			
	}
}

Parametre parametrage(int nbArgs, char *args[]){

	//si le parametre args est vide, le port est initialisé avec la valeur suivante :
	Parametre parametre;
	parametre.PORT = 5000;
	parametre.IP = "127.0.0.1";
	
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
		if( strcmp(args[i], parametre_IP) == 0){
			i++;
			parametre.IP = args[i];
		}
	}
	
	return parametre;
}

void menu() {

	puts("\t\t ╔═════════════════════════════════════╗");
	puts("\t\t ║          Commande au choix          ║");
	puts("\t\t ╠═════════════════════════════════════╣");
	puts("\t\t ║  1 - Afficher la matrice            ║");
	puts("\t\t ║  2 - Afficher la taille             ║");
	puts("\t\t ║  3 - Afficher les pix/min           ║");
	puts("\t\t ║  4 - Version du protocole           ║");
	puts("\t\t ║  5 - Temps d'attente                ║");
	puts("\t\t ║  6 - Mettre un pixel                ║");
	puts("\t\t ║  0 - Quitter                        ║");
	puts("\t\t ╚═════════════════════════════════════╝");
}

int main(int nbArgs, char *args[])
{
	/*recupère les paramètres pour initialiser le client*/
	Parametre parametres;
	parametres = parametrage(nbArgs, args);
	printf("PORT : %d\n", parametres.PORT);
	printf("IP : %s\n", parametres.IP);
	
	int descripteurSocket;
	
	struct sockaddr_in pointDeRencontreDistant;
	socklen_t longueurAdresse;
	
	char messageEnvoi[LG_MESSAGE]; /* le message de la couche Application ! */
	int ecrits, lus; /* nb d’octets ecrits et lus */
	int retour;
	
	
	// Crée un socket de communication
	descripteurSocket = socket(PF_INET, SOCK_STREAM, 0);
	/* 0 indique que l’on utilisera leprotocole par défaut associé à SOCK_STREAM soit TCP */// Teste la valeur renvoyée par l’appel système socket()
	if(descripteurSocket < 0)/* échec ? */
	{
		perror("socket");// Affiche le message d’erreur
		exit(-1);// On sort en indiquant un code erreur
	}
	
	printf("Socket créée avec succès ! (%d)\n", descripteurSocket);
	
	longueurAdresse = sizeof(pointDeRencontreDistant);
	// Initialise à 0 la structure sockaddr_in
	memset(&pointDeRencontreDistant, 0x00, longueurAdresse);
	// Renseigne la structure sockaddr_in avec les informations du serveur distant
	pointDeRencontreDistant.sin_family = PF_INET;
	// On choisit le numéro de port d’écoute du serveur
	pointDeRencontreDistant.sin_port = htons(parametres.PORT); // = 5000
	// On choisit l’adresse IPv4 du serveur
	inet_aton(parametres.IP, &pointDeRencontreDistant.sin_addr); // à modifier selon ses besoins
	// Débute la connexion vers le processus serveur distant
	if((connect(descripteurSocket, (struct sockaddr *)&pointDeRencontreDistant, longueurAdresse)) == -1)
	{
		perror("connect"); // Affiche le message d’erreur
		close(descripteurSocket); // On ferme la ressource avant de quitter
		exit(-2); // On sort en indiquant un code erreur
	}

	printf("Connexion au serveur réussie avec succès !\n");
	
	char *dimension[] = {"", ""};
	recup_size(dimension, descripteurSocket);
	
	// Initialise poll
	struct pollfd poll_message;
	memset(&poll_message, 0, sizeof(poll_message));
	poll_message.fd = 0;
	poll_message.events = POLLIN; 
	
	menu();
	printf("Faites votre choix (0 à 6):\n");
	
	int rgb[3]; 
	char *position = (char*) malloc(LG_MESSAGE*sizeof(char));
	char couleur_b64[5] = "";
	// poll
	while(1)
	{
		memset(messageEnvoi, 0x00, LG_MESSAGE*sizeof(char));
	
		if( poll(&poll_message, 1, 100) == 1){
			
			char *messageRecu; /* le message de la couche Application ! */
			int c = 0;
			saisieInt(&c, 0, 6, 1);


			switch(c){
				case 1:
					ecritMess("/getMatrix", descripteurSocket);
					//initialiser une matrice comme pour le serveur
					break;
				
				case 2:
					ecritMess("/getSize", descripteurSocket);
					printf("Faites votre choix (0 à 6):\n");
					break;
					
				case 3:
					ecritMess("/getLimits", descripteurSocket);
					break;
					
				case 4:
					ecritMess("/getVersion", descripteurSocket);
					break;
					
				case 5:
					ecritMess("/getWaitTime", descripteurSocket);
					break;
					
				case 6:
					//boucle for qui recup trois int et qui les stock dans un tableau 
					//cree une string
					//strcat(string, rgb_to_string(tableau de int)
					for(int i=0; i<3; i++){
						printf("entrer un nombre entre 0 et 255 : ");
						scanf("%d", &rgb[i]);
					}
					
					rgb_to_string(rgb, couleur_b64);
									
					strcat(messageEnvoi, "/setPixel ");
					
					printf("entrer sa position en largeur (nombre entre 0 et %s) : ", dimension[0]-1);
					scanf("%s", position);
					strcat(messageEnvoi, position);
					strcat(messageEnvoi, "x");
					printf("entrer sa position en hauteur (nombre entre 0 et %s) : ", dimension[1]-1);
					scanf("%s", position);
					strcat(messageEnvoi, position);
					strcat(messageEnvoi, " ");
						
					strcat(messageEnvoi, couleur_b64);
					printf("%s\n", messageEnvoi);
					ecritMess(messageEnvoi, descripteurSocket);
					break;
					
				default:
					exit(1);
					break;
			}
			system("clear");
			menu();
			
			/* Reception des données du serveur */
			if(c == 1){
				messageRecu = (char *) malloc(atoi(dimension[0])*atoi(dimension[1])*LG_MESSAGE*sizeof(char));
				lus = read(descripteurSocket, messageRecu, atoi(dimension[0])*atoi(dimension[1])*256*sizeof(char));
			}
			else{
				messageRecu = (char *) malloc(LG_MESSAGE*sizeof(char));
				lus = read(descripteurSocket, messageRecu, 256*sizeof(char));
			}
			switch(lus)
			{
				case -1 : /* une erreur ! */
					perror("read");
					close(descripteurSocket);
					exit(-4);
				case 0 : /* la socket est fermée */
					fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
					close(descripteurSocket);
				return 0;
				default: /* réception de n octets */
					printf("Message reçu du serveur : \n %s\n (%d octets)\n\n", messageRecu, lus);
					//if c == 1, on rempli la matrice avec la string recu decoupé par paquet de 4
			}
			strcpy(messageRecu, "\0");
		}
	}
	
	// On ferme la ressource avant de quitter
	close(descripteurSocket);
	return 0;
}
