//sim.cpp
#include "net_list.h"
int n_ceil(std::string origin){
	int result;
	if (origin.length() % 4 != 0)
		result = origin.length() / 4 + 1;
	else
		result = origin.length() / 4;
	return result;
}
std::string binary_to_hex(std::string origin){
	std::string out;
	std::bitset<32> set(origin);
	std::stringstream res;
	res << std::hex << std::uppercase << set.to_ulong();
	res >> out;
	if (origin.length() / 4 != 0)
		while (out.length() % n_ceil(origin) != 0)
			out = "0" + out;

	return out;
}

std::string hex_to_binary(std::string origin){
	char *temp = const_cast<char *>(origin.c_str());
	std::string out;
	for (size_t i = 0; i < origin.length(); ++i){
		switch (temp[i]){
		case '0': out = out + "0000"; break;
		case '1': out = out + "0001"; break;
		case '2': out = out + "0010"; break;
		case '3': out = out + "0011"; break;
		case '4': out = out + "0100"; break;
		case '5': out = out + "0101"; break;
		case '6': out = out + "0110"; break;
		case '7': out = out + "0111"; break;
		case '8': out = out + "1000"; break;
		case '9': out = out + "1001"; break;
		case 'a': out = out + "1010"; break;
		case 'b': out = out + "1011"; break;
		case 'c': out = out + "1100"; break;
		case 'd': out = out + "1101"; break;
		case 'e': out = out + "1110"; break;
		case 'f': out = out + "1111"; break;
		default: std::cerr << "error binary input" << std::endl;
		}
	}
	char *temp2 = const_cast<char *>(out.c_str());
	std::string out2 = "";
	bool ctr = false;
	for (size_t i = 0; i < out.length(); ++i){
		if (temp2[i] != '0')  ctr = true;
		if (ctr){
			out2 = out2 + temp2[i];
		}
	}
	if (out2 == "") out2 = "0";
	return out2;
}

void pin::set_as_output(){
	port = "output";
}

void pin::set_as_input(){
	port = "input";
}

std::vector<net *> pin::get_net(){
	return nets_;
}

bool evl_and::connect(){
	assert(pins_.size() >= 3);
	pins_[0]->set_as_output();
	for (size_t i = 1; i < pins_.size(); ++i){
		pins_[i]->set_as_input();
	}
	return true;
}

bool evl_or::connect(){
	assert(pins_.size() >= 3);
	pins_[0]->set_as_output();
	for (size_t i = 1; i < pins_.size(); ++i){
		pins_[i]->set_as_input();
	}
	return true;
}

bool evl_xor::connect(){
	assert(pins_.size() >= 3);
	pins_[0]->set_as_output();
	for (size_t i = 1; i < pins_.size(); ++i){
		pins_[i]->set_as_input();
	}
	return true;
}

bool evl_not::connect(){
	assert(pins_.size() == 2);
	pins_[0]->set_as_output();
	pins_[1]->set_as_input();
	return true;
}

bool evl_buf::connect(){
	assert(pins_.size() == 2);
	pins_[0]->set_as_output();
	pins_[1]->set_as_input();
	return true;
}

bool evl_one::connect(){
	assert(pins_.size() > 0);
	for (size_t i = 0; i < pins_.size(); ++i){
		pins_[i]->set_as_output();
	}
	return true;
}

bool evl_zero::connect(){
	assert(pins_.size() > 0);
	for (size_t i = 0; i < pins_.size(); ++i){
		pins_[i]->set_as_output();
	}
	return true;
}

bool evl_dff::connect() {
	if (pins_.size() != 3) return false;
	pins_[0]->set_as_output();
	pins_[1]->set_as_input();
	pins_[2]->set_as_input();
	return true;
}

bool evl_output::connect(){
	if (pins_.size() <= 0) return false;
	for (size_t i = 0; i < pins_.size(); ++i){
		pins_[i]->set_as_input();
	}
	return true;
}

bool evl_input::connect(){
	if (pins_.size() <= 0) return false;
	for (size_t i = 0; i < pins_.size(); ++i){
		pins_[i]->set_as_output();
	}
	return true;
}

bool evl_lut::connect(){
	if (pins_.size() <= 0) return false;
	for (size_t i = 0; i < pins_.size(); ++i){
		pins_[i]->set_as_input();
	}
	return true;
}

bool net::retrieve_value(std::map<net *, bool> &net_value, std::string &evl_file){//retrieve=======================================================
	for (std::list<pin *>::iterator it = connections_.begin(); it != connections_.end(); ++it){
		if ((*it)->get_port() == "output"){
			((*it)->get_gate())->run(net_value, evl_file);
			return true;
		}
	}
	return false;
}

