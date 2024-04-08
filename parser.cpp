#include "parser.h"
#define DEBUG(x) { cout << #x << " = "; cout << (x) << endl; }

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
    return true;
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
    if (t.GetToken() == IDENT) {
        defVar[t.GetLexeme()] = true; //declaration
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
    if (!Factor(in, line, sign)) {
        ParseError(line, "Missing Factor in SFactor");
        return false;
    }
    return true;
}

bool Factor(istream& in, int& line, int sign) {
    cout << "Enter Factor: " << endl;
    LexItem t = Parser::GetNextToken(in, line);
    if (t != IDENT && t != ICONST && t != RCONST && t != SCONST) {
        if (t.GetToken() != LPAREN) {
            ParseError(line, "Missing Left Parenthesis");
            return false;
        }

        if (!Expr(in, line)) {
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
            cout << "Exit Factor " << endl;
            ParseError(line, "Undeclared Variable");
            return false;
        }
    }
    return true;
}

bool TermExpr(istream& in, int& line) {
    bool status = SFactor(in, line);
    if (!status) {
        ParseError(line, "Missing SFactor in TermExpr");
        return false;
    }
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() == POW) {
        status = TermExpr(in, line);
    }
    else if (t.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << t.GetLexeme() << ")" << endl;
    }
    else {
        Parser::PushBackToken(t);
        return true;
    }
    return status;
}

bool MultExpr(istream& in, int& line) {
    bool status = TermExpr(in, line);
    if (!status) {
        ParseError(line, "Missing TermExpr in MultExpr");
        return false;
    }
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() == MULT || t.GetToken() == DIV) {
        status = MultExpr(in, line);
    }
    else if (t.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << t.GetLexeme() << ")" << endl;
    }
    else {
        Parser::PushBackToken(t);
        return true;
    }
    return status;
}

bool Expr(istream& in, int& line) {
    bool status = MultExpr(in, line);
    if (!status) {
        ParseError(line, "Missing MultExpr in Expr");
        return false;
    }
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() == PLUS || t.GetToken() == MINUS || t.GetToken() == CAT) {
        status = Expr(in, line);
    }
    else if (t.GetToken() == ERR) {
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << t.GetLexeme() << ")" << endl;
    }
    else {
        Parser::PushBackToken(t);
        return true;
    }
    return status;
}

bool RelExpr(istream& in, int& line) {
    bool status = Expr(in, line);
    if (!status) {
        ParseError(line, "Missing Expr");
        false;
    }
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() == EQ || t.GetToken() == LTHAN || t.GetToken() == GTHAN) {
        if (!Expr(in, line)) {
            ParseError(line, "Missing Expr after EQ, LTHAN, GTHAN");
            return false;
        }
        else return true;
    }
    else Parser::PushBackToken(t);
    return true;
}

bool AssignStmt(istream& in, int& line) {
    bool status = Var(in, line);
    if (!status) {
        ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
        return false;
    }
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() != ASSOP) {
        ParseError(line, "Undeclared Variable");
        return false;
    }
    bool ex = Expr(in, line);
    if (!ex) {
        ParseError(line, "Missing Expr after ASSOP");
        return false;
    }
    return true;
}

bool SimpleStmt(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() == IDENT) {
        Parser::PushBackToken(t);
        if (!AssignStmt(in, line)) {
            ParseError(line, "Incorrect Assignment Statement in SimpleStmt.");
            return false;
        }
    }
    else if (t.GetToken() == PRINT) {
        if (!PrintStmt(in, line)) {
            ParseError(line, "Incorrect Print Statement in SimpleStmt.");
            return false;
        }
    }
    else {
        ParseError(line, "Invalid SimpleStmt");
        return false;
    }
    return true;
}

bool SimpleIfStmt(istream& in, int& line) {
    if (!SimpleStmt(in, line)) {
        ParseError(line, "Missign Simple Stmt");
        return false;
    }
    return true;
}

