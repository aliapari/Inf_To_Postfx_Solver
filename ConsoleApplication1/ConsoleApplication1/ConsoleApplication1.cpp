#include"stdafx.h"

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cctype>
#include <string>
#include <vector>
#include <stack>
using namespace std;

/* This holds the transition states for our Finite State Machine
-- They are placed in numerical order for easy understanding within
the FSM array, which is located below */

enum FSM_TRANSITIONS
{
	REJECT = 0,
	INTEGER,
	REAL,
	NEGATIVE,
	OPERATOR,
	UNKNOWN,
	SPACE
};

/* This is the Finite State Machine
-- The zero represents a place holder, so the row in the array
starts on row 1 instead of 0

integer,  real,  negative, operator, unknown, space */
int stateTable[][7] = { { 0, INTEGER,  REAL, NEGATIVE, OPERATOR,  UNKNOWN,  SPACE },
/* STATE 1 */{ INTEGER,  INTEGER,  REAL,   REJECT,  REJECT,   REJECT,  REJECT },
/* STATE 2 */{ REAL,       REAL,  REJECT,  REJECT,  REJECT,   REJECT,  REJECT },
/* STATE 3 */{ NEGATIVE, INTEGER,  REAL,   REJECT,  REJECT,   REJECT,  REJECT },
/* STATE 4 */{ OPERATOR,  REJECT, REJECT,  REJECT,  REJECT,   REJECT,  REJECT },
/* STATE 5 */{ UNKNOWN,   REJECT, REJECT,  REJECT,  REJECT,   UNKNOWN, REJECT },
/* STATE 6 */{ SPACE,     REJECT, REJECT,  REJECT,  REJECT,   REJECT,  REJECT } };
class Equation {
	// function prototypes
	void DisplayDirections();
	string ConvertInfixToPostfix(string infix);
	bool IsMathOperator(char token);
	int OrderOfOperations(char token);
	vector<string> Lexer(string postfix);
	int Get_FSM_Col(char& currentChar);
	double EvaluatePostfix(const vector<string>& postfix);
	double Calculate(char token, double op1, double op2);
public:
	Equation() {}
	~Equation() {}
	void final_stuff() {
		Equation e;
		vector<string> vect;
		string s, ent;getline(cin, ent);
		s = e.ConvertInfixToPostfix(ent);
		cout << s << endl;
		vect = e.Lexer(s);
		cout << e.EvaluatePostfix(vect) << endl;
	}
};

int main()
{
	Equation x;
	x.final_stuff();

	return 0;
}// end of main


string Equation::ConvertInfixToPostfix(string infix)
{// this function converts an infix expression to postfix
 // declare function variables
	string postfix;
	stack<char> charStack;

	// loop thru array until there is no more data
	for (unsigned x = 0; x < infix.length(); ++x)
	{
		// place numbers (standard, decimal, & negative)
		// numbers onto the 'postfix' string
		if ((isdigit(infix[x])) || (infix[x] == '.') || (infix[x] == '~'))
		{
			postfix += infix[x];
		}
		else if (isspace(infix[x]))
		{
			continue;
		}
		else if (IsMathOperator(infix[x]))
		{
			postfix += " ";
			// use the 'OrderOfOperations' function to check equality
			// of the math operator at the top of the stack compared to
			// the current math operator in the infix string
			while ((!charStack.empty()) &&
				(OrderOfOperations(charStack.top()) >= OrderOfOperations(infix[x])))
			{
				// place the math operator from the top of the
				// stack onto the postfix string and continue the
				// process until complete
				postfix += charStack.top();
				charStack.pop();
			}
			// push the remaining math operator onto the stack
			charStack.push(infix[x]);
		}
		// push outer parentheses onto stack
		else if (infix[x] == '(')
		{
			charStack.push(infix[x]);
		}
		else if (infix[x] == ')')
		{
			// pop the current math operator from the stack
			while ((!charStack.empty()) && (charStack.top() != '('))
			{
				// place the math operator onto the postfix string
				postfix += charStack.top();
				// pop the next operator from the stack and
				// continue the process until complete
				charStack.pop();
			}

			if (!charStack.empty()) // pop '(' symbol off the stack
			{
				charStack.pop();
			}
			else // no matching '('
			{
				cout << "PARENTHESES MISMATCH #1";
				exit(1);
			}
		}
		else
		{
			cout << "INVALID INPUT #1";
			exit(1);
		}
	}

	// place any remaining math operators from the stack onto
	// the postfix array
	while (!charStack.empty())
	{
		postfix += charStack.top();
		charStack.pop();
	}

	return postfix;
}// end of ConvertInfixToPostfix

bool Equation::IsMathOperator(char token)
{// this function checks if operand is a math operator
	switch (tolower(token))
	{
	case '+': case '-': case '*': case '/':
	case '%': case '^': case '$': case 'c':
	case 's': case 't':
		return true;
		break;
	default:
		return false;
		break;
	}
}// end of IsMathOperator

int Equation::OrderOfOperations(char token)
{// this function returns the priority of each math operator
	int priority = 0;
	switch (tolower(token))
	{
	case 'c': case 's': case 't':
		priority = 5;
		break;
	case '^': case '$':
		priority = 4;
		break;
	case '*': case '/': case '%':
		priority = 3;
		break;
	case '-':
		priority = 2;
		break;
	case '+':
		priority = 1;
		break;
	}
	return priority;
}// end of OrderOfOperations

