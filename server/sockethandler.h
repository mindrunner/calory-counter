/****************************************************************************
 * Copyright (C) 2014 by Lukas Elsner                                       *
 *                                                                          *
 * This file is part of calory-counter.                                     *
 *                                                                          *
 ****************************************************************************/

/**
 * @file sockethandler.h
 * @author Lukas Elsner
 * @date 25-09-2014
 * @brief Header containing the public accessible sockethandler methods.
 *
 *
 */
#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include "../lib/foodlist.h"

/**
 *
 * @brief Forward declaration for food
 *
 * */
typedef struct sockethandler sockethandler;

/**
 * @brief Constructor for sockethandler
 * @return A pointer to the sockethandler structure, representing the created object
 *
 * After using this structure, it must be freed with sockethandler_destroy(food *)
 *
 * */
sockethandler *sockethandler_init(foodlist *);

/**
* @brief Main loop function for the sockethandling procedure
* @param sockethandler* A pointer to a valid initialized sockethandler structure
*
* This method starts a listening socket and produces client sockets for the spawned threads which are
* responsible for client connection handling. The method returns after sockethandler_shutdown() was called
* and all threads ended gracefully.
*
* */
void sockethandler_server_thread_func(sockethandler *s);

/**
* @brief Function to notify main loop thread, that it should shut down.
* @param sockethandler* A pointer to a valid initialized sockethandler structure
*
* This method sets the shutdown flag for the sockethandler structure. After that it joins all
* running threads before it returns.
*
* */
void sockethandler_shutdown(sockethandler *s);

/**
* @brief Method for setting the listening port of a sockethandler structure
* @param sockethandler* Pointer to structure to work on
* @param int Port to listen on
*
* */
void sockethandler_set_port(sockethandler *s, int port);

/**
 * @brief Destructor for sockethandler
 * @param sockethandler* Pointer to structure to be freed
 *
 * */
void sockethandler_destroy(sockethandler *);

#endif /* SOCKETHANDLER_H */
