/**
 * @file psyc.h
 * @brief Main PSYC interface providing crucial functionality.
 */

/**
 * @mainpage PSYC Core Library
 *
 * @section intro_sec Introduction
 *
 * Welcome to the libpsyc documentation!
 *
 * @section install_sec Installation
 */
// * @subsection step1 Step 1: Opening the box

#ifndef PSYC_H
#define PSYC_H

#ifdef __cplusplus
extern "C"
{
#if 0                           /* keep Emacsens' auto-indent happy */
}
#endif
#endif

#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#define PSYC_VERSION 1
#define PSYC_EPOCH 1440444041 // 2015-08-24 21:20:41 CET (Monday)

#define PSYC_C2STR(str)  (PsycString) {sizeof(str)-1, str}
#define PSYC_C2STRI(str) {sizeof(str)-1, str}
#define PSYC_C2ARG(str)  str, sizeof(str)-1
#define PSYC_C2ARG1(str) str, sizeof(str)
#define PSYC_C2ARG2(str) sizeof(str)-1, str
#define PSYC_S2ARG(str)  (str).data, (str).length
#define PSYC_S2ARG2(str) (str).length, (str).data
#define PSYC_S2ARGP(str) (int)(str).length, (str).data

#define PSYC_NUM_ELEM(a) (sizeof(a) / sizeof(*(a)))

/// Boolean: true/false, yes/no.
typedef enum {
    PSYC_FALSE = 0,
    PSYC_TRUE = 1,
    PSYC_NO = 0,
    PSYC_YES = 1,
} PsycBool;

/// Return code: OK/error.
typedef enum {
    PSYC_OK = 1,
    PSYC_ERROR = -1,
} PsycRC;

/**
 * String struct.
 *
 * Contains pointer and length for a buffer.
 */
typedef struct {
    /// Length of the data pointed to by ptr
    size_t length;
    /// pointer to the data
    char *data;
} PsycString;

#define PSYC_STRING(data, len) (PsycString) {len, data}

#include "psyc/match.h"
#include "psyc/method.h"
#include "psyc/packet.h"
#include "psyc/variable.h"
#include "psyc/parse.h"
#include "psyc/render.h"
#include "psyc/text.h"
#include "psyc/uniform.h"

#if 0                           /* keep Emacsens' auto-indent happy */
{
#endif
#ifdef __cplusplus
}
#endif

#endif
