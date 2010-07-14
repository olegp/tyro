/* $ANTLR 2.7.1: "Tyro.g" -> "TyroTreeWalker.cpp"$ */
#include "TyroTreeWalker.hpp"
#include "antlr/Token.hpp"
#include "antlr/AST.hpp"
#include "antlr/NoViableAltException.hpp"
#include "antlr/MismatchedTokenException.hpp"
#include "antlr/SemanticException.hpp"
#include "antlr/BitSet.hpp"
#line 1 "Tyro.g"

#line 12 "TyroTreeWalker.cpp"
TyroTreeWalker::TyroTreeWalker()
	: ANTLR_USE_NAMESPACE(antlr)TreeParser() {
	setTokenNames(_tokenNames);
}

float  TyroTreeWalker::expr(ANTLR_USE_NAMESPACE(antlr)RefAST _t) {
#line 55 "Tyro.g"
	float r;
#line 21 "TyroTreeWalker.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefAST expr_AST_in = _t;
	ANTLR_USE_NAMESPACE(antlr)RefAST i = ANTLR_USE_NAMESPACE(antlr)nullAST;
#line 55 "Tyro.g"
	
		float a,b;
		r=0;
	
#line 29 "TyroTreeWalker.cpp"
	
	try {      // for error handling
		if (_t == ANTLR_USE_NAMESPACE(antlr)nullAST )
			_t = ASTNULL;
		switch ( _t->getType()) {
		case PLUS:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t20 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp5_AST_in = _t;
			match(_t,PLUS);
			_t = _t->getFirstChild();
			a=expr(_t);
			_t = _retTree;
			b=expr(_t);
			_t = _retTree;
			_t = __t20;
			_t = _t->getNextSibling();
#line 60 "Tyro.g"
			r = a+b;
#line 49 "TyroTreeWalker.cpp"
			break;
		}
		case STAR:
		{
			ANTLR_USE_NAMESPACE(antlr)RefAST __t21 = _t;
			ANTLR_USE_NAMESPACE(antlr)RefAST tmp6_AST_in = _t;
			match(_t,STAR);
			_t = _t->getFirstChild();
			a=expr(_t);
			_t = _retTree;
			b=expr(_t);
			_t = _retTree;
			_t = __t21;
			_t = _t->getNextSibling();
#line 61 "Tyro.g"
			r = a*b;
#line 66 "TyroTreeWalker.cpp"
			break;
		}
		case INT:
		{
			i = _t;
			match(_t,INT);
			_t = _t->getNextSibling();
#line 62 "Tyro.g"
			r = atof(i->getText().c_str());
#line 76 "TyroTreeWalker.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(_t);
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		if ( _t != ANTLR_USE_NAMESPACE(antlr)nullAST )
			_t = _t->getNextSibling();
	}
	_retTree = _t;
	return r;
}

const char* TyroTreeWalker::_tokenNames[] = {
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



