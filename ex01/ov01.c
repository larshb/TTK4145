
// gcc 4.7.2 +
// gcc -std=gnu99 -Wall -g -o helloworld_c helloworld_c.c -lpthread

#include <pthread.h>
#include <stdio.h>


int i = 0;
// Note the return type: void*
void* Thread1(){
    int r=0;
    for (r = 0; r<=1000000; r= r+1){
    	i++;
    }
}

void* Thread2(){
    int t=0;
    for (t = 0; t<=1000000; t=t+1){
    	i--;
    }
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