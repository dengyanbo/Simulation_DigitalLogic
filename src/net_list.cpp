//net_list.cpp
#include "net_list.h"

net::net(std::string name, int w) {
	net_name = name; width = w; valued = false;
}//class net constructor

evl_wires_table make_wires_table(const evl_wires &wires) {
	evl_wires_table wires_table;
	for (evl_wires::const_iterator it = wires.begin();
		it != wires.end(); ++it) {
		wires_table.insert(std::make_pair(it->name, it->width));
	}
	return wires_table;
}

std::string make_net_name(std::string wire_name, int i, int width) {
	//assert(index >= 0);
	std::ostringstream oss;
	if (width <= 1)
		oss << wire_name;
	else
		oss << wire_name << "[" << i << "]";
	return oss.str();
}

int net::get_width(){
	return width;
}

std::string net::get_net_name(){
	return net_name;
}

//display member function
void net::display(std::ostream &out){
	out << "  net " << net_name << " " << connections_.size() << std::endl;
	for (std::list<pin *>::const_iterator ep = connections_.begin();
		ep != connections_.end(); ++ep){
		(*ep)->display_for_net(out);
	}
}

void net::display_name_for_gate(std::ostream &out){
	out << " " << net_name;
}

void pin::display_for_net(std::ostream &out){
	gate_->display_type_name(out);
	out << " " << pin_index_ << std::endl;
}

void pin::display_for_gate(std::ostream &out){
	out << "    pin " << nets_.size();
	for (std::vector<net *>::const_iterator itnet = nets_.begin();
		itnet != nets_.end(); ++itnet){
		(*itnet)->display_name_for_gate(out);
	}
	out << std::endl;
}

void gate::display(std::ostream &out){
	if (gate_name != "")
		out << "  component " << gate_type << " " << gate_name << " "
		<< pins_.size() << std::endl;
	else
		out << "  component " << gate_type << " " << pins_.size() << std::endl;
	for (std::vector<pin *>::const_iterator ep = pins_.begin();
		ep != pins_.end(); ++ep){
		(*ep)->display_for_gate(out);
	}
}

void gate::display_type_name(std::ostream &out){
	if (gate_name != "")
		out << "  " << gate_type << " " << gate_name;
	else
		out << "  " << gate_type;
}

void netlist::display(std::ostream &out, std::string module_name){
	out << "module " << module_name << std::endl;
	out << "nets " << nets_.size() << std::endl;
	for (std::list<net *>::const_iterator itnet = nets_.begin();
		itnet != nets_.end(); ++itnet){
		(*itnet)->display(out);
	}
	out << "components " << gates_.size() << std::endl;
	for (std::list<gate *>::const_iterator itgate = gates_.begin();
		itgate != gates_.end(); ++itgate){
		(*itgate)->display(out);
	}
}

//net member function
void net::append_pin(pin *p) {
	connections_.push_back(p);
}
//pin menber function
bool pin::create(gate *g, size_t pin_index, const evl_pin &p,
	const std::map<std::string, net *> &nets_table) {
	gate_ = g;
	pin_index_ = pin_index;//store g and pin_index;
	if (p.bus_msb == -1) { // a 1-bit wire
		std::string net_name = make_net_name(p.name, p.bus_msb, p.bus_msb + 1);
		if (nets_table.find(net_name) == nets_table.end()) {//net_name is not in the nets_table
			for (int i = 0;; ++i){
				std::string net_bus_name = make_net_name(p.name, i, 2);
				if (nets_table.find(net_bus_name) != nets_table.end()){//net_bus_name is in the nets_table
					std::map<std::string, net *>::const_iterator itnet = nets_table.find(net_bus_name);
					nets_.push_back(itnet->second);
					nets_.back()->append_pin(this);
					
					continue;
				}
				else if (i = 0){//no net_bus_name is in the nets_table
					std::cerr << "do not find net_bus_name in nets_table" << std::endl;
					return false;
				}
				else{//end of store nets in pin
					break;
				}//end branches
			}//end loop
		} 
		else {//net_name is in the nets_table
			std::map<std::string, net *>::const_iterator itnet = nets_table.find(net_name);//find net_name in nets_table
			if (itnet != nets_table.end())
				nets_.push_back(itnet->second);
			else{
				std::cerr << "do not find net_name in nets_table" << std::endl;
				return false;
			}
			nets_.back()->append_pin(this);//net->append_pin(pin);
		}//end else
		
	}
	else if (p.bus_msb != -1 && p.bus_lsb == -1) { // a bus
		std::string net_name = make_net_name(p.name, p.bus_msb, 2);
		assert(nets_table.find(net_name) != nets_table.end());
		std::map<std::string, net *>::const_iterator itnet = nets_table.find(net_name);//find net_name in nets_table
		nets_.push_back(itnet->second);	
		nets_.back()->append_pin(this);//net->append_pin(pin);
	}
	else if (p.bus_msb != -1 && p.bus_lsb != -1){
		for (int i = p.bus_lsb; i <= p.bus_msb; ++i){
			std::string net_name = make_net_name(p.name, i, p.bus_msb + 1);
			assert(nets_table.find(net_name) != nets_table.end());
			std::map<std::string, net *>::const_iterator itnet = nets_table.find(net_name);//find net_name in nets_table
			nets_.push_back(itnet->second);
			nets_.back()->append_pin(this);//net->append_pin(pin);
		}
	}
	else{
		std::cerr << "pin bus error when creating a pin" << std::endl;
		return false;
	}
	return true;//needed to be changed
}
//gate menber function
bool gate::create_pin(const evl_pin &ep, size_t pin_index,
	const std::map<std::string, net *> &nets_table,
	const evl_wires_table &wires_table) {
	evl_wires_table::const_iterator it = wires_table.find(ep.name);//resolve semantics of ep using wires_table;
	if (it == wires_table.end()){
		std::cerr << "can not find this pin in wires_table" << std::endl;
		return false;
	}

	pin *p = new pin;
	pins_.push_back(p);	
	return p->create(this, pin_index, ep, nets_table);;
}