void netlist::run(int run_time, std::string &evl_file){
	std::string out_name;
	int runtime = 0;

	for (std::list<gate *>::iterator it = gates_.begin(); it != gates_.end(); ++it)
			(*it)->ini_display(evl_file);

	while (runtime < run_time) {
		for (std::list<gate *>::iterator it = gates_.begin(); it != gates_.end(); ++it){
			//if ((*it)->get_gate_type() == "evl_output")
				(*it)->run(net_value, evl_file);
		}
		for (std::list<gate *>::iterator it = gates_.begin(); it != gates_.end(); ++it){
			if ((*it)->get_gate_type() == "evl_dff")
				(*it)->change_state();
		}
		++runtime;
		for (std::list<net *>::iterator it = nets_.begin(); it != nets_.end(); ++it)
			(*it)->set_valued(false);
	}
}

bool evl_and::run(std::map<net *, bool> &net_value, std::string &evl_file){
	bool output = true;
	for (size_t i = 1; i < pins_.size(); ++i){
		std::vector<net *> nets = pins_[i]->get_net();
		for (size_t j = 0; j < nets.size(); ++j){
			if (nets[j]->get_valued() == false)
				nets[j]->retrieve_value(net_value, evl_file);
			if (net_value[nets[j]] == false)
				output = false;
		}
	}
	std::vector<net *> nets = pins_[0]->get_net();
	for (size_t j = 0; j < nets.size(); ++j){
		net_value[nets[j]] = output;
		nets[j]->set_valued(true);
	}
	return true;
}

void evl_output::ini_display(std::string &evl_file){
	std::string out_name = evl_file + "." + get_gate_name() + ".evl_output";
	std::ofstream out(out_name.c_str());
	out << pins_.size() << std::endl;
	for (size_t i = 0; i < pins_.size(); ++i){
		out << pins_[i]->get_net().size() << std::endl;
	}
}

bool evl_output::run(std::map<net *, bool> &net_value, std::string &evl_file){
	std::string out_name = evl_file + "." + get_gate_name() + ".evl_output";
	std::ofstream out(out_name.c_str(),std::ios::app);

	for (size_t i = 0; i < pins_.size(); ++i){
		std::vector<net *> nets = pins_[i]->get_net();
		if (nets.size() != 1){//output bus
			std::string net_bus_value = "";
			for (size_t j = 0; j < nets.size(); ++j){
				if (nets[j]->get_valued() == false)
					nets[j]->retrieve_value(net_value, evl_file);
				
				if (net_value[nets[j]])
					net_bus_value = "1" + net_bus_value;
				else
					net_bus_value = "0" + net_bus_value;
			}
			out << binary_to_hex(net_bus_value) << " ";
		}//end if
		else{
			if (nets[0]->get_valued() == false)
				nets[0]->retrieve_value(net_value, evl_file);
			out << net_value[nets[0]] << " ";
		}//end else
	}
	out << std::endl;
	return true;
}

bool evl_zero::run(std::map<net *, bool> &net_value, std::string &evl_file){
	for (size_t i = 0; i < pins_.size(); ++i){
		std::vector<net *> nets = pins_[i]->get_net();
		for (size_t j = 0; j < nets.size(); ++j){
			nets[j]->set_valued(true);
			net_value[nets[j]] = false;
		}
	}
	return true;
}

bool evl_one::run(std::map<net *, bool> &net_value, std::string &evl_file){
	for (size_t i = 0; i < pins_.size(); ++i){
		std::vector<net *> nets = pins_[i]->get_net();
		for (size_t j = 0; j < nets.size(); ++j){
			nets[j]->set_valued(true);
			net_value[nets[j]] = true;
		}
	}
	return true;
}

bool evl_not::run(std::map<net *, bool> &net_value, std::string &evl_file){
	bool output = true;
	assert(pins_.size() == 2);
	std::vector<net *> nets = pins_[1]->get_net();
	assert(nets.size() == 1);
	if (nets[0]->get_valued() == false)
		nets[0]->retrieve_value(net_value, evl_file);//to be continued
	if (net_value[nets[0]] == true)
		output = false;

	std::vector<net *> nets_out = pins_[0]->get_net();
	assert(nets_out.size() == 1);
	net_value[nets_out[0]] = output;
	nets_out[0]->set_valued(true);
	return true;
}

bool evl_dff::run(std::map<net *, bool> &net_value, std::string &evl_file){
	assert(pins_.size() == 3);
	std::vector<net *> nets_in = pins_[1]->get_net();
	std::vector<net *> nets_out = pins_[0]->get_net();
	assert(nets_out.size() == 1);
	assert(nets_in.size() == 1);
	net_value[nets_out[0]] = state;
	nets_out[0]->set_valued(true);
	if (nets_in[0]->get_valued() == false)
		nets_in[0]->retrieve_value(net_value, evl_file);
		next_state = net_value[nets_in[0]];
	return true;
}

