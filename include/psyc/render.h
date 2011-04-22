#ifndef PSYC_RENDER_H
# define PSYC_RENDER_H

#include "syntax.h"

#define PSYC_FLAG_UNDEFINED     0
#define PSYC_FLAG_NOT_BINARY    1
#define PSYC_FLAG_YES_BINARY    2
#define PSYC_FLAG_CHECK_BINARY  3

typedef enum
{
	PSYC_RENDER_CHECK_LENGTH = 0,
	PSYC_RENDER_NEED_LENGTH = 1,
	PSYC_RENDER_NO_LENGTH = 2,
	PSYC_RENDER_ROUTING = 3,
} PSYC_RenderFlag;

typedef enum
{
	PSYC_RENDER_ERROR_ROUTING = -2,
	PSYC_RENDER_ERROR = -1,
	PSYC_RENDER_SUCCESS = 0,
} PSYC_RenderRC;

/**
 * Struct for keeping render state.
 */
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

/**
 * Initiates the state struct.
 *
 * @param state Pointer to the state struct that should be initiated.
 */
inline void PSYC_initRenderState (PSYC_RenderState* state);

int PSYC_renderVar(PSYC_RenderState* render,
                      const char* name, size_t nlength,
                      const char* value, size_t vlength,
                      PSYC_RenderFlag flags, char modifier);

int PSYC_renderBody(PSYC_RenderState* render,
                    const char* method, size_t mlength,
                    const char* data, size_t dlength);

#endif // PSYC_RENDER_H
