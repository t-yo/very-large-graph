#pragma once

#include <stdio.h>
#include <igraph_matrix.h>

/**
 * @brief Print a matrix as doubles on a stream
 * @param stream The output stream
 * @param matrix The matrix
 * @param indent The indentation of each line
 */
void matrix_fprint(FILE* stream, igraph_matrix_t* matrix, igraph_integer_t indent);

/**
 * @brief Print a matrix as integers on a stream
 * @param stream The output stream
 * @param matrix The matrix
 * @param indent The indentation of each line
 */
void matrix_int_fprint(FILE* stream, igraph_matrix_t* matrix, igraph_integer_t indent);

/**
 * @brief Get the maximum of the matrix, excluding infinity (which is returned otherwise)
 * @param matrix The matrix
 * @return The maximum value, -infinity if the matrix is empty or full of infinity
 */
igraph_real_t matrix_max_real(igraph_matrix_t* matrix);
