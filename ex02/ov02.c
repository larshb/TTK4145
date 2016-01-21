#include <pthread.h>
#include <stdio.h>

int i = 0;
pthread_mutex_t lock;

void* Thread1(){
    int j;
    for (j = 0; j < 1000000; j++){
        pthread_mutex_lock(&lock);
        i++;
        pthread_mutex_unlock(&lock);
    }
}

void* Thread2(){
    int j;
    for (j = 0; j < 1000000; j++){
        pthread_mutex_lock(&lock);
        i--;
        pthread_mutex_unlock(&lock);    
    }
}

int main(){
    pthread_t thread1;
    pthread_t thread2;
    pthread_create(&thread1, NULL, Thread1, NULL);
    pthread_create(&thread2, NULL, Thread2, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_mutex_destroy(&lock);
    printf("%d\n",i);
    return 0;
}
