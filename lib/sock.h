/****************************************************************************
 * Copyright (C) 2014 by Lukas Elsner                                       *
 *                                                                          *
 * This file is part of calory-counter.                                     *
 *                                                                          *
 ****************************************************************************/

/**
 * @file sock.h
 * @author Lukas Elsner
 * @date 02-09-2014
 * @brief Header file containing read and write functions for calory socket protocol
 *
 * Every write is BUF_LEN bytes long and has to be acknowledged with a RE_LEN bytes long answer
 * containing ACK or NACK.
 *
 */

#ifndef SOCK_H
#define SOCK_H

#include <stdbool.h>

#define BUF_LEN 4096
#define RE_LEN 32

/**
* @brief Lower level function to send data to the other endpoint
* @param int The socket to communicate with
* @param char* The data to send
* @return True, if the communication was successful, false otherwise
* */
bool sock_write(int socket, char *data);

/**
* @brief Function to read data from the other endpoint
* @param int The socket to communicate with
* @param char* A pointer to a buffer which for the read data. Must be at least 4096 bytes long.
* @return The size of read data
* */
size_t sock_read(int socket, char *data);

/**
* @brief Higher level function to send serialized food to the other endpoint
* @param int The socket to communicate with
* @param char* The serialized food to send
* @return True, if the communication was successful, false otherwise
* */
bool sock_send_food(int socket, char *data);

/**
* @brief Higher level function to send a search request to the other endpoint
* @param int The socket to communicate with
* @param char* The search term to send
* @return True, if the communication was successful, false otherwise
* */
bool sock_send_search(int socket, char *data);

/**
* @brief Higher level function to send the number of found items to the other endpoint
* @param int The socket to communicate with
* @param char* The number of found items as string value
* @return True, if the communication was successful, false otherwise
* */
bool sock_send_count(int socket, char *data);


#endif /* TOOLS_H */
