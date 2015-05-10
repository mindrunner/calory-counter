/****************************************************************************
* Copyright (C) 2014 by Lukas Elsner                                       *
*                                                                          *
* This file is part of calory-counter.                                     *
*                                                                          *
****************************************************************************/

/**
* @file foodlist.c
* @author Lukas Elsner
* @date 25-09-2014
* @brief File containing the foodlist structure and its member methods.
*
*/

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include "food.h"
#include "foodlistnode.h"
#include "foodlist.h"

/**
* @brief foodlist structure for representing a food item
*
*/
struct foodlist {
    pthread_mutex_t rw_mutex;
    /**< Mutex for thread safe read-write access */
    pthread_mutex_t r_mutex/**< Mutex for thread safe write access */;
    int read_count;
    /**< Integer for thread safe read access */
    foodlistnode *data;
    /**< First node of this list */
    char *file;/**< Filename for loading/saving data from/to file */
};

/**
* @brief Helper function to enter a critical section for reading
* @param foodlist* The foodlist structure to lock
*
* */
void start_read(foodlist *fl) {
    pthread_mutex_lock(&(fl->r_mutex));
    if (++fl->read_count == 1)
        pthread_mutex_lock(&(fl->rw_mutex));
    pthread_mutex_unlock(&(fl->r_mutex));
}

/**
* @brief Helper function to exit a critical section for reading
* @param foodlist* The foodlist structure to unlock
*
* */
void end_read(foodlist *fl) {
    pthread_mutex_lock(&(fl->r_mutex));
    if (--fl->read_count == 0)
        pthread_mutex_unlock(&(fl->rw_mutex));
    pthread_mutex_unlock(&(fl->r_mutex));
}

/**
* @brief Helper function to enter a critical section for writing
* @param foodlist* The foodlist structure to lock
*
* */
void start_write(foodlist *fl) {
    pthread_mutex_lock(&(fl->rw_mutex));
}

/**
* @brief Helper function to exit a critical section for writing
* @param foodlist* The foodlist structure to unlock
*
* */
void end_write(foodlist *fl) {
    pthread_mutex_unlock(&(fl->rw_mutex));
}

/**
* @brief Compare function for using qsort() with food objects
*
* @param void* Pointer to first food object
* @param void* Pointer to second food object
* @return An integer less than, equal to, or greater than zero if a (or the first n bytes thereof) is found,
*         respectively, to be less than, to match, or be greater than b
*
* */
int cmpfunc(const void *a, const void *b) {
    food *f1 = *(food *const *) a;
    food *f2 = *(food *const *) b;
    char *c1 = food_get_name(f1);
    char *c2 = food_get_name(f2);
    return strcasecmp(c1, c2);
}

foodlist *foodlist_init() {
    foodlist *f = (foodlist *) malloc(sizeof(foodlist));
    pthread_mutex_init(&(f->rw_mutex), NULL);
    pthread_mutex_init(&(f->r_mutex), NULL);
    f->read_count = 0;
    f->data = NULL;
    char *fname = "calories.csv";
    f->file = malloc(strlen(fname) + 1);
    sprintf(f->file, "%s", fname);
    return f;
}

foodlist *foodlist_init_csv(char *file) {
    foodlist *fl = foodlist_init();
    free(fl->file);
    fl->file = malloc(strlen(file) + 1);
    sprintf(fl->file, "%s", file);
    const int max_line_len = 4096;
    FILE *fptr = fopen(fl->file, "r");
    if (!fptr) {
        printf("cannot read file %s\n", fl->file);
    } else {
        char line[max_line_len];
        while (fgets(line, max_line_len, fptr)) {
            if (*line == '#')
                continue;
            food *f = food_deserialize(line);
            foodlist_append(fl, &f);
        }
        fclose(fptr);
    }
    return fl;
}

int foodlist_count(foodlist *fl) {
    int count = 0;
    start_read(fl);
    if (!foodlist_is_empty(fl)) {
        count = foodlistnode_count(fl->data);
    }
    end_read(fl);
    return count;
}

bool foodlist_is_empty(foodlist *fl) {
    bool ret;
    start_read(fl);
    ret = (NULL == fl->data);
    end_read(fl);
    return ret;
}

void foodlist_append(foodlist *fl, food **f) {
    foodlistnode *newnode = foodlistnode_init();
    foodlistnode_set_item(newnode, f);
    if (foodlist_is_empty(fl)) {
        /* this is going to be the first element */
        start_write(fl);
        fl->data = newnode;
        end_write(fl);
    } else {
        /* if elements already existing, search for the tail */
        start_write(fl);
        foodlistnode *n = fl->data;
        while (foodlistnode_has_next(n)) {
            n = foodlistnode_get_next(n);
        }
        foodlistnode_set_next(n, &newnode);
        end_write(fl);
    }
}

foodlistnode *foodlist_get_data(foodlist *fl) {
    start_read(fl);
    foodlistnode *fln = fl->data;
    end_read(fl);
    return fln;
}

food **foodlist_find(foodlist *fl, char *str, size_t *num) {
    size_t max_items = 25;
    food **ret = calloc(max_items, sizeof(food *));
    *num = 0;
    foodlistnode *n = foodlist_get_data(fl);
    while (NULL != n) {
        if (*num == max_items - 1) {
            max_items *= 2;
            ret = realloc(ret, max_items * food_get_size());
        }
        food *f = foodlistnode_get_item(n);
        char *name = food_get_name(f);

        /*
         * To satisfy all search criteria, the string we are searching for has obviously to be shorter than
         * the string in which we are searching. Furthermore, the first srtlen(str) characters have to match
         * and either the searchstring has to end with a comma, or the next character in the string we are
         * searching in has to be a comma. This makes sure, that either "Milk," or "Milk" can match e.g.
         * "Milk,Whole,3.3% Fat"
         */
        if (strlen(name) >= strlen(str) && strncasecmp(str, name, strlen(str)) == 0) {
            if (strlen(name) == strlen(str)
                    || name[strlen(str)] == ','
                    || str[strlen(str) - 1] == ',') {
                ret[*num] = f;
                *num += 1;
            }
        }
        n = foodlistnode_get_next(n);
    }
    return ret;
}

void foodlist_save(foodlist *fl) {
    /* copy list into array, to be able to use qsort */
    size_t numfoods = foodlist_count(fl);
    food **foods = calloc(numfoods, sizeof(food *));

    int i = 0;
    foodlistnode *n = foodlist_get_data(fl);
    while (NULL != n) {
        foods[i] = foodlistnode_get_item(n);
        n = foodlistnode_get_next(n);
        ++i;
    }

    /* sort with qsort */
    qsort(foods, numfoods, sizeof(food *), cmpfunc);

    /* save sorted array to file */
    FILE *fptr = fopen(fl->file, "w");
    if (!fptr) {
        printf("cannot write file %s\n", fl->file);
    } else {
        for (i = 0; i < numfoods; ++i) {
            food *f = foods[i];
            char *c = food_serialize(f);
            fprintf(fptr, "%s\n", c);
            free(c);
        }
        fclose(fptr);
    }
    free(foods);
}

void foodlist_destroy(foodlist *fl) {
    pthread_mutex_destroy(&fl->rw_mutex);
    pthread_mutex_destroy(&fl->r_mutex);
    free(fl->file);
    if (fl->data) {
        foodlistnode_destroy(fl->data);
    }
    free(fl);
}






