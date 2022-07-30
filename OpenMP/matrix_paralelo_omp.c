// programa de multiplicacion vectorial de matrices cuadradas usando hilos y mutex
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>        // for syscall()
#include <sys/syscall.h>   // for syscall() 
#include <time.h>          // used to measure time
#include <omp.h>

double **matriz1;
double **matriz2;
double **matriz3;
double suma_total;      //va a contener la suma de todos los elementos de matriz 3

//no modificar
struct timespec crt[2]; //to measure elapsed time
struct timespec cpu[2]; //to measure cpu (user) times
double real[2];         //to measure elapsed times
double user[2];         //to measure user times

void allocar_memoria(int filas,int columnas);
void llenar_numeros(int filas,int columnas);
void imprimir_matrix_1_and_2(int filas,int columnas);
void imprimir_matrix_3(int filas,int columnas);

//funcion que calcula la multiplicacion por cada hilo
void calcular (int num_fila, int num_columna)
{
  //funcion que utiliza las funciones de omp para 
  //obtener el numero de hilos y la posicion del hilo
  //ambos usados para el cálculo
  //variable acumuladora de suma
  double valor_suma=0;
  //obtencion de num_hilos y posicion_hilo
  int my_rank = omp_get_thread_num();
  int num_hilo = omp_get_num_threads();
  //fin obtencion parametros omp
  //local_m indica donde se inicia el for
  int local_m = num_columna/num_hilo;
  //los siguientes sentencias indicas los limites de cada for 
  //inicio_fila nos dice el inicio del for
  int inicio_fila = my_rank*local_m;
  //ultimo_fila nos dice el final del for 
  int ultimo_fila = (my_rank+1)*local_m - 1;
  int i,j,k;
  for(i = inicio_fila; i<=ultimo_fila; i++){
    for(j = 0; j < num_fila ; j++){
      matriz3[i][j]=0.0;
      //suma total para matriz3[i][j]
      for(k=0;k<num_fila;k++){
        matriz3[i][j]+= matriz1[i][k]*matriz2[k][j];
      }
      //se agrega a la suma local
      valor_suma=valor_suma + matriz3[i][j];
    }
  }
  //critical para la suma total
  #pragma omp critical
  suma_total=suma_total + valor_suma;
}

int main(int argc, char *argv[])
{
  int m,n,NUM_HILOS;
  if(argc!=4){
    printf("ERROR,debe ingresar la siguiente forma ./ejecutable fila columna num_hilos\n");
    return 0;
  }
  m=strtol(argv[1],NULL,10);
  n=strtol(argv[2],NULL,10);
  NUM_HILOS=strtol(argv[3],NULL,10);
  //verificacion de igualdad
  if(m!=n){
    printf("ERROR,la matriz no es cuadrada\n");
    return 0;
  }else if(m%NUM_HILOS!=0){
      printf("ERROR, el número de hilos debe ser divisor del tamaño de la matriz");
      return 0;
    }
	int i;
	suma_total=0.0;//la suma total empieza en 0
	printf("La Matriz 1 es %d x %d \n",m,n);
	printf("La Matriz 2 es %d x %d \n",m,n);
	printf("La Matriz 3 es %d x %d \n",m,n);

	allocar_memoria(m,n);
	llenar_numeros(m,n);
	imprimir_matrix_1_and_2(m,n);
//inicio medicion de tiempo
  syscall(SYS_clock_gettime, CLOCK_REALTIME, &crt[0]); //elapsed time
	syscall(SYS_clock_gettime, CLOCK_PROCESS_CPUTIME_ID, &cpu[0]); //cpu time

//aca comienza la creación de hilos 
  #pragma omp parallel num_threads(NUM_HILOS)
  calcular(m,n);

//fin medicion de tiempo
	syscall(SYS_clock_gettime, CLOCK_PROCESS_CPUTIME_ID, &cpu[1]); //cpu time
	syscall(SYS_clock_gettime, CLOCK_REALTIME, &crt[1]); //elapsed time

	printf("Matrix 3 despues de multiplicar...\n");
	imprimir_matrix_3(m,n);

	printf("suma total = %8.1lf \n",suma_total);
	printf("Fin del programa...\n");
	
	for (i=0; i<2; i++) {
	    real[i] = crt[i].tv_sec*1e6 + crt[i].tv_nsec*1e-3; // in us
	    user[i] = cpu[i].tv_sec*1e6 + cpu[i].tv_nsec*1e-3; // in us
	}
	printf("Elapsed time for Matrix Multiplication: %8.2lfus\n", (real[1]-real[0]));
	printf("  CPU   time for Matrix Multiplication: %8.2lfus\n", (user[1]-user[0]));
	 
	free(matriz1);
	free(matriz2);
	free(matriz3);
	return 0;
}//fin funcion main

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
