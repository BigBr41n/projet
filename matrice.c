import java.util.Random;

// Ccass to hold data for each thread's indexes and result  ;
class IndexesOfThreadsData {
    int row;
    int column;
    int data;
}



// Class responsible for multiplying matrices ; 
class MatrixMultiplier {
    private final int[][] matrixA;
    private final int[][] matrixB;
    private final int[][] matrixResult;

    private final int MatA_Rows;
    private final int MatB_Columns;
    private final int Mat_Brows_Acolumns;

    private final int BUFFER_SIZE;
    private final Object lock = new Object();
    private int ip = 0, ic = 0;
    private final IndexesOfThreadsData[] buffer;
    

    // Constructor initializes matrices 
    public MatrixMultiplier(int[][] matrixA, int[][] matrixB) {
        this.matrixA = matrixA;
        this.matrixB = matrixB;
        this.MatA_Rows = matrixA.length;
        this.MatB_Columns = matrixB[0].length;
        this.Mat_Brows_Acolumns = matrixB.length;
        this.matrixResult = new int[MatA_Rows][MatB_Columns];

        this.BUFFER_SIZE = MatA_Rows * MatB_Columns; 
        this.buffer = new IndexesOfThreadsData[BUFFER_SIZE];
    }
    

    // Method to multiply matrices
    public int[][] multiplyMatrices() {
        IndexesOfThreadsData[] vector = generateIndexVector();

        Thread[] producers = new Thread[MatA_Rows * MatB_Columns];
        Thread consumer;

        // Create threads for matrix multiplication
        for (int i = 0; i < MatA_Rows * MatB_Columns; i++) {
            final IndexesOfThreadsData currentVector = vector[i];
            producers[i] = new Thread(() -> {
                producer(currentVector); // Perform matrix multiplication for each thread
                synchronized (lock) {
                    lock.notify(); // Notify after each thread completes its operation
                }
            });
            producers[i].start(); // Start thread
        }

        // Create consumer thread to collect results and fill the result matrix
        consumer = new Thread(() -> {
            for (int i = 0; i < MatA_Rows * MatB_Columns; i++) {
                synchronized (lock) {
                    while (ip == ic) {
                        try {
                            lock.wait(); // Wait while buffer is empty
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                    matrixResult[buffer[ic].row][buffer[ic].column] = buffer[ic].data; // Fill result matrix
                    ic = (ic + 1) % BUFFER_SIZE;
                    lock.notify(); // Notify after consuming
                }
            }
        });
        consumer.start(); 
        

        //wait for their completion
        try {
            for (int i = 0; i < MatA_Rows * MatB_Columns; i++) {
                producers[i].join(); // Wait for each thread to finish
            }
            synchronized (lock) {
                lock.notify(); // Notify consumer to finish after producers are done
            }
            consumer.join(); 
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        return matrixResult; // Return the the result matrix
    }



    private IndexesOfThreadsData[] generateIndexVector() {
        IndexesOfThreadsData[] vector = new IndexesOfThreadsData[MatA_Rows * MatB_Columns];
        int rowsOfMatrix_A = 0;
        int columnsOfMatrix_B = 0;

        for (int i = 0; i < MatA_Rows * MatB_Columns; i++) {
            vector[i] = new IndexesOfThreadsData();
            vector[i].row = rowsOfMatrix_A;
            vector[i].column = columnsOfMatrix_B;
            columnsOfMatrix_B++;
            if (columnsOfMatrix_B == MatB_Columns) {
                columnsOfMatrix_B = 0;
                rowsOfMatrix_A++;
            }
        }
        return vector;
    }

    // the producer threads function
    private void producer(IndexesOfThreadsData args) {
        args.data = 0;
        for (int i = 0; i < Mat_Brows_Acolumns; i++) {
            if (args.row < MatA_Rows && args.column < MatB_Columns && i < Mat_Brows_Acolumns) {
                args.data += matrixA[args.row][i] * matrixB[i][args.column];
            } else {
                System.out.println("Index out of bounds or incorrect matrix dimensions.");
                return;
            }
        }

        try {
            synchronized (lock) {
                while ((ip + 1) % BUFFER_SIZE == ic) {
                    lock.wait(); // Wait while buffer is full
                }
                buffer[ip] = args; // Place data into buffer
                ip = (ip + 1) % BUFFER_SIZE;
                lock.notify(); // Notify after producing
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}

// Main class to generate matrices and perform matrix multiplication
public class Main {
    private static final int MIN_ROWS_COLS = 3;
    private static final int MAX_ROWS_COLS = 6;
    private static final int MAX_VALUE = 9;

    public static void main(String[] args) {
        int[][] matrixA = generateMatrix();
        int[][] matrixB = generateMatrix();

        System.out.println("Matrix A:");
        printMatrix(matrixA);
        System.out.println("Matrix B:");
        printMatrix(matrixB);

        if (matrixA[0].length != matrixB.length) {
            System.out.println("Matrix dimensions are incorrect for multiplication.");
            return;
        }

        MatrixMultiplier matrixMultiplier = new MatrixMultiplier(matrixA, matrixB);
        int[][] result = matrixMultiplier.multiplyMatrices();

        System.out.println("Result:");
        printMatrix(result);
    }
    

    // Generate random matrix with random values
    private static int[][] generateMatrix() {
        Random rand = new Random();
        int rows = rand.nextInt(MAX_ROWS_COLS - MIN_ROWS_COLS + 1) + MIN_ROWS_COLS;
        int columns = rand.nextInt(MAX_ROWS_COLS - MIN_ROWS_COLS + 1) + MIN_ROWS_COLS;

        int[][] matrix = new int[rows][columns];

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                matrix[i][j] = rand.nextInt(MAX_VALUE + 1);
            }
        }
        return matrix;
    }

    // Print matrix
    private static void printMatrix(int[][] matrix) {
        for (int[] row : matrix) {
            for (int value : row) {
                System.out.print(value + "\t");
            }
            System.out.println();
        }
        System.out.println();
    }
}
