<!DOCTYPE html>
<html>
<head>
  <title>Matrix Multiplication with Multithreading</title>
</head>
<body>
  <h1>Matrix Multiplication with Multithreading</h1>
  <p>This Java program performs matrix multiplication using multithreading. It utilizes threads to multiply matrices concurrently, enhancing the performance for larger matrices.</p>

  <h2>Files</h2>
  <ul>
    <li><code>Main.java</code>: Contains the main method to generate matrices, perform matrix multiplication, and display the results.</li>
    <li><code>IndexesOfThreadsData.java</code>: Defines a class to hold thread-related indexes and data.</li>
    <li><code>MatrixMultiplier.java</code>: Class responsible for matrix multiplication using multithreading.</li>
  </ul>

  <h2>Classes</h2>

  <h3>Main.java</h3>
  <p>The <code>Main</code> class handles the execution flow, matrix generation, and result display.</p>
  <ul>
    <li><code>main(String[] args)</code>: Entry point of the program. Generates random matrices, performs matrix multiplication, and prints the resulting matrix.</li>
    <li><code>generateMatrix()</code>: Generates a random matrix with specified dimensions and random values.</li>
    <li><code>printMatrix(int[][] matrix)</code>: Prints the given matrix to the console.</li>
  </ul>

  <h3>IndexesOfThreadsData.java</h3>
  <p>This class defines the data structure to hold thread-related indexes and data used in the matrix multiplication process.</p>
  <ul>
    <li><code>int row</code>: Holds the row index.</li>
    <li><code>int column</code>: Holds the column index.</li>
    <li><code>int data</code>: Holds the computed data during the multiplication.</li>
  </ul>

  <h3>MatrixMultiplier.java</h3>
  <p>This class handles matrix multiplication using multithreading.</p>
  <ul>
    <li><code>MatrixMultiplier(int[][] matrixA, int[][] matrixB)</code>: Constructor initializes matrices and sets buffer size based on matrix dimensions.</li>
    <li><code>multiplyMatrices()</code>: Method to perform matrix multiplication with multithreading and return the resulting matrix.</li>
    <li><code>generateIndexVector()</code>: Generates an index vector for threads based on matrix dimensions.</li>
    <li><code>producer(IndexesOfThreadsData args)</code>: Performs matrix multiplication for each thread and places the result in the buffer.</li>
  </ul>

  <h2>How to Use</h2>
  <p>To use this code:</p>
  <ol>
    <li>Compile the Java files: <code>javac Main.java</code></li>
    <li>Run the compiled program: <code>java Main</code></li>
  </ol>
  </body>
</html>
