#include "data_struct.h"

int count_wires(const std::list<evl_wire> &wires);
int count_components(const std::list<evl_component> &components);
int count_pins(const std::list<evl_pin> &pins);

void display_tokens(std::ostream &out,
	const std::list<evl_wire> &wires, const std::list<evl_component> &components, const std::string &module_name) {

	out << "module " << module_name << std::endl;
	out << "wires " << count_wires(wires) << std::endl;
	for (std::list<evl_wire>::const_iterator wire = wires.begin();
		wire != wires.end(); ++wire){
		out << "  wire " << (*wire).name << " " << (*wire).width << std::endl;
	}//print wires

	out << "components " << count_components(components) << std::endl;
	for (std::list<evl_component>::const_iterator comp = components.begin();
		comp != components.end(); ++comp){
		std::string comp_name;
		if ((*comp).name != ""){
			comp_name = (*comp).name + " ";
		}
		else {
			comp_name = (*comp).name;
		}
		out << "  component " << (*comp).type << " " << comp_name << count_pins((*comp).pin) << std::endl;
		for (std::list<evl_pin>::const_iterator pin = (*comp).pin.begin();
			pin != (*comp).pin.end(); ++pin){
			std::string msb = "";
			std::string lsb = "";

			if ((*pin).bus_msb >= 0){
				std::stringstream tostring;
				tostring << (*pin).bus_msb;
				tostring >> msb;
				msb = " " + msb;
			}
			if ((*pin).bus_lsb >= 0){
				std::stringstream tostring;
				tostring << (*pin).bus_lsb;
				tostring >> lsb;
				lsb = " " + lsb;
			}
			out << "    pin " << (*pin).name << msb << lsb << std::endl;
		}
	}//print components
}

bool store_tokens_to_file(std::string file_name,
	const std::list<evl_wire> &wires, const std::list<evl_component> &components, const std::string &module_name) {
	std::ofstream output_file(file_name.c_str());
	if (!output_file) {
		std::cerr << "I can't read " << file_name << "." << std::endl;
		return false;
	} // verify output_file is ready
	display_tokens(output_file, wires, components, module_name);
	return true;
}
