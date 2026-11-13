#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MATRIX_SIZE 20
#define NUM_THREADS 10

int matA[MATRIX_SIZE][MATRIX_SIZE];
int matB[MATRIX_SIZE][MATRIX_SIZE];

int matSumResult[MATRIX_SIZE][MATRIX_SIZE];
int matDiffResult[MATRIX_SIZE][MATRIX_SIZE];
int matProductResult[MATRIX_SIZE][MATRIX_SIZE];


int MAX;

typedef struct{
    int start;
    int end;
} ThreadArgs;

void fillMatrix(int matrix[MATRIX_SIZE][MATRIX_SIZE]) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            matrix[i][j] = rand()%10+1;
        }
    }
}

void printMatrix(int matrix[MATRIX_SIZE][MATRIX_SIZE]) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the sum of the
// values at the coordinates of matA and matB.
void* computeSum(void* args) {
    ThreadArgs* data = (ThreadArgs*)args;

    for (int cell = data->start; cell < data->end; cell++) {
        int i = cell / MAX;
        int j = cell % MAX;

        matSumResult[i][j] = matA[i][j] + matB[i][j];
    }

    return NULL;
}

void* computeDiff(void* args) {
    ThreadArgs* data = (ThreadArgs*)args;

    for (int cell = data->start; cell < data->end; cell++) {
        int i = cell / MAX;
        int j = cell % MAX;

        matDiffResult[i][j] = matA[i][j] - matB[i][j];
    }

    return NULL;
}

void* computeProduct(void* args) {
    ThreadArgs* data = (ThreadArgs*)args;

    for (int cell = data->start; cell < data->end; cell++) {
        int i = cell / MAX;
        int j = cell % MAX;

        int sum = 0;

        for (int k = 0; k < MAX; k++) {
            sum += matA[i][k] * matB[k][j];
        }

        matProductResult[i][j] = sum;
    }

    return NULL;
}

void* computeAll(void* args) {
    computeSum(args);
    computeDiff(args);
    computeProduct(args);

    return NULL;
}

int main(int argc, char* argv[]) {
    srand(time(0));

    if (argc != 2) {
        printf("Usage: %s <matrix_size>\n", argv[0]);
        return 1;
    }

    MAX = atoi(argv[1]);

    if (MAX < 1 || MAX > MATRIX_SIZE) {
        printf("Matrix size must be between 1 and %d\n", MATRIX_SIZE);
        return 1;
    }

    // 1. Fill the matrices
    fillMatrix(matA);
    fillMatrix(matB);

    // 2. Print initial matrices
    printf("Matrix A:\n");
    printMatrix(matA);

    printf("Matrix B:\n");
    printMatrix(matB);

    // 3. Create pthread objects
    pthread_t threads[NUM_THREADS];
    ThreadArgs args[NUM_THREADS];

    int totalCells = MAX * MAX;
    int cellsPerThread = (totalCells + NUM_THREADS - 1) / NUM_THREADS;

    // 4. Create 10 threads
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].start = i * cellsPerThread;
        args[i].end = args[i].start + cellsPerThread;

        if (args[i].end > totalCells) {
            args[i].end = totalCells;
        }

        if (args[i].start >= totalCells) {
            args[i].end = args[i].start;
        }

        pthread_create(&threads[i], NULL, computeAll, &args[i]);
    }

    // 5. Wait for all threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // 6. Print results
    printf("Results:\n");

    printf("Sum:\n");
    printMatrix(matSumResult);

    printf("Difference:\n");
    printMatrix(matDiffResult);

    printf("Product:\n");
    printMatrix(matProductResult);

    return 0;
}