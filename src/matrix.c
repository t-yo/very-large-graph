#include "matrix.h"


#define MATRIX_FPRINT_IMPLEMENTATION(name, format, type)              \
    void matrix_##name##fprint(FILE* stream, igraph_matrix_t* matrix, \
        igraph_integer_t indent)                                      \
    {                                                                 \
        igraph_integer_t rows = igraph_matrix_nrow(matrix);           \
        igraph_integer_t cols = igraph_matrix_ncol(matrix);           \
        for (igraph_integer_t i = 0; i < rows; ++i)                   \
        {                                                             \
            fprintf(stream, "%*s", indent, "");                       \
            if (cols > 1)                                             \
            {                                                         \
                fprintf(stream, format, (type)MATRIX(*matrix, i, 0)); \
                for (igraph_integer_t j = 1; j < cols; ++j)           \
                {                                                     \
                    fprintf(stream, " "format,                        \
                        (type)MATRIX(*matrix, i, j));                 \
                }                                                     \
            }                                                         \
            fprintf(stream, "\n");                                    \
        }                                                             \
    }


MATRIX_FPRINT_IMPLEMENTATION(, "%f", igraph_real_t)

MATRIX_FPRINT_IMPLEMENTATION(int_, "%d", igraph_integer_t)

igraph_real_t matrix_max_real(igraph_matrix_t* matrix)
{
    igraph_real_t max = -IGRAPH_INFINITY;
    igraph_integer_t rows = igraph_matrix_nrow(matrix);
    igraph_integer_t cols = igraph_matrix_ncol(matrix);
    for (igraph_integer_t i = 0; i < rows; ++i)
    {
        for (igraph_integer_t j = 0; j < cols; ++j)
        {
            igraph_real_t value = MATRIX(*matrix, i, j);
            if ((max < value) && (value < INFINITY))
            {
                max = value;
            }
        }
    }
    return max;
}
