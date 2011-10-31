#include <stdint.h>

struct PsycParser;

/** @brief initialize a pstate struct
 *
 *  @param pstate pointer to an allocated
 *         PsycParser struct.
 */
void psyc_parse_state_init(struct PsycParser* pstate);


/** @brief parses a packet
 *
 * This function parses rawdata
 * and uses the callbacks in PsycParser
 * to communicate with the caller.
 *
 * First the header will be parsed,
 * after that the stateCallback
 * (with inEntity set to false)
 * will be called for each variable.
 * Then, routingCallback will be called
 * to find out if further parsing
 * is desired.
 * If it returns false, psyc_parse returns.
 * If it returns true, parsing continues
 * to the body.
 * After the entitystate has been parsed,
 * stateCallback will be called for each
 * variable, having inEntity set to true.
 * Finally, bodyCallback will be called,
 * containing the method, and its data.
 *
 * In case of an parsing error <to continue>
 *
 * @param data constant pointer to the 
 *        raw data that is to be processed.
 * @param length the amount of bytes to parse
 * @param pstate pointer to a preallocated 
 *        and initialized (psyc_parse_state_init)
 *        instance of the struct state
 *        
*/
void psyc_parse(const uint8_t* data, unsigned int length, 
                struct PsycParser* pstate);

/** @brief FlagMod */
enum PsycOperator
{ 
	// modifier operators
	ASSIGN = 0x02,
	AUGMENT = 0x04,
	DIMINISH = 0x08,
	SET = 0x10,
	QUERY = 0x20,
};

struct PsycParser
{     
	/** @brief Callback for the states
	 *
	 * This callback will be called to inform
	 * the caller about the states.
	 *
	 * It will be called once for each variable.
	 *
	 * @param pstate pointer to the ParserState 
	 *        struct for identification
	 * @param name not null terminated c-string, 
	 *        containing the name of the variable
	 * @param nlength the length of the variable name
	 * @param value not null terminated c-string,
	 *        containing the value of the variable
	 * @param vlength the length of the variable value
	 * @param modifers modifer of the variable (see Modifer)
	 * @param inEntity wether this variable is an entity 
	 *        variable(true) or a routing variable(false) */
	void (*stateCallback)(struct PsycParser* pstate,
	       const uint8_t *name, const unsigned int nlength,
	       const uint8_t *value, const unsigned int vlength,
	       enum PsycOperator oper, char inEntity);

	/** @brief gets called after the routing-header was parsed
	 *
	 * @return if 0, parser will continue to parse 
	 *         the content part and calls bodyCallback 
	 *         when finished, 
	 *         if not 0, parser will stop parsing and 
	 *         calls contentCallback */
	char (*routingCallback)(struct PsycParser* pstate);

	/** @brief Body callback, gets called when the body was parsed
	 * 
	 * @param pstate pointer to the ParserState struct
	 *        for identificiation
	 * @param method not null terminated c-string, 
	 *        containing the method name
	 * @param mlength the length of the methodname
	 * @param dlength the length of the data
	 * @param data not null terminated c-string,
	 *        containing the data section
	 * @param content not null terminated c-string
	 * @param clength length of the content string */
	void (*bodyCallback)(struct PsycParser* pstate,
	                 const uint8_t* method, unsigned int mlength,
	                 const uint8_t* data, unsigned int dlength,   
	                 const uint8_t* content, unsigned int clength);

	/** @brief Error callback, gets called to indicate 
	 *         an error and the start of an error packet
	 *          
	 * If there was an error while parsing the rawdata,
	 * instead of passing the packets data to the callbacks,
	 * an error packet will be passed back, describing the 
	 * the error in more detail.
	 * 
	 * On error, errorCallback will be called 
	 * to report the errortype (in the method),
	 * after that errorStateCallback will be 
	 * called to inform about more detailed facts
	 * of the error. 
	 *
	 * Any previous state or body callbacks become 
	 * invalid and have to be purged.*/
	void (*errorCallback)(struct PsycParser* pstate, 
	                 const uint8_t *method, unsigned int mlength);

	/** @brief error state callback
	 *
	 * The parameters are the same as for stateCallback.
	 * The callback will be called once for each
	 * state variable in the error report packet
	 */
	void (*errorStateCallback)(struct PsycParser* pstate,
	       const uint8_t *name, const unsigned int nlength,
	       const uint8_t *value, const unsigned int vlength,
	       enum PsycOperator oper);


	/*******************************************
	 *  The following variables and datatypes  *
	 *  are being used to remember the         * 
	 *  internal state. You should not         *
	 *  touch them.                            *
	 *******************************************/
	uint8_t glyph;
	unsigned int contpos; // position inside the content
	unsigned int mstart,mlength, // position and length of the method
	dstart,dlength;  // 

};


