/*Maisha Nabila
COP3503 Section 19F2*/

#include <cstdio>
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>

#include "pa3.h"

//Tests equality
bool eq_string(std::string a, std::string b) {
	return 0 == std::strcmp(a.c_str(), b.c_str());
}

//Tests type
bool type_is(std::vector<Token> &tokens, int index, std::string to_compare) {
	return eq_string(tokens.at(index).type, to_compare);
}

//Tests text
bool text_is(std::vector<Token> &tokens, int index, std::string to_compare) {
	return eq_string(tokens.at(index).text, to_compare);
}

ProgramWalker::ProgramWalker() {
	this->max_loop_depth = 0;
	this->num_for_declarations = 0;
	this->num_begins = 0;
	this->num_ends = 0;
}

int ProgramWalker::compare_n_begins_ends() {
	if (num_begins > num_ends) {
		return -1;
	}
	if (num_begins < num_ends) {
		return 1;
	}
	return 0;
}

void ProgramWalker::add_line(LineWalker lw) {
	if (lw.is_for_declaration) {
		this->num_for_declarations++;
		this->keywords.insert("FOR");
	}
	else if (lw.is_begin) {
		this->num_begins++;
		this->keywords.insert("BEGIN");
	}
	else if (lw.is_end) {
		this->num_ends++;
		this->keywords.insert("END");
	}
	this->max_loop_depth = std::max(this->max_loop_depth,
		this->num_for_declarations - this->num_ends - 1);
	for (size_t i = 0; i < lw.tokens.size(); i++) {
		if (type_is(lw.tokens, i, "identifier")) {
			this->identifier.insert(lw.tokens.at(i).text);
		}
		else if (type_is(lw.tokens, i, "constant")) {
			this->constants.insert(lw.tokens.at(i).text);
		}
		else if (type_is(lw.tokens, i, "binary_operator") ||
			type_is(lw.tokens, i, "self_operator")) {
			this->operators.insert(lw.tokens.at(i).text);
		}
		else if (type_is(lw.tokens, i, "delimiter")) {
			this->delimiters.insert(lw.tokens.at(i).text);
		}
		else if (type_is(lw.tokens, i, "unexpected")) {
			this->unexpected.insert(lw.tokens.at(i).text);
		}
	}
	for (size_t i = 0; i < lw.missing.size(); i++) {
		this->missing.insert(lw.missing.at(i).text);
	}
	for (size_t i = 0; i < lw.unexpected.size(); i++) {
		this->unexpected.insert(lw.unexpected.at(i).text);
	}
}

void ProgramWalker::insert_missing(std::string s) {
	this->missing.insert(s);
}

void ProgramWalker::print_loop_depth() {
	if (this->num_for_declarations == 0) {
		std::printf("There were no loops in this program\n");
	}
	else {
		std::printf("The depth of nested loop(s) is %d\n", max_loop_depth);
	}
}

void ProgramWalker::print_keywords() {
	std::printf("Keywords: ");
	for (std::set<std::string>::iterator it = this->keywords.begin();
		it != this->keywords.end(); ++it) {
		std::string curr = *it;
		std::printf("%s ", curr.c_str());
	}
	std::printf("\n");
}

void ProgramWalker::print_identifiers() {
	std::printf("Identifiers: ");
	for (std::set<std::string>::iterator it = this->identifier.begin();
		it != this->identifier.end(); ++it) {
		std::string curr = *it;
		std::printf("%s ", curr.c_str());
	}
	std::printf("\n");
}

void ProgramWalker::print_constants() {
	std::printf("Constants: ");
	for (std::set<std::string>::iterator it = this->constants.begin();
		it != this->constants.end(); ++it) {
		std::string curr = *it;
		std::printf("%s ", curr.c_str());
	}
	std::printf("\n");
}

void ProgramWalker::print_operators() {
	std::printf("Operators: ");
	for (std::set<std::string>::iterator it = this->operators.begin();
		it != this->operators.end(); ++it) {
		std::string curr = *it;
		std::printf("%s ", curr.c_str());
	}
	std::printf("\n");
}

void ProgramWalker::print_delimiters() {
	std::printf("Delimiters: ");
	for (std::set<std::string>::iterator it = this->delimiters.begin();
		it != this->delimiters.end(); ++it) {
		std::string curr = *it;
		if (!eq_string(curr, "(") && !eq_string(curr, ")")) {
			std::printf("%s ", curr.c_str());
		}
	}
	std::printf("\n");
}

void ProgramWalker::print_syntax_errors() {
	std::printf("Syntax Error(s): ");
	for (std::set<std::string>::iterator it = this->unexpected.begin();
		it != this->unexpected.end(); ++it) {
		std::string curr = *it;
		std::printf("%s ", curr.c_str());
	}
	std::printf("\n");
}

//Tests if in ascii range
bool in_range(char c, int lower, int upper) {
	return lower <= (int)(c) && (int)(c) <= upper;
}

//Tests if space
bool is_space(char c) {
	return c == ' ' || c == '\t';
}

