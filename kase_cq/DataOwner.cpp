#include "DataOwner.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstring>
using namespace std;
void DataOwner::keygen() {
  element_init_Zr(sk1, sys_param->pairing);
  element_init_Zr(sk2, sys_param->pairing);
  element_init_G1(pk1, sys_param->pairing);
  element_init_G1(pk2, sys_param->pairing);
  element_random(sk1);
  element_random(sk2);
  element_pow_zn(pk1, sys_param->g, sk1);
  element_pow_zn(pk2, sys_param->g, sk2);
}

DataOwner::~DataOwner() {
    element_clear(sk1);
    element_clear(pk1);
    element_clear(sk2);
    element_clear(pk2);
}

void DataOwner::encrypt(Cipher* cipher) {
    string filename = "character/5W.txt";
    fstream f(filename);
    vector<file> data;
    if(!f)
    {
        cout <<"character txt not exit" << endl;
        return;
    }
    vector<string> input;
    string line;
    while (getline(f, line))
    {
        line.pop_back();
        input.push_back(line);
    }
    f.close();
    for(int i = 0; i<sys_param->n; i++)
    {
        vector<string> v1(input.begin()+(i*sys_param->m), input.begin() + ((i+1)*sys_param->m));	
        file f1(i+1, v1);
        data.push_back(f1);
    }

    char save[10];
    sprintf(save, "%d%d", sys_param->n, sys_param->m);
    ofstream f1(save, ios::trunc | ios::out);
    for(vector<file>::iterator it=data.begin(); it!= data.end(); it++)
    {
        f1 << it->id <<endl;
        for(vector<string>::iterator its = (it->keyword).begin(); its != (it->keyword).end(); its++)
        {
            f1 << *its << endl;
        }
    }
    f1.close();
auto start_enc = chrono::system_clock::now();	
//encryption
  for(int i = 1; i< sys_param->n + 1; i++)
  {
    element_t t,r;
    element_init_Zr(t, sys_param->pairing);
    element_random(t);
    element_init_Zr(r, sys_param->pairing);
    element_random(r);

	element_pow_zn(cipher[i].c1, sys_param->g, t);
	element_mul(cipher[i].c2, pk2, sys_param->gn[i]);
	element_pow_zn(cipher[i].c2, cipher[i].c2, t);
	element_pairing(cipher[i].c3, sys_param->gn[1], sys_param->gn[sys_param->n]);
	element_pow_zn(cipher[i].c3, cipher[i].c3, t);
	element_t bt;
	element_init_Zr(bt, sys_param->pairing);
	element_div(bt, t, sk1);
	element_pow_zn(cipher[i].c4, sys_param->g, bt);
	element_clear(bt);
	element_t rt;
	element_init_Zr(rt, sys_param->pairing);
	element_mul(rt, r, t);
	element_pairing(cipher[i].c5, sys_param->g, sys_param->g);
	element_pow_zn(cipher[i].c5, cipher[i].c5, rt);
	element_clear(rt);

	for(int j=0; j<sys_param->m; j++ )
	{
		string temp = (data[i-1].keyword)[j];
		int len = temp.length();
		char* keyword = new char[len + 1];
		strcpy(keyword, temp.c_str());
		element_t hash;
		element_t br;
		element_t add1;

		element_init_Zr(hash, sys_param->pairing);
		sys_param->Hash(hash, keyword, len);

		element_init_Zr(br, sys_param->pairing);
		element_mul(br, sk1, r);
		element_init_Zr(add1, sys_param->pairing);
		element_add(add1, hash, br);

		element_pow_zn((cipher[i].Cw)[j], *ser_pk, add1);

		element_clear(hash);
		element_clear(add1);
		element_clear(br);
		delete[] keyword;
	}
	element_clear(t);
	element_clear(r);
  }
auto end_enc= chrono::system_clock::now();
chrono::duration<double> time_enc = end_enc - start_enc;
cout << "enc generte time " << GREEN << time_enc.count() << WHITE << " s"<<endl;
cout << "------------------------------------------" <<endl;
}

void DataOwner::extract(std::vector<size_t>& wanted, element_t &agg) {
auto start_agg = chrono::system_clock::now();		
    element_init_G1(agg, sys_param->pairing);
    for(vector<size_t>::iterator it = wanted.begin(); it!= wanted.end(); it++)
    {
        element_t temp;
        size_t temp2 = (sys_param->n + 1) - *it;
        element_init_G1(temp, sys_param->pairing);
        element_pow_zn(temp, sys_param->gn[temp2], sk2);
        if(it == wanted.begin()) element_set(agg, temp);
        else element_mul(agg, agg, temp);
        element_clear(temp);
    }
auto end_agg= chrono::system_clock::now();
chrono::duration<double> time_agg = end_agg - start_agg;
cout << "agg generte time " << GREEN << time_agg.count() << WHITE << " s"<<endl;
cout << "------------------------------------------" <<endl;
}