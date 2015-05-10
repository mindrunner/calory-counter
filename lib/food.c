/****************************************************************************
 * Copyright (C) 2014 by Lukas Elsner                                       *
 *                                                                          *
 * This file is part of calory-counter.                                     *
 *                                                                          *
 ****************************************************************************/

/**
 * @file food.c
 * @author Lukas Elsner
 * @date 01-09-2014
 * @brief File containing the food structure and its member methods.
 *
 */

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "food.h"

/**
 * @brief food structure for representing a food item
 *
 */
struct food {
  char name[MAX_NAME_LEN]; /**< Name of the food. */
  char measure[MAX_NAME_LEN]; /**< Name of the food. */
  int weight;/**< Weight (g) of the food. */
  int kcal;/**< kCal of the food. */
  int fat;/**< Fat (g) of the food. */
  int carbo;/**< Carbo (g) of the food. */
  int protein;/**< Protein (g) of the food. */
};

food *food_init()
{
  food *f = (food *)malloc(sizeof(food));
  memset(f->name, 0, 1024);
  memset(f->measure, 0, 1024);
  f->weight = -1;
  f->kcal = -1;
  f->fat = -1;
  f->carbo = -1;
  f->protein = -1;
  return f;
}

void food_set_name(food *f, const char *name)
{
  strncpy(f->name, name, MAX_NAME_LEN);
}

char *food_get_name(food *f)
{
  return f->name;
}

void food_set_measure(food *f, const char *measure)
{
  strncpy(f->measure, measure, MAX_MEASURE_LEN);
}

char *food_get_measure(food *f)
{
  return f->measure;
}

char *food_to_string(food *f)
{
  char *ret = malloc(4096);
  sprintf(ret, "Name: %s\n Measure: %s\n Weight (g): %d\n kCal: %d\n Fat (g): %d\n Carbo (g): %d\n Protein (g): %d\n",
          f->name,
          f->measure,
          f->weight,
          f->kcal,
          f->fat,
          f->carbo,
          f->protein);
  return ret;
}


void food_set_weight(food *f, const int weight)
{
  f->weight = weight;
}

int food_get_weight(food *f)
{
  return f->weight;
}

void food_set_kcal(food *f, const int kcal)
{
  f->kcal = kcal;
}

int food_get_kcal(food *f)
{
  return f->kcal;
}

void food_set_fat(food *f, const int fat)
{
  f->fat = fat;
}

int food_get_fat(food *f)
{
  return f->fat;
}

void food_set_carbo(food *f, const int carbo)
{
  f->carbo = carbo;
}
int food_get_carbo(food *f)
{
  return f->carbo;
}

void food_set_protein(food *f, const int protein)
{
  f->protein = protein;
}

int food_get_protein(food *f)
{
  return f->protein;
}

char *food_serialize(food *f)
{
  char *buf = malloc(4096);
  snprintf(buf, 4096, "%s,%s,%d,%d,%d,%d,%d", f->name, f->measure, f->weight, f->kcal, f->fat, f->carbo, f->protein);
  return buf;
}

food *food_deserialize(char *c)
{
  /* count how many items are needed */
  int count = 1;
  char *p = c;
  while (*p++ != 0)
    if(*p == ',') count++;
  char **res = calloc(count, sizeof(char *));

  /* tokenize string */
  int i = 0;
  p = strtok (c, ",");
  while (p != NULL) {
    *(res + i) = p;
    ++i;
    p = strtok (NULL, ",");
  }

  food *f = food_init();
  food_set_protein(f, atoi(res[--count]));
  food_set_carbo(f, atoi(res[--count]));
  food_set_fat(f, atoi(res[--count]));
  food_set_kcal(f, atoi(res[--count]));
  food_set_weight(f, atoi(res[--count]));
  food_set_measure(f, res[--count]);

  char *foodname = calloc(1024, sizeof(char));
  for(int i = 0; i < count; ++i) {
    foodname = strncat(foodname, res[i], 1024);
    if(i < count - 1) /* do not add comma at the end of string */
      foodname = strncat(foodname, ",", 1024);
  }
  food_set_name(f, foodname);
  free(foodname);
  free(res);
  return f;
}

size_t food_get_size()
{
  return sizeof(food);
}

void food_destroy(food *f)
{
  free(f);
}