void evl_dff::change_state(){
	state = next_state;
	next_state = false;
}

bool evl_buf::run(std::map<net *, bool> &net_value, std::string &evl_file){
	bool output = false;
	assert(pins_.size() == 2);
	std::vector<net *> nets = pins_[1]->get_net();
	assert(nets.size() == 1);
	if (nets[0]->get_valued() == false)
		nets[0]->retrieve_value(net_value, evl_file);//to be continued
	if (net_value[nets[0]] == true)
		output = true;

	std::vector<net *> nets_out = pins_[0]->get_net();
	assert(nets_out.size() == 1);
	net_value[nets_out[0]] = output;
	nets_out[0]->set_valued(true);
	return true;
}

bool evl_or::run(std::map<net *, bool> &net_value, std::string &evl_file){
	bool output = false;
	for (size_t i = 1; i < pins_.size(); ++i){
		std::vector<net *> nets = pins_[i]->get_net();
		for (size_t j = 0; j < nets.size(); ++j){
			if (nets[j]->get_valued() == false)
				nets[j]->retrieve_value(net_value, evl_file);
			if (net_value[nets[j]] == true)
				output = true;
		}
	}
	std::vector<net *> nets_out = pins_[0]->get_net();
	assert(nets_out.size() == 1);
	net_value[nets_out[0]] = output;
	nets_out[0]->set_valued(true);
	return true;
}

bool evl_xor::run(std::map<net *, bool> &net_value, std::string &evl_file){
	int count_even = 0;
	for (size_t i = 1; i < pins_.size(); ++i){
		std::vector<net *> nets = pins_[i]->get_net();
		for (size_t j = 0; j < nets.size(); ++j){
			if (nets[j]->get_valued() == false)
				nets[j]->retrieve_value(net_value, evl_file);
			if (net_value[nets[j]] == true)
				++count_even;
		}
	}
	std::vector<net *> nets_out = pins_[0]->get_net();
	assert(nets_out.size() == 1);
	if (count_even % 2 == 0)
		net_value[nets_out[0]] = false;
	else
		net_value[nets_out[0]] = true;
	nets_out[0]->set_valued(true);
	return true;
}

bool evl_input::run(std::map<net *, bool> &net_value, std::string &evl_file){

	int tokens_in_one_line = pins_.size() + 1;
	if (line_no == 0){
		std::string in_name = evl_file + "." + get_gate_name() + ".evl_input";
		std::list<std::string> tokens;

		if (!extract_input_from_file(in_name, tokens)) {
			std::cerr << "cannot find input file" << std::endl;
		}
		for (std::list<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it){
			pin_input.push_back(*it);
		}
		assert(atoi(pin_input.front().c_str()) == pins_.size());
		pin_input.pop_front();
		for (size_t i = 0; i < pins_.size(); ++i){
			assert(atoi(pin_input.front().c_str()) == pins_[i]->get_net().size());
			pin_input.pop_front();
		}
		++line_no;
	}
	if (!pin_input.empty()){
		//number of transitions
		if (count == total){
			count = 0;
			total = atoi(pin_input.front().c_str());
			pin_input.pop_front();
			while (!temp.empty()){
				temp.pop_back();
			}
			for (size_t i = 0; i < pins_.size(); ++i){
				temp.push_back(pin_input.front());//store pin_input into temp
				pin_input.pop_front();
			}
			++count;
		}
		else{
			++count;
		}
		//pin_input
		for (size_t i = 0; i < pins_.size(); ++i){
			std::vector<net *> nets = pins_[i]->get_net();
			std::string s = hex_to_binary(temp[i]);
			while (s.length() != nets.size()){
				s = "0" + s;
			}
			int len = s.length();
			char *pin_v = const_cast<char *>(s.c_str());
			for (size_t j = 0; j < nets.size(); ++j){
				if (pin_v[len - j - 1] == '0')
					net_value[nets[j]] = false;
				else if (pin_v[len - j - 1] == '1')
					net_value[nets[j]] = true;
				else std::cerr << "error pin value" << std::endl;
				nets[j]->set_valued(true);
			}
		}
		++line_no;
	}
	else{
		//set rest all 0
		for (size_t i = 0; i < pins_.size(); ++i){
			std::vector<net *> nets = pins_[i]->get_net();
			for (size_t j = 0; j < nets.size(); ++j){
				net_value[nets[j]] = false;
				nets[j]->set_valued(true);
			}
		}
	}//end else
	return true;
}
