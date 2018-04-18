// netlist.h
#ifndef GUARD_NETLIST_H
#define GUARD_NETLIST_H

#include <assert.h>
#include "data_struct.h"

class netlist;
class gate;
class net;
class pin;

class net {
	int width;
	bool valued;
	std::string net_name;
	std::list<pin *> connections_; 
public:
	net(std::string name, int w);
	int get_width();
	bool get_valued(){ return valued; }
	void set_valued(bool value){ valued = value; }
	std::string get_net_name();
	//bool creat(const evl_wire &wire,
	//const evl_wires_table &wires_table);
	bool retrieve_value(std::map<net *, bool> &net_value, std::string &evl_file);
	void append_pin(pin *p);
	void display(std::ostream &out);
	void display_name_for_gate(std::ostream &out);
}; // class net

class pin {
	gate *gate_;
	size_t pin_index_;
	std::string port;
	std::vector<net *> nets_;
public:
	bool create(gate *g, size_t pin_index, const evl_pin &p,
		const std::map<std::string, net *> &nets_table);
	void display_for_net(std::ostream &out);
	void display_for_gate(std::ostream &out);
	void set_as_output();
	void set_as_input();
	std::vector<net *> get_net();
	std::string get_port(){ return port; }
	gate *get_gate(){ return gate_; }
}; // class pin

class gate {
protected:
	std::string gate_type;
	std::string gate_name;
	std::vector<pin *> pins_;
public:
	bool create_pin(const evl_pin &ep, size_t pin_index,
		const std::map<std::string, net *> &nets_table,
		const evl_wires_table &wires_table);
	bool create(const evl_component &c,
		const std::map<std::string, net *> &nets_table,
		const evl_wires_table &wires_table);
	std::string get_gate_type(){ return gate_type; }
	std::string get_gate_name(){ return gate_name; }
	void display_type_name(std::ostream &out);
	void display(std::ostream &out);
	virtual bool connect() = 0;
	virtual bool run(std::map<net *, bool> &net_value, std::string &evl_file) = 0;
	virtual void ini_display(std::string &evl_file) = 0;
	virtual void change_state() = 0;
	~gate();
}; // class gate

class netlist {
	std::list<gate *> gates_;
	std::list<net *> nets_;
	std::map<std::string, net *> nets_table_;
	std::map<net *, bool> net_value;
public:
	void create_net(const std::string &net_name, int i, int width, std::map<net *, bool> &net_value);
	bool create_nets(const evl_wires &wires, std::map<net *, bool> &net_value);
	bool create_gate(const evl_component &c, 
		const evl_wires_table &wires_table);
	bool create_gates(const evl_components &comps, 
		const evl_wires_table &wires_table);
	bool create(const evl_wires &wires, 
		const evl_components &comps, 
		const evl_wires_table &wires_table);
	void display(std::ostream &out, std::string module_name);
	void save(std::string &file_name, std::string module_name);
	void run(int run_time, std::string &evl_file);//new added
	virtual ~netlist();
}; // class netlist

class evl_and : public gate{
	bool connect();
	bool run(std::map<net *, bool> &net_value, std::string &evl_file);
	void ini_display(std::string &evl_file){ }
	void change_state(){}
};

class evl_or : public gate{
	bool connect();//need implement
	bool run(std::map<net *, bool> &net_value, std::string &evl_file);
	void ini_display(std::string &evl_file){}
	void change_state(){}
};

class evl_xor : public gate{
	bool connect();//need implement
	bool run(std::map<net *, bool> &net_value, std::string &evl_file);
	void ini_display(std::string &evl_file){}
	void change_state(){}
};

class evl_not : public gate{
	bool connect();//need implement
	bool run(std::map<net *, bool> &net_value, std::string &evl_file);
	void ini_display(std::string &evl_file){}
	void change_state(){}
};

class evl_buf : public gate{
	bool connect();//need implement
	bool run(std::map<net *, bool> &net_value, std::string &evl_file);
	void ini_display(std::string &evl_file){}
	void change_state(){}
};

class evl_one : public gate{
	bool connect();//need implement
	bool run(std::map<net *, bool> &net_value, std::string &evl_file);
	void ini_display(std::string &evl_file){}
	void change_state(){}
};

class evl_zero : public gate{
	bool connect();//need implement
	bool run(std::map<net *, bool> &net_value, std::string &evl_file);
	void ini_display(std::string &evl_file){}
	void change_state(){}
};

class evl_dff : public gate{
	bool state, next_state;
	
	bool connect();//need implement
	bool run(std::map<net *, bool> &net_value, std::string &evl_file);
	void ini_display(std::string &evl_file){}
	void change_state();
public:
	evl_dff() :state(false), next_state(false){}
};

class evl_clock : public gate{
	bool connect(){ return true; };
	bool run(std::map<net *, bool> &net_value, std::string &evl_file){ return true; }
	void ini_display(std::string &evl_file){}
	void change_state(){}
};

class evl_output : public gate{
	bool connect();//need implement
	bool run(std::map<net *, bool> &net_value, std::string &evl_file);
	void ini_display(std::string &evl_file);
	void change_state(){}
};

class evl_input : public gate{
	int count;
	int total;
	int line_no;
	std::vector<std::string> temp;
	std::list<std::string> pin_input;
	bool connect();//need implement
	bool run(std::map<net *, bool> &net_value, std::string &evl_file);
	void ini_display(std::string &evl_file){}
	void change_state(){}
public:
	evl_input() :count(0), total(0), line_no(0){}
};

class evl_lut : public gate{
	bool connect();//need implement
};

//class tris : public gate{
//	bool connect();//need implement
//};

#endif
