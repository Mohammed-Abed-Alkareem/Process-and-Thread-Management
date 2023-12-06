//Importing the required libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>

//Defining constants
#define matrixSize 100
#define threadsNum 10
#define processesNum 10

//Defining M data type as int**
typedef int** M;

//A struct that contains the parameters for the threads
struct d{
    M matrix_A , matrix_B , result;
    int end , start;
    double elapsed_time;
};

typedef struct d Data;

M initializeMatrix();
void assignMatrix(M );
void deleteMatrix(M);
M multiplyMatrix(M , M , int , int);
void childProcess(int* pipe_1,M matrix_A , M matrix_B ,int start, int end);
void* threadRoutine(void* arg);
void check_results(M,M,M,M);


int main() {
    //Variables to store time values
    struct timeval start_time, end_time;
    double elapsed_time;

    pid_t process[processesNum] ; //array to store processes ID
    pthread_t threads[threadsNum];//array to store threads
    Data *thread_args[threadsNum];//array to store the struct for each thread

    //matrix A
    M matrix_A = initializeMatrix();
    assignMatrix(matrix_A);


    //matrix B
    M matrix_B = initializeMatrix();
    assignMatrix(matrix_B);
    for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j)
            matrix_B[i][j] *=2003;

    }

    //result matrices
    M result_1;
    M result_2 = initializeMatrix();
    M result_3 = initializeMatrix() ;
    M result_4 = initializeMatrix() ;

    while (1) //always ask the user for the choice
    {
        int choice;

        // Display the main menu and ask for the choice
        printf("\nChoose from the below operations:\n\n");
        printf("1. Regular Solution\n");
        printf("2. Processes Solution \n");
        printf("3. Joined Threads Solution\n");
        printf("4. Detached Threads Solution\n");
        printf("5. EXIT\n");

        printf("What do you want to do: ");
        scanf("%d", &choice);
        printf("\n");

        switch (choice) {
            case 1: //Regular Solution
                gettimeofday(&start_time, NULL); //store the time (start the timer)

                result_1 = multiplyMatrix(matrix_A, matrix_B, 0, matrixSize);

                gettimeofday(&end_time, NULL); //store the time (end time)

                elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) * 1e-6; //calculate the time
                printf("Time taken to multiply matrices: %f seconds\n", elapsed_time);

                break;

            case 2: // Processes Solution

                gettimeofday(&start_time, NULL);//store the time (start the timer)

                // Create an array to hold the pipe file descriptors
                int Pipe[processesNum][2];

                for (int i = 0; i < processesNum; ++i) {
                    // Create a pipe for each child process
                    if (pipe(Pipe[i]) == -1) { //check for errors
                        perror("Pipe creation failed");
                        exit(EXIT_FAILURE);
                    }
                }
                //creating n process
                for ( int i = 0; i < processesNum; ++i) {
                    process[i] = fork(); //creating process

                    if(process[i] <0){ //checking for errors
                        printf("error with creating process *_* \n");
                        exit(EXIT_FAILURE);
                    }
                    if(process[i] == 0){ //child process

                        close(Pipe[i][0]); // Close the read end of the pipe in the child process
                        childProcess(Pipe[i], matrix_A, matrix_B, (i*matrixSize)/processesNum, ((i+1)*matrixSize)/processesNum );
                        exit(EXIT_SUCCESS); //exit the process when finish
                    }
                }
                //Parent Process:
                int index = 0; //store the result in the correct index
                // Parent process reads from each pipe
                for (int i = 0; i < processesNum; ++i) {
                    // Close the write end_time_t of the pipe in the parent process
                    close(Pipe[i][1]);

                    while (read(Pipe[i][0], result_2[index] , sizeof(int)*matrixSize) > 0) {
                        index++;
                    }
                }

                for (int i = 0; i < processesNum; ++i) { //make sure that all child has been terminated
                    wait(NULL);
                }

                gettimeofday(&end_time, NULL); //store the time (end the timer)

                elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) * 1e-6; //calculate the time
                printf("Time taken to multiply matrices using %d Processes: %f seconds\n", processesNum, elapsed_time);

                break;

            case 3: //Joined Threads Solution
                gettimeofday(&start_time, NULL); //store the time (start the timer)

                // Create arguments for each thread
                for (int i = 0; i < threadsNum; ++i) {
                    thread_args[i] = (Data*)malloc(sizeof(Data));
                    if (thread_args[i] == NULL)
                    {
                        printf("Memory allocation failed.\n");
                        exit(0);
                    }
                    thread_args[i]->matrix_A = matrix_A;
                    thread_args[i]->matrix_B = matrix_B;
                    thread_args[i]->result = result_3;
                    thread_args[i]->start = (i * matrixSize) / threadsNum; //assign the start index that the thread begins calculation at
                    thread_args[i]->end = ((i + 1) * matrixSize) / threadsNum ;//assign the end index that the thread stops calculation at
                }

                // Creating the Joined Threads and Execute matrix multiplication in threads
                for (int i = 0; i < threadsNum; ++i) {
                    if (pthread_create(&threads[i], NULL, &threadRoutine, (void*)thread_args[i]) != 0) { //checking for errors
                        perror("Failed to create thread\n");
                        exit(5);
                    }
                }

                // Wait for all threads to complete
                for (int i = 0; i < threadsNum; ++i) {
                    if (pthread_join(threads[i], NULL) != 0) {
                        perror("Failed to join thread\n");
                        exit(6);
                    }
                    free(thread_args[i]); //deallocating the arguments of the threads
                }

                gettimeofday(&end_time, NULL); //store the time (end timer)

                elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) * 1e-6; //calculate the time
                printf("Time taken to multiply matrices using %d Joined Threads: %f seconds\n", threadsNum , elapsed_time);

                break;

            case 4: //Detached Threads Solution

                gettimeofday(&start_time, NULL); //store the time (start the timer)

                // Create arguments for each thread
                for (int i = 0; i < threadsNum; ++i) {
                    thread_args[i] = (Data*)malloc(sizeof(Data));
                    if (thread_args[i] == NULL)
                    {
                        printf("Memory allocation failed.\n");
                        exit(0);
                    }
                    thread_args[i]->matrix_A = matrix_A;
                    thread_args[i]->matrix_B = matrix_B;
                    thread_args[i]->result = result_4;
                    thread_args[i]->start = (i * matrixSize) / threadsNum; //assign the start index that the thread begins calculation at
                    thread_args[i]->end = ((i + 1) * matrixSize) / threadsNum ;//assign the end index that the thread stops calculation at
                }

                // Creating the Detached Threads and Execute matrix multiplication in threads
                pthread_attr_t detachedThread;
                pthread_attr_init(& detachedThread);
                pthread_attr_setdetachstate(&detachedThread , PTHREAD_CREATE_DETACHED);
                for (int i = 0; i < threadsNum; ++i) {
                    if (pthread_create(&threads[i], &detachedThread, &threadRoutine, (void*)thread_args[i]) != 0) {
                        perror("Failed to create thread\n");
                        exit(5);
                    }
                }

                sleep(1);  // Wait for a while to let detached threads complete

                double elapsed_time_for_thread = 0;
                for (int i = 0; i < threadsNum; ++i) {
                    if (thread_args[i]->elapsed_time > elapsed_time_for_thread)
                    elapsed_time_for_thread =thread_args[i]->elapsed_time;
                }

                pthread_attr_destroy(& detachedThread);

                for (int i = 0; i < threadsNum; ++i) {
                    free(thread_args[i]);
                }

                gettimeofday(&end_time, NULL); //store the time (end the timer)

                 elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) * 1e-6; //Calculate the time
                printf("Time taken to multiply matrices using %d Detached Threads: %f seconds\n", threadsNum , elapsed_time-1 + elapsed_time_for_thread);

                break;

            case 5:     // Exit and free memory

                check_results(result_1,  result_2,  result_3,  result_4);

                deleteMatrix(matrix_A);
                deleteMatrix(matrix_B);
                deleteMatrix(result_1);
                deleteMatrix(result_2);
                deleteMatrix(result_4);
                deleteMatrix(result_3);

                return 0;

            default:
                printf("Enter a choice between 1 and 5\n");
        }
    }
}

