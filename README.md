# Simple-Fortran95-Like

## Lexical Analyzer

- The syntax definitions of the SFort95 language are given below using [Extended Backus–Naur form (EBNF)](https://w.wiki/BVYN) notations:

| RegExpr      | Meaning |
| ----------- | :-----------: |
| x |	a character *x* |
| \x | an escaped character |
| M \| N | M or N |
| MN | M followed by N |
| M* | ***zero or more*** occurrences of M |
| M+ | ***one or more*** occurrences of M |
| M? | ***zero or one*** occurrence of M |
| [characters] | choose from the characters in [] |
| [0-9] | set of digits |
| . | Any single character


- Identifiers (**IDENT**)
  - **IDENT** ::= Letter ( Letter | Digit | _ )*
  - Letter ::= [a-z A-Z]
  - Digit ::= [0-9]
  
- Integer constants (**ICONST**)
  - **ICONST** ::= [0-9]+
  
- Real constants (**RCONST**)
  - **RCONST** ::= ([0-9]*)\.([0-9]+)
  
- String constants (**SCONST**)
  - **SCONST** ::= \'.*\'
  
- Reserved words: program, end, else, if, integer, real, character, print, len
  - Tokens (not case sensititve), respectively: **PROGRAM**, **END**, **ELSE**, **IF**, **INTEGER**, **REAL**, **CHARACTER**, **PRINT**, **LEN**
  
- Operators: +, -, *, /, **, =, ==, <, >, //
  - Tokens, respectively: **PLUS**, **MINUS**, **MULT**, **DIV**, **POW**, **ASSOP**, **EQ**, **LTHAN**, **GTHAN**, **CAT**

- Delimiters: comma, left parenthesis, right parenthesis, double colons (::), dot, default mark
  - Tokens, respectively: **COMMA**, **LPAREN**, **RPAREN**, **DCOLON**, **DOT**, **DEF**
  
- A comment is defined by all the characters following the character “!” as starting delimiter till the end of the line

- Error (**ERR**)

- End of file (**DONE**)

## Recursive-Descent Parser

- Language Definition:

```
1. Prog ::= PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
2. Decl ::= Type :: VarList
3. Type ::= INTEGER | REAL | CHARARACTER [\(LEN = (ICONST \ *) \)]
4. VarList ::= Var [= Expr] {, Var [= Expr]}
5. Stmt ::= AssigStmt | BlockIfStmt | PrintStmt | SimpleIfStmt
6. PrintStmt ::= PRINT *, ExprList
7. BlockIfStmt ::= IF (RelExpr) THEN {Stmt} [ELSE {Stmt}] END IF
8. SimpleIfStmt ::= IF (RelExpr) SimpleStmt
9. SimpleStmt ::= AssigStmt | PrintStmt
10. AssignStmt ::= Var = Expr
11. ExprList ::= Expr {, Expr}
12. RelExpr ::= Expr [ ( == | < | > ) Expr ]
13. Expr ::= MultExpr { ( + | - | // ) MultExpr }
14. MultExpr ::= TermExpr { ( * | / ) TermExpr }
15. TermExpr ::= SFactor { ** SFactor }
16. SFactor ::= [+ | -] Factor
17. Var = IDENT
18. Factor ::= IDENT | ICONST | RCONST | SCONST | (Expr)
```

- Table of Operators Precedence Levels:

| Precedence | Operator | Description | Associativity |
| :-----------: | :-----------: | :-----------: | :-----------: |
| 1 |	Unary + , - | Unary plus, minus | Right-to-Left |
| 2 |	** | Exponentiation | Right-to-Left |
| 3 |	\*, / | Multiplication, Division | Left-to-Right |
| 4 |	+, -, // | Addition, Subtraction, Concatenation | Left-to-Right |
| 5 |	<, >, == | Less than, greater than, and equality | (no cascading) |

## Interpreter

- Implement `Value` class member functions and overloaded operators from `val.h`

- **interpreter** does:
  - Performs syntax analysis of the input source code statement by statement, then executes the statement if there is no syntactic or semantic error.
  - Builds information of variables types in the symbol table for all the defined variables.
  - Evaluates expressions and determines their values and types.
  - The results of an unsuccessful parsing and interpreting are a set of error messages printed by the parser/interpreter functions, as well as the error messages that might be detected by the lexical analyzer.


## How it's built:

- Language: C++
- Libraries/Frameworks: `string`, `queue`, `map`, `iomanip`, `stdexcept`, `cmath`, `sstream`, `iostream`

## How to run:

- Clone this repo.
- Run `program.cpp` by command line with exactly one argument represents a filename (some of the the testcases are provided under ***testcase*** directory).