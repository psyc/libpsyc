#ifndef PSYC_TEXT_H
#define PSYC_TEXT_H

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
typedef enum {
    /// No substitution was made, nothing was written to the buffer.
    PSYC_TEXT_NO_SUBST = -1,
    /// Text template parsing & rendering complete.
    PSYC_TEXT_COMPLETE = 0,
    /// Text template parsing & rendering is incomplete, because the buffer was too
    /// small. Another call is required to this function after setting a new buffer.
    PSYC_TEXT_INCOMPLETE = 1,
} PsycTextRC;

/**
 * Return values for PsycTextCB.
 */
typedef enum {
    /// Value not found, don't substitute anything.
    PSYC_TEXT_VALUE_NOT_FOUND = -1,
    /// Value found, substitute contents of the value variable.
    PSYC_TEXT_VALUE_FOUND = 0,
} PsycTextValueRC;

/**
 * Struct for keeping PSYC text parser state.
 */
typedef struct {
    size_t cursor;		///< current position in the template
    size_t written;		///< number of bytes written to buffer
    PsycString tmpl;		///< input buffer with text template to parse
    PsycString buffer;		///< output buffer for rendered text
    PsycString open;
    PsycString close;
} PsycTextState;

/**
 * Callback for psyc_text() that produces a value for a match.
 *
 * The application looks up a match such as _fruit from [_fruit] and
 * if found sets value->data & value->length to point to the found value,
 * "Apple" for example. 0 is a legal value for the length.
 * The callbacks returns either PSYC_TEXT_VALUE_FOUND or
 * PSYC_TEXT_VALUE_NOT_FOUND if no match found in which case psyc_text
 * leaves the original template text as is.
 */
typedef PsycTextValueRC (*PsycTextCB) (void *cls, const char *name, size_t namelen,
				       PsycString *value);

/**
 * Initializes the PSYC text state struct.
 *
 * @param state Pointer to the PSYC text state struct that should be initialized.
 * @param tmpl Input buffer with text template to be parsed.
 * @param tmplen Length of input buffer.
 * @param buffer Output buffer where the rendered text is going to be written.
 * @param buflen Length of output buffer.
 */
static inline void
psyc_text_state_init (PsycTextState *state,
		      char *tmpl, size_t tmplen,
		      char *buffer, size_t buflen)
{
    state->cursor = 0;
    state->written = 0;
    state->tmpl = (PsycString) {
    tmplen, tmpl};
    state->buffer = (PsycString) {
    buflen, buffer};
    state->open = (PsycString) {
    1, "["};
    state->close = (PsycString) {
    1, "]"};
}

/**
 * Initializes the PSYC text state struct with custom opening & closing braces.
 *
 * @param state Pointer to the PSYC text state struct that should be initialized.
 * @param tmpl Input buffer with text template to be parsed.
 * @param tmplen Length of input buffer.
 * @param buffer Output buffer where the rendered text is going to be written.
 * @param buflen Length of output buffer.
 * @param open Opening brace.
 * @param openlen Length of opening brace.
 * @param close Closing brace.
 * @param closelen Length of closing brace.
 */
static inline void
psyc_text_state_init_custom (PsycTextState *state,
			     char *tmpl, size_t tmplen,
			     char *buffer, size_t buflen,
			     char *open, size_t openlen,
			     char *close, size_t closelen)
{
    state->cursor = 0;
    state->written = 0;
    state->tmpl = (PsycString) {
    tmplen, tmpl};
    state->buffer = (PsycString) {
    buflen, buffer};
    state->open = (PsycString) {
    openlen, open};
    state->close = (PsycString) {
    closelen, close};
}

/**
 * Sets a new output buffer in the PSYC text state struct.
 */
static inline void
psyc_text_buffer_set (PsycTextState *state, char *buffer, size_t length)
{
    state->buffer = (PsycString) {
    length, buffer};
    state->written = 0;
}

static inline size_t
psyc_text_bytes_written (PsycTextState *state)
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
 * Before calling this function psyc_text_state_init should be called to initialize
 * the state struct. By default PSYC's "[" and "]" are used but you can provide
 * any other brace strings such as "${" and "}" or "<!--" and "-->" if you use
 * the psyc_text_state_init_custom variant.
 *
 * @see http://about.psyc.eu/psyctext
 **/
PsycTextRC
psyc_text (PsycTextState *state, PsycTextCB get_value, void *get_value_cls);

extern const PsycTemplates psyc_templates;

static inline const char *
psyc_template (PsycMethod mc, size_t *len) {
    PsycString t = psyc_templates.a[mc];
    if (len)
	*len = t.length;
    return t.data;
}

/** @} */ // end of text group

#endif