bool gate::create(const evl_component &c,
	const std::map<std::string, net *> &nets_table,
	const evl_wires_table &wires_table) {
	gate_type = c.type;
	gate_name = c.name;//store gate type and name
	size_t pin_index = 0;
	for (evl_pins::const_iterator ep = (c.pin).begin(); ep != (c.pin).end(); ++ep) {//each evl_pin ep in c
		create_pin(*ep, pin_index, nets_table, wires_table);
		++pin_index;
	}
	return connect();// validate_structural_semantics();
}

//netlist member function
void netlist::create_net(const std::string &net_name, int i, int width, std::map<net *, bool> &net_value) {
	assert(nets_table_.find(net_name) == nets_table_.end());//no net_name is in the nets_table_
	std::string net_name_detail = make_net_name(net_name, i, width);
	net *n = new net(net_name_detail, width);
	nets_table_[net_name_detail] = n;
	nets_.push_back(n);
	net_value[n]=NULL;
}

bool netlist::create_nets(const evl_wires &wires, std::map<net *, bool> &net_value) {
	for (evl_wires::const_iterator w = wires.begin();
		w != wires.end(); ++w){
		for (int i = 0; i < w->width; ++i) {
			create_net(w->name, i, w->width, net_value);
		}//end inner loop
	}//end whole loop
	return true;
}

bool netlist::create_gate(const evl_component &c,
	const evl_wires_table &wires_table) {
	if (c.type == "and"){
		gates_.push_back(new evl_and);
	}
	else if (c.type == "or"){
		gates_.push_back(new evl_or);
	}
	else if (c.type == "xor"){
		gates_.push_back(new evl_xor);
	}
	else if (c.type == "not"){
		gates_.push_back(new evl_not);
	}
	else if (c.type == "buf"){
		gates_.push_back(new evl_buf);
	}
	else if (c.type == "evl_one"){
		gates_.push_back(new evl_one);
	}
	else if (c.type == "evl_zero"){
		gates_.push_back(new evl_zero);
	}
	else if (c.type == "evl_dff"){
		gates_.push_back(new evl_dff());
	}
	else if (c.type == "evl_output"){
		gates_.push_back(new evl_output);
	}
	else if (c.type == "evl_input"){
		gates_.push_back(new evl_input());
	}
	//else if (c.type == "evl_lut"){
	//	gates_.push_back(new evl_lut);
	//}
	//else if (c.type == "tris"){
	//	gates_.push_back(new tris);
	//}
	else if (c.type == "evl_clock"){
		gates_.push_back(new evl_clock);
	}
	else{
		std::cerr << "no such type: " << c.type << std::endl;
		return false;
	}
	return gates_.back()->create(c, nets_table_, wires_table);
}

bool netlist::create_gates(const evl_components &comps,
	const evl_wires_table &wires_table) {
	for (evl_components::const_iterator c = comps.begin(); c != comps.end(); ++c){
		create_gate(*c, wires_table);
	}//end loop
	return true;
}

bool netlist::create(const evl_wires &wires,
	const evl_components &comps,
	const evl_wires_table &wires_table) {
	return create_nets(wires,net_value) && create_gates(comps, wires_table);
}

void netlist::save(std::string &file_name, std::string module_name){
	std::ofstream output_file(file_name.c_str());
	assert(output_file);// verify output_file is ready
	display(output_file, module_name);
}
//destructors
gate::~gate(){
	for (std::vector<pin *>::iterator it = pins_.begin(); it != pins_.end(); it++)
		delete *it;
}

netlist::~netlist(){
	for (std::list<gate *>::iterator it = gates_.begin(); it != gates_.end(); it++)
		delete *it;
	for (std::list<net *>::iterator it = nets_.begin(); it != nets_.end(); it++)
		delete *it;
}


