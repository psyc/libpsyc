#include "syntax.h"

/**
 * Struct for keeping render state.
 */
typedef enum {
	PSYC_FINE = 0,
	PSYC_NEED_LENGTH = 1
} PSYC_RenderFlag;

typedef enum {
	PSYC_FLAG_ROUTING = 1
} PSYC_RenderHeaderFlag;

typedef struct {
	PSYC_RenderFlag flag; ///< flags for the renderer
	PSYC_Part part; ///< part of the packet being rendered
	size_t cursor; ///< current position in buffer
	size_t spot; ///< space for rendered length between the headers
	size_t contentLength; ///< length of the content part
	size_t length; ///< how big is the buffer we allocated
	char buffer[]; ///< OMG a C99 feature! variable size buffer!
} PSYC_RenderState;

int PSYC_renderHeader(PSYC_RenderState* render,
                      const char* name, size_t nlength,
                      const char* value, size_t vlength,
                      PSYC_RenderHeaderFlag flags, char modifier);

int PSYC_renderBody(PSYC_RenderState* render,
                    const char* method, size_t mlength,
                    const char* data, size_t dlength);

int PSYC_doneRender(PSYC_RenderState* render,
                    uint8_t** buf, size_t* written);
