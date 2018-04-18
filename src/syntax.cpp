#include "data_struct.h"

int count_wires(const std::list<evl_wire> &wires){
	int count = 0;
	for (std::list<evl_wire>::const_iterator wire = wires.begin(); wire != wires.end(); ++wire){
		++count;
	}
	return count;
}

int count_components(const std::list<evl_component> &components){
	int count = 0;
	for (std::list<evl_component>::const_iterator comp = components.begin();
		comp != components.end(); ++comp){
		++count;
	}
	return count;
}

int count_pins(const std::list<evl_pin> &pins){
	int count = 0;
	for (std::list<evl_pin>::const_iterator pin = pins.begin();
		pin != pins.end(); ++pin){
		++count;
	}
	return count;
}

bool extract_wire_statement(const std::list<evl_token> &tokens,
	std::list<evl_wire> &wires, std::list<evl_token>::const_iterator &token){
	enum wire_state_type {
		WIRE, DONE, WIRES, WIRE_NAME,
		BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE
	};
	wire_state_type state = WIRE;
	evl_wire wire;
	int width = 1;
	for (; token != tokens.end();){
		if (state == WIRE){

			if ((*token).type == "NAME"){
				wire.name = (*token).name;
				wire.width = width;
				wires.push_back(wire);
				state = WIRE_NAME;
				continue;
			}
			else if ((*token).name == "["){
				state = BUS;
				continue;
			}
			else{
				std::cerr << "NAME ERROR" << std::endl;
				return false;
			}
		}// in state of WIRE
		else if (state == DONE){
			return true;
		}
		else if (state == WIRES){
			++token;
			if ((*token).type == "NAME"){
				wire.name = (*token).name;
				wire.width = width;
				wires.push_back(wire);
				state = WIRE_NAME;
				continue;
			}
			else {
				std::cerr << "ERROR in WIRES" << std::endl;
				return false;
			}
		}// in state of WIRES
		else if (state == WIRE_NAME){
			++token;
			if ((*token).name == ","){
				state = WIRES;
				continue;
			}
			else if ((*token).name == ";"){
				state = DONE;
				++token;
				continue;
			}
			else{
				std::cerr << "ERROR in WIRE_NAME" << std::endl;
				return false;
			}
		}// in state of WIRE_NAME
		else if (state == BUS){
			++token;
			if ((*token).type == "NUMBER"){
				width = 1 + std::atoi((*token).name.c_str());
				state = BUS_MSB;
				continue;
			}
			else{
				std::cerr << "ERROR in BUS" << std::endl;
				return false;
			}
		}
		else if (state == BUS_MSB){
			++token;
			if ((*token).name == ":"){
				state = BUS_COLON;
				continue;
			}
			else{
				std::cerr << "ERROR in BUS_MSB" << std::endl;
				return false;
			}
		}
		else if (state == BUS_COLON){
			++token;
			if ((*token).name == "0"){
				state = BUS_LSB;
				continue;
			}
			else{
				std::cerr << "ERROR in BUS_COLON" << std::endl;
				return false;
			}
		}
		else if (state == BUS_LSB){
			++token;
			if ((*token).name == "]"){
				state = BUS_DONE;
				continue;
			}
			else{
				std::cerr << "ERROR in BUS_LSB" << std::endl;
				return false;
			}
		}
		else if (state == BUS_DONE){
			++token;
			if ((*token).type == "NAME"){
				wire.name = (*token).name;
				wire.width = width;
				wires.push_back(wire);
				state = WIRE_NAME;
				continue;
			}
			else{
				std::cerr << "ERROR in BUS_DONE" << std::endl;
				return false;
			}
		}
	}//loop
	return false;
}

