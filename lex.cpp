#include <cctype>
#include <map>

using std::map;
using namespace std;

#include "lex.h"
//Keywords mapping
LexItem id_or_kw(const string& lexeme , int linenum) {
	static const map<string, Token> kwmap = {
        {"program", PROGRAM},
        {"print", PRINT},
        {"if", IF},
        {"else", ELSE},
        {"end", END},
        {"integer", INTEGER},
        {"real", REAL},
        {"character", CHARACTER},
        {"then", THEN},
	    {"len", LEN},
    };

	string lowerKw;
	lowerKw.reserve(lexeme.length());
	for (char c : lexeme) lowerKw += tolower(c);
	
    auto it = kwmap.find(lowerKw);
    if (it != kwmap.end()) {
        return LexItem(it->second, lexeme, linenum);
    } else {
        return LexItem(IDENT, lexeme, linenum);
    }
}

map<Token,string> tokenPrint = {
	{ IF, "IF" },
	{ ELSE, "ELSE" },
	{ PRINT, "PRINT" },
	{ INTEGER, "INTEGER" },
	{ REAL, "REAL" },
	{ CHARACTER, "CHARACTER" },
	{ END, "END" },
	{ THEN, "THEN" },
	{ PROGRAM, "PROGRAM" },
	{ LEN, "LEN" },

	{ IDENT, "IDENT" },
	{ ICONST, "ICONST" },
	{ RCONST, "RCONST" },
	{ SCONST, "SCONST" },
	{ BCONST, "BCONST" },
			
	{ PLUS, "PLUS" },
	{ MINUS, "MINUS" },
	{ MULT, "MULT" },
	{ DIV, "DIV" },
	{ ASSOP, "ASSOP" },
	{ EQ, "EQ" },
	{ POW, "POW" },
	{ GTHAN, "GTHAN" },
	{ LTHAN, "LTHAN" },
	{ CAT, "CAT" },
		            
	{ COMMA, "COMMA" },
	{ LPAREN, "LPAREN" },
	{ RPAREN, "RPAREN" },
	{ DOT, "DOT" },
	{ DCOLON, "DCOLON" },
	{ DEF, "DEF"},
		
	{ ERR, "Error" },

	{ DONE, "" },
};

ostream& operator<<(ostream& out, const LexItem& tok) {
	Token tt = tok.GetToken();
	out << tokenPrint[ tt ];
	if (tt == BCONST || tt == ICONST || tt == RCONST) out << ": (" << tok.GetLexeme() << ")";
	else if (tt == SCONST) out << ": \"" << tok.GetLexeme() << "\"";
	else if (tt == IDENT) out << ": '" << tok.GetLexeme() << "'";
	else if (tt == ERR) out << " in line " << tok.GetLinenum() + 1 << ": Unrecognized Lexeme {" << tok.GetLexeme() << "}";
	return out;
}

LexItem getNextToken(istream& in, int& linenum) {
	enum TokState { START, INID, INSTRING1, INSTRING2, ININT, INFLOAT, INCOMMENT } lexstate = START;
	string lexeme;
	char ch, nextchar;
	Token tt;
	bool decimal = false;
	       
	while (in.get(ch)) {
		switch (lexstate) {
			case START:
				if (ch == '\n') linenum++;
					
				if (isspace(ch)) continue;

				lexeme = ch;

				if (isalpha(ch) || ch == '_') {
					lexeme = ch;
					lexstate = INID;
				}

				else if (ch == '\'') lexstate = INSTRING1;
				else if (ch == '"') lexstate = INSTRING2;
				else if (isdigit(ch)) lexstate = ININT;
				else if (ch == '.' && isdigit(in.peek())) {
					in.putback(ch);
					lexeme = "";
					lexstate = INFLOAT;
				}
				else if (ch == '!') {
					lexeme += ch;
					lexstate = INCOMMENT;
					in.get(ch);
				}
				else {
					tt = ERR;
					switch (ch) {
						case '+':
							tt = PLUS;
							break;  
							
						case '-':
							lexeme = ch;
							nextchar = in.peek();
							nextchar = tolower(nextchar);
							tt = MINUS;
							break; 
							
						case '*':
							nextchar = in.peek();
							if (nextchar == '*') {
								in.get(ch);
								tt = POW;
								break;
							}
							else if (nextchar == ' ' || nextchar == '\n') tt = MULT;
							else tt = DEF;
							break;

						case '/':
							nextchar = in.peek();
							if (nextchar == '/') {
								in.get(ch);
								tt = CAT;
								break;
							}
							tt = DIV;
							break;

						case '=':
							nextchar = in.peek();
							if (nextchar == '='){
								in.get(ch);
								tt = EQ;
								break;
							}
							tt = ASSOP;
							break;
						
						case '(':
							tt = LPAREN;
							break;	

						case ')':
							tt = RPAREN;
							break;
							
						case ',':
							tt = COMMA;
							break;
							
						case '>':
							tt = GTHAN;
							break;
						
						case '<':
							tt = LTHAN;
							break;
							
						case '.':
							tt = DOT;
							break;
						
						case ':':
							nextchar = in.peek();
							if (nextchar == ':') {
								in.get(ch);
								tt = DCOLON;
								break;
							}
							tt = ERR;
							break;
					}
					if (lexstate == INFLOAT) break;
					return LexItem(tt, lexeme, linenum);
				}
				break;

			case INID:
				if (isalpha(ch) || isdigit(ch) || ch == '_' || ch == '$') lexeme += ch;
				else {
					in.putback(ch);
					return id_or_kw(lexeme, linenum);
				}
				break;
						
			case INSTRING1:     
				if (ch == '\n') return LexItem(ERR, lexeme, linenum);
				lexeme += ch;
				if (ch == '\'') {
					lexeme = lexeme.substr(1, lexeme.length()-2);
					return LexItem(SCONST, lexeme, linenum);
				}
				break;

			case INSTRING2:
				if (ch == '\n') return LexItem(ERR, lexeme, linenum);
				lexeme += ch;
				if (ch == '"') {
					lexeme = lexeme.substr(1, lexeme.length()-2);
					return LexItem(SCONST, lexeme, linenum);
				}
				break;

			case ININT:
				if (isdigit(ch)) lexeme += ch;
				else if (ch == '.') {
					lexstate = INFLOAT;
					in.putback(ch);
				} else {
					in.putback(ch);
					return LexItem(ICONST, lexeme, linenum);
				}
				break;
			
			case INFLOAT:
				if (ch == '.' && isdigit(in.peek()) && !decimal) {
					lexeme += ch; 
					decimal = true;
				} else if (ch == '.' && !isdigit(in.peek()) && !decimal) {
					lexeme += ch;
					return LexItem(RCONST, lexeme, linenum);
				} 
				else if (isdigit(ch) && decimal) lexeme += ch;
				else if (ch == '.' && decimal){
					lexeme += ch;
					return LexItem(ERR, lexeme, linenum);
				} else {
					in.putback(ch);
					return LexItem(RCONST, lexeme, linenum);
				}
				break;
			
			case INCOMMENT:
				if(ch == '\n') {
					linenum++;
					lexstate = START;
				}
				break;
			
			}
	} //end of while loop
	
	if (in.eof()) return LexItem(DONE, "", linenum);
		
	return LexItem(ERR, "some strange I/O error", linenum);
}