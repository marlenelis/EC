/*
Autor: Marlene Vásquez
compilar: gcc -m32 popcount.c-o popcout -O<n>
uso: for((i=0;i<11;i++)); do echo $i; ./popcount; done |pr -11 -l20 -w 80

*/
#include <stdio.h>	     // para printf()
#include <stdlib.h>	    // para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval

#define TEST 0
#define COPY_PASTE_CALC 1

#if !TEST
    #define NBITS 20
    #define SIZE (1<< NBITS)//tmaño suficiente para tiempo apreciable
    unsigned lista[SIZE];// unsigned para desplazamiento derecha sea logico
    #define RESULT (NBITS *(1<< NBITS-1) )//nueva formula
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

/*--------------------------------------------------------------------------/
#define SIZE 8
unsigned lista[SIZE]={0x0, 0x10204080, 0x3590ac06, 0x70b0d0e0, 0xffffffff, 0x12345678, 0x9abcdef0, 0xcafebeef};
#define RESULT 116
/*--------------------------------------------------------------------------*/
#endif
unsigned resultado=0;
/*--------------------------------------------------------------------------*/
int popcount1(unsigned* array, unsigned len)
{   
        int  i, j,  res=0;
        unsigned x;
    for (i=0; i< len; i++){
        x = array[i];
            for (j=0; j<8*sizeof(unsigned); j++){   
            res += x & 0x1;
            x>>=1;
        }       
    }
     
    return res;
}
/*--------------------------------------------------------------------------*/
int popcount2(unsigned* array, unsigned len)
{

    	int  i,   res=0;
    	unsigned x;
    	for (i=0; i<len; i++){
		x = array[i];

		do{
			res += x & 0x1;
			x>>=1;
		}while(x);		
	}
	 
    return res;
}
/*--------------------------------------------------------------------------*/
int popcount3(unsigned *lista, int len)
{
//corregir
    int i, result=0;
    unsigned x;
    for (i=0; i<len; i++)
    { 
        x= lista[i];
        asm("\n"
        "ini3:                      \n\t"   //seguri mientras que x!=0
                "shr %[x]           \n\t"   // LSB en CF
                "adc $0,%[r]        \n\t"      
                "test %[x],%[x]     \n\t"  
                "jnz ini3           \n\t"   // si no cero vuelve a ini3
                : [r] "+r" (result)         // e/s:    añadir a lo acumulador por el momentos
                : [x] "r" (x) );            // entrada valor elemento
    }
    return result;
}

/*--------------------------------------------------------------------------*/
int popcount4(unsigned* lista, unsigned len)//bucle interno whilw
{
    int  i, j,  res=0, val;
    unsigned x;

    for (i=0; i< len; i++){
        val = 0;
        x = lista[i];

        for (j=0; j< 8; j++){   
            val += x & 0x01010101;
            x>>=1;
        }   
        val += (val>>16);
        val += (val>>8);  
        res += val & 0xFF;  
    }   
    
    return res;
}
/*--------------------------------------------------------------------------*/
int popcount5(unsigned *array, int len)
{
    int result=0,i,val;
    int SSE_mask[] = {0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f};
    int SSE_LUTb[] = {0x02010100, 0x03020201, 0x03020201, 0x04030302};
    if (len & 0x3) printf("leyendo 128b pero len no múltiplo de 4?\n");
    for (i=0; i<len; i+=4)
    {
        asm("movdqu %[x], %%xmm0 \n\t"
            "movdqa     %%xmm0, %%xmm1 \n\t" // dos copias de x
            "movdqu     %[m], %%xmm6 \n\t" // máscara
            "psrlw      $4 , %%xmm1 \n\t"
            "pand        %%xmm6, %%xmm0 \n\t" //; xmm0 – nibbles inferiores
            "pand        %%xmm6, %%xmm1 \n\t" //; xmm1 – nibbles superiores

            "movdqu %[l], %%xmm2 \n\t" //; ...como pshufb sobrescribe LUT
            "movdqa %%xmm2, %%xmm3 \n\t" //; ...queremos 2 copias
            "pshufb %%xmm0, %%xmm2 \n\t" //; xmm2 = vector popcount inferiores
            "pshufb %%xmm1, %%xmm3 \n\t" //; xmm3 = vector popcount superiores

            "paddb %%xmm2, %%xmm3 \n\t"
            "pxor %%xmm0, %%xmm0 \n\t"
            "psadbw %%xmm0, %%xmm3 \n\t"
            "movhlps %%xmm3, %%xmm0 \n\t"
            "paddd %%xmm3, %%xmm0 \n\t"
            "movd %%xmm0, %[val] \n\t"

        : [val]"=r" (val)
        : [x] "m" (array[i]),
          [m] "m" (SSE_mask[0]),
          [l] "m" (SSE_LUTb[0])
        );
            result += val;
        }
return result;

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
    printf("%-40s:%9ld us\n", msg, tv_usecs);
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
    crono(popcount1, "popcount1 (lenguaje C- for  )");
    crono(popcount2, "popcount2 (lenguaje C- while)");
    crono(popcount3, "popcount3 (lenguaje ASM-cuerpo while)");
    crono(popcount4, "popcount4 (I.CS:APP 349)");
    crono(popcount5, "popcount5 (asm SSSE3)");
    
#if !COPY_PASTE_CALC
   printf("\ncalculando = %d\n",RESULT );
#endif
    exit(0);
}
