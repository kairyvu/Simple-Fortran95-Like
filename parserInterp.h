#ifndef PARSE_H_
#define PARSE_H_

#include <iostream>

using namespace std;

#include "lex.h"
#include "val.h"

extern bool Prog(istream &in, int &line);
extern bool Decl(istream &in, int &line); // DONE
extern bool Type(istream &in, int &line);
extern bool VarList(istream &in, int &line, LexItem &idtok, int strlen = 1); // DONE
extern bool Stmt(istream &in, int &line);                                    // DONE
extern bool SimpleStmt(istream &in, int &line);                              // DONE
extern bool PrintStmt(istream &in, int &line);                               // DONE
extern bool BlockIfStmt(istream &in, int &line);                             // DONE
extern bool SimpleIfStmt(istream &in, int &line);                            // DONE
extern bool AssignStmt(istream &in, int &line);                              // DONE
extern bool Var(istream &in, int &line, LexItem &idtok);                     // DONE
extern bool ExprList(istream &in, int &line);                                // DONE
extern bool RelExpr(istream &in, int &line, Value &retVal);                  // DONE
extern bool Expr(istream &in, int &line, Value &retVal);                     // DONE
extern bool MultExpr(istream &in, int &line, Value &retVal);                 // DONE
extern bool TermExpr(istream &in, int &line, Value &retVal);                 // DONE
extern bool SFactor(istream &in, int &line, Value &retVal);                  // DONE
extern bool Factor(istream &in, int &line, int sign, Value &retVal);         // DONE

extern int ErrCount();

#endif /* PARSE_H_ */
