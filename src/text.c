#include "lib.h"
#include <psyc/text.h>

const PsycTemplates psyc_templates = { .s = {
#include "templates.h"
}};

PsycTextRC
psyc_text (PsycTextState *state, PsycTextCB get_value, void *get_value_cls)
{
    const char *start = state->tmpl.data, *end; // start & end of variable name
    const char *prev = state->tmpl.data + state->cursor;
    PsycString value;
    int ret;
    size_t len;
    uint8_t no_subst = (state->cursor == 0); // whether we can return NO_SUBST

    while (state->cursor < state->tmpl.length) {
	start = memmem(state->tmpl.data + state->cursor,
		       state->tmpl.length - state->cursor,
		       state->open.data, state->open.length);
	if (!start)
	    break;

	state->cursor = (start - state->tmpl.data) + state->open.length;
	if (state->cursor >= state->tmpl.length)
	    break; // [ at the end

	end = memmem(state->tmpl.data + state->cursor,
		     state->tmpl.length - state->cursor,
		     state->close.data, state->close.length);
	state->cursor = (end - state->tmpl.data) + state->close.length;

	if (!end)
	    break; // ] not found
	if (start + state->open.length == end) {
	    state->cursor += state->close.length;
	    continue; // [] is invalid, name can't be empty
	}

	ret = get_value(get_value_cls, start + state->open.length,
			end - start - state->open.length, &value);

	if (ret < 0)
	    continue; // value not found, no substitution

	// First copy the part in the input from the previous subst.
	// to the current one, if there's enough buffer space for that.
	len = start - prev;
	if (state->written + len > state->buffer.length) {
	    state->cursor = prev - state->tmpl.data;
	    return PSYC_TEXT_INCOMPLETE;
	}

	memcpy((void *) (state->buffer.data + state->written), prev, len);
	state->written += len;

	// Now substitute the value if there's enough buffer space.
	if (state->written + value.length > state->buffer.length) {
	    state->cursor = start - state->tmpl.data;
	    return PSYC_TEXT_INCOMPLETE;
	}

	memcpy((void *) (state->buffer.data + state->written), value.data,
	       value.length);
	state->written += value.length;

	// Mark the start of the next chunk of text in the template.
	prev = state->tmpl.data + state->cursor;
	no_subst = 0;
    }

    if (no_subst)
	return PSYC_TEXT_NO_SUBST;

    // Copy the rest of the template after the last var.
    len = state->tmpl.length - (prev - state->tmpl.data);
    if (state->written + len > state->buffer.length)
	return PSYC_TEXT_INCOMPLETE;

    memcpy((void *) (state->buffer.data + state->written), prev, len);
    state->written += len;

    return PSYC_TEXT_COMPLETE;
}
