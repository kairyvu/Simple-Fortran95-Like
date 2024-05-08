#include "parserInterp.h"
#include <string>
#include <map>
#include <stack>
#include "lex.h"
#include "val.h"

#define sz(x) int((x).size())

map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults;
queue<Value> *ValQue;
bool state = true;
namespace Parser
{
	bool pushed_back = false;
	LexItem pushed_token;

	static LexItem GetNextToken(istream &in, int &line)
	{
		if (pushed_back)
		{
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem &t)
	{
		if (pushed_back)
		{
			abort();
		}
		pushed_back = true;
		pushed_token = t;
	}

}

static int error_count = 0;

int ErrCount()
{
	return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream &in, int &line);

// Program is: Prog = PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream &in, int &line)
{
	bool dl = false, sl = false;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() == PROGRAM)
	{
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT)
		{
			dl = Decl(in, line);
			if (!dl)
			{
				ParseError(line, "Incorrect Declaration in Program");
				return false;
			}
			sl = Stmt(in, line);
			if (!sl)
			{
				ParseError(line, "Incorrect Statement in program");
				return false;
			}
			tok = Parser::GetNextToken(in, line);

			if (tok.GetToken() == END)
			{
				tok = Parser::GetNextToken(in, line);

				if (tok.GetToken() == PROGRAM)
				{
					tok = Parser::GetNextToken(in, line);

					if (tok.GetToken() == IDENT)
					{
						cout << "(DONE)" << endl;
						return true;
					}
					else
					{
						ParseError(line, "Missing Program Name");
						return false;
					}
				}
				else
				{
					ParseError(line, "Missing PROGRAM at the End");
					return false;
				}
			}
			else
			{
				ParseError(line, "Missing END of Program");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing Program name");
			return false;
		}
	}
	else if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	else
	{
		ParseError(line, "Missing Program keyword");
		return false;
	}
}

// Decl ::= Type :: VarList
// Type ::= INTEGER | REAL | CHARARACTER [(LEN = ICONST)]
bool Decl(istream &in, int &line)
{
	bool status = false;
	LexItem tok;
	string strLen;

	LexItem t = Parser::GetNextToken(in, line);

	if (t == INTEGER || t == REAL || t == CHARACTER)
	{
		tok = t;
		LexItem idtok = t; // Save type to idtok
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == DCOLON)
		{
			status = VarList(in, line, idtok);
			if (status)
			{
				status = Decl(in, line);
				if (!status)
				{
					ParseError(line, "Declaration Syntactic Error.");
					return false;
				}
				return status;
			}
			else
			{
				ParseError(line, "Missing Variables List.");
				return false;
			}
		}
		else if (t == CHARACTER && tok.GetToken() == LPAREN)
		{
			tok = Parser::GetNextToken(in, line);

			if (tok.GetToken() == LEN)
			{
				tok = Parser::GetNextToken(in, line);

				if (tok.GetToken() == ASSOP)
				{
					tok = Parser::GetNextToken(in, line);

					if (tok.GetToken() == ICONST)
					{
						strLen = tok.GetLexeme();

						tok = Parser::GetNextToken(in, line);
						if (tok.GetToken() == RPAREN)
						{
							tok = Parser::GetNextToken(in, line);
							if (tok.GetToken() == DCOLON)
							{
								status = VarList(in, line, idtok, stoi(strLen));

								if (status)
								{
									// cout << "Definition of Strings with length of " << strLen << " in declaration statement." << endl;
									status = Decl(in, line);
									if (!status)
									{
										ParseError(line, "Declaration Syntactic Error.");
										return false;
									}
									return status;
								}
								else
								{
									ParseError(line, "Missing Variables List.");
									return false;
								}
							}
						}
						else
						{
							ParseError(line, "Missing Right Parenthesis for String Length definition.");
							return false;
						}
					}
					else
					{
						ParseError(line, "Incorrect Initialization of a String Length");
						return false;
					}
				}
			}
		}
		else
		{
			ParseError(line, "Missing Double Colons");
			return false;
		}
	}
	Parser::PushBackToken(t);
	return true;
} // End of Decl

// Stmt ::= AssigStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
bool Stmt(istream &in, int &line)
{
	bool status;

	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken())
	{

	case PRINT:
		status = PrintStmt(in, line);

		if (status)
			status = Stmt(in, line);
		break;

	case IF:
		status = BlockIfStmt(in, line);
		if (status)
			status = Stmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
		status = AssignStmt(in, line);
		if (status)
			status = Stmt(in, line);
		break;

	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
} // End of Stmt

