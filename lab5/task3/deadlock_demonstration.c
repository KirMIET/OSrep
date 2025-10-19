#include <pthread.h>
#include <stdio.h>

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

int main() {
    pthread_mutex_lock(&m);
    pthread_mutex_lock(&m);
    printf("hello");
    return 0;
}