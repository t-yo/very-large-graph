#include "vector.h"

#define VECTOR_FPRINT_IMPLEMENTATION(name, format, type)              \
    void vector_##name##fprint(FILE* stream, igraph_vector_t* vector) \
    {                                                                 \
        igraph_integer_t size = igraph_vector_size(vector);           \
        if (size == 0)                                                \
        {                                                             \
            return;                                                   \
        }                                                             \
        fprintf(stream, format, (type)VECTOR(*vector)[0]);            \
        for (igraph_integer_t i = 1; i < size; ++i)                   \
        {                                                             \
            fprintf(stream, " "format, (type)VECTOR(*vector)[i]);     \
        }                                                             \
    }

VECTOR_FPRINT_IMPLEMENTATION(, "%f", igraph_real_t)

VECTOR_FPRINT_IMPLEMENTATION(int_, "%d", igraph_integer_t)
