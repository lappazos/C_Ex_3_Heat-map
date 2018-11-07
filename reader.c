/**
* @file reader.c
* @author Lior Paz <lior.paz1@mail.huji.ac.il>
* @version 1.0
* @date 22 Aug 2018
*
* @brief Heat Distribution map.
*
* @section DESCRIPTION
* Heat Distribution map
* Input : Input File.
* Process: calculates the heat spread in iterations.
* Output : Stages of calculation until final converge.
*/

// ------------------------------ includes ------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "calculator.h"
#include "heat_eqn.h"

// -------------------------- const definitions -------------------------
/**
 * @brief stderr messages
 */
#define WRONG_ARGS_MSG "Wrong Number of Args"
#define FILE_PROBLEM_MSG "Problem with File"
#define BAD_FORMAT_MSG "Bad File Format"
#define MEM_ERR_MSG "Memory problem"

#define NUM_OF_ARGS 2

#define NULL_CHAR '\0'

/**
 * @brief rand num for line length for input
 */
#define RAND_LINE_LENGTH 100

#define NUM_OF_GRID_DIM 2

/**
 * @brief number of point parameters
 */
#define NUM_POINTS_VALS 3

#define NUM_VAL_INPUT 1

// ------------------------------ functions -----------------------------
/**
 * free the memory
 * @param arr sources array to free
 * @param grid grid to free
 * @param m number of grid lines
 */
void freeMem(source_point *arr, double **grid, size_t m)
{
    free(arr);
    if (grid == NULL)
    {
        return;
    }
    for (size_t i = 0; i < m; ++ i)
    {
        if (grid[i] == NULL)
        {
            break;
        }
        free(grid[i]);
    }
    free(grid);
}

/**
 * Comperator for qsort
 * @param a point
 * @param b point
 * @return bool for bigger point
 */
int helper(const void *a, const void *b)
{
    source_point *one = (source_point *) a;
    source_point *two = (source_point *) b;
    int diff = one->x - two->x;
    if (diff != 0)
    {
        return diff;
    }
    return one->y - two->y;
}

/**
 * perform safe exit
 * @param input file
 * @param message err message
 * @return exit failure int
 */
int safeExitNoMem(FILE *input, const char *message)
{
    fprintf(stderr, "%s", message);
    fclose(input);
    return EXIT_FAILURE;
}

/**
 * perform safe exit with memory free
 * @param input file
 * @param message stderr message
 * @param ptr ptr to free
 * @return exit failure int
 */
int safeExit(FILE *input, const char *message, void *ptr)
{
    free(ptr);
    return safeExitNoMem(input, message);
}

/**
 * creates a pointer for grid
 * @param m num of lines
 * @param sourceArr
 * @param n num of columns
 * @param counter source array size
 * @return 2dim double array pointer
 */
double **gridCreation(size_t m, source_point *sourceArr, size_t n, size_t counter)
{
    double **grid = (double **) malloc(sizeof(double *) * m);
    if (grid == NULL)
    {
        return NULL;
    }
    for (size_t i = 0; i < m; ++ i)
    {
        grid[i] = (double *) malloc(sizeof(double) * n);
        if (grid[i] == NULL)
        {
            freeMem(sourceArr, grid, m);
            return NULL;
        }
        for (size_t j = 0; j < n; ++ j)
        {
            grid[i][j] = 0;
        }
    }
    for (size_t k = 0; k < counter; ++ k)
    {
        grid[sourceArr[k].x][sourceArr[k].y] = sourceArr[k].value;
    }
    return grid;
}

/**
 * creates new source
 * @param sourceArr
 * @param counter
 * @param x line index
 * @param y column index
 * @param value point heat
 */
void newSource(source_point *sourceArr, size_t counter, int x, int y, double value)
{
    sourceArr[counter - 1].x = x;
    sourceArr[counter - 1].y = y;
    sourceArr[counter - 1].value = value;
}

/**
 * prints the grid
 * @param n num of lines
 * @param m num of columns
 * @param grid grid to print
 */
void printGrid(size_t n, size_t m, double *const *grid)
{
    for (size_t i = 0; i < m; ++ i)
    {
        for (size_t j = 0; j < n; ++ j)
        {
            printf("%2.4f,", grid[i][j]);
        }
        printf("\n");
    }
}

/**
 * perform iterations of calculator
 * @param n num of columns
 * @param m num of lines
 * @param sourceArr array of heat sources
 * @param counter num of sources
 * @param terminate terminate diff value
 * @param iterNum num of iterations to print
 * @param isCyclic is the calculation cyclic or not
 * @param grid 2 dim double array
 * @return calculate difference
 */