bool SimpleStmt(istream &in, int &line)
{
	bool status;

	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken())
	{

	case PRINT:
		status = PrintStmt(in, line);

		if (!status)
		{
			ParseError(line, "Incorrect Print Statement");
			return false;
		}
		cout << "Print statement in a Simple If statement." << endl;
		break;

	case IDENT:
		Parser::PushBackToken(t);
		status = AssignStmt(in, line);
		if (!status)
		{
			ParseError(line, "Incorrect Assignent Statement");
			return false;
		}
		cout << "Assignment statement in a Simple If statement." << endl;

		break;

	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
} // End of SimpleStmt

// VarList ::= Var [= Expr] {, Var [= Expr]}
bool VarList(istream &in, int &line, LexItem &idtok, int strlen)
{
	bool status = false, exprstatus = false;
	string identstr;

	LexItem tok = Parser::GetNextToken(in, line);
	if (tok == IDENT)
	{
		identstr = tok.GetLexeme();
		if (!(defVar.find(identstr)->second))
		{
			defVar[identstr] = true;
			SymTable[identstr] = idtok.GetToken(); // Var Declaration
		}
		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
	}
	else
	{
		ParseError(line, "Missing Variable Name");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok != ASSOP && idtok.GetToken() == CHARACTER)
	{
		string temp = "";
		Value val = Value(temp);
		for (int i = sz(val.GetString()); i < strlen; i++)
		{
			val.SetString(val.GetString() + " ");
		}
		TempsResults[identstr] = val;
	}

	if (tok == ASSOP)
	{
		Value val;
		exprstatus = Expr(in, line, val);
		if (!exprstatus)
		{
			ParseError(line, "Incorrect initialization for a variable.");
			return false;
		}

		// cout << "Initialization of the variable " << identstr << " in the declaration statement." << endl;
		if (idtok.GetToken() == CHARACTER)
		{
			if (sz(val.GetString()) >= strlen)
			{
				val.SetString(val.GetString().substr(0, strlen));
			}
			else
			{
				for (int i = sz(val.GetString()); i < strlen; i++)
				{
					val.SetString(val.GetString() + " ");
				}
			}
		}
		else if (idtok.GetToken() == REAL && val.GetType() == VINT)
		{
			val = Value(1.0 * (val.GetInt()));
		}
		else if (idtok.GetToken() == INTEGER && val.GetType() == VREAL)
		{
			val = Value((int)(val.GetReal()));
		}
		TempsResults[identstr] = val;
		tok = Parser::GetNextToken(in, line);

		if (tok == COMMA)
		{
			status = VarList(in, line, idtok, strlen);
		}
		else
		{
			Parser::PushBackToken(tok);
			return true;
		}
	}
	else if (tok == COMMA)
	{

		status = VarList(in, line, idtok, strlen);
	}
	else if (tok == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");

		return false;
	}
	else
	{
		Parser::PushBackToken(tok);
		return true;
	}

	return status;

} // End of VarList

// PrintStmt:= PRINT *, ExpreList
bool PrintStmt(istream &in, int &line)
{
	LexItem t;
	ValQue = new queue<Value>;

	t = Parser::GetNextToken(in, line);

	if (t != DEF)
	{
		ParseError(line, "Print statement syntax error.");
		return false;
	}
	t = Parser::GetNextToken(in, line);

	if (t != COMMA)
	{

		ParseError(line, "Missing Comma.");
		return false;
	}
	bool ex = ExprList(in, line);

	if (!ex)
	{
		ParseError(line, "Missing expression after Print Statement");
		return false;
	}
	if (state)
	{
		while (!(*ValQue).empty())
		{
			cout << (*ValQue).front();
			ValQue->pop();
		}
		cout << endl;
	}

	return ex;
} // End of PrintStmt

