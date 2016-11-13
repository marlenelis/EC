//  según la versión de gcc y opciones de optimización usadas, tal vez haga falta
//  usar gcc –fno-omit-frame-pointer si gcc quitara el marco pila (%ebp)

#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval

/*
/proc/cpuinfo 
 modelo :  Intel(R) Core(TM) i5-4460  CPU @ 3.20GHz
 cache size: 6144 KB


gcc -m32 popcount.c-o popcout -O<n>
for((i=0;i<10;i++)); do ./popcount; done
for((i=0;i<11;i++)); do echo $i; ./popcount; done |pr -11 -l20 -w 80
hacemos 11 mediciones porque la 0 sale mal
*/

#define TEST 0
#define COPY_PASTE_CALC 1

#if !TEST
#define NBITS 20
#define SIZE (1<< NBITS)//tmaño suficiente para tiempo apreciable
unsigned lista[SIZE];// unsigned para desplazamiento derecha sea logico
#define RESULT (NBITS(1<<NBITS-1))//nueva formula
#else
/*--------------------------------------------------------------------------/
#define SIZE 4
unsigned  lista[SIZE]={	0x80000000, 0x00100000, 0x00000800, 0x00000001};// sin signo 4
#define RESULT 4
/*---------------------------------------------------------------------------/
#define SIZE 8
unsigned lista[SIZE]={	0x7FFFFFFF, 0XFFEFFFFF, 0XFFFFF7FF, 0XFFFFFFFE,
			0x01000024, 0x00356700, 0x8900ac00, 0x00bd00ef};//156
#define RESULT 156

/*--------------------------------------------------------------------------*/
#define SIZE 8
unsigned lista[SIZE]={0x0, 0x10204080, 0x3590ac06, 0x70b0d0e0, 0xffffffff, 0x12345678, 0x9abcdef0, 0xcafebeef};
#define RESULT 116
/*--------------------------------------------------------------------------*/
#endif
unsigned resultado=0;
/*--------------------------------------------------------------------------*/
int popcount1(unsigned* array, unsigned len)
{

    	int  i,   res=0;
    	unsigned x;
    	for (i=0; i<len; i++){
		x = lista[i];
		do{
			res += x & 0x1;
			x>>=1;
		}while(x);		
	}
	 
    return res;
}
/*--------------------------------------------------------------------------*/
int popcount2(unsigned* array, unsigned len)
{   
    	int  i, j,  res=0;
    	unsigned x;
	for (i=0; i< len; i++){
		x = lista[i];
    		for (j=0; j<8*sizeof(unsigned); j++){	
			res += x & 0x1;
			x>>=1;
		}		
	}
	 
    return res;
}
/*--------------------------------------------------------------------------*/
void crono(int (*func)(), char* msg){
    struct timeval tv1,tv2;	// gettimeofday() secs-usecs
    long           tv_usecs;	// y sus cuentas

    gettimeofday(&tv1,NULL);
    resultado = func(lista, SIZE);
    gettimeofday(&tv2,NULL);

    tv_usecs=(tv2.tv_sec -tv1.tv_sec )*1E6+
             (tv2.tv_usec-tv1.tv_usec);

#if COPY_PASTE_CALC  //cuando hagamos c-p a Calc
	printf("%ld" "\n", tv_usecs); //queremos solo los numeros
#else
    printf("resultado = %d\t", resultado); //durante el desarrollo
    printf("%s:%9ld us\n", msg, tv_usecs);
#endif
}
/*--------------------------------------------------------------------------*/
int main()
{
#if !TEST
    int i;			// inicializar array
    for (i=0; i<SIZE; i++)	// se queda en cache
	 lista[i]=i;
#endif
    crono(popcount1, "popcount1 (lenguaje C-\t  for  )");
    crono(popcount2, "popcount2 (lenguaje C-\t while)");
    
#if !COPY_PASTE_CALC
    printf("calculando = %d \n",RESULT ); /*OF*/
#endif
    exit(0);
}
