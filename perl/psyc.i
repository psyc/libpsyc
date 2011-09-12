 %module PSYC
 %{
 /* Includes the headers in the wrapper code */
 #include "psyc.h"
 #include "psyc/packet.h"
 #include "psyc/parse.h"
 #include "psyc/render.h"
 #include "psyc/syntax.h"
 #include "psyc/text.h"
 %}

/* Parse the headers to generate wrappers */
%include "cdata.i"

%rename("%(regex:/^PSYC_(.*)/\\1/)s") "";
%rename("%(regex:/^psyc_?(.*)/\\1/)s") "";
%rename("%(regex:/^psyc_(.*)2$/\\1/)s") "";

%apply (char *STRING, size_t LENGTH) { (const char *buffer, size_t length) };
%apply (char *STRING, size_t LENGTH) { (const char *name, size_t len) };
%apply (char *STRING, size_t LENGTH) { (const char *value, size_t len) };

// initTextState
%apply (char *STRING, size_t LENGTH) { (char *template, size_t tlen) };
%apply (char *STRING, size_t LENGTH) { (char *buffer, size_t blen) };
// setTextBrackets
%apply (char *STRING, size_t LENGTH) { (char *open, size_t openlen) };
%apply (char *STRING, size_t LENGTH) { (char *close, size_t closelen) };

//%apply (char *STRING, size_t LENGTH) {  };

psycParseRC psyc_parse2 (psycParseState *state, char *oper,
												 char *name, size_t namelen,
												 char *value, size_t valuelen);


%include "psyc.h"
%include "psyc/packet.h"
%include "psyc/parse.h"
%include "psyc/render.h"
%include "psyc/syntax.h"
%include "psyc/text.h"
