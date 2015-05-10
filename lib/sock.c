/****************************************************************************
* Copyright (C) 2014 by Lukas Elsner                                       *
*                                                                          *
* This file is part of calory-counter.                                     *
*                                                                          *
****************************************************************************/

/**
* @file sock.c
* @author Lukas Elsner
* @date 02-09-2014
* @brief File containing read and write functions for calory socket protocol
*
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include "sock.h"


bool sock_write(int socket, char *data) {
    char buf[BUF_LEN] = {0};
    char re[RE_LEN] = {0};
    snprintf(buf, BUF_LEN, "%s", data);
    int num_w = write(socket, buf, BUF_LEN);
    if (num_w <= 0) {
        /* if we could not write to socket, something went wrong */
        return false;
    }
    while (num_w != BUF_LEN) {
        /* write as long as all expexted bytes are arrived */
        int w = write(socket, buf + num_w, BUF_LEN - num_w);
        if (w < 0) {
            /* if we could not write to socket, something went wrong */
            return false;
        }
        num_w += w;
    }
    /* repeat the above procedure for reading and await an ACK as answer */
    int num_r = read(socket, re, RE_LEN);
    if (num_r < 0) {
        return false;
    }
    while (num_r != RE_LEN) {
        int r = read(socket, re + num_r, RE_LEN - num_r);
        if (r < 0) {
            return false;
        }
        num_r += r;
    }
    if (!strcmp(re, "ACK")) {
        return true;
    } else {
        /* this should never happen */
        assert(true);
        return false;
    }
}

size_t sock_read(int socket, char *data) {
    /* this is working similar to the sock_write procedure, after we read all bytes, always confirm that with an ACK */
    char buf[BUF_LEN] = {0};
    char re[RE_LEN] = {0};
    int num_r = read(socket, buf, BUF_LEN);
    if (num_r <= 0) {
        return num_r;
    }
    while (num_r != BUF_LEN) {
        int r = read(socket, buf + num_r, BUF_LEN - num_r);
        if (r <= 0) {
            return r;
        }
        num_r += r;
    }
    sprintf(re, "ACK");
    write(socket, re, RE_LEN);
    snprintf(data, BUF_LEN, "%s", buf);
    return num_r;
}

bool sock_send_food(int socket, char *data) {
    char buf[BUF_LEN] = {0};
    snprintf(buf, BUF_LEN, "FOOD:%s", data);
    return sock_write(socket, buf);
}

bool sock_send_search(int socket, char *data) {
    char buf[BUF_LEN] = {0};
    snprintf(buf, BUF_LEN, "SEARCH:%s", data);
    return sock_write(socket, buf);
}

bool sock_send_count(int socket, char *data) {
    char buf[BUF_LEN] = {0};
    snprintf(buf, BUF_LEN, "COUNT:%s", data);
    return sock_write(socket, buf);
}
