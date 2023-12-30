#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define MIN_ROWS_COLS 3
#define MAX_ROWS_COLS 6
#define MAX_VALUE 9

int BUFFER_SIZE;

typedef struct IndexesOfThreadsDate {
    int row;
    int column;
    int data;
} IndexesOfThreadsDate;

// Semaphores for synchronization
sem_t full, empty, mutex;

int MatA_Rows, MatB_Columns, Mat_Brows_Acolumns;

int matrixA[MAX_ROWS_COLS][MAX_ROWS_COLS];
int matrixB[MAX_ROWS_COLS][MAX_ROWS_COLS];
int matrixResult[MAX_ROWS_COLS][MAX_ROWS_COLS];

int ip = 0, ic = 0;
IndexesOfThreadsDate *buffer; // Dynamic buffer declaration

// Function to generate matrices A and B
void generate() {
    srand(time(NULL));

    // Random generation of matrix dimensions
    MatA_Rows = rand() % (MAX_ROWS_COLS - MIN_ROWS_COLS + 1) + MIN_ROWS_COLS;
    MatB_Columns = rand() % (MAX_ROWS_COLS - MIN_ROWS_COLS + 1) + MIN_ROWS_COLS;
    Mat_Brows_Acolumns = rand() % (MAX_ROWS_COLS - MIN_ROWS_COLS + 1) + MIN_ROWS_COLS;

    BUFFER_SIZE = MatB_Columns;

    // Filling matrix A with random values
    for (int i = 0; i < MatA_Rows; ++i) {
        for (int j = 0; j < Mat_Brows_Acolumns; ++j) {
            matrixA[i][j] = rand() % (MAX_VALUE + 1);
        }
    }

    // Filling matrix B with random values
    for (int i = 0; i < Mat_Brows_Acolumns; ++i) {
        for (int j = 0; j < MatB_Columns; ++j) {
            matrixB[i][j] = rand() % (MAX_VALUE + 1);
        }
    }

    // Printing matrices A and B
    printf("Matrix A:\n");
    for (int i = 0; i < MatA_Rows; ++i) {
        for (int j = 0; j < Mat_Brows_Acolumns; ++j) {
            printf("%d\t", matrixA[i][j]);
        }
        printf("\n");
    }

    printf("=======================================================\nMatrix B:\n");
    for (int i = 0; i < Mat_Brows_Acolumns; ++i) {
        for (int j = 0; j < MatB_Columns; ++j) {
            printf("%d\t", matrixB[i][j]);
        }
        printf("\n");
    }
    printf("\n\n\n");
}

// Producer thread function
void *producer(void *arg) {
    IndexesOfThreadsDate *args = (IndexesOfThreadsDate *)arg;

    // Matrix multiplication logic
    args->data = 0;
    for (int i = 0; i < Mat_Brows_Acolumns; i++) {
        args->data += matrixA[args->row][i] * matrixB[i][args->column];
    }

    // Semaphore operations for synchronization
    sem_wait(&empty);
    sem_wait(&mutex);

    buffer[ip] = *args;
    ip = (ip + 1) % BUFFER_SIZE;

    sem_post(&full);
    sem_post(&mutex);
}

// Consumer thread function
void *consumer(void *arg) {
    sem_wait(&full);
    sem_wait(&mutex);

    matrixResult[buffer[ic].row][buffer[ic].column] = buffer[ic].data;
    ic = (ic + 1) % BUFFER_SIZE;

    sem_post(&empty);
    sem_post(&mutex);
}

// Function to print the result matrix
void printTheResultMatrix() {
    printf("The result is:\n\n");
    for (int i = 0; i < MatA_Rows; i++) {
        for (int j = 0; j < MatB_Columns; j++) {
            printf("%d\t", matrixResult[i][j]);
        }
        printf("\n");
    }
}

int main() {
    generate();

    int resultMatSize = MatA_Rows * MatB_Columns;
    buffer = malloc(BUFFER_SIZE * sizeof(IndexesOfThreadsDate)); // Allocating memory for buffer

    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);

    pthread_t producers[resultMatSize];
    pthread_t consumers[resultMatSize];

    int rowsOfMatrix_A = 0;
    int columnsOfMatrix_B = 0;

    IndexesOfThreadsDate vector[resultMatSize];

    // Creating indices for the matrices
    for (int i = 0; i < resultMatSize; i++) {
        vector[i].row = rowsOfMatrix_A;
        vector[i].column = columnsOfMatrix_B;

        columnsOfMatrix_B++;

        if (columnsOfMatrix_B == MatB_Columns) {
            columnsOfMatrix_B = 0;
            rowsOfMatrix_A++;
        }
    }

    // Creating producer threads
    for (int i = 0; i < resultMatSize; i++) {
        pthread_create(&producers[i], NULL, &producer, (void *)&vector[i]);
    }

    // Creating consumer threads
    for (int i = 0; i < resultMatSize; i++) {
        pthread_create(&consumers[i], NULL, &consumer, (void *)&vector[i]);
    }

    // Waiting for all producer threads to finish
    for (int i = 0; i < resultMatSize; i++) {
        pthread_join(producers[i], NULL);
    }

    // Waiting for all consumer threads to finish
    for (int i = 0; i < resultMatSize; i++) {
        pthread_join(consumers[i], NULL);
    }

    // Printing the result matrix
    printTheResultMatrix();

    free(buffer); // Freeing allocated memory

    return 0;
}
