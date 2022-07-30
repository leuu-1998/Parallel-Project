// programa de multiplicacion vectorial de matrices cuadradas, version serial
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>        // for syscall()
#include <sys/syscall.h>   // for syscall() 
#include <time.h>          // used to measure time

double **matriz1; //puntero para arreglo bidimensional de matriz A
double **matriz2; //puntero para arreglo bidimensional de matriz B
double **matriz3; //puntero para arreglo bidimensional de matriz C

//para medir los tiempos
struct timespec crt[2]; //to measure elapsed time
struct timespec cpu[2]; //to measure cpu (user) times
double real[2];         //to measure elapsed times
double user[2];         //to measure user times

//prototipos
void allocar_memoria(int filas,int columnas);
void llenar_numeros(int filas,int columnas);
void imprimir_matrix_1_and_2(int filas,int columnas);
void imprimir_matrix_3(int filas,int columnas);
//agrega linea de comandos
int main(int argc, char *argv[])
{
	int i,j,k;
  //verificacion de argc
  if(argc!=3){
    printf("Error, la forma correcta de ingresar el comando es: ./ejecutable.exe filas columnas  \n");
    return 0;
  }

  //se recibe de la linea de comandos
	int m,n;
  m = strtol(argv[1],NULL,10);
  n = strtol(argv[2],NULL,10);
  //veri argv
  if(m!=n){
    printf("ERROR, ARG DIFERENTES\n");
    return 0;
  }

  /*
	printf("Multiplicacion de matrices cuadradas\n");
	printf("Cuantas filas tienen las matrices?\n");
	scanf("%d",&m);
	printf("Cuantas columnas tienen las matrices?\n");
	scanf("%d",&n);
  */
	printf("La Matriz 1 es %d x %d \n",m,n);
	printf("La Matriz 2 es %d x %d \n",m,n);
	printf("La Matriz 3 es %d x %d \n",m,n);

	allocar_memoria(m,n);
	llenar_numeros(m,n);
	imprimir_matrix_1_and_2(m,n);

//inicio esta parte del codigo se debe paralelizar
	syscall(SYS_clock_gettime, CLOCK_REALTIME, &crt[0]); //elapsed time
	syscall(SYS_clock_gettime, CLOCK_PROCESS_CPUTIME_ID, &cpu[0]); //cpu time
    for (i = 0; i < m; i++) { 
        for (j = 0; j < n; j++) { 
            matriz3[i][j] = 0; 
            for (k = 0; k < n; k++) {
				matriz3[i][j] += matriz1[i][k] * matriz2[k][j]; 
			}
        } 
    }
	syscall(SYS_clock_gettime, CLOCK_PROCESS_CPUTIME_ID, &cpu[1]); //cpu time
	syscall(SYS_clock_gettime, CLOCK_REALTIME, &crt[1]); //elapsed time
//fin esta parte del codigo se debe paralelizar

	printf("Matrix 3 despues de multiplicar...\n");
	imprimir_matrix_3(m,n);

	for (i=0; i<2; i++) {
	    real[i] = crt[i].tv_sec*1e6 + crt[i].tv_nsec*1e-3; // in us
	    user[i] = cpu[i].tv_sec*1e6 + cpu[i].tv_nsec*1e-3; // in us
	}
	printf("Elapsed time for Matrix Multiplication: %8.2lfus\n", (real[1]-real[0]));
	printf("  CPU   time for Matrix Multiplication: %8.2lfus\n", (user[1]-user[0]));

	printf("Fin del programa...\n");
	free(matriz1);
	free(matriz2);
	free(matriz3);
	return 0;
}//fin de main

void allocar_memoria(int filas,int columnas)
{
    int i;
    matriz1=(double**)malloc(filas*sizeof(double*));
    for (i=0;i<filas;i++) {
		matriz1[i]=(double*)malloc(columnas*sizeof(double));
    }
    matriz2=(double**)malloc(filas*sizeof(double*));
	for (i=0;i<filas;i++) {
		matriz2[i]=(double*)malloc(columnas*sizeof(double));
    }
    matriz3=(double**)malloc(filas*sizeof(double*));
    for (i=0;i<filas;i++) {
		matriz3[i]=(double*)malloc(columnas*sizeof(double));
    }
} //fin de allocar_memoria

void llenar_numeros(int filas, int columnas)
{
    int i,j;
    for (i=0;i<filas;i++) {
		for (j=0;j<columnas;j++) {
	    	matriz1[i][j]=j*3+2*(rand()%15) ;
	    	matriz2[i][j]=j*3+2*(rand()%15) ;
		}
    }
}//fin de llenar_numeros

void imprimir_matrix_1_and_2(int filas,int columnas)
{
    int i,j;
    printf("MATRIX 1\n");
    for (i=0;i<filas;i++) {
		for (j=0;j<columnas;j++) {
	    	printf("%6.1lf \t",matriz1[i][j]);
		}
    	printf("\n");
    }

    printf("MATRIX 2\n");
    for (i=0;i<filas;i++) {
		for (j=0;j<columnas;j++) {
	    	printf("%6.1lf \t",matriz2[i][j]);
		}
        printf("\n");
    }
}//fin de imprimir_matrix_1_and_2

void imprimir_matrix_3(int filas,int columnas)
{
	int i,j;
    printf("MATRIX 3\n");
    for (i=0;i<filas;i++) {
		for (j=0;j<columnas;j++) {
	    	printf("%8.1lf ",matriz3[i][j]);
		}
    	printf("\n");
    }
}//fin de imprimir_matrix_3
