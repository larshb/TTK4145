#include <pthread.h>
#include <stdio.h>

int i = 0;

void* Thread1(){
    int j;
    for (j = 0; j < 1000000; j++)
    	i++;
}

void* Thread2(){
    int j;
    for (j = 0; j < 1000000; j++)
    	i--;
}

int main(){
    pthread_t thread1;
    pthread_t thread2;
    pthread_create(&thread1, NULL, Thread1, NULL);
    pthread_create(&thread2, NULL, Thread2, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    printf("%d\n",i);
    return 0;
}
