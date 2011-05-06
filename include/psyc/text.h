/**
 * @file psyc/text.h
 * @brief Interface for text template rendering.
 *
 * All text template functions and the definitions they use are
 * defined in this file.
 */

/**
 * @defgroup text Text template functions
 *
 * This module contains all text template functions.
 * @{
 */

/**
 * Return values for the text template parsing function.
 * @see psyc_text()
 */
typedef enum
{
	/// No substitution was made, nothing was written to the buffer.
	PSYC_TEXT_NO_SUBST = -1,
	/// Text template parsing & rendering complete.
	PSYC_TEXT_COMPLETE = 0,
	/// Text template parsing & rendering is incomplete, because the buffer was too small.
	/// Another call is required to this function with a new buffer.
	PSYC_TEXT_INCOMPLETE = 1,
} psycTextRC;

/**
 * Return values for psycTextCB.
 */
typedef enum
{
	/// Value not found, don't substitute anything.
	PSYC_TEXT_VALUE_NOT_FOUND = -1,
	/// Value found, substitute contents of the value variable.
	PSYC_TEXT_VALUE_FOUND = 0,
} psycTextValueRC;

/**
 * Struct for keeping PSYC text parser state.
 */
typedef struct
{
	size_t cursor; ///< current position in the template
	size_t written; ///< number of bytes written to buffer
	psycString template; ///< template to parse
	psycString buffer; ///< buffer for writing to
	psycString open;
	psycString close;
} psycTextState;

/**
 * Callback for psyc_text() that produces a value for a match.
 *
 * The application looks up a match such as _fruit from [_fruit] and
 * if found sets value->ptr & value->length to point to the found value,
 * "Apple" for example. 0 is a legal value for the length.
 * The callbacks returns either PSYC_TEXT_VALUE_FOUND or
 * PSYC_TEXT_VALUE_NOT_FOUND if no match found in which case psyc_text
 * leaves the original template text as is.
 */
typedef psycTextValueRC (*psycTextCB)(const char *name, size_t len, psycString *value);

/**
 * Initializes the PSYC text state struct.
 *
 * @param state Pointer to the PSYC text state struct that should be initialized.
 * @param template Text template to be parsed.
 * @param tlen Length of template.
 * @param buffer Buffer where the rendered text is going to be written.
 * @param blen Length of buffer.
 */
static inline
void psyc_initTextState (psycTextState *state,
                         char *template, size_t tlen,
                         char *buffer, size_t blen)
{
	state->cursor   = 0;
	state->written  = 0;
	state->template = (psycString) {tlen, template};
	state->buffer   = (psycString) {blen, buffer};
	state->open     = (psycString) {1, "["};
	state->close    = (psycString) {1, "]"};
}

/**
 * Initializes the PSYC text state struct with custom open & closing braces.
 *
 * @param state Pointer to the PSYC text state struct that should be initialized.
 * @param template Text template to be parsed.
 * @param tlen Length of template.
 * @param buffer Buffer where the rendered text is going to be written.
 * @param blen Length of buffer.
 * @param open Opening brace.
 * @param openlen Length of opening brace.
 * @param close Closing brace.
 * @param closelen Length of closing brace.
 */
static inline
void psyc_initTextState2 (psycTextState *state,
                          char *template, size_t tlen,
                          char *buffer, size_t blen,
                          char *open, size_t openlen,
                          char *close, size_t closelen)
{
	state->template = (psycString) {tlen, template};
	state->buffer   = (psycString) {blen, buffer};
	state->open     = (psycString) {openlen, open};
	state->close    = (psycString) {closelen, close};
}

/**
 * Sets a new buffer in the PSYC text state struct.
 */
static inline
void psyc_setTextBuffer (psycTextState *state, psycString buffer)
{
	state->buffer = buffer;
	state->written = 0;
}

/**
 * Sets a new buffer in the PSYC text state struct.
 */
static inline
void psyc_setTextBuffer2 (psycTextState *state,
                          char *buffer, size_t length)
{
	psycString buf = {length, buffer};
	psyc_setTextBuffer(state, buf);
}

static inline
size_t psyc_getTextBytesWritten (psycTextState *state)
{
	return state->written;
}

/**
 * Fills out text templates by asking a callback for content.
 *
 * Copies the contents of the template into the buffer while looking for the
 * opening and closing brace strings and calling the callback for each enclosed
 * string between these braces. Should the callback return
 * PSYC_TEXT_VALUE_NOT_FOUND, the original template text is copied as is.
 *
 * Before calling this function psyc_initTextState or psyc_initTextState should
 * be called to initialze the state struct. By default PSYC's "[" and "]" are
 * used but you can provide any other brace strings such as "${" and "}" or
 * "<!--" and "-->".
 *
 * @see http://about.psyc.eu/psyctext
 **/
psycTextRC psyc_text (psycTextState *state, psycTextCB getValue);

/** @} */ // end of text group
