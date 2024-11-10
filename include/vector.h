#ifndef VECTOR_H
#define VECTOR_H

#include <unistd.h>

typedef enum
{
    VECTOR_ERR_INVALID_MAX_SIZE = -1,
    VECTOR_ERR_BUFFER_OVERFLOW  = -2,
} VectorErrors;

typedef void Vector;
typedef void (*destroy_fn)(void *);

void  *vector_init(size_t nelems, int *err);
int    vector_destroy(Vector *vector, destroy_fn destroy, int *err);
void  *vector_add(Vector *vector, void *element, int *err);
void  *vector_remove(Vector *vector, size_t idx);
void  *vector_get(Vector *vector, size_t idx, int *err);
size_t vector_size(Vector *vector);
// void  vector_each(Vector *vector)

#endif
