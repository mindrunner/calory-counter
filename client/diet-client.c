/****************************************************************************
* Copyright (C) 2014 by Lukas Elsner                                       *
*                                                                          *
* This file is part of calory-counter.                                     *
*                                                                          *
****************************************************************************/

/**
* @file diet-client.c
* @author Lukas Elsner
* @date 01-09-2014
* @brief Main program file with main() entry point.
*
* The calory-counter client application.
*
*/

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include "../lib/sock.h"
#include "../lib/food.h"

/**
* @brief Client config structure
*
*
*/
struct client_config {
    char *host; /**< Hostname to connect to */
    unsigned int port; /**< Port to connect to */
};

/**
* @brief Forward declaration of client_config
*
*
*/
typedef struct client_config client_config;

/**
* @brief This is set to exit when the application should exit gracefully.
*
* */
bool client_exit = false;

/**
* @brief Prints the help for diet-client to the console.
* @param char* Program name
*
* */
void usage(char *pname) {
    fprintf(stderr, "usage: %s <host> <port>\n", pname);
}

/**
* @brief Method for getting a true/false user input.
* @return True, if the user decided to cancel, false if the user wants to continue
*
* */
bool ask_user_cancel() {
    char c = -1;
    do {
        printf("Really cancel? y/n\n> ");
        c = getchar();
        getchar();
        if (c == 'y' || c == 'Y') {
            return true;
        }
        if (c == 'n' || c == 'N') {
            continue;
        }
    } while (c != 'n' && c != 'N');
    return false;
}

/**
* @brief Method for creating a food object with help of user input.
* @return A pointer to a food structure if the user entered all data correctly,
*         NULL if the user decided to cancel the operation.
*
* */
food *get_food_from_user() {
    food *f = food_init();
    char *input = NULL;
    size_t inputlen = 0;
    int read = 0;
    /* The first two questions are asking for strings */
    for (int i = 0; i < 2; ++i) {
        int max_len = 0;
        do {
            if (i == 0) {
                max_len = MAX_NAME_LEN;
                printf("Please enter name of food or ctrl+d to cancel:\n> ");
                fflush(stdout);
            }
            if (i == 1) {
                max_len = MAX_MEASURE_LEN;
                printf("Please enter measure for %s or ctrl+d to cancel:\n> ", food_get_name(f));
                fflush(stdout);
            }
            read = getline(&input, &inputlen, stdin);
            if (read == -1) {
                free(input);
                input = NULL;
                inputlen = 0;
                if (ask_user_cancel()) {
                    food_destroy(f);
                    return NULL;
                } else
                    continue;
            }
            if (read > max_len) {
                if (i == 0)
                    printf("Name of food must not exeed %d characters\n", max_len);
                if (i == 1)
                    printf("Measure must not exeed %d characters\n", max_len);
            }
        } while (read > max_len || read == 1 || read == -1);
        input[strlen(input) - 1] = 0;
        if (i == 0)
            food_set_name(f, input);
        if (i == 1)
            food_set_measure(f, input);
        free(input);
        input = NULL;
        inputlen = 0;
    }

    /* The last five questions are asking for integers */
    for (int i = 2; i < 7; ++i) {
        bool is_digit = true;
        bool too_long = false;
        do {
            if (!is_digit) {
                printf("Entered input is not a number\n");
            }
            if (too_long) {
                printf("Entered input too long\n");
            }
            is_digit = true;
            too_long = false;
            if (i == 2)
                printf("Please enter weight (g) of %s or ctrl+d to cancel:\n> ", food_get_name(f));
            if (i == 3)
                printf("Please enter kCal of %s or ctrl+d to cancel:\n> ", food_get_name(f));
            if (i == 4)
                printf("Please enter fat (g) of %s or ctrl+d to cancel:\n> ", food_get_name(f));
            if (i == 5)
                printf("Please enter carbo (g) of %s or ctrl+d to cancel:\n> ", food_get_name(f));
            if (i == 6)
                printf("Please enter protein (g) of %s or ctrl+d to cancel:\n> ", food_get_name(f));
            read = getline(&input, &inputlen, stdin);
            if (read == -1) {
                free(input);
                input = NULL;
                inputlen = 0;
                if (ask_user_cancel()) {
                    food_destroy(f);
                    return NULL;
                } else
                    continue;
            }
            /* check if user typed in a number */
            for (int j = 0; j < read - 1; j++)
                if (!isdigit(input[j])) {
                    is_digit = false;
                }
            /* only allow numbers with seven digits */
            if (read > 7) {
                too_long = true;
            }
        } while (!is_digit || too_long || read == 1 || read == -1);
        input[strlen(input) - 1] = 0;
        if (i == 2)
            food_set_weight(f, atoi(input));
        if (i == 3)
            food_set_kcal(f, atoi(input));
        if (i == 4)
            food_set_fat(f, atoi(input));
        if (i == 5)
            food_set_carbo(f, atoi(input));
        if (i == 6)
            food_set_protein(f, atoi(input));
        free(input);
        input = NULL;
        inputlen = 0;
    }
    return f;
}

