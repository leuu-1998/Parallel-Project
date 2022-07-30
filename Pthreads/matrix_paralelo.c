// programa de multiplicacion vectorial de matrices cuadradas usando hilos y mutex
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>        // for syscall()
#include <sys/syscall.h>   // for syscall() 
#include <time.h>          // used to measure time
#include <pthread.h>
#define MAX_HILOS 64

double **matriz1;
double **matriz2;
double **matriz3;
double suma_total;      //va a contener la suma de todos los elementos de matriz 3
pthread_mutex_t mutex1; //se declara el mutex 

struct timespec crt[2]; //to measure elapsed time
struct timespec cpu[2]; //to measure cpu (user) times
double real[2];         //to measure elapsed times
double user[2];         //to measure user times


struct datos
{
	long thread_id;
	int num_filas;
	int num_columnas;
	int num_hilos;
};

struct datos datos_pasar[MAX_HILOS];



void allocar_memoria(int filas,int columnas);
void llenar_numeros(int filas,int columnas);
void imprimir_matrix_1_and_2(int filas,int columnas);
void imprimir_matrix_3(int filas,int columnas);

void *calcular (void *parametros)
{
  //se recibe parametro == datos struct

// esta es la función que implementa cada hilo (thread)
// completar el código...
// no olvidar de obtener la suma parcial de los elementos de la matriz 3
// usando mutex, de otra manera la vatiable global suma_total no se 
// actualizará convenientemente
  //se comienza el cálculo para la multiplicacion de la matriz
   
  //parametro (nombre de paso para funcion)
  struct datos * pDatos  = (struct datos *)parametros;
  int num_fila = (int)pDatos->num_filas;
  int num_columna = (int)pDatos->num_columnas;
  long my_rank = (long)pDatos->thread_id;
  int num_hilo = (int)pDatos->num_hilos;
  //variable acumuladora de suma
  double valor_suma=0;
  int local_m = num_columna/num_hilo;
  int inicio_fila = my_rank*local_m;
  int ultimo_fila = (my_rank+1)*local_m - 1;
  int i,j,k;
  //printf("hola Mundo desde el hilo %2ld \n", my_rank);
  
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
  
  //mutex para la suma
  pthread_mutex_lock(&mutex1);
  suma_total=suma_total + valor_suma;
  pthread_mutex_unlock(&mutex1);
  //salir 
  pthread_exit((void*)pDatos->thread_id);
  return NULL;
} //fin de la funcion calcular que ejecuta cad hilo (thread)

int main(int argc, char *argv[]) //modificar para incluir paso de parametros por linea de comandos
{
  int m,n,NUM_HILOS;
  if(argc!=4){
    printf("ERROR, el número de argumentos deben ser 4\n");
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
      printf("ERROR,la dimensión no es múltiplo del número de hilos");
      return 0;
    }


	int i,rc[NUM_HILOS];
	long t;
	pthread_t hilos[NUM_HILOS]; //declaro mi arreglo de hilos 
	pthread_attr_t attr; //Esta linea indica que hay un atributo
	pthread_mutex_init(&mutex1,NULL);
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

	suma_total=0.0;//la suma total empieza en 0

	printf("La Matriz 1 es %d x %d \n",m,n);
	printf("La Matriz 2 es %d x %d \n",m,n);
	printf("La Matriz 3 es %d x %d \n",m,n);

	allocar_memoria(m,n);
	llenar_numeros(m,n);
	imprimir_matrix_1_and_2(m,n);

	
//inicializacion de las estructuras para la funcion de cada hilo
  for(i = 0; i<NUM_HILOS;i++){
    datos_pasar[i].thread_id=i;
    datos_pasar[i].num_filas=n;
    datos_pasar[i].num_columnas=m;
    datos_pasar[i].num_hilos=NUM_HILOS;
  }
  
  
  	syscall(SYS_clock_gettime, CLOCK_REALTIME, &crt[0]); //elapsed time
	syscall(SYS_clock_gettime, CLOCK_PROCESS_CPUTIME_ID, &cpu[0]); //cpu time
//aca comienza la creación de hilos 
  // se crea el numero de hilos total
  // t long , NUM_HILOS  int
  //se pasa "attr" cada atritubo de cada hilo será joinable
  for(t = 0; t< NUM_HILOS; t++){
    rc[t]= pthread_create(&hilos[t],&attr,&calcular,&datos_pasar[t]);
    //verificamos que se crean correctamente
    if(rc[t]){
      printf("thread creation filed: %d \n",rc[t]);
      exit(-1);
    }
  }

//aca termina la creación de hilos 

	pthread_attr_destroy(&attr);

//aca comienza el join de los hilos
// los hilos esperarán hasta que todos terminen para poder 
// unirse de nuevo
  for(t=0;t<NUM_HILOS;t++){
    pthread_join(hilos[t],NULL);
  }
//aca termina el join de los hilos
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
	
	pthread_mutex_destroy(&mutex1);
	pthread_exit(NULL);
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
