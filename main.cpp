#include <iostream>
#include <map>
#include <fstream>
#include <cctype>
#include <map>
#include "lex.h"
// #include "lex.cpp"

/*
 * CS280 - Spring 2024
*/

using std::map;

using namespace std;

int main(int argc, char *argv[])
{
	ifstream file;
    map<string, int> summary;
    map<string, int> identCount;
    map<int, int> intCount;
    map<double, int> realCount;
    map<string, int> strCount;
    map<string, int> kwCount;
    LexItem token;

	string str1;
    bool printAll = false, printInt = false, printReal = false, printStr = false, printId = false, printKw = false;
	if (argc == 1)
	{
		cerr << "NO SPECIFIED INPUT FILE." << endl;
		return 0;
	}
	else
	{
		if(argv[1][0] == '-')
		{
			cerr << "NO SPECIFIED INPUT FILE." << endl;
			return 0;
		}
		file.open(argv[1]);
		if( file.is_open() == false ) {
			cerr << "CANNOT OPEN THE FILE " << argv[1] << endl;
			return 0;
		}
	}
    
    if (file.peek() == std::ifstream::traits_type::eof()) {
        cerr << "Empty File." << endl;
        return 0;
    }
    for (int i = 2; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-all") {
            printAll = true;
        } else if (arg == "-int") {
            printInt = true;
        } else if (arg == "-real") {
            printReal = true;
        } else if (arg == "-str") {
            printStr = true;
        } else if (arg == "-id") {
            printId = true;
        } else if (arg == "-kw") {
            printKw = true;
        } else if (arg[0] != '-') {
            cout << "ONLY ONE FILE NAME IS ALLOWED." << endl;
            return 0;
        }
        else {
            cout << "UNRECOGNIZED FLAG {" << arg << "}" << endl;
            return 0;
        }
    }
    if (printAll) {
        do {
            token = getNextToken(file, summary["lines"]);
            if (token == IDENT) identCount[token.GetLexeme()]++;
            if (token == ICONST) intCount[stoi(token.GetLexeme())]++;
            try {
                if (token == RCONST) realCount[stod(token.GetLexeme())]++;
            }
            catch (std::invalid_argument) {
                realCount[stod('0' + token.GetLexeme())]++;
            }
            if (token == SCONST) strCount[token.GetLexeme()]++;
            if (token == PROGRAM || token == END || token == ELSE || token == IF || token == INTEGER || token == REAL || token == CHARACTER || token == PRINT || token == LEN || token == THEN) kwCount[token.GetLexeme()]++;
            cout << token << "\n";
            if (token == ERR) return -1;
            summary["totalToken"]++;
        } while (token != DONE);    
    }

    else {
        do {
            token = getNextToken(file, summary["lines"]);
            if (token == IDENT) identCount[token.GetLexeme()]++;
            if (token == ICONST) intCount[stoi(token.GetLexeme())]++;
            try {
                if (token == RCONST) realCount[stod(token.GetLexeme())]++;
            }
            catch (std::invalid_argument) {
                realCount[stod('0' + token.GetLexeme())]++;
            }            
            if (token == SCONST) strCount[token.GetLexeme()]++;
            if (token == PROGRAM || token == END || token == ELSE || token == IF || token == INTEGER || token == REAL || token == CHARACTER || token == PRINT || token == LEN || token == THEN) kwCount[token.GetLexeme()]++;
            if (token == ERR) return -1;
            summary["totalToken"]++;
        } while (token != DONE);
        cout << '\n';
    }

    cout << "Lines: " << summary["lines"] << '\n';
    cout << "Total Tokens: " << summary["totalToken"] - 1 << '\n';
    cout << "Identifiers: " << identCount.size() << '\n';
    cout << "Integers: " << intCount.size() << '\n';
    cout << "Reals: " << realCount.size() << '\n';
    cout << "Strings: " << strCount.size() << '\n';

    if (printId) {
        cout << "IDENTIFIERS:" << '\n';
        int size = identCount.size();
        map<string, int>::iterator it;
        for (it = identCount.begin(); it != identCount.end(); it++) {
            size--;
            if (size != 0) cout << it->first << " (" << it->second << "), ";
            else cout << it->first << " (" << it->second << ")";
        }
        cout << endl;
    }

    if (printKw) {
        cout << "KEYWORDS:" << '\n';
        int size = kwCount.size();
        map<string, int>::iterator it;
        for (it = kwCount.begin(); it != kwCount.end(); it++) {
            size--;
            if (size != 0) cout << it->first << " (" << it->second << "), ";
            else cout << it->first << " (" << it->second << ")";
        }
        cout << endl;
    }

    if (printInt) {
        cout << "INTEGERS:" << '\n';
        int size = intCount.size();
        map<int, int>::iterator it;
        for (it = intCount.begin(); it != intCount.end(); it++) {
            size--;
            if (size != 0) cout << it->first << ", ";
            else cout << it->first;
        }
        cout << endl;
    }

    if (printReal) {
        cout << "REALS:" << '\n';
        int size = realCount.size();
        map<double, int>::iterator it;
        for (it = realCount.begin(); it != realCount.end(); it++) {
            size--;
            if (size != 0) cout << it->first << ", ";
            else cout << it->first;
        }
        cout << endl;
    }

    if (printStr) {
        cout << "STRINGS:" << '\n';
        int size = strCount.size();
        map<string, int>::iterator it;
        for (it = strCount.begin(); it != strCount.end(); it++) {
            size--;
            if (size != 0) cout << "\"" << it->first << "\", ";
            else cout << "\"" << it->first << "\"";   
        }
        cout << endl;
    }
    return 0;
}
