#include <psyc.h>
#include <stdint.h>

/// Routing variables in alphabetical order.
const psycString PSYC_routingVars[] =
{
	PSYC_C2STR("_amount_fragments"),
	PSYC_C2STR("_context"),
	//PSYC_C2STR("_count"),			// older PSYC
	PSYC_C2STR("_counter"), // the name for this is supposed to be _count, not _counter
	PSYC_C2STR("_fragment"),
	//PSYC_C2STR("_length"),		// older PSYC
	PSYC_C2STR("_source"),
	PSYC_C2STR("_source_identification"),
	PSYC_C2STR("_source_relay"),
	PSYC_C2STR("_source_relay_relay"), // until you have a better idea.. is this really in use?
	PSYC_C2STR("_tag"),
	PSYC_C2STR("_tag_relay"),
	//PSYC_C2STR("_tag_reply"),		// older PSYC
	PSYC_C2STR("_target"),
	PSYC_C2STR("_target_forward"),
	PSYC_C2STR("_target_relay"),
	//PSYC_C2STR(19, "_understand_modules"),    // older PSYC
	//PSYC_C2STR(14, "_using_modules"),	   	// older PSYC
};

const size_t PSYC_routingVarsNum = sizeof(PSYC_routingVars) / sizeof(*PSYC_routingVars);

/**
 * Get the type of variable name.
 */
psycBool psyc_isRoutingVar(const char *name, size_t len)
{
	size_t cursor = 1;
	int8_t matching[PSYC_routingVarsNum]; // indexes of matching vars
	memset(&matching, -1, sizeof(matching));
	uint8_t i, m = 0;

	if (len < 2 || name[0] != '_')
		return PSYC_FALSE;

	// first find the vars with matching length
	for (i=0; i<PSYC_routingVarsNum; i++)
		if (len == PSYC_routingVars[i].length)
			matching[m++] = i;

	while (cursor < len && matching[0] >= 0)
	{
		for (i = m = 0; i < PSYC_routingVarsNum; i++)
		{
			if (matching[i] < 0)
				break;
			if (PSYC_routingVars[matching[i]].ptr[cursor] == name[cursor])
				matching[m++] = matching[i]; // found a match, update matching indexes
			else if (PSYC_routingVars[matching[i]].ptr[cursor] > name[cursor])
				break; // passed the possible matches in alphabetical order
		}

		if (m < PSYC_routingVarsNum)
			matching[m] = -1; // mark the end of matching indexes

		cursor++;
	}

	return matching[0] >= 0 ? PSYC_TRUE : PSYC_FALSE;
}

/**
 * Get the type of variable name.
 */
psycType psyc_getVarType(char *name, size_t len)
{
	return PSYC_TYPE_UNKNOWN;
}
