#include "parser.h"

map<string, bool> defVar;
map<string, Token> SymTable;

namespace Parser {
    bool pushed_back = false;
    LexItem pushed_token;
    static LexItem GetNextToken(istream& in, int& line) {
        if (pushed_back) {
            pushed_back = false;
            return pushed_token;
        }
        return getNextToken(in, line);
    }

    static void PushBackToken(LexItem& t) {
        if (pushed_back) {
            abort();
        }
        pushed_back = true;
        pushed_token = t;
    }
}

static int error_count = 0;
int ErrCount() {
    return error_count;
}

void ParseError(int line, string msg) {
    ++error_count;
    cout << line << ": " << msg << endl;
}

bool PrintStmt(istream& in, int& line) {
    LexItem t;
    t = Parser::GetNextToken(in, line);
    if (t != DEF) {
        ParseError(line, "Print statement syntax error.");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t != COMMA) {
        ParseError(line, "Missing Comma.");
        return false;
    }
    bool ex = ExprList(in, line);
    if (!ex) {
        ParseError(line, "Missing expressipon after writeln");
        return false;
    }
}

bool ExprList(istream& in, int& line) {
	bool status = false;
	
	status = Expr(in, line);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		status = ExprList(in, line);
	}
    else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}

bool Var(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t == IDENT) {
        defVar[t.GetLexeme()] = true;
        return true;
    }
    ParseError(line, "Incorrect Declaration in Program");
    return false;
}

bool SFactor(istream& in, int& line) {
    int sign = 1;
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() == MINUS) sign = -1;
    else if (t.GetToken() == PLUS) sign = 1;
    else Parser::PushBackToken(t);
    return Factor(in, line, sign);
}

bool Factor(istream& in, int& line, int sign) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t != IDENT && t != ICONST && t != RCONST && t != SCONST) {
        if (t.GetToken() != LPAREN) {
            ParseError(line, "Missing Left Parenthesis");
            return false;
        }

        if (Expr(in, line)) {
            ParseError(line, "Missing Expression");
            return false;
        }

        if (t.GetToken() != RPAREN) {
            ParseError(line, "Missing Right Parenthesis");
            return false;
        }
    }
    else {
        if (!(defVar.find(t.GetLexeme())->second) || defVar[t.GetLexeme()] == false) {
            ParseError(line, "Undeclared Variable");
            return false;
        }
    }
    return true;
}