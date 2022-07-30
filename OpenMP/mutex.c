#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void *functionC(){
  pthread_mutex_lock(&mutex1); //mutea el hilo 2
  counter++;  //modifica contador var global
  printf("Counte value: %d\n",counter);
  pthread_mutex_unlock(&mutex1); //desbloqueda el bloqueo
  pthread_exit(NULL);
}

int main(){
  int rc1, rc2;
  pthread_t thread1, thread2;
  
  //crea hilos independientes para que ejecute cada funcion
  if((rc1=pthread_create(&thread1, NULL, &functionC,NULL))){
    printf("Thread creation failed: %d\n",rc1);
  }
  if((rc2=pthread_create(&thread2, NULL, &functionC,NULL))){
    printf("Thread creation failed: %d\n",rc2);
  }
  
  pthread_join( thread1, NULL);
  pthread_join( thread2, NULL);


  return 0;
}
