#include <psyc/lib.h>
#include <psyc/text.h>

inline void psyc_initTextState (psycTextState *state,
                                char *template, size_t tlen,
                                char *buffer, size_t blen)
{
	state->cursor = state->written = 0;
	state->template = psyc_newString(template, tlen);
	state->buffer = psyc_newString(buffer, blen);
	state->open = psyc_newString("[", 1);
	state->close = psyc_newString("]", 1);
}

inline void psyc_initTextState2 (psycTextState* state,
                                 char *template, size_t tlen,
                                 char *buffer, size_t blen,
                                 char *open, size_t openlen,
                                 char *close, size_t closelen)
{
	state->template = psyc_newString(template, tlen);
	state->buffer = psyc_newString(buffer, blen);
	state->open = psyc_newString(open, openlen);
	state->close = psyc_newString(close, closelen);
}

inline void psyc_setTextBuffer (psycTextState* state, psycString buffer)
{
	state->buffer = buffer;
	state->written = 0;
}

inline void psyc_setTextBuffer2 (psycTextState* state, char *buffer, size_t length)
{
	psyc_setTextBuffer(state, psyc_newString(buffer, length));
}

psycTextRC psyc_text (psycTextState *state, psycTextCB getValue)
{
	char *start = state->template.ptr, *end; // start & end of variable name
	char *prev = state->template.ptr + state->cursor;
	psycString value;
	int ret;
	size_t len;
	uint8_t no_subst = (state->cursor == 0); // whether we can return NO_SUBST

	while (state->cursor < state->template.length)
	{
		start = memmem(state->template.ptr + state->cursor,
		               state->template.length - state->cursor,
		               state->open.ptr, state->open.length);
		if (!start)
			break;

		state->cursor = (start - state->template.ptr) + state->open.length;
		if (state->cursor >= state->template.length)
			break; // [ at the end

		end = memmem(state->template.ptr + state->cursor,
		             state->template.length - state->cursor,
		             state->close.ptr, state->close.length);
		state->cursor = (end - state->template.ptr) + state->close.length;

		if (!end)
			break; // ] not found
		if (start + state->open.length == end)
		{
			state->cursor += state->close.length;
			continue; // [] is invalid, name can't be empty
		}

		ret = getValue(start + state->open.length, end - start - state->open.length, &value);

		if (ret < 0)
			continue; // value not found, no substitution

		// first copy the part in the template from the previous subst. to the current one
		// if there's enough buffer space for that
		len = start - prev;
		if (state->written + len > state->buffer.length)
		{
			state->cursor = prev - state->template.ptr;
			return PSYC_TEXT_INCOMPLETE;
		}

		memcpy((void *)(state->buffer.ptr + state->written), prev, len);
		state->written += len;

		// now substitute the value if there's enough buffer space
		if (state->written + value.length > state->buffer.length)
		{
			state->cursor = start - state->template.ptr;
			return PSYC_TEXT_INCOMPLETE;
		}

		memcpy((void *)(state->buffer.ptr + state->written), value.ptr, value.length);
		state->written += value.length;

		// mark the start of the next chunk of text in the template
		prev = state->template.ptr + state->cursor;
		no_subst = 0;
	}

	if (no_subst)
		return PSYC_TEXT_NO_SUBST;

	// copy the rest of the template after the last var
	len = state->template.length - (prev - state->template.ptr);
	if (state->written + len > state->buffer.length)
		return PSYC_TEXT_INCOMPLETE;

	memcpy((void *)(state->buffer.ptr + state->written), prev, len);
	state->written += len;

	return PSYC_TEXT_COMPLETE;
}
