%{
#include <stdio.h>
#include "sina_parser.tab.h"
%}

%%

[0-9]+					return INTEGER;
'.'						return INTEGER;
\(						return OPEN_PAREN;
\)						return CLOSE_PAREN;
\{						return OPEN_CURLY;
\}						return CLOSE_CURLY;
[a-zA-Z][-a-zA-Z0-9_]*	return SYMBOL;
:						return COLON;
\n						/* ignore end of line */
[ \t]+					/* ignore whitespace */
\/\*.*\*\/				/* ignore comments */
"([^"]|\\")*"			return STRING;
.						return (int) yytext[0];
%%
