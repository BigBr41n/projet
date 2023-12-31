#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#define MIN_ROWS_COLS 3
#define MAX_ROWS_COLS 6
#define MAX_VALUE 9
#define BUFFER_SIZE 10



// Structure to store thread data
typedef struct IndexesOfThreadsDate {
    int row;
    int column;
    int data;
} IndexesOfThreadsDate; 




// Semaphores for synchronization
sem_t full, empty, mutex;




// Global variables for matrix sizes and matrices
int MatA_Rows, MatB_Columns, Mat_Brows_Acolumns;
int **matrixA, **matrixB, **matrixResult;
int ip = 0, ic = 0; // Producer and consumer indexes
IndexesOfThreadsDate *buffer; // Buffer for thread data





// Function to generate random matrices
void generate() {
    srand(time(NULL));
    
    // Randomly determine matrix sizes 
    MatA_Rows = rand() % (MAX_ROWS_COLS - MIN_ROWS_COLS + 1) + MIN_ROWS_COLS;
    MatB_Columns = rand() % (MAX_ROWS_COLS - MIN_ROWS_COLS + 1) + MIN_ROWS_COLS;
    Mat_Brows_Acolumns = rand() % (MAX_ROWS_COLS - MIN_ROWS_COLS + 1) + MIN_ROWS_COLS;
    
    
    

    // Allocate memory for matrices A, B, and the result matrix
    matrixA = (int **)malloc(MatA_Rows * sizeof(int *));
    matrixB = (int **)malloc(Mat_Brows_Acolumns * sizeof(int *));
    matrixResult = (int **)malloc(MatA_Rows * sizeof(int *));
    
    
    

    // Populate matrix A with random values
    for (int i = 0; i < MatA_Rows; ++i) {
        matrixA[i] = (int *)malloc(Mat_Brows_Acolumns * sizeof(int));
        for (int j = 0; j < Mat_Brows_Acolumns; ++j) {
            matrixA[i][j] = rand() % (MAX_VALUE + 1);
        }
    }
    

    // Populate matrix B with random values
    for (int i = 0; i < Mat_Brows_Acolumns; ++i) {
        matrixB[i] = (int *)malloc(MatB_Columns * sizeof(int));
        for (int j = 0; j < MatB_Columns; ++j) {
            matrixB[i][j] = rand() % (MAX_VALUE + 1);
        }
    }
    
    

    // Allocate memory for the result matrix
    for (int i = 0; i < MatA_Rows; ++i) {
        matrixResult[i] = (int *)malloc(MatB_Columns * sizeof(int));
    }

    // Display matrices A and B
    printf("the Matrix A : \n");
    for (int i = 0; i < MatA_Rows; ++i) {
        for (int j = 0; j < Mat_Brows_Acolumns; ++j) {
            printf("%d\t", matrixA[i][j]);
        }
        printf("\n");
    }

    printf("=======================================================\n the Matrix B : \n");
    for (int i = 0; i < Mat_Brows_Acolumns; ++i) {
        for (int j = 0; j < MatB_Columns; ++j) {
            printf("%d\t", matrixB[i][j]);
        }
        printf("\n");
    }

    printf("\n\n\n");
}





// Producer thread function to perform matrix multiplication
void *producer(void *arg) {


    // Wait for empty buffer slot and acquire mutex for buffer access
    sem_wait(&empty);
    sem_wait(&mutex);
    
    IndexesOfThreadsDate *args = (IndexesOfThreadsDate *)arg;
    
    for (int i = 0; i < Mat_Brows_Acolumns; i++) {
        args->data += matrixA[args->row][i] * matrixB[i][args->column];
    }

    // Store data in the buffer and update buffer index
    buffer[ip] = *args;
    ip = (ip + 1) % BUFFER_SIZE;

    // Release mutex and signal that buffer is full
    sem_post(&full);
    sem_post(&mutex);
}







// Consumer thread function to store calculated data in the result matrix
void *consumer(void *arg) {
        // Wait for full buffer slot and acquire mutex for buffer access
    sem_wait(&full);
    sem_wait(&mutex);

        // Store data from buffer into the result matrix and update buffer index
    matrixResult[buffer[ic].row][buffer[ic].column] = buffer[ic].data;
    ic = (ic + 1) % BUFFER_SIZE;

        // Release mutex and signal that buffer is empty
    sem_post(&empty);
    sem_post(&mutex);
}






// Function to print the result matrix
void printTheResultMatrix() {
    printf("The result is : \n\n");
    for (int i = 0; i < MatA_Rows; i++) {
        for (int j = 0; j < MatB_Columns; j++) {
            printf("%d\t", matrixResult[i][j]);
        }
        printf("\n");
    }
}







int main() {
    generate(); // Generate matrices A and B

    int resultMatSize = MatA_Rows * MatB_Columns;
    buffer = (IndexesOfThreadsDate *)malloc(resultMatSize * sizeof(IndexesOfThreadsDate));

    sem_init(&mutex, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);

    pthread_t *producers = (pthread_t *)malloc(resultMatSize * sizeof(pthread_t));
    pthread_t *consumers = (pthread_t *)malloc(resultMatSize * sizeof(pthread_t));

    int rowsOfMatrix_A = 0;
    int columnsOfMatrix_B = 0;
    
    

    // Fill the buffer with corresponding row and column indices
    for (int i = 0; i < resultMatSize; i++) {
        buffer[i].row = rowsOfMatrix_A;
        buffer[i].column = columnsOfMatrix_B;
        buffer[i].data =0 ; 

        columnsOfMatrix_B++;

        if (columnsOfMatrix_B == MatB_Columns) {
            columnsOfMatrix_B = 0;
            rowsOfMatrix_A++;
        }
    }



    // Create producer threads to perform matrix multiplication
    for (int i = 0; i < resultMatSize; i++) {
        pthread_create(&producers[i], NULL, &producer, (void *)&buffer[i]);
    }
    
    
    

    // Create the consumer thread to store the calculated data in the result matrix
    for (int i = 0; i < resultMatSize; i++) {
        pthread_create(&consumers[i], NULL, &consumer, NULL);
    }
    
    

    // Wait for all producer threads to finish their work
    for (int i = 0; i < resultMatSize; i++) {
        pthread_join(producers[i], NULL);
    }
    
    
    for (int i = 0; i < resultMatSize; i++) {
        pthread_join(consumers[i], NULL);
    }
    

    // Print the result matrix
    printTheResultMatrix();

    // Free allocated memory
    for (int i = 0; i < MatA_Rows; i++) {
        free(matrixA[i]);
    }
    free(matrixA);

    for (int i = 0; i < Mat_Brows_Acolumns; i++) {
        free(matrixB[i]);
    }
    free(matrixB);

    for (int i = 0; i < MatA_Rows; i++) {
        free(matrixResult[i]);
    }
    free(matrixResult);

    free(buffer);

    printf("Memory freed.\n");

    return 0;
}