bool BlockIfStmt(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() != ELSE && t.GetToken() != END) {
        Parser::PushBackToken(t);
        if (!Stmt(in, line)) {
            ParseError(line, "Missing statement in THEN");
            return false;
        }
    }
    if (t.GetToken() == ELSE) {
        t = Parser::GetNextToken(in, line);
        if (t.GetToken() != END) {
            Parser::PushBackToken(t);
            if (!Stmt(in, line)) {
                ParseError(line, "Missing Stmt in the ELSE statement");
                return false;
            }
        }
    }
    if (t.GetToken() == END) {
        t = Parser::GetNextToken(in, line);
        if (t.GetToken() != IF) {
            ParseError(line, "Missing IF after END");
            return false;
        }
    }
    return true;
}

bool Stmt(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() == IDENT) {
        Parser::PushBackToken(t);
        if (!AssignStmt(in, line)) {
            ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
            return false;
        }
        return true;
    }
    else if (t.GetToken() == IF) {
        Parser::PushBackToken(t);
        if (!ConditionStmt(in, line)) {
            ParseError(line, "Incorrect If statement in Stmt");
            return false;
        }
        return true;
    }
    else if (t.GetToken() == PRINT){
        if (!PrintStmt(in, line)) {
            ParseError(line, "Invalid Print statement");
            return false;
        }
        return true;
    }
    return false;
}

bool VarList(istream& in, int& line) {
    bool status = false;
    if (!Var(in, line)) {
        ParseError(line, "Missing Variable Name");
        return false;
    }
    else status = true;
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() == ASSOP) {
        if (!Expr(in, line)) {
            ParseError(line, "Missing Expression after ASSOP");
            return false;
        }
        else {
            t = Parser::GetNextToken(in, line);
        }
    }
    else if (t.GetToken() == COMMA) {
        return VarList(in, line);
    } else {
        Parser::PushBackToken(t);
    }
    return status;
}


bool Type(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() == INTEGER || t.GetToken() == REAL) {
        return true;
    }
    else if (t.GetToken() == CHARACTER) {
        t = Parser::GetNextToken(in, line);
        if (t.GetToken() == LPAREN) {
            t = Parser::GetNextToken(in, line);
            if (t.GetToken() != LEN) {
                ParseError(line, "Missing LEN after LPAREN");
                return false;
            }
            t = Parser::GetNextToken(in, line);
            if (t.GetToken() != ASSOP) {
                ParseError(line, "Missing ASSOP after LEN");
                return false;
            }
            t = Parser::GetNextToken(in, line);
            if (t.GetToken() != ICONST) {
                ParseError(line, "Missing ICONST after ASSOP");
                return false;
            }
            t = Parser::GetNextToken(in, line);
            if (t.GetToken() != RPAREN) {
                ParseError(line, "Missing Right Parenthesis after ICONST");
                return false;
            }
        }
        else Parser::PushBackToken(t);
        return true;
    }
    return false;
}

// bool Decl(istream &in, int &line)
// {
// 	LexItem t{};

// 	bool parseType = Type(in, line);
// 	if (!parseType)
// 	{
// 		return false;
// 	}

// 	t = Parser::GetNextToken(in, line);
// 	if (t != DCOLON)
// 	{
// 		ParseError(line, "Invalid Token, in decl");
// 		return false;
// 	}

// 	return VarList(in, line);
// }

bool Decl(istream& in, int& line) {
    cout << "Enter Decl " << endl; 
    if (!Type(in, line)) {
        ParseError(line, "Missing Type");
        cout << "Exit Decl" << endl;
        return false;
    }
    LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() != DCOLON) {
        ParseError(line, "Missing Double Colons");
        cout << "Exit Decl" << endl;
        return false;
    }
    if (!VarList(in, line)) {
        ParseError(line, "No provide VarList");
        cout << "Exit Decl" << endl;
        return false;
    }
    cout << "Exit Decl" << endl;
    return true;
}

