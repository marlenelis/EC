
/*
 * compilación: gcc -m32 -g bomba.c -o bomba
*/
#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <string.h>	// para strncmp()/strlen()
#include <sys/time.h>	// para gettimeofday(), struct timeval

typedef int bool;

#define true 1
#define false 0

bool decodepas(char pass[]){
	char password[]="M30vc12+\n";
	bool res;
	res = (strncmp(pass,password,strlen(password))==0)? true : false;
	//strcmp devuelve 0 si son iguales
	return res;
}
 bool decodecod(int cod){
	int  passcode  = 6977;
	bool res =  (passcode==cod)? true : false;
}
void boom(){
	printf("***************\n");
	printf("*** BOOM!!! ***\n");
	printf("***************\n");
	exit(-1);
}

void defused(){
	printf("·························\n");
	printf("··· bomba desactivada ···\n");
	printf("·························\n");
	exit(0);
}

int main(){
#define SIZE 100
	char pass[SIZE];
	int  pasv;
#define TLIM 5
	struct timeval tv1,tv2;	// gettimeofday() secs-usecs

	gettimeofday(&tv1,NULL);

	printf("Introduce la contraseña: ");
	fgets(pass,SIZE,stdin);
	if (!decodepas(pass))	    boom();

	gettimeofday(&tv2,NULL);
	if (tv2.tv_sec - tv1.tv_sec > TLIM)
	    boom();

	printf("Introduce el código: ");
	scanf("%i",&pasv);
	if (!decodecod(pasv)) boom();

	gettimeofday(&tv1,NULL);
	if (tv1.tv_sec - tv2.tv_sec > TLIM)
	    boom();

	defused();
}
