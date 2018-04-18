//
//  main.cpp
//  Netlist Control
//
//  Created by Yanbo Deng on 10/16/2014
//  Copyright (c) 2014 Yanbo Deng. All rights reserved.
//
#include "data_struct.h"
#include "net_list.h"

//functions declaretion
void display_tokens(std::ostream &out,
	const std::list<evl_wire> &wires, const std::list<evl_component> &components, const std::string &module_name);
bool store_tokens_to_file(std::string file_name,
	const std::list<evl_wire> &wires, const std::list<evl_component> &components, const std::string &module_name);
bool extract_statement_from_tokens(const std::list<evl_token> &tokens,
	std::list<evl_wire> &wires, std::list<evl_component> &components, std::string &module_name);
evl_wires_table make_wires_table(const evl_wires &wires);

//main
int main(int argc, char *argv[]) {
	std::list<evl_token> tokens;
	std::list<evl_component> components;
	std::list<evl_wire> wires;
	std::string module_name;
	if (argv == NULL) {
		std::cerr << "argument does not exist!" << std::endl;
		return -1;
	}// verify that argv[1] exists
	std::string evl_file = argv[1];
	if (!extract_tokens_from_file(evl_file, tokens)) {
		return -1;
	}
	if (!extract_statement_from_tokens(tokens, wires, components, module_name)){
		return -1;
	}
	if (!store_tokens_to_file(evl_file + ".syntax", wires, components, module_name)) {
		return -1;
	}
	//parse_evl_file above

	evl_wires_table wires_table = make_wires_table(wires);

	netlist nl;
	if (!nl.create(wires, components, wires_table)){
		std::cout << "unable to creat netlist!" << std::endl;
		return -1;
	}
	std::string nl_file = std::string(argv[1]) + ".netlist";
	//nl.display(std::cout, module_name);
	nl.save(nl_file, module_name); // save the netlist 	
	nl.run(1000,evl_file);
	return 0;
}

