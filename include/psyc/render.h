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
 * Return codes for PSYC_render.
 */
typedef enum
{
	//PSYC_RENDER_ERROR_ROUTING = -2,
	PSYC_RENDER_ERROR = -1,
	PSYC_RENDER_SUCCESS = 0,
} PSYC_RenderRC;

/**
 * Render a PSYC packet into a buffer.
 */
PSYC_RenderRC PSYC_render(PSYC_Packet *packet, char *buffer, size_t buflen);

/*
typedef enum
{
	PSYC_RENDER_CHECK_LENGTH = 0,
	PSYC_RENDER_NEED_LENGTH = 1,
	PSYC_RENDER_NO_LENGTH = 2,
	PSYC_RENDER_ROUTING = 3,
} PSYC_RenderFlag;
*/

/**
 * Struct for keeping render state.
 */
/*
typedef struct
{
	PSYC_RenderFlag flag; ///< flags for the renderer
	PSYC_Part part; ///< part of the packet being rendered
	size_t cursor; ///< current position in buffer
	size_t spot; ///< space for rendered length between the headers
	size_t contentLength; ///< length of the content part
	size_t length; ///< how big is the buffer we allocated
	char buffer[]; ///< OMG a C99 feature! variable size buffer!
} PSYC_RenderState;
*/
/**
 * Initiates the state struct.
 *
 * @param state Pointer to the state struct that should be initiated.
 */
/*
inline void PSYC_initRenderState (PSYC_RenderState* state);

int PSYC_renderModifier(PSYC_RenderState* render,
                      const char* name, size_t nlength,
                      const char* value, size_t vlength,
                      PSYC_RenderFlag flags, char oper);

int PSYC_renderBody(PSYC_RenderState* render,
                    const char* method, size_t mlength,
                    const char* data, size_t dlength);
*/
#endif // PSYC_RENDER_H

/** @} */ // end of rendering group
