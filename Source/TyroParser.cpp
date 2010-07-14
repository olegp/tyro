/* $ANTLR 2.7.1: "Tyro.g" -> "TyroParser.cpp"$ */
#include "TyroParser.hpp"
#include "antlr/NoViableAltException.hpp"
#include "antlr/SemanticException.hpp"
#line 1 "Tyro.g"

#line 8 "TyroParser.cpp"
TyroParser::TyroParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
	setTokenNames(_tokenNames);
}

TyroParser::TyroParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,1)
{
	setTokenNames(_tokenNames);
}

TyroParser::TyroParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
	setTokenNames(_tokenNames);
}

TyroParser::TyroParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,1)
{
	setTokenNames(_tokenNames);
}

TyroParser::TyroParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,1)
{
	setTokenNames(_tokenNames);
}

void TyroParser::expr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST expr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		mexpr();
		astFactory.addASTChild(currentAST, ANTLR_USE_NAMESPACE(antlr)RefAST(returnAST));
		{
		for (;;) {
			if ((LA(1)==PLUS)) {
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp1_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp1_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, ANTLR_USE_NAMESPACE(antlr)RefAST(tmp1_AST));
				match(PLUS);
				mexpr();
				astFactory.addASTChild(currentAST, ANTLR_USE_NAMESPACE(antlr)RefAST(returnAST));
			}
			else {
				goto _loop3;
			}
			
		}
		_loop3:;
		}
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp2_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp2_AST = astFactory.create(LT(1));
		match(SEMI);
		expr_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_0);
	}
	returnAST = expr_AST;
}

void TyroParser::mexpr() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST mexpr_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		atom();
		astFactory.addASTChild(currentAST, ANTLR_USE_NAMESPACE(antlr)RefAST(returnAST));
		{
		for (;;) {
			if ((LA(1)==STAR)) {
				ANTLR_USE_NAMESPACE(antlr)RefAST tmp3_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
				tmp3_AST = astFactory.create(LT(1));
				astFactory.makeASTRoot(currentAST, ANTLR_USE_NAMESPACE(antlr)RefAST(tmp3_AST));
				match(STAR);
				atom();
				astFactory.addASTChild(currentAST, ANTLR_USE_NAMESPACE(antlr)RefAST(returnAST));
			}
			else {
				goto _loop6;
			}
			
		}
		_loop6:;
		}
		mexpr_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_1);
	}
	returnAST = mexpr_AST;
}

void TyroParser::atom() {
	returnAST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	ANTLR_USE_NAMESPACE(antlr)ASTPair currentAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST atom_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
	
	try {      // for error handling
		ANTLR_USE_NAMESPACE(antlr)RefAST tmp4_AST = ANTLR_USE_NAMESPACE(antlr)nullAST;
		tmp4_AST = astFactory.create(LT(1));
		astFactory.addASTChild(currentAST, ANTLR_USE_NAMESPACE(antlr)RefAST(tmp4_AST));
		match(INT);
		atom_AST = ANTLR_USE_NAMESPACE(antlr)RefAST(currentAST.root);
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_2);
	}
	returnAST = atom_AST;
}

const char* TyroParser::_tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	"PLUS",
	"SEMI",
	"STAR",
	"INT",
	"WS_",
	"LPAREN",
	"RPAREN",
	"DIGIT",
	0
};

const unsigned long TyroParser::_tokenSet_0_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet TyroParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long TyroParser::_tokenSet_1_data_[] = { 48UL, 0UL, 0UL, 0UL };
// PLUS SEMI 
const ANTLR_USE_NAMESPACE(antlr)BitSet TyroParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long TyroParser::_tokenSet_2_data_[] = { 112UL, 0UL, 0UL, 0UL };
// PLUS SEMI STAR 
const ANTLR_USE_NAMESPACE(antlr)BitSet TyroParser::_tokenSet_2(_tokenSet_2_data_,4);


