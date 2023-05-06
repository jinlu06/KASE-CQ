#include "User.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstring>
using namespace std;

User::User(Params* param) {
	sys_param = param;
	net_to_dow.init("127.0.0.1", DEFPORT);
	net_to_server.init("127.0.0.1", DEFPORT + 2);
	init();
};

void User::init() {
	size_t buffer_size = sys_param->string_to_param(NULL);
	char buf[buffer_size];
    net_to_server.receive(buf, buffer_size);
    sys_param->string_to_param(buf);
}

User::~User() {element_clear(do_pk1);};

void User::keygen() {
	element_init_G1(do_pk1, sys_param->pairing);
	receive_pk1_from_dow();
}

void User::receive_pk1_from_dow() {
	char buf[ELEMENT_SIZE];
	net_to_dow.receive(buf, ELEMENT_SIZE);
	element_set_str(do_pk1, buf, 0);
}

void User::gen_trapdoor() {
size_t wanted_size;
net_to_dow.receive(reinterpret_cast<char*>(&wanted_size), sizeof(wanted_size));
size_t wanted[wanted_size];
size_t buffer_size = ELEMENT_SIZE + wanted_size * sizeof(size_t);
char buf[buffer_size];
net_to_dow.receive(buf, buffer_size);
element_t agg;
element_init_G1(agg, sys_param->pairing);
element_set_str(agg, buf, 0);
memcpy(wanted, buf + ELEMENT_SIZE, wanted_size * sizeof(size_t));

Trapdoor trapdoor;
trapdoor.init(sys_param->pairing);
//input keywords
	vector<string> Q;
	while(1)
	{
		cin.clear();
		cin.ignore();
		string a;
		int a1;
		bool contol;
		cout << "please input you wanted keyword field: ";
		cin >> a1;
		cin.clear();
		cin.ignore();
		cout <<  "please input Corresponding keyword: ";
		cin >> a;
		cin.clear();
		cin.ignore();
		Q.push_back(a);
		trapdoor.t3.push_back(--a1);
		cout << "if continue query, input 1, else input 0: ";
		cin >> contol;
		if(contol == 0) break;
	}

auto start_trap = chrono::system_clock::now();		
	int l = Q.size();		//l是查询的关键词数量
	element_t s;	
	element_init_Zr(s, sys_param->pairing);
	element_random(s);
	element_t gs;
	element_init_G1(gs, sys_param->pairing);
	element_pow_zn(gs, sys_param->g, s);
	element_mul(trapdoor.t1, agg ,gs);
	element_clear(gs);
	element_t vs, hashsum, ghash;
	element_init_G1(vs, sys_param->pairing);
	element_pow_zn(vs, do_pk1, s);
	element_init_Zr(hashsum, sys_param->pairing);
	for(int i =0; i<l ;i++)
	{
		element_t hash;
		element_init_Zr(hash, sys_param->pairing);
		int len = Q[i].length();
		char* keyword = new char[len + 1];
		strcpy(keyword, Q[i].c_str());
		element_from_hash(hash, keyword, len);
		delete[] keyword;
		if(i == 0) element_set(hashsum, hash);
		else element_add(hashsum, hashsum, hash);
		element_clear(hash);
	}
	element_init_G1(ghash, sys_param->pairing);
	element_pow_zn(ghash, sys_param->g, hashsum);
	element_div(trapdoor.t2, vs, ghash);
	element_clear(vs);
	element_clear(hashsum);
	element_clear(ghash);
	element_clear(s);
auto end_trap= chrono::system_clock::now();
chrono::duration<double> time_trap = end_trap - start_trap;
cout << "trap generte time " << GREEN << time_trap.count() << WHITE << " s"<<endl;
cout << "------------------------------------------" <<endl;

 	net_to_server.send(reinterpret_cast<char*>(&wanted_size), sizeof(size_t));
	net_to_server.send(reinterpret_cast<char*>(&wanted), wanted_size * sizeof(size_t));
	size_t trapdoor_size = trapdoor.to_string(NULL);
	net_to_server.send(reinterpret_cast<char*>(&trapdoor_size), sizeof(size_t));
	char buf_trap[trapdoor_size];
	trapdoor.to_string(buf_trap);
	net_to_server.send(buf_trap, trapdoor_size);
}