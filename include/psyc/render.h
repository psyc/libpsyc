/**
 * Struct for keeping render state.
 */
typedef enum {
	PSYC_FINE = 0,
	PSYC_NEED_LENGTH = 1
} PSYC_RenderFlag;

typedef struct {
	PSYC_RenderFlag flag; ///< flags for the renderer
	PSYC_Part part; ///< part of the packet being rendered
	size_t cursor; ///< current position in buffer
	size_t start; ///< position where the rendered packet starts
	size_t contentLength; ///< length of the content part
	size_t length; ///< how big is the buffer we allocated
	uint8_t buffer[]; ///< OMG a C99 feature! variable size buffer!
} PSYC_RenderState;

int PSYC_renderHeader(PSYC_RenderState* render,
                      const uint8_t* name, const size_t nlength,
                      const uint8_t* value, const size_t vlength,
                      const uint8_t flags, const uint8_t modifier);

int PSYC_renderBody(PSYC_RenderState* render,
                    const uint8_t* method, const size_t mlength,
                    const uint8_t* data, const size_t dlength,
                    const uint8_t flags);

int PSYC_doneRender(PSYC_RenderState* render,
                    uint8_t** buf, size_t* written);
