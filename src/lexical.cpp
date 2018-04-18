#include "data_struct.h"

bool extract_tokens_from_line(std::string line, int line_no,
	std::list<evl_token> &tokens) { // use reference to modify it
	evl_token token;
	for (size_t i = 0; i < line.size();) {
		if (line[i] == '/') {
			++i;
			if ((i == line.size()) || (line[i] != '/')) {
				std::cerr << "LINE " << line_no
					<< ": a single / is not allowed" << std::endl;
				return false;
			}
			break; // skip the rest of the line by exiting the loop
		}
		// We should generate at most one token per iteration here

		else if (isspace(line[i]) || line[i] == '\n' || line[i] == '\r' || line[i] == '\t') { // skip spaces
			++i;
			continue; // skip the rest of the iteration
		}
		else if ((line[i] == '(') || (line[i] == ')') || (line[i] == '[') || (line[i] == ']')
			|| (line[i] == ':') || (line[i] == ';') || (line[i] == ',')) { // a SINGLE token
			token.name = std::string(1, line[i]);
			token.type = "SINGLE";
			tokens.push_back(token);
			++i;
			continue; // skip the rest of the iteration) {
		}
		else if (isalpha(line[i]) || (line[i] == '_')) {//a NAME token

			int name_begin = i;
			for (++i; isalpha(line[i]) || (line[i] == '_') || isdigit(line[i]) || (line[i] == '$'); ++i);
			token.name = line.substr(name_begin, i - name_begin);
			token.type = "NAME";
			tokens.push_back(token);
			continue; // skip the rest of the iteration) {
		}
		else if (isdigit(line[i])) {

			int number_begin = i;
			for (; isdigit(line[i]); ++i);
			token.name = line.substr(number_begin, i - number_begin);
			token.type = "NUMBER";
			tokens.push_back(token);
			continue; // skip the rest of the iteration) {
		}
		else {

			std::cerr << "LINE " << line_no
				<< ": invalid character" << std::endl;
			return false;
		}
	}
	return true; // nothing left
}

bool extract_tokens_from_file(std::string file_name,
	std::list<evl_token> &tokens) { // use reference to modify it
	evl_token token;
	std::ifstream input_file(file_name.c_str());
	if (!input_file) {
		std::cerr << "I can't read " << file_name << "." << std::endl;
		return false;
	}
	tokens.clear(); // be defensive, make it empty
	std::string line;
	for (int line_no = 1; std::getline(input_file, line); ++line_no) {
		if (!extract_tokens_from_line(line, line_no, tokens)) {
			return false;
		}
	}
	return true;
}

bool extract_input_from_line(std::string line, int line_no,
	std::list<std::string> &tokens) { // use reference to modify it
	for (size_t i = 0; i < line.size();) {
		if (line[i] == '/') {
			++i;
			if ((i == line.size()) || (line[i] != '/')) {
				std::cerr << "LINE " << line_no
					<< ": a single / is not allowed" << std::endl;
				return false;
			}
			break; // skip the rest of the line by exiting the loop
		}
		// We should generate at most one token per iteration here

		else if (isspace(line[i]) || line[i] == '\n' || line[i] == '\r' || line[i] == '\t') { // skip spaces
			++i;
			continue; // skip the rest of the iteration
		}
		else if (isdigit(line[i]) || isalpha(line[i])) {

			int number_begin = i;
			for (; (isdigit(line[i]) || isalpha(line[i])); ++i);
			tokens.push_back(line.substr(number_begin, i - number_begin));
			continue; // skip the rest of the iteration) {
		}
		else {
			std::cerr << "LINE " << line_no
				<< ": invalid character" << std::endl;
			return false;
		}
	}
	return true; // nothing left
}

bool extract_input_from_file(std::string file_name,
	std::list<std::string> &tokens) { // use reference to modify it
	evl_token token;
	std::ifstream input_file(file_name.c_str());
	if (!input_file) {
		std::cerr << "I can't read " << file_name << "." << std::endl;
		return false;
	}
	tokens.clear(); // be defensive, make it empty
	std::string line;
	for (int line_no = 1; std::getline(input_file, line); ++line_no) {
		if (!extract_input_from_line(line, line_no, tokens)) {
			return false;
		}
	}
	return true;
}