double
calcIter(size_t n, size_t m, source_point *sourceArr, size_t counter, double terminate,
         unsigned int iterNum, int isCyclic, double **grid)
{
    double diff = calculate(heat_eqn, grid, n, m, sourceArr, counter, terminate, iterNum, isCyclic);
    printf("%f\n", diff);
    printGrid(n, m, grid);
    return diff;
}

/**
 * exit in case of grid failure
 * @param m num of lines
 * @param sourceArr array of sources
 * @param grid grid ptr
 * @return
 */
int gridExit(size_t m, source_point *sourceArr, double **grid)
{
    freeMem(sourceArr, grid, m);
    fprintf(stderr, MEM_ERR_MSG);
    return EXIT_FAILURE;
}

/**
 * Runs the program
 * @param argc num of arguments
 * @param argv array of args
 * @return Program succesful or not
 */
int main(int argc, char *argv[])
{
    //args check
    if (argc > NUM_OF_ARGS)
    {
        fprintf(stderr, WRONG_ARGS_MSG);
        return EXIT_FAILURE;
    }
    //file check
    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        fprintf(stderr, FILE_PROBLEM_MSG);
        return EXIT_FAILURE;
    }
    //random array of char to recieve input line
    char line[RAND_LINE_LENGTH] = {NULL_CHAR};
    if (fgets(line, RAND_LINE_LENGTH, input) == NULL)
    {
        return safeExitNoMem(input, BAD_FORMAT_MSG);
    }
    //dim check
    size_t n, m;
    if ((sscanf(line, "%zu, %zu", &m, &n) != NUM_OF_GRID_DIM) ||
        (m <= 0 || n <= 0) ||
        (fgets(line, RAND_LINE_LENGTH, input) == NULL) ||
        ((strcmp(line, "----\n") == 0) && (strcmp(line, "----\r\n") == 0)))
    {
        return safeExitNoMem(input, BAD_FORMAT_MSG);
    }
    source_point *sourceArr = (source_point *) malloc(sizeof(source_point)*2);
    if (sourceArr == NULL)
    {
        return safeExitNoMem(input, MEM_ERR_MSG);
    }
    if (fgets(line, RAND_LINE_LENGTH, input) == NULL)
    {
        return safeExit(input, BAD_FORMAT_MSG, sourceArr);
    }
    int x, y;
    double value;
    size_t counter = 0;
    //sources check
    while (strcmp(line, "----\n") && strcmp(line, "----\r\n"))
    {
        if ((sscanf(line, "%d, %d, %lf", &x, &y, &value) != NUM_POINTS_VALS) ||
            ((x < 0) || (x >= (int)m) || (y < 0) || (y >= (int)n)))
        {
            return safeExit(input, BAD_FORMAT_MSG, sourceArr);
        }
        counter ++;
        sourceArr = (source_point*)realloc(sourceArr, (sizeof(source_point) * (counter + 1)));
        if ((sourceArr == NULL) ||
            (fgets(line, RAND_LINE_LENGTH, input) == NULL))
        {
            return safeExit(input, MEM_ERR_MSG, sourceArr);
        }
        newSource(sourceArr, counter, x, y, value);
    }
    if (fgets(line, RAND_LINE_LENGTH, input) == NULL)
    {
        return safeExit(input, BAD_FORMAT_MSG, sourceArr);
    }
    //terminate check
    double terminate = 0;
    if ((sscanf(line, "%lf", &terminate) != NUM_VAL_INPUT) ||
        (fgets(line, RAND_LINE_LENGTH, input) == NULL))
    {
        return safeExit(input, BAD_FORMAT_MSG, sourceArr);
    }
    //iterations check
    unsigned int iterNum = 0;
    if ((sscanf(line, "%d", &iterNum) != NUM_VAL_INPUT) ||
        (fgets(line, RAND_LINE_LENGTH, input) == NULL))
    {
        return safeExit(input, BAD_FORMAT_MSG, sourceArr);
    }
    //iterations cyclic
    int isCyclic = 0;
    if (sscanf(line, "%d", &isCyclic) != NUM_VAL_INPUT)
    {
        return safeExit(input, BAD_FORMAT_MSG, sourceArr);
    }
    fclose(input);
    //grid creation
    double **grid = gridCreation(m, sourceArr, n, counter);
    if (grid == NULL)
    {
        return gridExit(m, sourceArr, grid);
    }
    //sources array sort to increase efficiency in calculation
    qsort(sourceArr, counter, sizeof(source_point), helper);
    double diff;
    //calculation
    do
    {
        diff = calcIter(n, m, sourceArr, counter, terminate, iterNum, isCyclic, grid);
    } while (diff >= terminate);
    //free mem
    freeMem(sourceArr, grid, m);
    return EXIT_SUCCESS;
}