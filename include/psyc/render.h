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

/*
typedef enum
{
	PSYC_RENDER_CHECK_LENGTH = 0,
	PSYC_RENDER_NEED_LENGTH = 1,
	PSYC_RENDER_NO_LENGTH = 2,
	PSYC_RENDER_ROUTING = 3,
} psycRenderFlag;
*/

/**
 * Struct for keeping render state.
 */
/*
typedef struct
{
	psycRenderFlag flag; ///< flags for the renderer
	psycPart part; ///< part of the packet being rendered
	size_t cursor; ///< current position in buffer
	size_t spot; ///< space for rendered length between the headers
	size_t contentLength; ///< length of the content part
	size_t length; ///< how big is the buffer we allocated
	char buffer[]; ///< OMG a C99 feature! variable size buffer!
} psycRenderState;
*/
/**
 * Initiates the state struct.
 *
 * @param state Pointer to the state struct that should be initiated.
 */
/*
inline void psyc_initRenderState (psycRenderState* state);

int psyc_renderModifier(psycRenderState* render,
                      const char* name, size_t nlength,
                      const char* value, size_t vlength,
                      psycRenderFlag flags, char oper);

int psyc_renderBody(psycRenderState* render,
                    const char* method, size_t mlength,
                    const char* data, size_t dlength);
*/
#endif // PSYC_RENDER_H

/** @} */ // end of rendering group
