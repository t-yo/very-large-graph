#pragma once

#include <stdio.h>
#include <igraph_datatype.h>

/**
 * @brief Print a vector as doubles on a stream
 * @param stream The output stream
 * @param vector The vector
 */
void vector_fprint(FILE* stream, igraph_vector_t* vector);

/**
 * @brief Print a vector as integers on a stream
 * @param stream The output stream
 * @param vector The vector
 */
void vector_int_fprint(FILE* stream, igraph_vector_t* vector);
