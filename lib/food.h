/****************************************************************************
 * Copyright (C) 2014 by Lukas Elsner                                       *
 *                                                                          *
 * This file is part of calory-counter.                                     *
 *                                                                          *
 ****************************************************************************/

/**
 * @file food.h
 * @author Lukas Elsner
 * @date 25-09-2014
 * @brief Header containing the public accessible food methods.
 *
 */

#ifndef FOOD_H
#define FOOD_H

#define MAX_NAME_LEN 1024
#define MAX_MEASURE_LEN 256


/**
 *
 * @brief Forward declaration for food
 *
 * */
typedef struct food food;

/**
 * @brief Constructor for food
 * @return A pointer to the food structure, representing the created object
 *
 * After using this structure, it must be freed with food_destroy(food *)
 *
 * */
food *food_init();


/**
* @brief Method for serializing a food structure into a character array
* @param food* Pointer to structure to work on
* @return A pointer to the serialized structure. Must be freed by caller
*
* */
char *food_serialize(food *);

/**
* @brief Method for deserializing a char array into a food structure, this method is being used for
*        loading foods from the csv file, as well as for the network communication
* @param char* The serialized structure
* @return A pointer to the deserialized structure. Must be freed with food_destroy(food *)
*
* */
food *food_deserialize(char *);

/**
* @brief Method for setting the name of a food structure
* @param food* Pointer to structure to work on
* @param char* Pointer to character array containing the name to be set
*
* */
void food_set_name(food *, const char *);
/**
 * @brief Method for getting the name of a food structure
 * @param food* Pointer to structure to work on
 * @return A pointer to a string containing the name of the food. Must be freed by caller.
 *
 * */
char *food_get_name(food *);

/**
* @brief Method for setting the measure of a food structure
* @param food* Pointer to structure to work on
* @param char* Pointer to character array containing the measure to be set
*
* */
void food_set_measure(food *, const char *);

/**
 * @brief Method for getting the measure of a food structure
 * @param food* Pointer to structure to work on
 * @return A pointer to a string containing the measure of the food. Must be freed by caller.
 *
 *
 * */
char *food_get_measure(food *);

/**
* @brief Method for setting the weight of a food structure
* @param food* Pointer to structure to work on
* @param int Weight value in g to be set
*
* */
void food_set_weight(food *, const int);

/**
* @brief Method for getting the weight of a food structure
* @param food* Pointer to structure to work on
* @return Weight value in g of the food
*
* */
int food_get_weight(food *);


/**
* @brief Method for setting the kCal of a food structure
* @param food* Pointer to structure to work on
* @param int kCal value to be set
*
* */
void food_set_kcal(food *, const int);

/**
* @brief Method for getting the kCal of a food structure
* @param food* Pointer to structure to work on
* @return kCal value of the food
*
* */
int food_get_kcal(food *);

/**
* @brief Method for setting the fat of a food structure
* @param food* Pointer to structure to work on
* @param int Fat value in g to be set
*
* */
void food_set_fat(food *, const int);

/**
* @brief Method for getting the fat of a food structure
* @param food* Pointer to structure to work on
* @return Fat value in g of the food
*
* */
int food_get_fat(food *);

/**
* @brief Method for setting the carbo of a food structure
* @param food* Pointer to structure to work on
* @param int Carbo value in g to be set
*
* */
void food_set_carbo(food *, const int);

/**
* @brief Method for getting the carbo of a food structure
* @param food* Pointer to structure to work on
* @return Carbo value in g of the food
*
* */
int food_get_carbo(food *);

/**
* @brief Method for setting the protein of a food structure
* @param food* Pointer to structure to work on
* @param int Protein value in g to be set
*
* */
void food_set_protein(food *, const int);

/**
* @brief Method for getting the protein of a food structure
* @param food* Pointer to structure to work on
* @return Protein value in g of the food
*
* */
int food_get_protein(food *);

/**
* @brief Method for getting a string representation of a food structure
* @param food* Pointer to structure to work on
* @return String representation of food structure. Must be freed by caller.
*
* */
char *food_to_string(food *);


/**
* @brief Method for getting the size of a food structure
* @return Size of a food structure
*
* */
size_t food_get_size();

/**
 * @brief Destructor for food
 * @param food* Pointer to structure to be freed
 *
 * */
void food_destroy(food *);

#endif /* FOOD_H */
