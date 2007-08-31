%{
#include <stdio.h>
#include <stddef.h>
#include "sina_parser.tab.h"
#include "sina_symbols.h"
%}

%%

[0-9]+					yylval.number=atoi(yytext); return INTEGER;
'.'						yylval.number=yytext[1]; return INTEGER;
\(						return OPEN_PAREN;
\)						return CLOSE_PAREN;
\{						return OPEN_CURLY;
\}						return CLOSE_CURLY;
[a-zA-Z][-a-zA-Z0-9_]*	yylval.number=add_symbol(yytext); return SYMBOL;
:						return COLON;
\n						/* ignore end of line */
[ \t]+					/* ignore whitespace */
\/\*.*\*\/				/* ignore comments */
".*"					yylval.string=yytext; return STRING;
.						return (int) yytext[0];
%%
