/****************************************************************************
 * Copyright (C) 2014 by Lukas Elsner                                       *
 *                                                                          *
 * This file is part of calory-counter.                                     *
 *                                                                          *
 ****************************************************************************/

/**
 * @file diet-server.c
 * @author Lukas Elsner
 * @date 01-09-2014
 * @brief Main program file with main() entry point.
 *
 * The calory-counter server application.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "sockethandler.h"

/**
 * @brief Representation of the food list
 *
 * */
foodlist *fl;


/**
 * @brief Representation of the socket handler
 *
 * */
sockethandler *s;

/**
 * @brief Prints the help for diet-server to the console.
 * @param char* Program name
 *
 * */
void usage(char *pname)
{
  fprintf(stderr, "usage: %s <port>\n", pname);
}

/**
 * @brief Define the function to be called when ctrl-c (SIGINT) signal is sent to process
 *
 * */
void signal_callback_handler(int signum)
{
  printf("Caught signal %d\n", signum);
  sockethandler_shutdown(s);
}

/**
 * @brief Main entry point of airport-sim
 * @param int Number of arguments
 * @param char** Pointer to array of arguments
 * @return Exit code of airport-sim
 *
 * */
int main(int argc, char **argv)
{
  /* set default port */
  unsigned int port = 12345;

  /* program started with one argument */
  if(argc > 1) {
    /* user wants to see help */
    if(!strcmp(argv[1], "-h")) {
      usage(argv[0]);
      return 0;
    }
    port = atoi(argv[1]);
  }

  /* initialize the foodlist */
  fl = foodlist_init_csv("calories.csv");

    /* initialize the sockethandler */
  s = sockethandler_init(fl);
  sockethandler_set_port(s, port);

  /* Register signal and signal handler */
  signal(SIGINT, signal_callback_handler);
  /* Register SUGUSR1 as well to be able to test the signal handler when debugging with gdb. */
  signal(SIGUSR1, signal_callback_handler);

  /* Main server functionality */
  sockethandler_server_thread_func(s);

  /* free the sockethandler object */
  sockethandler_destroy(s);

  /* save the foodlist before exiting */
  foodlist_save(fl);

  /* free the foodlist object */
  foodlist_destroy(fl);

  return 0;
}