//Test if numeric
bool is_numeric(char c) {
	return in_range(c, 48, 57);
}

//Tests operator
bool is_operator(char c) {
	return c == '+' || c == '-' || c == '*' || c == '/' || c == '=';
}

//Tests if letter
bool is_letter(char c) {
	return in_range(c, 65, 90) || in_range(c, 97, 122);
}

//Tests if all capital
bool is_all_capital(std::string s) {
	for (size_t i = 0; i < s.size(); i++) {
		if (!in_range(s[i], 65, 90)) {
			return false;
		}
	}
	return true;
}

//Tests if keyword
bool is_keyword(std::string s) {
	return eq_string(s, "FOR") || eq_string(s, "BEGIN") ||
		eq_string(s, "END");
}

//Tests if delimiter
bool is_delimiter(char c) {
	return c == '(' || c == ')' || c == ';' || c == ',';
}

void jump(std::string &s, int size) {
	s = s.substr(size);
}

void handle_number(std::string &to_parse, std::vector<Token> &tokens) {
	char curr = to_parse[0];
	std::string text(1, curr);
	jump(to_parse, 1);
	while (to_parse.size() > 0) {
		curr = to_parse[0];
		if (is_numeric(curr)) {
			text = text + curr;
			jump(to_parse, 1);
		}
		else {
			break;
		}
	}
	tokens.push_back(Constant(text));
}

void handle_operator(std::string &to_parse, std::vector<Token> &tokens) {
	char curr = to_parse[0];
	std::string text(1, curr);
	jump(to_parse, 1);
	if (curr == '+' && to_parse[0] == '+') {
		text = text + '+';
		jump(to_parse, 1);
		tokens.push_back(SelfOperator(text));
	}
	else {
		tokens.push_back(BinaryOperator(text));
	}
}

//If identifier or keyword
void handle_letter(std::string &to_parse, std::vector<Token> &tokens) {
	char curr = to_parse[0];
	std::string text(1, curr);
	jump(to_parse, 1);
	while (to_parse.size() > 0) {
		curr = to_parse[0];
		if (is_letter(curr)) {
			text += curr;
			jump(to_parse, 1);
		}
		else {
			break;
		}
	}
	if (is_all_capital(text)) {
		if (is_keyword(text)) {
			tokens.push_back(Keyword(text));
		}
		else {
			tokens.push_back(Unexpected(text));
		}
	}
	else {
		tokens.push_back(Identifier(text));
	}
}

//If delimiter
void handle_delimiter(std::string &to_parse, std::vector<Token> &tokens) {
	char curr = to_parse[0];
	std::string text(1, curr);
	jump(to_parse, 1);
	tokens.push_back(Delimiter(text));
}

void tokenize_line(std::string &to_parse, std::vector<Token> &tokens) {
	if (to_parse.size() == 0) {
		return;
	}
	char curr = to_parse[0];
	if (is_numeric(curr)) {
		handle_number(to_parse, tokens);
		return tokenize_line(to_parse, tokens);
	}
	else if (is_operator(curr)) {
		handle_operator(to_parse, tokens);
		return tokenize_line(to_parse, tokens);
	}
	else if (is_letter(curr)) {
		handle_letter(to_parse, tokens);
		return tokenize_line(to_parse, tokens);
	}
	else if (is_delimiter(curr)) {
		handle_delimiter(to_parse, tokens);
		return tokenize_line(to_parse, tokens);
	}
	else if (is_space(curr)) {
		jump(to_parse, 1);
		return tokenize_line(to_parse, tokens);
	}
	else {
		std::cerr << "Encountered unknown token: " << curr << std::endl;
		jump(to_parse, 1);
		return tokenize_line(to_parse, tokens);
	}
}

std::string read_line(std::ifstream &ifs) {
	char line[256];
	ifs.getline(line, 256);
	return std::string(line);
}

bool is_identifier(std::vector<Token> &tokens, int index) {
	return type_is(tokens, index, "identifier");
}

void handle_missing_token(LineWalker &lw, Token missing) {
	lw.missing.push_back(missing);
	std::vector<Token>::iterator it;
	it = lw.tokens.begin();
	std::advance(it, lw.index);
	lw.tokens.insert(it, missing);
}

void handle_unexpected_token(LineWalker &lw, Token unexpected) {
	lw.unexpected.push_back(unexpected);
}

void expect_left_paren(LineWalker &lw) {
	if (!text_is(lw.tokens, lw.index, "(")) {
		handle_missing_token(lw, Delimiter("("));
	}
	lw.index++;
}

void expect_right_paren(LineWalker &lw) {
	if (!text_is(lw.tokens, lw.index, ")")) {
		handle_missing_token(lw, Delimiter(")"));
	}
	lw.index++;
}

void expect_comma(LineWalker &lw) {
	if (!text_is(lw.tokens, lw.index, ",")) {
		handle_missing_token(lw, Delimiter(","));
	}
	lw.index++;
}

void expect_identifier(LineWalker &lw) {
	if (!type_is(lw.tokens, lw.index, "identifier")) {
		handle_missing_token(lw, Identifier("_unknown_"));
	}
	lw.index++;
}

