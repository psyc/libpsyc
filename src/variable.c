#include <psyc.h>
#include <stdint.h>

const PSYC_Array PSYC_routingVars[] =
{
	{17, "_amount_fragments"},
	{ 8, "_context"},
	{ 6, "_count"},
	{ 8, "_counter"}, // the name for this is supposed to be _count, not _counter
	{ 9, "_fragment"},
	{ 7, "_length"},
	{ 7, "_source"},
	{22, "_source_identification"},
	{13, "_source_relay"},
	{19, "_source_relay_relay"}, // until you have a better idea.. is this really in use?
	{ 4, "_tag"},
	{10, "_tag_relay"},
	{10, "_tag_reply"}, // should be obsolete, but.. TODO
	{ 7, "_target"},
	{15, "_target_forward"},
	{13, "_target_relay"},
	{19, "_understand_modules"},
	{14, "_using_modules"},
};

const size_t PSYC_routingVarsNum = sizeof(PSYC_routingVars) / sizeof(*PSYC_routingVars);

/**
 * Get the type of variable name.
 */
PSYC_Bool PSYC_isRoutingVar(const char* name, size_t len)
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
PSYC_Type PSYC_getVarType(char* name, size_t len)
{
	return PSYC_TYPE_UNKNOWN;
}
