options {
	language="Cpp";
}

class TyroParser extends Parser;
options {
	buildAST = true;	// uses CommonAST by default
}

expr
	:	mexpr (PLUS^ mexpr)* SEMI!
	;

mexpr
	:	atom (STAR^ atom)*
	;

atom:	INT
	;

class TyroLexer extends Lexer;

WS_	:	(' '
	|	'\t'
	|	'\n'
	|	'\r')
		{ _ttype = ANTLR_USE_NAMESPACE(antlr)Token::SKIP; }
	;

LPAREN:	'('
	;

RPAREN:	')'
	;

STAR:	'*'
	;

PLUS:	'+'
	;

SEMI:	';'
	;

protected
DIGIT
	:	'0'..'9'
	;

INT	:	(DIGIT)+
	;

class TyroTreeWalker extends TreeParser;

expr returns [float r]
{
	float a,b;
	r=0;
}
	:	#(PLUS a=expr b=expr)	{r = a+b;}
	|	#(STAR a=expr b=expr)	{r = a*b;}
	|	i:INT			{r = atof(i->getText().c_str());}
	;

