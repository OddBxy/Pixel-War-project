#include "header.h"

const int BIT = 7;

char ascii[64] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','0','P','Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j','k','1','m','n','o','p','q','r','s','t','u','v','w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'};

char values[64][64] = { "000000","000001","000010","000011","000100","000101","000110","000111","001000","001001","001010","001011","001100","001101","001110","001111","010000","010001","010010","010011","010100","010101","010110","010111","011000","011001","011010","011011","011100","011101","011110","011111","100000","100001","100010","100011","100100","100101","100110","100111","101000","101001","101010","101011","101100","101101","101110","101111","110000","110001","110010","110011","110100","110101","110110","110111","111000","111001","111010","111011","111100","111101","111110","111111"};


char* binaire_8bits(int i){
	char *str = (char*) calloc(BIT, sizeof(int));
	if(0 <= i && i <= 65535) {
		for(int j = 0; j < BIT + 1; j++) {
		    if (i % 2 == 0) {
			strcpy(&str[j], "0");
		    } 
		    else {
			strcpy(&str[j], "1");
		    }
		    i = i / 2;
		}
	}
	
	//reverse
	for(int i = BIT; i > BIT/2; i--)
	{
		char tmpry = str[i];
		str[i] = str[BIT - i];
		str[BIT - i] = tmpry;
	}
	return str;
}

int binaire_toRGB(char *string){
	int nb = 0;
	
	//reverse string
	char str[8] = "";
	strcat(str, string);
	for(int i = BIT; i > BIT/2; i--)
	{
		char tmpry = str[i];
		str[i] = str[BIT - i];
		str[BIT - i] = tmpry;
	}
	
	for(int i=0; i<8; i++){
		int value = str[i] -'0'; //convertie le char en int
		if(value ==1){
			int p = pow(2, i);
			nb += p;
		}
	}	
	return nb;
}


int* ascii_to_rgb(char *str){

	char *binaire_total = (char *) malloc(BIT*sizeof(char));
	
	
	//on convertit la string en un binaire de 24bits (6*4 = 24)
	for(int i=0; i < 4; i++){
		for(int j=0; j<64; j++){
			if(strncmp(&str[i], &ascii[j], 1) == 0){
				strcat(binaire_total, values[j]);
			}		
		}	
	}

	//on decoupe le binaire de 24 bits en 3 chaine de 8bits
	char **binaire_8bit = (char **) malloc(sizeof(char));
	int pallier = 0;
	int j =0;
	for(int i=0; i<3; i++){
		binaire_8bit[i] = (char *) malloc(BIT*sizeof(char)); //on initialise
		for(int index=0; j < pallier+8; index++){
			binaire_8bit[i][index] = binaire_total[j];
			j++;
		}
		pallier += 8;
	}
	//on a binaire_8bit = [[qqchose]; [qqchose]; [qqchose]]
	
	int *rgb = (int*) malloc(3*sizeof(int));
	for(int i=0; i<3; i++){
		rgb[i] = binaire_toRGB(binaire_8bit[i]);
	}

	return rgb;
}


void rgb_to_string(int *rgb, char *old_string){

	char **binaire = (char **) malloc(sizeof(char));
	char *binaire_total = (char *) malloc(BIT*sizeof(char));
	
	char **SIXbit = (char **) malloc(sizeof(char));

	
	
	for(int i=0; i<3; i++){
		binaire[i] = (char *) malloc(BIT*sizeof(char)); //on initialise
		strcpy(binaire[i], binaire_8bits(rgb[i])); //on remplit avec le binaire calculé grace à la fonction binaire_8bit
	}
	//on a binaire = [[qqchose]; [qqchose]; [qqchose]]
	
	//on met tout sous forme d'1 seul string
	for(int i=0; i<3; i++){
		strcat(binaire_total, binaire[i]);
	}
	//printf("%s\n", binaire_total); //on vérifie, c'est ok
	
	
	
	int pallier = 0;
	int j =0;
	for(int i=0; i<4; i++){
		SIXbit[i] = (char *) malloc(BIT*sizeof(char)); //on initialise
		for(int index=0; j < pallier+6; index++){
			SIXbit[i][index] = binaire_total[j];
			j++;
		}
		pallier += 6;
		//printf("%s\n", SIXbit[i]);
	}
	
	//on a SIXbit = [[qqchose]; [qqchose]; [qqchose]; [qqchose]]
	
	char new_string[256] = "";
	for(int i=0; i<4; i++){
		for(int j=0; j<64; j++){
			if(strncmp(SIXbit[i], values[j], 6) == 0){
				new_string[i] = ascii[j];
			}
		}
	}
	
	strcpy(old_string, new_string);
}
