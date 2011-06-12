module psyc.render;

import psyc.packet;


extern (C):

 /*
 * All rendering functions and the definitions they use are defined here.
 */

/**
 * Return codes for psyc_render.
 */
enum RenderRC
{
	/// Error, method is missing, but data is present.
	ERROR_METHOD_MISSING = -3,
	/// Error, a modifier name is missing.
	ERROR_MODIFIER_NAME_MISSING = -2,
	/// Error, buffer is too small to render the packet.
	ERROR = -1,
	/// Packet is rendered successfully in the buffer.
	SUCCESS = 0,
} ;

/**
 * Return codes for psyc_renderList.
 */
enum RenderListRC
{
	/// Error, buffer is too small to render the list.
	ERROR = -1,
	/// List is rendered successfully in the buffer.
	SUCCESS = 0,
};

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
RenderRC psyc_render (Packet *packet, char *buffer, size_t buflen);

/**
 * Render a PSYC list into a buffer.
 */
RenderListRC psyc_renderList (List *list, char *buffer, size_t buflen);

