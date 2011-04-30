#ifndef PSYC_RENDER_H
# define PSYC_RENDER_H

/**
 * @file psyc/render.h
 * @brief Interface for PSYC packet rendering.
 *
 * All rendering functions and the definitions they use are
 * defined in this file.
 */

/**
 * @defgroup rendering Rendering Functions
 *
 * This module contains all rendering functions.
 * @{
 */

/**
 * Return codes for psyc_render.
 */
typedef enum
{
	//PSYC_RENDER_ERROR_ROUTING = -2,
	PSYC_RENDER_ERROR = -1,
	PSYC_RENDER_SUCCESS = 0,
} psycRenderRC;

/**
 * Render a PSYC packet into a buffer.
 */
psycRenderRC psyc_render(psycPacket *packet, char *buffer, size_t buflen);

/**
 * Render a PSYC list into a buffer.
 */
inline psycRenderRC psyc_renderList(psycList *list, char *buffer, size_t buflen);

#endif // PSYC_RENDER_H

/** @} */ // end of rendering group
