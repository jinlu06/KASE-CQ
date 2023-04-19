#include "User.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstring>
using namespace std;
void User::gen_trapdoor(element_t &agg, Trapdoor& trapdoor) {
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
	element_pow_zn(vs, *do_pk1, s);
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
}