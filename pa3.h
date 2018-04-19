/*Maisha Nabila
COP3503 Section 19F2*/

#ifndef PA3_HEADER
#define PA3_HEADER

#include <string>
#include <set>

//Set of tokens
struct Token {
	std::string text;
	std::string type;
	Token(std::string text, std::string type) {
		this->text = text;
		this->type = type;
	}
};

//Set of lines
struct Line {
	std::vector<Token> tokens;
};

struct CodeBlock {
	std::vector<Line> lines;

	void add_line(Line l) {
		lines.push_back(l);
	}
};

struct Unexpected : Token {
	Unexpected(std::string in) : Token(in, "unexpected") {}
};

struct Identifier : Token {
	Identifier(std::string in) : Token(in, "identifier") {}
};

struct Constant : Token {
	Constant(std::string in) : Token(in, "constant") {}
};

struct Keyword : Token {
	Keyword(std::string in) : Token(in, "keyword") {}
};

struct Delimiter : Token {
	Delimiter(std::string in) : Token(in, "delimiter") {}
};

struct Operator : Token {
	Operator(std::string in, std::string type) : Token(in, type) {}
};

struct BinaryOperator : Operator {
	BinaryOperator(std::string in) : Operator(in, "binary_operator") {}
};

struct SelfOperator : Operator {
	SelfOperator(std::string in) : Operator(in, "self_operator") {}
};

struct LineWalker {
	std::vector<Token> tokens;
	std::vector<Token> missing;
	std::vector<Token> unexpected;
	int index;
	bool is_for_declaration;
	bool is_begin;
	bool is_end;
	LineWalker(std::vector<Token> tokens) {
		this->tokens = tokens;
		index = 0;
		is_for_declaration = false;
		is_begin = false;
		is_end = false;
	}
};

//Program Walker Class
class ProgramWalker {
private:
	int max_loop_depth;
	int num_for_declarations;
	int num_begins;
	int num_ends;
	std::set<std::string> keywords;
	std::set<std::string> identifier;
	std::set<std::string> constants;
	std::set<std::string> operators;
	std::set<std::string> delimiters;
	std::set<std::string> unexpected;
	std::set<std::string> missing;

	//Methods
public:
	ProgramWalker();
	int compare_n_begins_ends();

	void add_line(LineWalker lw);

	void insert_missing(std::string s);

	void print_loop_depth();
	void print_keywords();
	void print_identifiers();
	void print_constants();
	void print_operators();
	void print_delimiters();

	void print_syntax_errors();
};

#endif