bool extract_component_statement(const std::list<evl_token> &tokens,
	std::list<evl_component> &components, std::list<evl_token>::const_iterator &token){
	enum component_state_type {
		TYPE, NAME, PINS, PIN_NAME,
		BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE, PINS_DONE, DONE
	};
	component_state_type state = TYPE;
	evl_component comp;
	evl_pin pin;

	for (; token != tokens.end();){
		if (state == TYPE){
			comp.type = (*token).name;
			comp.name = "";
			++token;
			if ((*token).name == "("){
				//--token;
				//comp.type = (*token).name;
				//comp.name = "";
				//++token;
				++token;
				state = PINS;
				continue;
			}
			else if ((*token).type == "NAME"){
				state = NAME;
				continue;
			}
			else{
				std::cerr << "TYPE ERROR" << std::endl;
				return false;
			}
		}// in state of TYPE
		else if (state == PINS){
			if ((*token).type == "NAME"){
				pin.name = (*token).name;
				pin.bus_msb = -1;
				pin.bus_lsb = -1;
				++token;
				state = PIN_NAME;
				continue;
			}
			else{
				std::cerr << "PINS ERROR" << std::endl;
				return false;
			}
		}//in state of PINS
		else if (state == PIN_NAME){
			if ((*token).name == "["){
				++token;
				state = BUS;
				continue;
			}
			else if ((*token).name == ")"){
				comp.pin.push_back(pin);
				components.push_back(comp);
				++token;
				state = PINS_DONE;
				continue;
			}
			else if ((*token).name == ","){
				comp.pin.push_back(pin);
				++token;
				state = PINS;
				continue;
			}
			else{
				std::cerr << "PIN_NAME ERROR" << std::endl;
				return false;
			}
		}
		else if (state == BUS){
			if ((*token).type == "NUMBER"){
				pin.bus_msb = std::atoi((*token).name.c_str());
				++token;
				state = BUS_MSB;
				continue;
			}
			else{
				std::cerr << "BUS ERROR" << std::endl;
				return false;
			}
		}
		else if (state == BUS_MSB){
			if ((*token).name == "]"){
				++token;
				state = BUS_DONE;
				continue;
			}
			else if ((*token).name == ":"){
				++token;
				state = BUS_COLON;
				continue;
			}
			else{
				std::cerr << "BUS_MSB ERROR" << std::endl;
				return false;
			}
		}
		else if (state == BUS_COLON){
			if ((*token).type == "NUMBER"){
				pin.bus_lsb = std::atoi((*token).name.c_str());
				++token;
				state = BUS_LSB;
				continue;
			}
			else{
				std::cerr << "BUS_COLON ERROR" << std::endl;
				return false;
			}
		}
		else if (state == BUS_LSB){
			if ((*token).name == "]"){
				++token;
				state = BUS_DONE;
				continue;
			}
			else{
				std::cerr << "BUS_LSB ERROR" << std::endl;
				return false;
			}
		}
		else if (state == BUS_DONE){
			if ((*token).name == ")"){
				comp.pin.push_back(pin);
				components.push_back(comp);
				++token;
				state = PINS_DONE;
			}
			else if ((*token).name == ","){
				comp.pin.push_back(pin);
				++token;
				state = PINS;
			}
			else{
				std::cerr << "BUS_DONE ERROR" << std::endl;
				return false;
			}
		}
		else if (state == PINS_DONE){
			if ((*token).name == ";"){
				++token;
				state = DONE;
				continue;
			}
			else {
				std::cerr << "ERROR in PINS_DONE" << std::endl;
				return false;
			}
		}
		else if (state == DONE){
			return true;
		}
		else if (state == NAME){
			comp.name = (*token).name;
			++token;
			if ((*token).name == "("){
				state = PINS;
				++token;
				continue;
			}
			else{
				std::cerr << "NAME ERROR" << std::endl;
				return false;
			}
		}//in state of NAME
		else {
			std::cerr << "state ERROR" << std::endl;
			return false;
		}


	}//loop
	return false;
}


bool extract_statement_from_tokens(const std::list<evl_token> &tokens,
	std::list<evl_wire> &wires, std::list<evl_component> &components, std::string &module_name) {
	enum statement_state_type { INIT, MODULE, WIRE, COMPONENT, DONE };
	statement_state_type state = MODULE;

	for (std::list<evl_token>::const_iterator token = tokens.begin();
		token != tokens.end();){
		if (state == INIT){
			if ((*token).name == "module"){
				std::cerr << "Multi module" << std::endl;
				return false;
			}
			else if ((*token).name == "wire"){
				++token;
				state = WIRE;
				continue;
			}
			else if ((*token).name == "endmodule"){
				state = DONE;
				continue;
			}
			else {
				state = COMPONENT;
				continue;
			}//put state into wires or components
		}
		else if (state == MODULE){
			if ((*token).name == "module"){
				++token;
			}
			else{
				std::cerr << "Missing module" << std::endl;
				return false;
			}
			if ((*token).type == "NAME"){
				module_name = (*token).name;
				++token;
				if ((*token).name != ";"){
					std::cerr << "Missing ; after top" << std::endl;
					return false;
				}//store module name
				++token;
				state = INIT;
				continue;
			}
			else {
				std::cerr << "module name ERROR" << std::endl;
				return false;
			}
		}
		else if (state == WIRE){
			if (extract_wire_statement(tokens, wires, token)){
				state = INIT;
				continue;
			}//successfully extract wire statement
			else{
				std::cerr << "ERROR in extract wire statement" << std::endl;
				return false;
			}//fail to extract wire statement
		}
		else if (state == COMPONENT){
			if (extract_component_statement(tokens, components, token)){
				state = INIT;
				continue;
			}//successfully extract component statement
			else{
				std::cerr << "ERROR in extract component statement" << std::endl;
				return false;
			}//fail to extract component statement
		}
		else if (state == DONE){
			return true;
		}
		else {
			std::cerr << "statement state ERROR" << std::endl;
			return false;
		}

	}
	return false;
}
