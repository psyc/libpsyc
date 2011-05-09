#ifndef PSYC_RENDER_H

#include <psyc/packet.h>

/**
 * @file psyc/render.h
 * @brief Interface for PSYC packet rendering.
 *
 * All rendering functions and the definitions they use are
 * defined in this file.
 */

/**
 * @defgroup render Rendering Functions
 *
 * This module contains all rendering functions.
 * @{
 */

/**
 * Return codes for psyc_render.
 */
typedef enum
{
	PSYC_RENDER_ERROR_METHOD_MISSING = -3, ///< method missing, but data present
	PSYC_RENDER_ERROR_MODIFIER_NAME_MISSING = -2, ///< modifier name missing
	PSYC_RENDER_ERROR = -1,
	PSYC_RENDER_SUCCESS = 0,
} psycRenderRC;

/**
 * Render a PSYC packet into a buffer.
 */
psycRenderRC psyc_render (psycPacket *packet, char *buffer, size_t buflen);

/**
 * Render a PSYC list into a buffer.
 */
psycRenderRC psyc_renderList (psycList *list, char *buffer, size_t buflen);

/** @} */ // end of renderer group

#define PSYC_RENDER_H
#endif
