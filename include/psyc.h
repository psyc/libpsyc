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

#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#define PSYC_VERSION 1
#define PSYC_EPOCH 1440444041 // 2015-08-24 21:20:41 CET (Monday)

#define PSYC_STRING(data, len) (PsycString) {len, data}
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

/// PSYC packet parts.
typedef enum {
    PSYC_PART_RESET = -1,
    PSYC_PART_ROUTING = 0,
    PSYC_PART_LENGTH = 1,
    PSYC_PART_CONTENT = 2,
    PSYC_PART_METHOD = 3,
    PSYC_PART_DATA = 4,
    PSYC_PART_END = 5,
} PsycPart;

/**
 * Different types that a variable can have.
 *
 * This enum lists PSYC variable types that
 * this library is capable of checking for
 * validity. Other variable types are treated
 * as opaque data.
 */
typedef enum {
    PSYC_TYPE_UNKNOWN,
    PSYC_TYPE_AMOUNT,
    PSYC_TYPE_COLOR,
    PSYC_TYPE_COUNTER,
    PSYC_TYPE_DEF,
    PSYC_TYPE_DATE,
    PSYC_TYPE_DEGREE,
    PSYC_TYPE_ENTITY,
    PSYC_TYPE_FLAG,
    PSYC_TYPE_LANGUAGE,
    PSYC_TYPE_LIST,
    PSYC_TYPE_NICK,
    PSYC_TYPE_PAGE,
    PSYC_TYPE_TABLE,
    PSYC_TYPE_TIME,
    PSYC_TYPE_UNIFORM,
} PsycType;

/**
 * List types.
 * Possible types are text and binary.
 */
typedef enum {
    PSYC_LIST_TEXT = 1,
    PSYC_LIST_BINARY = 2,
} PsycListType;

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

#include "psyc/syntax.h"
#include "psyc/match.h"
#include "psyc/method.h"
#include "psyc/packet.h"
#include "psyc/parse.h"
#include "psyc/render.h"
#include "psyc/text.h"
#include "psyc/uniform.h"
#include "psyc/variable.h"

#endif
