/*
  Simple integer arithmetic calculator according to the following BNF
  exps		--> exp | exp NEWLINE exps
  exp		--> term {addop term}
  addop		--> + | -
  term		--> factor {mulop factor}
  mulop		--> * | /
  factor	--> ( exp ) | INT


  Damon Hughes
  Dr. Dong
  3210
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include "tokens.h"
#include "FlexLexer.h"

using namespace std;

string toknames[] = {
"INT", "LPAREN", "RPAREN", "PLUS", "MINUS", "TIMES", "DIVIDE", "NEWLINE"
};

string tokname(int tok) {
  return tok<257 || tok>264 ? "BAD_TOKEN" : toknames[tok-257];
}

yyFlexLexer			lexer;
YYSTYPE				yylval;

int		nextToken;	//global variable stores the token to be processed

void readNextToken( void ); //read the next token into the variable nextToken

void exps( void );	//process all expressions in the input
int  exp( void );	//returns the integer value of an expression
int term ( void );	//returns the integer value of an term
int factor( void );	//returns the integer value of an factor

//If the next token matches expectedToken, read the next token and return true
//otherwise, print an error message and return false
bool match( int expectedToken );

//print the error message
void error( string errorMsg );

//skip the rest of the line
void skipline( void );

int main(int argc, char **argv) {
	
	ifstream	ifs; 
	if (argc!=2) 
	{
		cerr << "usage: " << argv[0] << " filename" << endl;
		return 1;	
	}
	ifs.open( argv[1] );
	if( !ifs ) 
	{
		cerr << "Input file cannot be opened.\n";
        return 0;
	}
	cout << "Lexcial Analysis of the file " << argv[1] << endl;
	
	lexer.switch_streams(&ifs, NULL);

	readNextToken();

	exps();
	
	return 0;
}

//print the error message, and
//terminate the program
void error( string errorMsg )
{
	cout << errorMsg << endl;
	exit(1);
}

//skip the rest of the line
void skipline( void )
{
	while( nextToken != NEWLINE && nextToken != 0 )
		readNextToken();
}


//read the next token into the variable nextToken
void readNextToken( void )
{
	nextToken = lexer.yylex();
}

//If the next token is expectedToken, call readNextToken and return true,
//otherwise, print an error message and return false
bool match( int expectedToken )
{
	if ( expectedToken == nextToken )
	{
		readNextToken();
		return true;
	}
	else
		return false;
}

void exps( void )
{
	int		count = 1;
	int		value,x;

	do 
	{
		try {
			value = exp();
			cout << "expression " << count << " : " << value << endl;
			
		} catch(runtime_error e) {
			skipline();
			cout << "expression " << count << " :    wrong syntax -- " << e.what() << endl;
		}
		count ++;
	} while ( match(NEWLINE) );
}


//returns the integer value of an expression
int exp( void )
{
	int x=term();		//checks the term to see if it has value to return
	
	while ((nextToken ==260)||(nextToken==261))			//checks for plus or minus symbol
		switch (nextToken) {
		case 260: match(260);			//if plus adds the value to end production
				  x += term();
				  break;
		case 261:  match(261);			//if it sub it subtract the from the total
					x -= term();
			break;
		}

	return x;	           //returns the finish product
}

int term ( void )
{
	
	int x = factor();
	while ((nextToken == 262) || (nextToken == 263))			//checks to see if there is multipicaton or divison being done
		switch (nextToken) {
		case 262: match(262);			//if multi it takes what it has and multiplies it
			x *= factor();
			break;
		case 263:  match(263);			//if div it divs the amount
			x /= factor();
			break;
		}
	return x;
}

int factor( void )
{
	int x;
	
	if (nextToken == 258) {			//checks to see if there is left pren
		match(258);
		x = exp();
		if(!match(259))				//checks to make sure there is a right pren
		throw runtime_error("Token RPAREN was not found!");	//if righ pren not found throws error
		
	}
	else if (nextToken == 257) {		//if a  it coverts token and grabs the next value
		x = yylval.ival;
		readNextToken();

		
	}
	else throw runtime_error("Token LPAREN or INT expected!");  //if nothing is found throws a common error
	return x ;   //returns the value
}