vector<string> Equation::Lexer(string postfix)
{// this function parses a postfix string using an FSM to generate
 //  each individual token in the expression
	vector<string> tokens;
	char currentChar = ' ';
	int col = REJECT;
	int currentState = REJECT;
	string currentToken = "";

	// use an FSM to parse multidigit "$$$GGG" and decimal numbers
	// also does error check for invalid input of decimals
	for (unsigned x = 0; x < postfix.length();)
	{
		currentChar = postfix[x];

		// get the column number for the current character
		col = Get_FSM_Col(currentChar);

		// exit if the real number has multiple periods "."
		// in the expression (i.e: 19.3427.23)
		if ((currentState == REAL) && (col == REAL))
		{
			cerr << "INVALID INPUT #2";
			exit(1);
		}
		/* ========================================================

		THIS IS WHERE WE CHECK THE FINITE STATE MACHINE TABLE
		USING THE "col" VARIABLE FROM ABOVE ^

		========================================================= */

		// get the current state of our machine
		currentState = stateTable[currentState][col];

		/* ===================================================

		THIS IS WHERE WE CHECK FOR A SUCESSFUL PARSE
		- If the current state in our machine == REJECT
		(the starting state), then we have sucessfully parsed
		a token, which is returned to its caller

		- ELSE we continue trying to find a sucessful token

		=================================================== */
		if (currentState == REJECT)
		{
			if (currentToken != " ") // we dont care about whitespace
			{
				tokens.push_back(currentToken);
			}
			currentToken = "";
		}
		else
		{
			currentToken += currentChar;
			++x;
		}

	}
	// this ensures the last token gets saved when
	// we reach the end of the postfix string buffer
	if (currentToken != " ") // we dont care about whitespace
	{
		tokens.push_back(currentToken);
	}
	return tokens;
}// end of Lexer

int Equation::Get_FSM_Col(char& currentChar)
{// this function determines the state of the type of character being examined
 // check for whitespace
	if (isspace(currentChar))
	{
		return SPACE;
	}

	// check for integer numbers
	else if (isdigit(currentChar))
	{
		return INTEGER;
	}

	// check for real numbers
	else if (currentChar == '.')
	{
		return REAL;
	}

	// check for negative numbers
	else if (currentChar == '~')
	{
		currentChar = '-';
		return NEGATIVE;
	}

	// check for math operators
	else if (IsMathOperator(currentChar))
	{
		return OPERATOR;
	}
	return UNKNOWN;
}// end of Get_FSM_Col

double Equation::EvaluatePostfix(const vector<string>& postfix)
{// this function evaluates a postfix expression
 // declare function variables
	double op1 = 0;
	double op2 = 0;
	double answer = 0;
	stack<double> doubleStack;

	cout << "Calculations:";

	// loop thru array until there is no more data
	for (unsigned x = 0; x < postfix.size(); ++x)
	{
		// push numbers onto the stack
		if ((isdigit(postfix[x][0])) || (postfix[x][0] == '.'))
		{
			doubleStack.push(atof(postfix[x].c_str()));
		}
		// push negative numbers onto the stack
		else if ((postfix[x].length() > 1) && ((postfix[x][0] == '-') &&
			(isdigit(postfix[x][1]) || (postfix[x][1] == '.'))))
		{
			doubleStack.push(atof(postfix[x].c_str()));
		}
		// if expression is a math operator, pop numbers from stack
		// & send the popped numbers to the 'Calculate' function
		else if (IsMathOperator(postfix[x][0]) && (!doubleStack.empty()))
		{
			char token = tolower(postfix[x][0]);

			// if expression is square root, sin, cos,
			// or tan operation only pop stack once
			if (token == '$' || token == 's' || token == 'c' || token == 't')
			{
				op2 = 0;
				op1 = doubleStack.top();
				doubleStack.pop();
				answer = Calculate(token, op1, op2);
				doubleStack.push(answer);
			}
			else if (doubleStack.size() > 1)
			{
				op2 = doubleStack.top();
				doubleStack.pop();
				op1 = doubleStack.top();
				doubleStack.pop();
				answer = Calculate(token, op1, op2);
				doubleStack.push(answer);
			}
		}
		else // this should never execute, & if it does, something went really wrong
		{
			cout << "INVALID INPUT #3";
			exit(1);
		}
	}
	// pop the final answer from the stack, and return to main
	if (!doubleStack.empty())
	{
		answer = doubleStack.top();
	}
	return answer;
}// end of EvaluatePostfix

double Equation::Calculate(char token, double op1, double op2)
{// this function carries out the actual math process
	double ans = 0;
	switch (tolower(token))
	{
	case '+':
		cout << op1 << token << op2 << " = ";
		ans = op1 + op2;
		break;
	case '-':
		cout << op1 << token << op2 << " = ";
		ans = op1 - op2;
		break;
	case '*':
		cout << op1 << token << op2 << " = ";
		ans = op1 * op2;
		break;
	case '/':
		cout << op1 << token << op2 << " = ";
		ans = op1 / op2;
		break;
	case '%':
		cout << op1 << token << op2 << " = ";
		ans = ((int)op1 % (int)op2) + modf(op1, &op2);
		break;
	case '^':
		cout << op1 << token << op2 << " = ";
		ans = pow(op1, op2);
		break;
	case '$':
		cout << char(251) << op1 << " = ";
		ans = sqrt(op1);
		break;
	case 'c':
		cout << "cos(" << op1 << ") = ";
		ans = cos(op1);
		break;
	case 's':
		cout << "sin(" << op1 << ") = ";
		ans = sin(op1);
		break;
	case 't':
		cout << "tan(" << op1 << ") = ";
		ans = tan(op1);
		break;
	default:
		ans = 0;
		break;
	}
	cout << ans << endl;
	return ans;
}
