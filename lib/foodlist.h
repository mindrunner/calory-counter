/****************************************************************************
 * Copyright (C) 2014 by Lukas Elsner                                       *
 *                                                                          *
 * This file is part of calory-counter.                                     *
 *                                                                          *
 ****************************************************************************/

/**
 * @file foodlist.h
 * @author Lukas Elsner
 * @date 25-09-2014
 * @brief Header containing the public accessible foodlist methods.
 *
 *
 */

#ifndef FOODLIST_H
#define FOODLIST_H

/**
 *
 * @brief Forward declaration for foodlist
 *
 * */

#include "food.h"
#include "foodlistnode.h"

typedef struct foodlist foodlist;

/**
 * @brief Constructor for foodlist
 * @return A pointer to the foodlist structure, representing the created object
 *
 * After using this structure, it must be freed with foodlist_destroy(foodlist *)
 *
 * */
foodlist *foodlist_init();

/**
 * @brief Constructor for foodlist, loads a csv file which is passed as argument,
 *        throws a warning if file cannot be read and starts with empty dataset then.
 * @param char* Filename to the csv-file to be loaded.
 * @return A pointer to the foodlist structure, representing the created object
 *
 * After using this structure, it must be freed with foodlist_destroy(foodlist *)
 *
 * */
foodlist *foodlist_init_csv(char *);

/**
* @brief Method for appending a food structure to the list
* @param foodlist* Pointer to structure to work on
* @param food** pointer to pointer to food structure to add
*
* */
void foodlist_append(foodlist *, food **);

/**
* @brief Method for finding food within the food list
* @param foodlist* Pointer to structure to work on
* @param char* A pointer to the string which should be found
* @param size_t* Pointer to a size_t instance. The method updates its value to the length of the returned list.
* @return food** A pointer to an array of food pointers, which are satisfying the search criteria.
*
* */
food **foodlist_find(foodlist *, char *, size_t *);

/**
* @brief Method for saving the food structure to a file
* @param foodlist* Pointer to structure to work on
*
*  Before saving, the foodlist is being sorted by the name of the foods. If the file does not exist, it will be created.
*
* */
void foodlist_save(foodlist *);

/**
* @brief Method for getting the length of the list
* @param foodlist* Pointer to structure to work on
* @return Length of the list
*
* */
int foodlist_count(foodlist *);

/**
* @brief Method for getting the data of the list
* @param foodlist* Pointer to structure to work on
* @return First node of the list.
*
* */
foodlistnode *foodlist_get_data(foodlist *);

/**
* @brief Method for checking if the foodlist is empty
* @param foodlist* Pointer to structure to work on
* @return True, if the foodlist is empty, false otherwise
*
* */
bool foodlist_is_empty(foodlist *);

/**
 * @brief Destructor for foodlist
 * @param foodlist* Pointer to structure to be freed
 *
 * */
void foodlist_destroy(foodlist *);

#endif /* FOODLIST_H */
