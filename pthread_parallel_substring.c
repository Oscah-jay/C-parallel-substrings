#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define MAX 10240
#define NUM_THREADS 10

int total = 0;
int n1, n2;
char *s1, *s2;
FILE *fp;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int readf(FILE *fp) {
    if ((fp = fopen("strings.txt", "r")) == NULL) {
        printf("ERROR: can't open string.txt!\n");
        return 0;
    }
    s1 = (char *)malloc(sizeof(char) * MAX);
    if (s1 == NULL) {
        printf("ERROR: Out of memory!\n");
        return -1;
    }
    s2 = (char *)malloc(sizeof(char) * MAX);
    if (s2 == NULL) {
        printf("ERROR: Out of memory\n");
        return -1;
    }
    s1 = fgets(s1, MAX, fp);
    s2 = fgets(s2, MAX, fp);
    n1 = strlen(s1);
    n2 = strlen(s2) - 1;
    if (s1 == NULL || s2 == NULL || n1 < n2)
        return -1;
    return 0;
}

void *num_substring(void *arg) {
    int thread_id = *(int *)arg;
    int chunk_size = n1 / NUM_THREADS;
    int start = thread_id * chunk_size;
    int end = (thread_id == NUM_THREADS - 1) ? n1 - n2 : start + chunk_size - n2;
    int local_total = 0;

    for (int i = start; i <= end; i++) {
        int count = 0;
        for (int j = i, k = 0; k < n2; j++, k++) {
            if (*(s1 + j) != *(s2 + k)) {
                break;
            } else {
                count++;
            }
            if (count == n2) {
                local_total++;
            }
        }
    }

    pthread_mutex_lock(&mutex);
    total += local_total;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    if (readf(fp) != 0) {
        printf("Error reading file\n");
        return -1;
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, num_substring, (void *)&thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("The number of substrings is: %d\n", total);

    free(s1);
    free(s2);
    pthread_mutex_destroy(&mutex);

    return 0;
}