// Function to initialize the matrix
int** initializeMatrix(){
    int** matrix = (int**) malloc(sizeof (int*)*matrixSize);
    if (matrix == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(0);
    }

    for(int i = 0 ; i < matrixSize ; i++){
        matrix[i] = (int*) malloc(sizeof (int)*matrixSize);
        if (matrix[i] == NULL)
        {
            printf("Memory allocation failed.\n");
            exit(0);
        }
    }

    for(int i = 0 ; i <matrixSize ; i++){
        for(int j = 0 ; j < matrixSize ; j++)
            matrix[i][j] = 0;
    }
    return matrix;
}

// Function to free the memory allocated for the matrix
void deleteMatrix(int** matrix){
    for(int i = 0 ; i <matrixSize ; i++)
        free(matrix[i]);

    free(matrix);
}

// Function to perform matrix multiplication
int** multiplyMatrix(int** matrix_A , int** matrix_B,int start , int end){

    int** matrix_c =initializeMatrix();

    for (int i = start; i<end; ++i) {
        for (int j = 0; j < matrixSize; ++j) {
            for (int k = 0; k < matrixSize; ++k) {
                matrix_c[i][j] += matrix_A[i][k] * matrix_B[k][j];
            }
        }
    }
    return matrix_c;
}

// Function to assign random values to the matrix
void assignMatrix(int** matrix) {

    int my_id = 1210708;

    int counter = 0 ;
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {

            // matrix[i][j]=0;
            matrix[i][j] = my_id / (int)pow(10.0, 6.0- counter);
            my_id %= (int)pow(10, 6- (counter++));

            if (counter > 6) {
                counter = 0;
                my_id = 1210708;
            }
        }
    }
}
// Function for child process to perform partial matrix multiplication
void childProcess(int* pipe, M matrix_A, M matrix_B, int start, int end) {

    for (int i = start; i < end; ++i) {

        int *rowRes = (int*)malloc(sizeof(int) * matrixSize);
        if (rowRes == NULL)
        {
            printf("Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }

        // Initialize rowRes to zeros
        for (int j = 0; j < matrixSize; ++j) {
            rowRes[j] = 0;
        }
// Perform partial matrix multiplication
        for (int j = 0; j < matrixSize; ++j) {

            for (int k = 0; k < matrixSize; ++k) {
                rowRes[j] += matrix_A[i][k] * matrix_B[k][j];
            }
        }
        // Write the partial result to the pipe
        write(pipe[1], rowRes, sizeof(int) * matrixSize);
        free(rowRes);
    }

    close(pipe[1]); //finish writing
}

// Thread routine for matrix multiplication
void* threadRoutine(void* arg) {
    struct timeval start_time, end_time;
    Data* thread_args = (Data*)arg;
    gettimeofday(&start_time, NULL); //store the time (start the timer)


    for (int i = thread_args->start; i < thread_args->end; ++i) {
        for (int j = 0; j < matrixSize; ++j) {

            for (int k = 0; k < matrixSize; ++k) {
                thread_args->result[i][j] += thread_args->matrix_A[i][k] * thread_args->matrix_B[k][j];
            }
        }
    }
    gettimeofday(&end_time, NULL); //store the time (end time)

    thread_args->elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) * 1e-6; //calculate the time
    return NULL;
}

void check_results(M result_1, M result_2, M result_3, M result_4){

    for (int i = 0; i < matrixSize; ++i) {

        for (int j = 0; j < matrixSize; ++j) {

            if(!(result_1[i][j] == result_2[i][j]   &&  result_2[i][j] == result_3[i][j] &&  result_3[i][j] == result_4[i][j] )){
                printf("there is an error with the results is somewhere *_* \n \n \n");
                exit (EXIT_FAILURE);
            }
        }
    }
    printf("there is no error with the results  ^_^ \n \n \n");
}