#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void *P(void *arg) {
    for (int i = 0; i < 10; i++) {
    char *message;
    message = (char *) arg;
    printf("%s \n", message);
    sleep(1);
    }   
}

int main(int argc, char *argv[]) {
    pthread_attr_t attr;
    pthread_t p1, p2, p3;

    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);


    pthread_create(&p1, &attr, P, "Huey");
    pthread_create(&p2, &attr, P, "Dewey");
    pthread_create(&p3, &attr, P, "Louie");

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);

}