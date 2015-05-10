/****************************************************************************
 * Copyright (C) 2014 by Lukas Elsner                                       *
 *                                                                          *
 * This file is part of calory-counter.                                     *
 *                                                                          *
 ****************************************************************************/

/**
 * @file foodlistnode.c
 * @author Lukas Elsner
 * @date 01-09-2014
 * @brief File containing the foodlistnode structure and its member methods.
 *
 */

#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include "foodlistnode.h"


/**
 * @brief foodlistnode structure for representing a foodlistnode item
 *
 */

struct foodlistnode {
  food *item; /**< Item of this foodlistnode */
  foodlistnode *next;/**< Next foodlistnode of this foodlistnode */
};

foodlistnode *foodlistnode_init() {
  foodlistnode *fln = (foodlistnode *)malloc(sizeof(foodlistnode));
  fln->item = NULL;
  fln->next = NULL;
  return fln;
}

foodlistnode *foodlistnode_get_next(foodlistnode *fln) {
  return fln->next;
}

food *foodlistnode_get_item(foodlistnode *fln) {
  return fln->item;
}

void foodlistnode_set_next(foodlistnode *fln, foodlistnode **f) {
  assert(fln->next == NULL);
  fln->next = *f;
}

void foodlistnode_set_item(foodlistnode *fln, food **f) {
  assert(fln->item == NULL);
  fln->item = *f;
}

bool foodlistnode_has_next(foodlistnode *fln) {
  return NULL != fln->next;
}

int foodlistnode_count(foodlistnode *fln) {
  if(!foodlistnode_get_next(fln)) {
    return 1;
  } else {
    return 1 + foodlistnode_count(foodlistnode_get_next(fln));
  }
}

void foodlistnode_destroy(foodlistnode *fln) {
  if(fln->item) {
    food_destroy(fln->item);
  }
  if(fln->next) {
    foodlistnode_destroy(fln->next);
  }
  free(fln);
}
