// programa de multiplicacion de matrices cuadradas, version serial.
#include <stdio.h>
#include <stdlib.h>
#include "timer.h" // include this file to measure the elapsed time

double **matriz1; // pointer for bidimensional array in matrix A
double **matriz2; // pointer for bidimensional array in matrix B
double **matriz3; // pointer for bidimensional array in matrix C

// define the variables to be used for measuring elapsed time

void allocar_memoria(int filas, int columnas);
void llenar_numeros(int filas, int columnas);
void imprimir_matrix_1_and_2(int filas, int columnas);
void imprimir_matrix_3(int filas, int columnas);

int main(int argc, char *argv[])
{
	int i, j, k;
	int m, n;
	// the modified serial version should not ask via scanf() any
	// parameter, though, the parameters are passed via command line

	// mesasure the elapsed time of all function calls, except
	// those function calls that print the matrices
	printf("Multiplicacion de matrices cuadradas\n");
	// verificacion de argc
	if (argc != 3)
	{
		printf("Error, la cantidad de comandos es incorrecta, la cantidad correcta de comandos es 3\n");
		printf("Debe ingresar el comando de la siguiente forma: ./ejecutable.exe filas columnas\n") return 0;
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
	// salida en pantalla
	printf("La Matriz 1 es %d x %d \n", m, n);
	printf("La Matriz 2 es %d x %d \n", m, n);
	printf("La Matriz 3 es %d x %d \n", m, n);

	allocar_memoria(m, n);
	llenar_numeros(m, n);
	// no need to measure the elapsed time of printing the matrices
	// you may still print the matrices, but do not account the elapsed time
	imprimir_matrix_1_and_2(m, n);

	for (i = 0; i < m; i++)
	{
		for (j = 0; j < n; j++)
		{
			matriz3[i][j] = 0;
			for (k = 0; k < n; k++)
			{
				matriz3[i][j] += matriz1[i][k] * matriz2[k][j];
			}
		}
	}

	// no need to measure the elapsed time of printing the matrix 3
	// you may still print the matrix 3, but do not account the elapsed time
	printf("Matrix 3 despues de multiplicar...\n");
	imprimir_matrix_3(m, n);

	printf("Fin del programa...\n");
	free(matriz1);
	free(matriz2);
	free(matriz3);
	return 0;
} // fin de main

void allocar_memoria(int filas, int columnas)
{
	int i;
	matriz1 = (double **)malloc(filas * sizeof(double *));
	for (i = 0; i < filas; i++)
	{
		matriz1[i] = (double *)malloc(columnas * sizeof(double));
	}
	matriz2 = (double **)malloc(filas * sizeof(double *));
	for (i = 0; i < filas; i++)
	{
		matriz2[i] = (double *)malloc(columnas * sizeof(double));
	}
	matriz3 = (double **)malloc(filas * sizeof(double *));
	for (i = 0; i < filas; i++)
	{
		matriz3[i] = (double *)malloc(columnas * sizeof(double));
	}
} // fin de allocar_memoria

void llenar_numeros(int filas, int columnas)
{
	int i, j;
	for (i = 0; i < filas; i++)
	{
		for (j = 0; j < columnas; j++)
		{
			matriz1[i][j] = j * 3 + 2 * (rand() % 15);
			matriz2[i][j] = j * 3 + 2 * (rand() % 15);
		}
	}
} // fin de llenar_numeros

void imprimir_matrix_1_and_2(int filas, int columnas)
{
	int i, j;
	printf("MATRIX 1\n");
	for (i = 0; i < filas; i++)
	{
		for (j = 0; j < columnas; j++)
		{
			printf("%6.1lf \t", matriz1[i][j]);
		}
		printf("\n");
	}

	printf("MATRIX 2\n");
	for (i = 0; i < filas; i++)
	{
		for (j = 0; j < columnas; j++)
		{
			printf("%6.1lf \t", matriz2[i][j]);
		}
		printf("\n");
	}
} // fin de imprimir_matrix_1_and_2

void imprimir_matrix_3(int filas, int columnas)
{
	int i, j;
	printf("MATRIX 3\n");
	for (i = 0; i < filas; i++)
	{
		for (j = 0; j < columnas; j++)
		{
			printf("%8.1lf ", matriz3[i][j]);
		}
		printf("\n");
	}
} // fin de imprimir_matrix_3
