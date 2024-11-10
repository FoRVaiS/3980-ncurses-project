#include "vector.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * A generic array data structure to dynamically reallocate space when adding
 * new elements that exceed the limit of the array.
 */

typedef struct
{
    size_t _elemsize;
    size_t maxsize;
    void **list;
    size_t size;
} vector_t;

static void *vector_resize(vector_t *vector, size_t maxelems, int *err);

void *vector_init(size_t nelems, int *err)
{
    void *retval;

    vector_t *vector;
    void    **list;

    if(nelems == 0)
    {
        *err   = VECTOR_ERR_INVALID_MAX_SIZE;
        retval = NULL;
        goto exit;
    }

    errno  = 0;
    vector = (vector_t *)calloc(1, sizeof(vector_t));
    if(vector == NULL)
    {
        *err   = errno;
        retval = NULL;
        goto exit;
    }

    errno = 0;
    list  = (void **)calloc(nelems, sizeof(void *));
    if(list == NULL)
    {
        *err   = errno;
        retval = NULL;
        goto cleanup;
    }

    vector->_elemsize = sizeof(void *);
    vector->maxsize   = nelems;
    vector->list      = list;
    vector->size      = 0;

    retval = vector;
    goto exit;

cleanup:
    free(vector);

exit:
    return retval;
}

int vector_destroy(Vector *vector, destroy_fn destroy, int *err)
{
    int retval;

    vector_t *_vec = (vector_t *)vector;

    for(size_t idx = 0; idx < vector_size(vector); idx++)
    {
        void *element;

        *err    = 0;
        element = vector_get(vector, idx, err);
        if(*err < 0)
        {
            retval = -1;
            goto exit;
        }

        destroy(element);
    }

    free((void *)(_vec->list));
    free(vector);

    retval = 0;
exit:
    return retval;
}

void *vector_add(Vector *vector, void *element, int *err)
{
    vector_t *_vec = (vector_t *)vector;

    if(_vec->size >= _vec->maxsize)
    {
        const void *resize;

        resize = vector_resize(_vec, _vec->maxsize * 2, err);    // NOTE: By default, double the capacity
        if(resize == NULL)
        {
            return NULL;
        }
    }

    _vec->list[_vec->size] = (uint8_t *)element;
    _vec->size += 1;

    return vector;
}

void *vector_remove(Vector *vector, size_t idx)
{
    vector_t *_vec = (vector_t *)vector;

    void  *target_element = NULL;
    size_t cursor         = 0;

    // A naive approach to removing an element from a dynamic array
    for(size_t _idx = idx; _idx < _vec->size; _idx++)
    {
        void *element = _vec->list[_idx];

        if(_idx == idx)
        {
            target_element = element;
            continue;    // Skip copying this component
        }

        _vec->list[cursor] = element;

        cursor++;
    }

    _vec->size -= 1;

    return target_element;

    // Trim (or free) the list
    // TODO: vector_trim
}

void *vector_get(Vector *vector, size_t idx, int *err)
{
    const vector_t *_vec = (vector_t *)vector;

    if(idx >= _vec->size)
    {
        *err = VECTOR_ERR_BUFFER_OVERFLOW;
        return NULL;
    }

    return ((vector_t *)vector)->list[idx];
}

static void *vector_resize(vector_t *vector, size_t maxelems, int *err)
{
    void **new_list;

    errno    = 0;
    new_list = (void **)realloc((void *)vector->list, sizeof(void *) * maxelems);
    if(new_list == NULL)
    {
        *err = errno;
        return NULL;
    }

    vector->list    = new_list;
    vector->maxsize = maxelems;

    return vector;
}

size_t vector_size(Vector *vector)
{
    return ((vector_t *)vector)->size;
}