void expect_constant(LineWalker &lw) {
	if (!type_is(lw.tokens, lw.index, "constant")) {
		handle_missing_token(lw, Constant("_unknown_"));
	}
	lw.index++;
}

void expect_operator(LineWalker &lw) {
	if (!type_is(lw.tokens, lw.index, "binary_operator") &&
		!type_is(lw.tokens, lw.index, "self_operator")) {
		handle_missing_token(lw, SelfOperator("_unknown_"));
	}
	lw.index++;
}

void expect_binary_operator(LineWalker &lw) {
	if (!type_is(lw.tokens, lw.index, "binary_operator")) {
		handle_missing_token(lw, BinaryOperator("_unknown_"));
	}
	lw.index++;
}

void expect_end_line(LineWalker &lw) {
	while (lw.index < (int)lw.tokens.size()) {
		handle_unexpected_token(lw, lw.tokens.at(lw.index));
		lw.index++;
	}
}

void expect_semicolon(LineWalker &lw) {
	if (!text_is(lw.tokens, lw.index, ";")) {
		handle_missing_token(lw, Delimiter(";"));
	}
	lw.index++;
}

void parse_for_declaration(LineWalker &lw) {
	lw.index++;
	expect_left_paren(lw);
	expect_identifier(lw);
	expect_comma(lw);
	expect_constant(lw);
	expect_comma(lw);
	expect_operator(lw);
	expect_right_paren(lw);
	expect_end_line(lw);
}

bool is_for_declaration(LineWalker &lw) {
	return text_is(lw.tokens, lw.index, "FOR");
}

bool is_begin(LineWalker &lw) {
	return text_is(lw.tokens, lw.index, "BEGIN");
}

bool is_end(LineWalker &lw) {
	return text_is(lw.tokens, lw.index, "END");
}

bool is_operable_token(LineWalker &lw) {
	return type_is(lw.tokens, lw.index, "constant") ||
		type_is(lw.tokens, lw.index, "identifier");
}

bool is_binary_operator(LineWalker &lw) {
	return type_is(lw.tokens, lw.index, "binary_operator");
}

void parse_statement(LineWalker &lw) {
	lw.index++;
	bool should_be_bin_op = true;
	bool missing_semicolon = !eq_string(lw.tokens.back().text, ";");
	int num_tokens = lw.tokens.size();
	int upper_limit = missing_semicolon ? num_tokens : num_tokens - 1;
	while (lw.index < upper_limit) {
		if (is_binary_operator(lw)) {
			if (!should_be_bin_op) {
				lw.unexpected.push_back(lw.tokens.at(lw.index));
			}
		}
		else if (is_operable_token(lw)) {
			if (should_be_bin_op) {
				lw.unexpected.push_back(lw.tokens.at(lw.index));
			}
		}
		lw.index++;
		should_be_bin_op = !should_be_bin_op;
	}
	if (!missing_semicolon) {
		expect_semicolon(lw);
	}
	else {
		lw.missing.push_back(Delimiter(";"));
	}
	expect_end_line(lw);
}

//Goes through each token
void parse_line(LineWalker &lw) {
	if (lw.tokens.size() == 0) {
		return;
	}
	if (is_for_declaration(lw)) {
		parse_for_declaration(lw);
		lw.is_for_declaration = true;
	}
	else if (is_begin(lw)) {
		lw.is_begin = true;
	}
	else if (is_end(lw)) {
		lw.is_end = true;
	}
	else if (is_operable_token(lw)) {
		parse_statement(lw);
	}
	else {
		while (lw.index < (int)lw.tokens.size()) {
			lw.unexpected.push_back(lw.tokens.at(lw.index));
			lw.index++;
		}
	}
}

//File to vector
CodeBlock tokenize_input(std::ifstream &ifs) {
	CodeBlock program;
	while (!ifs.eof()) {
		Line line;
		std::string to_parse = read_line(ifs);
		tokenize_line(to_parse, line.tokens);
		program.add_line(line);
	}
	return program;
}

//Print summary
void parse_program(ProgramWalker &pw, CodeBlock &program) {
	for (size_t i = 0; i < program.lines.size(); i++) {
		LineWalker lw(program.lines.at(i).tokens);
		parse_line(lw);
		pw.add_line(lw);
	}
	if (pw.compare_n_begins_ends() < 0) {
		pw.insert_missing("END");
	}
	else if (pw.compare_n_begins_ends() > 0) {
		pw.insert_missing("BEGIN");
	}
	pw.print_loop_depth();
	pw.print_keywords();
	pw.print_identifiers();
	pw.print_constants();
	pw.print_operators();
	pw.print_delimiters();
	pw.print_syntax_errors();
}

//Main Method
//Takes in text file through command argument
int main(int argc, char **argv) {
	std::ifstream ifs;
	ifs.open(argv[1], std::ifstream::in);
	CodeBlock program = tokenize_input(ifs);
	ProgramWalker pw;
	parse_program(pw, program);
	return 0;
}