/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traceroute.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qpeng <qpeng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/13 01:10:34 by qpeng             #+#    #+#             */
/*   Updated: 2019/05/13 02:01:29 by qpeng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <stdlib.h>
#include <inttypes.h> // portable integer types

// macros
#define GET_BIT(X,N) (((X) >> (N)) & 1)
#define SET_BIT(X,N) ((X) | (1 << (N)))
#define RST_BIT(X,N) ((X) & ~(1 << (N)))

// consts
#define INITIAL_TTL 1
#define MAX_TTL 30
#define DEFAULT_PROBE 3
#define DEFAULT_WAIT 5
#define DEFAULT_PORT_NUM 33434
#define NUM_OF(x) (sizeof (x) / sizeof (*x))

enum    e_flags
{
    F_FLAGS = 0b00000001,
    M_FLAGS = 0b00000010,
    P_FLAGS = 0b00000100,
    S_FLAGS = 0b00001000,
    W_FLAGS = 0b00010000
};

/**
 *  global variable for storing all the flags info.
 *  0 = not used
 *  |0|0|0|w|S|p|m|f|
 *   8 7 6 5 4 3 2 1
 */
extern uint8_t g_flags;

extern uint8_t g_inital_ttl;
extern uint8_t g_max_ttl;
extern uint8_t g_probes;
extern uint8_t g_waittime;
extern uint16_t g_port;
extern char * g_hostname;
#endif