// BlockIfStmt:= if (RelExpr) then {Stmt} [Else Stmt]
// SimpleIfStatement := if (RelExpr) Stmt
bool BlockIfStmt(istream &in, int &line) // DONE
{
	bool ex = false, status;
	static int nestlevel = 0;
	LexItem t;

	t = Parser::GetNextToken(in, line);
	if (t != LPAREN)
	{
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	Value val;
	ex = RelExpr(in, line, val);
	if (!ex)
	{
		ParseError(line, "Missing if statement condition");
		return false;
	}

	if (!val.IsBool())
	{
		ParseError(line, "RelExpr should return a boolean value");
		return false;
	}
	state = val.GetBool();

	t = Parser::GetNextToken(in, line);
	if (t != RPAREN)
	{
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != THEN)
	{
		Parser::PushBackToken(t);

		if (state)
		{
			status = SimpleStmt(in, line);
			if (status)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			int curLine = line;
			while (true)
			{
				LexItem t = Parser::GetNextToken(in, line);
				if (curLine != line)
				{
					Parser::PushBackToken(t);
					return true;
				}
			}
		}
	}
	nestlevel++;
	if (state)
	{
		status = Stmt(in, line);
		if (!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Then-Part");
			return false;
		}
	}
	else
	{
		while (true)
		{
			LexItem t = Parser::GetNextToken(in, line);
			if (t == ELSE)
			{
				Parser::PushBackToken(t);
				break;
			}
			if (t == END)
			{
				if (Parser::GetNextToken(in, line) == IF)
				{
					return true;
				}
			}
			if (t == DONE)
			{
				ParseError(line, "Wrong Block IF");
				return false;
			}
		}
	}

	t = Parser::GetNextToken(in, line);
	if (t == ELSE)
	{
		state = !val.GetBool();
		status = Stmt(in, line);
		if (!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Else-Part");
			return false;
		}
		else
			t = Parser::GetNextToken(in, line);
	}

	if (t != END)
	{
		ParseError(line, "Missing END of IF");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if (t == IF)
	{
		// cout << "End of Block If statement at nesting level " << level << endl;
		return true;
	}
	state = true;
	Parser::PushBackToken(t);
	ParseError(line, "Missing IF at End of IF statement");
	return false;
} // End of IfStmt function

// Var:= ident
bool Var(istream &in, int &line, LexItem &idtok)
{
	string identstr;

	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == IDENT)
	{
		identstr = tok.GetLexeme();

		if (!(defVar.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}
		idtok = tok;
		return true;
	}
	else if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
} // End of Var

// AssignStmt:= Var = Expr
bool AssignStmt(istream &in, int &line)
{
	bool varstatus = false, status = false;
	LexItem t, idtok;
	varstatus = Var(in, line, idtok);

	if (varstatus)
	{
		t = Parser::GetNextToken(in, line);

		if (t == ASSOP)
		{
			LexItem nxtTok = Parser::GetNextToken(in, line);
			if (nxtTok.GetToken() == IDENT)
			{
				if ((SymTable[nxtTok.GetLexeme()] != CHARACTER && SymTable[idtok.GetLexeme()] == CHARACTER) || (SymTable[nxtTok.GetLexeme()] == CHARACTER && SymTable[idtok.GetLexeme()] != CHARACTER))
				{
					ParseError(line, "Type error");
					return false;
				}
			}
			Parser::PushBackToken(nxtTok);
			Value val;
			status = Expr(in, line, val);
			if (!status)
			{
				ParseError(line, "Missing Expression in Assignment Statment");
				return status;
			}
			if (state)
			{
				if (TempsResults[idtok.GetLexeme()].GetType() == VSTRING)
				{
					int tempStrLen = sz(TempsResults[idtok.GetLexeme()].GetString());
					if (val.IsString())
					{
						if (tempStrLen >= sz(val.GetString()))
						{
							string temp = val.GetString();
							for (int i = sz(val.GetString()); i < tempStrLen; i++)
							{
								temp += " ";
							}
							val = Value(temp);
						}
						else
						{
							string temp = val.GetString().substr(0, tempStrLen);
							val = Value(temp);
						}
					}
					else
					{
						ParseError(line, "Illegal mixed-mode assignment operation");
						return false;
					}
				}
				TempsResults[idtok.GetLexeme()] = val;
			}
		}
		else if (t.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else
		{
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else
	{
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;
} // End of AssignStmt

// ExprList:= Expr {,Expr}
bool ExprList(istream &in, int &line)
{
	bool status = false;
	Value val;

	status = Expr(in, line, val);
	if (!status)
	{
		ParseError(line, "Missing Expression");
		return false;
	}
	ValQue->push(val);
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == COMMA)
	{
		status = ExprList(in, line);
	}
	else if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else
	{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
} // End of ExprList

// RelExpr ::= Expr  [ ( == | < | > ) Expr ]
bool RelExpr(istream &in, int &line, Value &retVal)
{
	Value val1;
	bool t1 = Expr(in, line, val1);
	retVal = val1;
	LexItem tok;

	if (!t1)
	{
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if (tok == EQ || tok == LTHAN || tok == GTHAN)
	{
		Value val2;
		t1 = Expr(in, line, val2);
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		if (tok == EQ)
			retVal = val1 == val2;
		else if (tok == LTHAN)
			retVal = val1 < val2;
		else
			retVal = val1 > val2;
		return true;
	}
	// if (!val1.IsBool()) {
	// 	ParseError(line, "Val1 has to be boolean type");
	// 	return false;
	// }

	return true;
} // End of RelExpr

// Expr ::= MultExpr { ( + | - | // ) MultExpr }
bool Expr(istream &in, int &line, Value &retVal)
{
	Value val1;
	bool t1 = MultExpr(in, line, val1);
	retVal = val1;
	LexItem tok;

	if (!t1)
	{
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == PLUS || tok == MINUS || tok == CAT)
	{
		Value val2;
		t1 = MultExpr(in, line, val2);
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		if (tok == PLUS)
		{
			retVal = retVal + val2;
		}
		else if (tok == MINUS)
			retVal = retVal - val2;
		else
			retVal = retVal.Catenate(val2);

		if (retVal.IsErr())
		{
			ParseError(line, "Illegal operand type for the operation.");
			return false;
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
} // End of Expr

// MultExpr ::= TermExpr { ( * | / ) TermExpr }
bool MultExpr(istream &in, int &line, Value &retVal)
{
	Value val1;
	bool t1 = TermExpr(in, line, val1);
	retVal = val1;
	LexItem tok;

	if (!t1)
	{
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == MULT || tok == DIV)
	{
		Value val2;
		t1 = TermExpr(in, line, val2);
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		if (tok == MULT)
			retVal = retVal * val2;
		else
		{
			if ((val2.GetType() == VINT && val2.GetInt() == 0) || (val2.GetType() == VREAL && val2.GetReal() == 0.0))
			{
				ParseError(line, "Run-Time Error-Illegal division by Zero");
				return false;
			}
			retVal = retVal / val2;
		}

		if (retVal.IsErr())
		{
			ParseError(line, "Illegal operand type for the operation.");
			return false;
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
} // End of MultExpr

// TermExpr ::= SFactor { ** SFactor }
bool TermExpr(istream &in, int &line, Value &retVal)
{
	Value val1;
	bool t1 = SFactor(in, line, val1);
	retVal = val1;
	LexItem tok;

	if (!t1)
	{
		return false;
	}
	tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if (tok.GetToken() != POW)
	{
		Parser::PushBackToken(tok);
		return true;
	}

	stack<Value> expo;
	expo.push(val1);
	while (tok == POW)
	{
		Value val2;
		t1 = SFactor(in, line, val2);

		if (!t1)
		{
			ParseError(line, "Missing exponent operand");
			return false;
		}
		expo.push(val2);

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Value res = Value(1);
	while (!expo.empty())
	{
		Value temp = expo.top();
		res = temp.Power(res);
		expo.pop();
	}
	retVal = res;
	Parser::PushBackToken(tok);
	return true;
} // End of TermExpr

// SFactor = Sign Factor | Factor
bool SFactor(istream &in, int &line, Value &retVal)
{
	Value val;
	LexItem t = Parser::GetNextToken(in, line);

	bool status;
	int sign = 0;
	if (t == MINUS)
	{
		sign = -1;
	}
	else if (t == PLUS)
	{
		sign = 1;
	}
	else
		Parser::PushBackToken(t);

	status = Factor(in, line, sign, val);
	if (!status)
	{
		ParseError(line, "Missing operand after operator");
		return false;
	}
	if ((t == MINUS || t == PLUS) && !val.IsInt() && !val.IsReal())
	{
		ParseError(line, "Val needs to be number in SFactor");
		return false;
	}
	retVal = val;
	return status;
} // End of SFactor

// Factor := ident | iconst | rconst | sconst | (Expr)
bool Factor(istream &in, int &line, int sign, Value &retVal)
{
	LexItem tok = Parser::GetNextToken(in, line);

	// cout << tok.GetLexeme() << endl;
	if (tok == IDENT)
	{
		string lexeme = tok.GetLexeme();
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undefined Variable");
			return false;
		}
		if (TempsResults.find(lexeme) == TempsResults.end())
		{
			ParseError(line, "Using uninitialized Variable");
			return false;
		}
		if (sign == 0)
			retVal = TempsResults[lexeme];
		else if (TempsResults[lexeme].GetType() != VSTRING)
			retVal = TempsResults[lexeme] * Value(sign);
		else
		{
			ParseError(line, "Illegal Operand Type for Sign Operator");
			return false;
		}
		return true;
	}
	else if (tok == ICONST)
	{
		if (sign != 0)
			retVal = Value(stoi(tok.GetLexeme()) * sign);
		else
			retVal = Value(stoi(tok.GetLexeme()));
		return true;
	}
	else if (tok == SCONST)
	{
		retVal = Value(tok.GetLexeme());
		return true;
	}
	else if (tok == RCONST)
	{
		if (sign != 0)
			retVal = Value(stod(tok.GetLexeme()) * sign);
		else
			retVal = Value(stod(tok.GetLexeme()));
		return true;
	}
	else if (tok == LPAREN)
	{
		Value val;
		bool ex = Expr(in, line, val);
		if (!ex)
		{
			ParseError(line, "Missing expression after (");
			return false;
		}
		if (Parser::GetNextToken(in, line) == RPAREN)
		{
			if (sign != 0)
				retVal = val * Value(sign);
			else
				retVal = val;
			return ex;
		}
		else
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}
	else if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	return false;
}