/**
* @brief Loop function with handles the client connection and user input stuff.
* @param client_config* A pointer to the client configuration
*
* */
void client_loop(client_config *c) {
    while (!client_exit) {
        int sock;
        struct sockaddr_in server;

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            printf("Could not create socket %d\n", errno);
        }

        server.sin_addr.s_addr = inet_addr(c->host);
        server.sin_family = AF_INET;
        server.sin_port = htons(c->port);

        /* Connect to remote server, retry every 5 seconds if failing */
        if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
            printf("connect failed. Error %d\n", errno);
            sleep(5);
            continue;
        }

        while (!client_exit) {
            printf("Enter the food name to search, ‘a’ to add a new food item, or ‘q’ to quit:\n> ");

            char *input = NULL;
            size_t inputlen = 0;
            int read = 0;
            read = getline(&input, &inputlen, stdin);

            /* add some food */
            if (read == 2 && *input == 'a') {
                food *f = get_food_from_user();
                if (f) {
                    char *sf = food_serialize(f);
                    if (sock_send_food(sock, sf)) {
                        printf("Sent food to server\n");
                    } else {
                        printf("Error sending food to server\n");
                    }
                    free(sf);
                    food_destroy(f);
                }
                /* quit application */
            } else if (read == 2 && *input == 'q') {
                client_exit = true;
                printf("quit application\n");
                /* everything else is a search request */
            } else if (read >= 2) {
                if (sock_send_search(sock, input)) {
                    /* server must reply with number of items */
                    char buf[BUF_LEN] = {0};
                    size_t count = 0;
                    if (sock_read(sock, buf)) {
                        if (!strncmp("COUNT:", buf, 6)) {
                            count = atoi(buf + 6);
                        } else {
                            printf("Error in protocol, expected COUNT");
                        }
                    } else {
                        printf("Read failed, %d\n", errno);
                        continue;
                    }
                    if (count == 0) {
                        printf("\nNo items found matching %sPlease check your spelling and try again!\n\n", input);
                    } else if (count == 1) {
                        printf("\nFound %zu item\n\n", count);
                    } else {
                        printf("\nFound %zu items\n\n", count);
                    }
                    /* now, server must send 'count' foods */
                    for (int i = 0; i < count; ++i) {
                        char buf[BUF_LEN] = {0};
                        if (sock_read(sock, buf)) {
                            if (!strncmp("FOOD:", buf, 5)) {
                                food *f = food_deserialize(buf + 5);
                                char *c = food_to_string(f);
                                printf("%s\n", c);
                                free(c);
                                free(f);
                            } else {
                                printf("Error in protocol, expected FOOD");
                            }
                        } else {
                            printf("Read failed, %d\n", errno);
                            continue;
                        }
                    }
                } else {
                    printf("Send failed, %d\n", errno);
                    continue;
                }
            } else {
                continue;
            }
            free(input);
        }
        close(sock);
    }
}

/**
* @brief Main entry point of diet-client
* @param int Number of arguments
* @param char** Pointer to array of arguments
* @return Exit code of diet-client
*
* */
int main(int argc, char **argv) {

    client_config cc;
    /* set default values */
    cc.port = 12345;
    cc.host = "127.0.0.1";

    /* program started with one argument */
    if (argc > 1) {
        /* user wants to see help */
        if (!strcmp(argv[1], "-h")) {
            usage(argv[0]);
            return 0;
        }
        cc.port = atoi(argv[1]);
    }

    /* program started with one argument */
    if (argc > 2) {
        cc.host = argv[1];
        cc.port = atoi(argv[2]);
    }
    client_loop(&cc);
    return 0;
}
