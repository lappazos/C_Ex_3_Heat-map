/**
* @file calculator.c
* @author Lior Paz <lior.paz1@mail.huji.ac.il>
* @version 1.0
* @date 22 Aug 2018
*
* @brief Grid heat calculator
*
* @section DESCRIPTION
* Grid heat calculator
* Process: calculates the heat spread in iterations.
*/

// ------------------------------ includes ------------------------------
#include "calculator.h"
#include <math.h>

// ------------------------------ functions -----------------------------
/**
 * @brief perform cyclic adjustments
 * @param n num of columns
 * @param m num of lines
 * @param is_cyclic bool
 * @param i line index
 * @param j column index
 * @param top top double ptr
 * @param bottom bottom double ptr
 * @param right right double ptr
 * @param left left double ptr
 */
void cyclicCheck(size_t n, size_t m, int is_cyclic, int i, int j, double *top, double *bottom,
                 double *right, double *left)
{
    if (! is_cyclic)
    {
        if (i == 0)
        {
            (*top) = 0;
        }
        else if (i == (int)m - 1)
        {
            (*bottom) = 0;
        }
        if (j == 0)
        {
            (*left) = 0;
        }
        else if (j == (int)n - 1)
        {
            (*right) = 0;
        }
    }
}

/**
 *
 * @param function heat func
 * @param grid double 2 dim array
 * @param n num of columns
 * @param m num of lines
 * @param sources array of source points
 * @param num_sources num of sources
 * @param terminate stop value for calculation
 * @param n_iter number of iterations to finish
 * @param is_cyclic bool for grid lip calculation form
 * @return difference in sum between iterations
 */
double calculate(diff_func function, double **grid, size_t n, size_t m, source_point *sources,
                 size_t num_sources, double terminate, unsigned int n_iter, int is_cyclic)
{
    double prevSum = 0;
    double currSum = 0;
    size_t rounds = 0;
    do
    {
        prevSum = currSum;
        currSum = 0;
        size_t counter = 0;
        for (int i = 0; i < (int)m; ++ i)
        {
            int p = i - 1;
            if (p == - 1)
            {
                p = (int) m - 1;
            }
            for (int j = 0; j < (int)n; ++ j)
            {
                if ((i == sources[counter].x) && (j == sources[counter].y))
                {
                    counter ++;
                    continue;
                }
                int q = j - 1;
                if (q == - 1)
                {
                    q = (int) n - 1;
                }
                double top = grid[p][j];
                double bottom = grid[(i + 1) % m][j];
                double right = grid[i][(j + 1) % n];
                double left = grid[i][q];
                cyclicCheck(n, m, is_cyclic, i, j, &top, &bottom, &right, &left);
                double result = function(grid[i][j], right, top, left, bottom);
                grid[i][j] = result;
                currSum += result;
            }
        }
        rounds ++;
        if (rounds == n_iter)
        {
            return (fabs(currSum - prevSum));
        }
    } while (fabs(currSum - prevSum) >= terminate);
    return fabs(currSum - prevSum);
}