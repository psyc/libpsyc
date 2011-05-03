/**
 * The return value definitions for the PSYC text parsing function.
 * @see psyc_text()
 */

typedef enum
{
	PSYC_TEXT_NO_SUBST = -1,
	PSYC_TEXT_COMPLETE = 0,
	PSYC_TEXT_INCOMPLETE = 1,
} psycTextRC;

typedef enum
{
	PSYC_TEXT_VALUE_NOT_FOUND = -1,
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
 * if found writes its current value from its variable store into the
 * outgoing buffer.. "Apple" for example. The template returns the
 * number of bytes written. 0 is a legal return value. Should the
 * callback return -1, psyc_text leaves the original template text as is.
 */
typedef psycTextValueRC (*psycTextCB)(char *name, size_t len, psycString *value);

inline void psyc_initTextState (psycTextState *state,
                                char *template, size_t tlen,
                                char *buffer, size_t blen);

inline void psyc_initTextState2 (psycTextState* state,
                                 char *template, size_t tlen,
                                 char *buffer, size_t blen,
                                 char *open, size_t openlen,
                                 char *close, size_t closelen);

inline void psyc_setTextBuffer (psycTextState *state, psycString buffer);

inline void psyc_setTextBuffer2 (psycTextState *state, char *buffer, size_t length);

/**
 * Fills out text templates by asking a callback for content.
 *
 * Copies the contents of the template into the buffer while looking
 * for braceOpen and braceClose strings and calling the callback for
 * each enclosed string between these braces. Should the callback
 * return -1, the original template text is copied as is.
 *
 * By default PSYC's "[" and "]" are used but you can provide any other
 * brace strings such as "${" and "}" or "<!--" and "-->".
 *
 * See also http://about.psyc.eu/psyctext
 */

psycTextRC psyc_text(psycTextState *state, psycTextCB getValue);
