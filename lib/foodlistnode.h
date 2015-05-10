/****************************************************************************
 * Copyright (C) 2014 by Lukas Elsner                                       *
 *                                                                          *
 * This file is part of calory-counter.                                     *
 *                                                                          *
 ****************************************************************************/

/**
 * @file foodlistnode.h
 * @author Lukas Elsner
 * @date 25-09-2014
 * @brief Header containing the public accessible foodlistnode methods.
 *
 *
 */
#ifndef FOODLISTNODE_H
#define FOODLISTNODE_H

#include <stdbool.h>
#include "food.h"

/**
 *
 * @brief Forward declaration for foodlistnode
 *
 * */
typedef struct foodlistnode foodlistnode;

/**
 * @brief constructor for foodlistnode
 * @return A pointer to the foodlistnode structure, representing the created object
 *
 * After using this structure, it must be freed with foodlistnode_destroy(foodlistnode *)
 *
 * */
foodlistnode *foodlistnode_init();

/**
* @brief Method for setting the next node of a node
* @param foodlistnode* Pointer to structure to work on
* @param foodlistnode** Pointer to pointer to foodlistnode to add
*
* */
void foodlistnode_set_next(foodlistnode *fln, foodlistnode **f);

/**
* @brief Method for getting the next foodlistnode of a foodlistnode structure
* @param foodlistnode* Pointer to structure to work on
* @return Next foodlistnode of the passed foodlistnode
*
* */
foodlistnode *foodlistnode_get_next(foodlistnode *fln);

/**
* @brief Method for setting the item of a node
* @param foodlistnode* Pointer to structure to work on
* @param foodlistnode** Pointer to pointer to food to set
*
* */
void foodlistnode_set_item(foodlistnode *fln, food **f);

/**
* @brief Method for getting the item of a foodlistnode structure
* @param foodlistnode* Pointer to structure to work on
* @return Item of the passed foodlistnode
*
* */
food *foodlistnode_get_item(foodlistnode *fln);

/**
* @brief Recursive method for getting the number of items in this foodlistnode structure
* @param foodlistnode* Pointer to structure to work on
* @return The number of items in this foodlistnode structure
*
* */
int foodlistnode_count(foodlistnode *);

/**
* @brief Method for checking if a foodlistnode has a next element.
* @param foodlistnode* Pointer to structure to work on
* @return True, if passed foodlistnode has a next element, false otherwise
*
* */
bool foodlistnode_has_next(foodlistnode *);

/**
 * @brief Destructor for foodlistnode
 * @param foodlistnode* Pointer to structure to be freed
 *
 * */
void foodlistnode_destroy(foodlistnode *);

#endif /* FOODLISTNODE_H */
