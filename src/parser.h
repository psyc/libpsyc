#include <stdint.h>

/** @brief parses a routerVariable
 * 
 *  This function parses one routing variable,
 *  advances the cursor after the variable,
 *  writes the variables name, value and their
 *  lengths in the corresponding out parameters
 *  and returns 0 or an errorcode.
 *
 *  
 *  
 *
 *  @param data pointer to the packet data
 *  @param dlength length of the data (amount of bytes)
 *  @param cursor pointer to the current parsing position
 *  @param name pointer-pointer, used to return the position
 *         of the name string
 *  @param nlength pointer to which the length of 
 *         the name string will be written
 *  @param value pointer-pointer, used to retrun the position
 *         of the value string
 *  @param vlength pointer to which the length of 
 *         the value string will be written 
 *
 *  @returns 0 on success
 *           1 on insufficient data.
 *             This does not advance
 *             the cursor.             
 *           2 when no longer in the header,
 *             This advances the cursor to the 
 *             body/entity section, but leaves 
 *             the other out parameters invalid.
 *           3 the packet is complete.
 *           >2 on a context error,
 *           <0 on a parsing error.
 *              This invalidates all but the cursor
 *              out paramater. */
int PSYC_parseHeader(
									unsigned int* cursor,
                  const uint8_t * data, unsigned int dlength,
                  const uint8_t** name, unsigned int *nlength,
                  const uint8_t** value, unsigned int *vlength);

/** @brief parses one variable in two buffers
 *
 *  This function is nearly identical to its
 *  brother parseHeader. The difference is,
 *  it uses two buffers and return parameters
 *  for everything. It is meant to be used 
 *  in case parseHeader returned 2 for 
 *  insufficient data and you don’t
 *  like to copy memory around to 
 *  have all the data in one buffer.
 *  Using this function, you can pass two
 *  data buffers. The consequence is, 
 *  that name and value can be distributed
 *  on two different buffers and thus need
 *  also two out paramaters. If only one 
 *  will be used, length of the second 
 *  will be 0.
 *
 *  If your data is spread over more
 *  than two buffers, you need to
 *  copy that in one or two buffers.
 *  Given the unlikleyness of that
 *  event, we don't offer a three
 *  or more buffer function here.
 *
 */
int PSYC_parseHeader2(
									unsigned int* cursor,
                  const uint8_t * data1, unsigned int dlength1,
                  const uint8_t * data2, unsigned int dlength2,
                  const uint8_t** name1, unsigned int *nlength1,
                  const uint8_t** name2, unsigned int *nlength2,
							    const uint8_t** value1, unsigned int *vlength1,
 									const uint8_t** value2, unsigned int *vlength2);

/** @brief parses one bodyelement
 *
 *  This parses one body element, that is
 *  either an entity-variable or the method
 *
 *  The function assumes that dlength is set
 *  to the exact length of the packet
 *  so that data[dlength-1]  would be the
 *  ending "|" of the packet.
 *
 *  The parameters are nearly the same as for
 *  PSYC_routerVariable, only difference is
 *  that a returnvalue of 2 means, we encountered
 *  the method. 
 *  This means that the out paramterer
 *  name contains the methodname and 
 *  value the content.
 *  */
int PSYC_parseClosedBody(
									unsigned int* cursor,
                  const uint8_t * data, unsigned int dlength,
                  const uint8_t** name, unsigned int *nlength,
                  const uint8_t** value, unsigned int *vlength);

/** @brief parses one bodyelement
 *
 * This function is nearly identical to 
 * its brother parseClosedBody. *
 *
 * It assumes that we don’t know the
 * real length of the packet and thus
 * searches for the terminator.
 */
int PSYC_parseOpenBody(
									unsigned int* cursor,
                  const uint8_t * data, unsigned int dlength,
                  const uint8_t** name, unsigned int *nlength,
                  const uint8_t** value, unsigned int *vlength);



/* @brief parses an bodyelement in two buffers
 *
 * This function is the borther of parseHeader2.
 * It behaives the same as
 * parseOpenBody and parseHeader2. */
int PSYC_parseOpenBody2(
									unsigned int* cursor,
                  const uint8_t * data1, unsigned int dlength1,
                  const uint8_t * data2, unsigned int dlength2,
                  const uint8_t** name1, unsigned int *nlength1,
                  const uint8_t** name2, unsigned int *nlength2,
                  const uint8_t** value1, unsigned int *vlength1,
                  const uint8_t** value2, unsigned int *vlength2);




//#include <parser.c>
