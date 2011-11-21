#include "lib.h"
#include <stdint.h>


/// Routing variables in alphabetical order.
const PsycString psyc_routing_vars[] = {
    PSYC_C2STRI("_amount_fragments"),
    PSYC_C2STRI("_context"),
    //PSYC_C2STRI("_count"), // older PSYC
    PSYC_C2STRI("_counter"), // the name for this is supposed to be _count, not _counter
    PSYC_C2STRI("_fragment"),
    //PSYC_C2STRI("_length"), // older PSYC
    PSYC_C2STRI("_source"),
    //PSYC_C2STRI("_source_identification"), // older PSYC
    PSYC_C2STRI("_source_identity"),
    PSYC_C2STRI("_source_relay"),
    // until you have a better idea.. is this really in use?
    PSYC_C2STRI("_source_relay_relay"),
    PSYC_C2STRI("_tag"),
    PSYC_C2STRI("_tag_relay"),
    //PSYC_C2STRI("_tag_reply"), // older PSYC
    PSYC_C2STRI("_target"),
    PSYC_C2STRI("_target_forward"),
    PSYC_C2STRI("_target_relay"),
    //PSYC_C2STRI("_understand_modules"), // older PSYC
    //PSYC_C2STRI("_using_modules"), // older PSYC
};

// Variable types in alphabetical order.
const PsycDictInt psyc_var_types[] = {
    {PSYC_C2STRI("_amount"), PSYC_TYPE_AMOUNT},
    {PSYC_C2STRI("_color"), PSYC_TYPE_COLOR},
    {PSYC_C2STRI("_date"), PSYC_TYPE_DATE},
    {PSYC_C2STRI("_degree"), PSYC_TYPE_DEGREE},
    {PSYC_C2STRI("_entity"), PSYC_TYPE_ENTITY},
    {PSYC_C2STRI("_flag"), PSYC_TYPE_FLAG},
    {PSYC_C2STRI("_language"), PSYC_TYPE_LANGUAGE},
    {PSYC_C2STRI("_list"), PSYC_TYPE_LIST},
    {PSYC_C2STRI("_nick"), PSYC_TYPE_NICK},
    {PSYC_C2STRI("_page"), PSYC_TYPE_PAGE},
    {PSYC_C2STRI("_table"), PSYC_TYPE_TABLE},
    {PSYC_C2STRI("_time"), PSYC_TYPE_TIME},
    {PSYC_C2STRI("_uniform"), PSYC_TYPE_UNIFORM},
};

const size_t psyc_routing_vars_num = PSYC_NUM_ELEM(psyc_routing_vars);
const size_t psyc_var_types_num = PSYC_NUM_ELEM(psyc_var_types);

/**
 * Get the type of variable name.
 */
inline PsycBool
psyc_var_is_routing (const char *name, size_t len)
{
    size_t cursor = 1;
    uint8_t i, m = 0;
    int8_t matching[psyc_routing_vars_num]; // indexes of matching vars

    if (len < 2 || name[0] != '_')
	return PSYC_FALSE;

    // first find the vars with matching length
    for (i = 0; i < psyc_routing_vars_num; i++)
	if (len == psyc_routing_vars[i].length)
	    matching[m++] = i;

    matching[m] = -1; // mark the end of matching indexes

    while (cursor < len && matching[0] >= 0) {
	for (i = m = 0; i < psyc_routing_vars_num; i++) {
	    if (matching[i] < 0)
		break; // reached the end of possible matches
	    if (psyc_routing_vars[matching[i]].data[cursor] == name[cursor])
		matching[m++] = matching[i]; // found a match, update matching indexes
	    else if (psyc_routing_vars[matching[i]].data[cursor] > name[cursor])
		break; // passed the possible matches in alphabetical order in the array
	}

	if (m < psyc_routing_vars_num)
	    matching[m] = -1; // mark the end of matching indexes

	cursor++;
    }

    return matching[0] >= 0 ? PSYC_TRUE : PSYC_FALSE;
}

/**
 * Get the type of variable name.
 */
inline PsycType
psyc_var_type (const char *name, size_t len)
{
    int8_t m[psyc_var_types_num];
    return (PsycType) psyc_dict_lookup((PsycDict *) psyc_var_types,
				       psyc_var_types_num, name, len, PSYC_YES,
				       (int8_t *) &m);
}
