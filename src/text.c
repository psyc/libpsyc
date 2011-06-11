#include "lib.h"
#include <psyc/text.h>

psycTextRC psyc_text (psycTextState *state, psycTextCB getValue)
{
	const char *start = state->tmpl.ptr, *end; // start & end of variable name
	const char *prev = state->tmpl.ptr + state->cursor;
	psycString value;
	int ret;
	size_t len;
	uint8_t no_subst = (state->cursor == 0); // whether we can return NO_SUBST

	while (state->cursor < state->tmpl.length)
	{
		start = memmem(state->tmpl.ptr + state->cursor,
		               state->tmpl.length - state->cursor,
		               state->open.ptr, state->open.length);
		if (!start)
			break;

		state->cursor = (start - state->tmpl.ptr) + state->open.length;
		if (state->cursor >= state->tmpl.length)
			break; // [ at the end

		end = memmem(state->tmpl.ptr + state->cursor,
		             state->tmpl.length - state->cursor,
		             state->close.ptr, state->close.length);
		state->cursor = (end - state->tmpl.ptr) + state->close.length;

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

		// first copy the part in the input from the previous subst. to the current one
		// if there's enough buffer space for that
		len = start - prev;
		if (state->written + len > state->buffer.length)
		{
			state->cursor = prev - state->tmpl.ptr;
			return PSYC_TEXT_INCOMPLETE;
		}

		memcpy((void *)(state->buffer.ptr + state->written), prev, len);
		state->written += len;

		// now substitute the value if there's enough buffer space
		if (state->written + value.length > state->buffer.length)
		{
			state->cursor = start - state->tmpl.ptr;
			return PSYC_TEXT_INCOMPLETE;
		}

		memcpy((void *)(state->buffer.ptr + state->written), value.ptr, value.length);
		state->written += value.length;

		// mark the start of the next chunk of text in the template
		prev = state->tmpl.ptr + state->cursor;
		no_subst = 0;
	}

	if (no_subst)
		return PSYC_TEXT_NO_SUBST;

	// copy the rest of the template after the last var
	len = state->tmpl.length - (prev - state->tmpl.ptr);
	if (state->written + len > state->buffer.length)
		return PSYC_TEXT_INCOMPLETE;

	memcpy((void *)(state->buffer.ptr + state->written), prev, len);
	state->written += len;

	return PSYC_TEXT_COMPLETE;
}