// bool Prog(istream& in, int& line) {
//     LexItem t = Parser::GetNextToken(in, line);
//     if (t.GetToken() != PROGRAM) {
//         ParseError(line, "Program error");
//         return false;
//     }
//     t = Parser::GetNextToken(in, line);
//     if (t.GetToken() != IDENT) {
//         ParseError(line, "Missing IDENT");
//         return false;
//     }
//     t = Parser::GetNextToken(in, line);
//     while (t.GetToken() == INTEGER || t.GetToken() == REAL || t.GetToken() == CHARACTER) {
//         Parser::PushBackToken(t);
//         if (!Decl(in, line)) {
//             ParseError(line, "Declaration error");
//             return false;
//         }
//     }    
//     while (t.GetToken() == IF || t.GetToken() == PRINT || t.GetToken() == IDENT)  {
//         if (!Stmt(in, line)) {
//             ParseError(line, "Statement error");
//             return false;
//         }
//     }
//     t = Parser::GetNextToken(in, line);
//     if (t.GetToken() == END) {
//         t = Parser::GetNextToken(in, line);
//         if (t.GetToken() != PROGRAM) {
//             ParseError(line, "Program end error PROGRAM");
//             return false;
//         }
//         t = Parser::GetNextToken(in, line);
//         if (t.GetToken() != IDENT) {
//             ParseError(line, "Program end error IDENT");
//             return false;
//         }
//         return true;
//     }
//     return false;
// }

bool Prog(istream &in, int &line)
{
	LexItem t{};
	bool status{false};

	t = Parser::GetNextToken(in, line);
	if (t != PROGRAM)
	{
		ParseError(line, "Invalid token, should be program");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if (t != IDENT)
	{
		ParseError(line, "Invalid token, should be ident 466");
		return false;
	}

	while (true)
	{
		t = Parser::GetNextToken(in, line);
		if (t == INTEGER || t == REAL || t == CHARACTER)
		{
			Parser::PushBackToken(t);
			status = Decl(in, line);
			if (!status)
			{
				ParseError(line, "Incorrect Declaration in Program");
				return false;
			}
		}
		else
		{
			Parser::PushBackToken(t);
			break;
		}
	}

	while (true)
	{
		t = Parser::GetNextToken(in, line);
        
		if (t == IDENT || t == IF || t == PRINT)
		{
			Parser::PushBackToken(t);
			status = Stmt(in, line);
			if (!status)
			{
				ParseError(line, "Incorrect Statement in program");
				return false;
			}
		}
		else
		{
			Parser::PushBackToken(t);
			break;
		}
	}

	t = Parser::GetNextToken(in, line);
	if (t != END)
	{
		ParseError(line, "Invalid token, should be ident 513");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != PROGRAM)
	{
		ParseError(line, "Invalid token, should be ident 520");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != IDENT)
	{
		ParseError(line, "Invalid token, should be ident 527");
		return false;
	}
    cout << "(DONE)" << endl;
	return true;
}


bool ConditionStmt(istream& in, int& line) {
	bool status = false;
	LexItem t = Parser::GetNextToken(in, line);
    if (t.GetToken() == IF) {
        t = Parser::GetNextToken(in, line);
        if (t != LPAREN) {
            ParseError(line, "If statement syntax error.");
            return false;
        }
        
        status = RelExpr(in, line);
        if(!status){
            ParseError(line, "Missing Expression");
            return false;
        }

        t = Parser::GetNextToken(in, line);
        if (t != RPAREN) {
            ParseError(line, "If statement syntax error.");
            return false;
        }

        t = Parser::GetNextToken(in, line);
        if (t == THEN) {
            return BlockIfStmt(in, line);
        } else {
            Parser::PushBackToken(t);
            return SimpleIfStmt(in, line);
        }
    }
    return true;
}