#ifndef data_struct
#define data_struct

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <sstream>
#include <vector>
#include <map>
#include <bitset>

struct evl_token
{
	std::string type;
	std::string name;
};
typedef std::list<evl_token> evl_tokens;

struct evl_wire{
	std::string name;
	int width;
};
typedef std::list<evl_wire> evl_wires;

struct evl_pin{
	std::string name;
	int bus_msb;
	int bus_lsb;
};
typedef std::list<evl_pin> evl_pins;

struct evl_component{
	std::string type;
	std::string name;
	std::list<evl_pin> pin;
};
typedef std::list<evl_component> evl_components;

typedef std::map<std::string, int> evl_wires_table;

struct tris_type{
	std::string type;
};

bool extract_tokens_from_file(std::string file_name,
	std::list<evl_token> &tokens);
bool extract_input_from_file(std::string file_name,
	std::list<std::string> &tokens);

#endif // !data_struct
