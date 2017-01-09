/*
Autor: Marlene Vásquez
compilar: gcc -m32 parity.c-o parity -O<n>
uso : for((i=0;i<11;i++)); do echo $i; ./parity; done |pr -11 -l20 -w 80
*/
#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval

#define TEST 0
#define COPY_PASTE_CALC 1

#if !TEST
#define NBITS 20
#define SIZE (1<< NBITS)//tmaño suficiente para tiempo apreciable
unsigned lista[SIZE];// unsigned para desplazamiento derecha sea logico
#define RESULT ((1<<NBITS-1))//nueva formula
#else
/*--------------------------------------------------------------------------/
#define SIZE 4
unsigned  lista[SIZE]={	0x80000000, 0x00100000, 0x00000800, 0x00000001};// sin signo 4
#define RESULT 4
/*---------------------------------------------------------------------------*
#define SIZE 8
unsigned lista[SIZE]={	0x7FFFFFFF, 0XFFEFFFFF, 0XFFFFF7FF, 0XFFFFFFFE,
			0x01000024, 0x00356700, 0x8900ac00, 0x00bd00ef};//156
#define RESULT 8

/*--------------------------------------------------------------------------*/
#define SIZE 8
unsigned lista[SIZE]={0x0, 0x10204080, 0x3590ac06, 0x70b0d0e0, 0xffffffff, 0x12345678, 0x9abcdef0, 0xcafebeef};
#define RESULT 2
/*--------------------------------------------------------------------------*/
#endif
unsigned resultado=0;
/*--------------------------------------------------------------------------*/
int parity1(unsigned* array, unsigned len)
{   
    int  i, j, res=0, val;
    unsigned x;

    for (i=0; i< len; i++){
        x = array[i];
        val = 0;
        for (j=0; j<8*sizeof(unsigned); j++){   
            val ^= x & 0x1;
            x>>=1;
        }   
        res += val;    
    }     
    return res;
}
/*--------------------------------------------------------------------------*/
int parity2(unsigned* array, unsigned len)
{

    int  i, res=0, val;
    unsigned x;
    for (i=0; i<len; i++){
		x = array[i];
        val=0;
    		do{
    			val ^= x & 0x1;
    			x>>=1;
    		}while(x);	
        res += val;	
	}
	 
    return res;
}

/*--------------------------------------------------------------------------*/
int parity3(unsigned* array, unsigned len)
{

    int  i, res=0, val;
    unsigned x;
    for (i=0; i<len; i++){
        x = array[i];
        val=0;

        while(x){
            val ^= x;
            x >>=1;
        }
        res += val&0x1;
    }
    return res;
}

/*--------------------------------------------------------------------------*/
int parity4(unsigned* array, unsigned len)
{

    int  i, res=0, val;
    unsigned x;
    for (i=0; i<len; i++){
        x = lista[i];
        val=0;
        asm("\n"
        "ini4:                  \n\t"            
            "xor %[x], %[v]     \n\t"
            "and $1,%[v]        \n\t"
            "shr %[x]           \n\t"            
            "test %[x], %[x]    \n\t"
            "jnz ini4           \n\t"
            :[v]"+r"(val)       
            :[x]"r"(x)         
            );        
        res += val;
    }
    return res;
}
/*--------------------------------------------------------------------------*/
int parity5(unsigned* array, unsigned len)
{

    int  i,j, res=0;
    unsigned x;
    for (i=0; i<len; i++){
        x = array[i];    
        for (j=16; j>0; j/=2 )               
            x ^= x >> j;        
        res += (x & 0x01);           
    }
    return res;
}
/*--------------------------------------------------------------------------*/
int parity6(unsigned* array, unsigned len)
{

    int  i,j, res=0;
    unsigned x;
    for (i=0; i<len; i++){
        x = array[i];    
        asm(
            "mov %[x], %%edx    \n\t"
            "shr $16, %%edx     \n\t"            
            "xor %[x], %%edx    \n\t"
            "xor %%dh, %%dl     \n\t"
            "setnp %%dl         \n\t"
            "movzx %%dl, %[x]   \n\t"
            :[x]"+r"(x)
            :
            :"edx"
            );
        res+= x;
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
    printf("%-35s:%9ld us\n", msg, tv_usecs);
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
    crono(parity1, "parity1 (lenguaje C- for  )");
    crono(parity2, "parity2 (lenguaje C- while)");
    crono(parity3, "parity3 (lenguaje C-l.CS:APP 3.22)");
    crono(parity4, "parity4 (lenguaje ASM-cuerpo while)");
    crono(parity5, "parity5 (l.CS:APP 3.49)");
    crono(parity6, "parity6 (leng.ASM-cuerpo for-setnp)");
    
#if !COPY_PASTE_CALC
    printf("calculando = %d \n",RESULT ); /*OF*/
#endif
    exit(0);
}
