/*
 *  UCF COP3330 Fall 2021 Assignment 6 Solution
 *  Copyright 2021 Carolina Duarte
 */

/*
Exercise 1:
Allow underscores in the calculator’s variable names.

Exercise 2:
Provide an assignment operator, =, so that you can change the value of a variable after you introduce it using let. 

Exercise 3:
Provide an assignment operator, =, so that you can change the value of a variable after you introduce it using let. 
*/

//User guide is provided from within the application once the user enters "h" or "help".

//Header file.
#include "std_lib_facilities.h"

//Token operators:
struct Token {
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { } //Operator.
	Token(char ch, double val) :kind(ch), value(val) { } //Operand.
	Token(char ch, string n) :kind(ch), name(n) { } //Variable's name.
};

//Tokens' stream:
class Token_stream {
	bool full;
	Token buffer; //Holds symbols. 
	string file;
public:
	Token_stream(istream& file) :full(0), buffer(0) { }

	Token get();
	void unget(Token t) { buffer=t; full=true; } //Returns symbols to buffer.

	void ignore(char); //Ignores unused characters.
};

const char assign = '='; //New value for variable.
const char constant = 'c'; //Defines user constant.
const char prompt = '>'; //New prompt.
const char quit = 'q'; 
const char help = 'h'; //Help for user manual.
const string result = "= "; 

const char let = 'L'; //Defines user variable.
const char print = ';'; //Prints result.
const char number = '8'; 
const char name = 'a'; //Variable indicator.
const char sqrts = 's'; //Square root.
const char pows = 'p'; //Power function.

//Gets characters from user:
Token Token_stream::get() 
{
	if (full) { full=false; return buffer; } //Return value from buffer if not empty.
	char ch;
	cin >> ch; //If empty, fill it from cin. 
	if (isspace(ch)) return Token(prompt); //New line, return prompt.
	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case ';':
	case '!':
		return Token(ch); //Return operator.
	case assign:
		return Token(assign); //Return assigned operator.
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{	
		cin.unget(); //Return number to stream.
		double val;
		cin >> val; 
		return Token(number,val);
	}
	default:
		if (isalpha(ch)) {
			string s;
			char ch2 = ch; 
			s += ch; 
			while(cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) { //Allows underscore in variable's name.
				s+=ch; 
			}
			cin.unget(); 
			if (s == "let") return Token(let); //Once variable is defined by user, return it.
			if (s == "sqrt") return Token(sqrts); //Square root definition.
			if (s == "const") return Token(constant); //Once constant is defined by user, return it.
			if (s == "pow") return Token(pows); //Power function.
			if (ch2 == quit || s == "quit") return Token(quit); //Quit program.
			if (ch2 == help || s == "help" || s =="Help") return Token(help); //User help guide.
			return Token(name,s); 
		}
		error("Bad token!");
	}
	return 0;
}

void Token_stream::ignore(char c) 
{
	if (full && c==buffer.kind) { 
		full = false;
		return;
	}
	full = false; 
	char ch;
	while (cin>>ch) 
		if (ch==c) return; //Return c once found. 
}

struct Variable { //Struct for variable names and values.
	string name; 
	double value; 
	char constant; 
	Variable(string n, double v) :name(n), value(v) { }
	Variable(string n, double v, char c) :name(n), value(v), constant(c) { } //Holds constant names and values. 
};

class Symbol_table {
	vector<Variable> names;
	public:
		double get_value(string s);
		void set_value(string s, double d);
		bool is_declared(string s);
		double define_name(char type);
};

double Symbol_table::get_value(string s) //Get for variable's value.
{
	for (Variable i: names) {
		if (i.name == s) return i.value;
	}
	error("Undefined name.",s);
	return 0;
}

void Symbol_table::set_value(string s, double d) //Set for variable's value.
{
	for (int i = 0; i<=names.size(); ++i)
		if (names[i].constant == constant) error("Constant assignment."); 
		else if (names[i].name == s) {	
			names[i].value = d;
			return;
		}
	error("Undefined name.",s);
}

bool Symbol_table::is_declared(string s) //Confirm if variable has already been declared or not.
{
	for (int i = 0; i<names.size(); ++i)
		if (names[i].name == s) return true;
	return false;
}

Token_stream ts(cin); //Tokens.

Symbol_table names; //Variables.

double expression(Token_stream&); 

double primary(Token_stream&); 

double squareroot(){ //Square root function.
	double d = expression(ts);
	if (d < 0) error("Square root of negative number is not allowed!"); //Negative number error catch.
	return sqrt(d);
}

double pow(){ //Power function.
	Token t = ts.get();
	if (t.kind == '(') {
		double lval = expression(ts);
		int rval = 0;
		t = ts.get();
		if(t.kind == ',') rval = narrow_cast<int>(primary(ts));
		else error("Second argument is not provided!");
		double result = 1;
		for(double i = 0; i < rval; i++) {
			result*=lval;
		}
		t = ts.get();
		if (t.kind != ')') error("')' missing!"); //Parathensis expected.
			return result;
	}
	else error("'(' missing!"); //Parathensis expected.
	return 0;
}

