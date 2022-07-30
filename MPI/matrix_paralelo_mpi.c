// programa de multiplicacion de matrices cuadradas, version serial.
#include <stdio.h>
#include <stdlib.h>
// call MPI library
#include <mpi.h>

// matrices usadas en cada proceso
double *matriz1; // pointer for unidimensional array in matrix A
double *matriz2; // pointer for unidimensional array in matrix B
double *matriz3; // pointer for unidimensional array in matrix C

// define the variables to be used for measuring elapsed time
void allocar_memoria(int filas, int columnas, int local_m);
void Get_dims(int *m_p, int *, int *, int *, int, int, MPI_Comm comm);
void llenar_numeros(int, int, int, int, MPI_Comm);

void Print_matrix(char title[], double local_A[], int m, int local_m, int n, int my_rank, MPI_Comm comm);
void matrix_mult(int local_m, int n, int local_n, MPI_Comm comm);

int main(int argc, char *argv[])
{

   int m, n;                                   // dimensiones de la matriz
   int local_m, local_n;                       // dimension para cada proceso/nodo/esclavo
   double start, finish, loc_elapsed, elapsed; // variables para medición del tiempo
   int my_rank, nprocs;                        // variables for process: range and process

   MPI_Comm comm;                // variable for communication
   MPI_Init(NULL, NULL);         // we not need arguments, so we use NULL
   comm = MPI_COMM_WORLD;        // communicate with the world
   MPI_Comm_size(comm, &nprocs); // asigna los valores de nprocs y my_rank
   MPI_Comm_rank(comm, &my_rank);

   // verificamos con el master la entrada de argumentos
   if (my_rank == 0)
   {
      // verificacion de argc
      if (argc != 3)
      {
         printf("Error, la cantidad de comandos es incorrecta, la cantidad correcta de comandos es 3\n");
         printf("Debe ingresar el comando de la siguiente forma: ./ejecutable.exe filas columnas\n");
         return 0;
      }
      // se recibe de la linea de comandos
      m = strtol(argv[1], NULL, 10);
      n = strtol(argv[2], NULL, 10);
      // veri argv
      if (m != n)
      {
         printf("ERROR, DIMENSIONES DIFERENTES, DEBE SER UNA MATRIZ CUADRADA\n");
         return 0;
      }
      printf("Multiplicacion de matrices cuadradas\n");
      // se asgina dimensiones locales para los procesos
   }
   // pasa las dimensiones a los procesos
   Get_dims(&m, &local_m, &n, &local_n, my_rank, nprocs, comm);
   allocar_memoria(m, n, local_m);               // se crean las matrices en el master
   llenar_numeros(m, n, local_m, my_rank, comm); // se llenan variables globales
   if (m < 16)
   {
      Print_matrix("1", matriz1, m, local_m, n, my_rank, comm);
      Print_matrix("2", matriz2, m, local_m, n, my_rank, comm);
   }
   // se comienza a medir el tiempo y la multiplicación
   MPI_Barrier(comm);
   start = MPI_Wtime();
   matrix_mult(local_m, n, local_n, comm); // multiplicación
   finish = MPI_Wtime();
   // se termina de medir el tiempo
   loc_elapsed = finish - start;
   // tomamos la medición max de todos los procesos
   MPI_Reduce(&loc_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

   if (m < 16)
      Print_matrix("3 Multilicada", matriz3, m, local_m, n, my_rank, comm);
   // solo se muestra el tiempo en el master
   if (my_rank == 0)
   {
      printf("Elapsed time = %e\n", elapsed);
      printf("Fin del programa...\n");
   }
   // se liberan las matrices de todos los procesos
   free(matriz1);
   free(matriz2);
   free(matriz3);
   // finaliza MPI
   MPI_Finalize();
   return 0;
} // fin de main
/*
* Function:  Get_dims
 * Purpose:   Get the dimensions of the matrix and the vectors from
 *            stdin.
 * In args:
 *            comm_sz:   number of processes in comm
 *            comm:      communicator containing all processes calling
 *                       Get_dims
 * Out args:  m_p:       global number of rows of A and components in y
 *            local_m_p: local number of rows of A and components of y
 *            n_p:       global number of cols of A and components of x
 *            local_n_p: local number of components of x

*/
void Get_dims(
    int *m_p /* out */,
    int *local_m_p /* out */,
    int *n_p /* out */,
    int *local_n_p /* out */,
    int my_rank /* in  */,
    int nprocs /* in  */,
    MPI_Comm comm /* in  */)
{

   MPI_Bcast(m_p, 1, MPI_INT, 0, comm);
   MPI_Bcast(n_p, 1, MPI_INT, 0, comm);

   *local_m_p = *m_p / nprocs;
   *local_n_p = *n_p / nprocs;
} /* Get_dims */

/*
 * Function:    allocar_memoria
 * Purpose:    asignar el espacio de memoria necesario para cada matriz en cada proceso
 * In args:    columnas: dimensión columna de cada matriz
 *             loca_m: dimensión fila de cada matriz
 */
void allocar_memoria(int filas, int columnas, int local_m)
{
   matriz1 = (double *)malloc(local_m * columnas * sizeof(double));
   matriz2 = (double *)malloc(local_m * columnas * sizeof(double));
   matriz3 = (double *)malloc(local_m * columnas * sizeof(double));

} // fin de allocar_memoria

/*
 * Function:    llenar_numeros
 * Purpose:    asignamos el valor de los elementos de la matrices m1 y m2 en el proceso master y con Scatter repartimos los datos
 */
void llenar_numeros(int filas /*m*/,
                    int columnas /*n*/,
                    int local_m,
                    int my_rank,
                    MPI_Comm comm)
{
   double *m1 = NULL;
   double *m2 = NULL;
   int i;

   if (my_rank == 0)
   {
      // creamos espacio para las matrices
      m1 = malloc(columnas * filas * sizeof(double));
      m2 = malloc(columnas * filas * sizeof(double));

      for (i = 0; i < filas * columnas; i++)
      {
         m1[i] = i * 3 + 2 * (rand() % 15);
         m2[i] = i * 3 + 2 * (rand() % 15);
      }
      // m1: datos para enviar a cada proceso
      // local_m*columnas: dimensión del dato a enviar
      // matriz1: espacio a donde va a llegar(en cada proceso)
      MPI_Scatter(m1, local_m * columnas, MPI_DOUBLE, matriz1, local_m * columnas, MPI_DOUBLE, 0, comm);
      // broadcast para matriz2
      MPI_Scatter(m2, local_m * columnas, MPI_DOUBLE, matriz2, local_m * columnas, MPI_DOUBLE, 0, comm);
      // se liberan las matrices
      free(m1);
      free(m2);
   }
   else
   {
      MPI_Scatter(m1, local_m * columnas, MPI_DOUBLE, matriz1, local_m * columnas, MPI_DOUBLE, 0, comm);
      MPI_Scatter(m2, local_m * columnas, MPI_DOUBLE, matriz2, local_m * columnas, MPI_DOUBLE, 0, comm);
   }
} // fin de llenar_numeros

/*
 * Function:    Print_matrix
 * Purpose:    se unen los valores de cada proceso con Gather y luego se imprimer la matriz resultando
 */
void Print_matrix(
    char title[] /* in */,
    double local_A[] /* in */,
    int m /* in */,
    int local_m /* in */,
    int n /* in */,
    int my_rank /* in */,
    MPI_Comm comm /* in */)
{

   double *A = NULL;
   int i, j;

   if (my_rank == 0)
   {
      A = malloc(m * n * sizeof(double));
      // local_A: matriz donde se tiene la información
      // A: matriz donde se mandará la información (de todos los procesos)
      MPI_Gather(local_A, local_m * n, MPI_DOUBLE, A, local_m * n, MPI_DOUBLE, 0, comm);
      printf("\nThe matrix %s\n", title);

      for (i = 0; i < m; i++)
      {
         for (j = 0; j < n; j++)
            printf("%8.1lf ", A[i * n + j]);
         printf("\n");
      }
      printf("\n");
      free(A);
   }
   else
   {
      MPI_Gather(local_A, local_m * n, MPI_DOUBLE, A, local_m * n, MPI_DOUBLE, 0, comm);
   }
}

/*
* Function:    matrix_mult
 * Purpose:    primero se reune la información de todos los procesos de la matriz2 para enviarla a todos los procesos usando Allgather,
                      luego realizamos la multiplicación de las partes de la matriz en cada proceso
*/
void matrix_mult(
    int local_m,
    int n,
    int local_n,
    MPI_Comm comm)
{
   double *m2 = NULL;

   m2 = malloc(n * n * sizeof(double));
   // matriz2: información a enviar
   // m2: matriz para almacenar la información de cada proceso
   MPI_Allgather(matriz2, local_m * n, MPI_DOUBLE, m2, local_m * n, MPI_DOUBLE, comm);

   int local_i, j, k;
   // local_i -> fila
   for (local_i = 0; local_i < local_m; local_i++)
   {
      // j -> columna
      for (j = 0; j < n; j++)
      {
         matriz3[local_i * n + j] = 0.0;
         // for para la suma de un elemento de matriz3
         // k -> columna en la suma
         for (k = 0; k < n; k++)
            matriz3[local_i * n + j] += matriz1[local_i * n + k] * m2[k * n + j];
      }
   }
   free(m2);
}
