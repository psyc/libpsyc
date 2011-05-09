#ifndef PSYC_RENDER_H

#include <psyc/packet.h>

/**
 * @file psyc/render.h
 * @brief Interface for PSYC packet rendering.
 *
 * All rendering functions and the definitions they use are defined here.
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
	/// Error, method is missing, but data is present.
	PSYC_RENDER_ERROR_METHOD_MISSING = -3,
	/// Error, a modifier name is missing.
	PSYC_RENDER_ERROR_MODIFIER_NAME_MISSING = -2,
	/// Error, buffer is too small to render the packet.
	PSYC_RENDER_ERROR = -1,
	/// Packet is rendered successfully in the buffer.
	PSYC_RENDER_SUCCESS = 0,
} psycRenderRC;

/**
 * Return codes for psyc_renderList.
 */
typedef enum
{
	/// Error, buffer is too small to render the list.
	PSYC_RENDER_LIST_ERROR = -1,
	/// List is rendered successfully in the buffer.
	PSYC_RENDER_LIST_SUCCESS = 0,
} psycRenderListRC;

/**
 * Render a PSYC packet into a buffer.
 *
 * The packet structure should contain the packet parts, either routing, entity,
 * method & data, or routing & content when rendering raw content.
 * It should also contain the contentLength & total length of the packet,
 * you can use psyc_setPacketLength() for calculating & setting these values.
 * This function renders packet->length bytes to the buffer,
 * if buflen is less than that an error is returned.
 *
 * @see psyc_newPacket
 * @see psyc_newPacket2
 * @see psyc_newRawPacket
 * @see psyc_newRawPacket2
 * @see psyc_setPacketLength
 */
psycRenderRC psyc_render (psycPacket *packet, char *buffer, size_t buflen);

/**
 * Render a PSYC list into a buffer.
 */
psycRenderListRC psyc_renderList (psycList *list, char *buffer, size_t buflen);

/** @} */ // end of render group

#define PSYC_RENDER_H
#endif