double primary(Token_stream& ts) //Processes characters.
{
	Token t = ts.get(); 
	switch (t.kind) {
	case '(': 
	{	double d = expression(ts); 
		t = ts.get(); 
		if (t.kind != ')') error("')' missing!"); 
		return d;
	}
	case sqrts: 
	{
		return squareroot();
	}
	case pows:
	{
		return pow();
	}
	case '-': //Negative digits.
		return - primary(ts); 
	case number: 
		if(narrow_cast<int>(t.value)) return t.value; 
	case name: 
	{	
		string s = t.name; //Save variable's name.
		t = ts.get();
		if (t.kind == assign) names.set_value(s,expression(ts)); 
		else ts.unget(t);
		return names.get_value(s); //Return variable's value.
	}
	case help:
		return primary(ts);
	default:
		error("Unlisted operation."); //Error message for unlisted character.
	}
	return 0;
}

double term(Token_stream& ts) //Performs operations.
{	
	double left = primary(ts); 
	while(true) {
		Token t = ts.get(); 
		switch(t.kind) {
		case '*': 
			left *= primary(ts); 
			break;
		case '/': 
		{	double d = primary(ts); 
			if (d == 0) error("Division by zero is not allowed!"); //Error for division by zero.
			left /= d;
			break;
		}
        case '!': 
            {   
                int x = left;
                for (int i = 1; i < left; i++) { 
                    x*=i;
                }
                if (x == 0) left = 1;
                else left = x;
                break;
            }        
        case '%':
            {
                double d = primary(ts);
                if(d == 0) error("Division by zero is not allowed!");
                left = fmod(left,d); //Mod.
                break;
            }
		default:
			ts.unget(t); 
			if (left == -0) return 0; //Using -0 and 0 to make a negative digit positive or keep it negative.
			return left; 
		}

	}
}

double expression(Token_stream& ts) //Performs further operations.
{
	double left = term(ts); 
	while(true) {
		Token t = ts.get();
		switch(t.kind) {
		case '+':
			left += term(ts); 
			break;
		case '-':
			left -= term(ts); //Subtraction. 
			break;
		default:
			ts.unget(t); 
			return left; 
		}
	}
}

//Defines a variable:
double Symbol_table::define_name(char type) 
{
	Token t = ts.get(); 
	if (t.kind != 'a') error ("Name missing in declaration."); //Name expected error.
	string name = t.name; 
	if (is_declared(name)) error(name, " declared twice!"); 
	Token t2 = ts.get(); 
	if (t2.kind != '=') error("= missing in definition of " ,name); //Equal sign expected error.
	double d = expression(ts);
	switch(type) {
	case let:
		names.push_back(Variable(name,d)); //New variable into vector.
		return d; 
	case constant:
		names.push_back(Variable(name,d,constant)); //New constant into vector.
		return d; 
	}
	return 0;
}


double statement() 
{
	Token t = ts.get();
	switch(t.kind) {
	case let: 
		return  names.define_name(let);
	case constant:
		return names.define_name(constant);
	default: 
		ts.unget(t);
		return expression(ts);
	}
}

void clean_up_mess() //All characters before ';' are ignored.
{
	ts.ignore(print);
}

//Calculations:
void calculate() 
{
	
	while(true) try {
		cout << prompt; 
		Token t = ts.get(); 
		if (t.kind == help) { 
		//USER GUIDE INFO:
		cout << "Your Calculator Guide: << List of commands:\n"
			"Operators are '+','-','/','*'. Floating-point numbers.\n"
			"'User input expression' + ';' - Prints the result, needs to be used everytime!\n"
			"'let' + 'name of variable' + '='' + 'value' - variable definition.\n"
			"'const' + 'name of constant' + '='' + 'value' - constant definition.\n"
			"'name of variable' + '=' + 'new value' - new variable's value.\n"
			"'pow(x,y)' - power function, y of type int only!\n"
			"'sqrt(x)' - square root function.\n"
			"'h' or 'help' - user guide request.\n"
			"'quit' - exit.\n";
			continue; 
		}
		while (t.kind == print) t=ts.get(); 
		if (t.kind == quit) return; 
		ts.unget(t); 
		cout << result << statement() << endl; 
	}
	catch(runtime_error& e) {
		cerr << e.what() << endl; 
		clean_up_mess(); 
	}
}

int main()
	try {	
		calculate(); //Calculations.
		return 0;
	}
	catch (exception& e) { 
		cerr << "exception: " << e.what() << endl; //Outputs appropriate error message depending on error.
		char c;
		while (cin >>c&& c!=';');
		return 1;
	}
	catch (...) { 
		cerr << "exception\n";
		char c;
		while (cin>>c && c!=';');
		return 2;
	}