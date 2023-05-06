#include "Server.h"
#include <iostream>
#include <chrono>

Server::Server(Params* param) {
	sys_param = param;
	cipher = new Cipher[sys_param->n + 1];
	for (size_t i = 0; i < sys_param->n + 1; ++i) {
		cipher[i].init(sys_param->pairing, sys_param->m);
	}
	net_to_dow.init(DEFPORT + 1);
	net_to_dow.server_accept();
	net_to_user.init(DEFPORT + 2);
	net_to_user.server_accept();
	init();
};

void Server::init() {
	size_t buffer_size = sys_param->to_string(NULL);
	char buf[buffer_size];
	sys_param->to_string(buf);
	net_to_dow.send(buf, buffer_size);
	net_to_user.send(buf, buffer_size);
}

void Server::keygen() {
    element_init_Zr(sks, sys_param->pairing);
    element_random(sks);
    element_init_G1(pks, sys_param->pairing);
    element_pow_zn(pks, sys_param->g, sks);
	this->send_pks_to_dow();
}

void Server::send_pks_to_dow() {
	char buf[ELEMENT_SIZE];
	element_snprint(buf, ELEMENT_SIZE, pks);
	net_to_dow.send(buf, ELEMENT_SIZE);
}

void Server::receive_index_from_dow() {
	size_t buffer_size = (sys_param->n + 1) * cipher[0].string_to_cipher(NULL);;
	char buf[buffer_size];
	size_t offset = 0;
	net_to_dow.receive(buf, buffer_size);
	for (size_t i = 0; i < sys_param->n + 1; ++i) {
		offset = cipher[i].string_to_cipher(buf + i * offset);
	}
}


Server::~Server() {
    element_clear(sks);
    element_clear(pks);
    delete[] cipher;
}

void Server::adjust(Trapdoor& tr_in, vector<size_t>& wanted,
                    int i, Trapdoor& tr_out) {
	element_t temp;
    int w = wanted.size();
	element_init_G1(temp, sys_param->pairing);
	element_set1(temp);
	for(int j=0; j<w; j++)
	{
		if(wanted[j] == wanted[i]) continue;
		int ij = sys_param->n + 1 - wanted[j] + wanted[i];
		element_mul(temp, temp, sys_param->gn[ij]);
	}
	element_mul(tr_out.t1, tr_in.t1, temp);
	element_clear(temp);
}

void Server::test(Trapdoor& tr_in, vector<size_t>& wanted, int i, Cipher& C) {
    element_t veri1;
	element_t veri2;
    int w = wanted.size();
    int l = tr_in.t3.size();
	element_t temp3, temp4, temp5;
	element_init_GT(temp3, sys_param->pairing);
	element_pairing(temp3, tr_in.t1, C.c1);
	element_init_G1(temp4, sys_param->pairing);
	element_set(temp4, sys_param->gn[(sys_param->n + 1)  - wanted[0]]);
	element_init_GT(temp5, sys_param->pairing);
	for(int j = 1; j < w; j++)
	{
		int ij = sys_param->n + 1 - wanted[j];
		element_mul(temp4, temp4, sys_param->gn[ij]);
	}
	element_pairing(temp5, temp4, C.c2);
	element_init_GT(veri1, sys_param->pairing);
	element_div(veri1, temp3, temp5);
	element_t temp6, temp7;
	element_init_G1(temp6, sys_param->pairing);
	element_set(temp6, C.Cw[tr_in.t3[0]]);
	for(int j = 1; j < l; j++)
	{
		element_mul(temp6, temp6, C.Cw[tr_in.t3[j]]);
	}
	element_init_G1(temp7, sys_param->pairing);
	element_t sks1;
	element_init_Zr(sks1, sys_param->pairing);
	element_invert(sks1, sks);
	element_pow_zn(temp7, temp6, sks1);
	element_mul(temp7, temp7, tr_in.t2);
	element_t div2;
	element_init_GT(div2, sys_param->pairing);
	element_pairing(div2, temp7, C.c4);
	element_clear(sks1);

	element_t temp8, temp9, el;
	element_init_Zr(el, sys_param->pairing);
	element_set_si(el, l);
	element_init_GT(temp8, sys_param->pairing);
	element_init_GT(temp9, sys_param->pairing);
	element_pow_zn(temp8, C.c5, el);
	element_mul(temp9, C.c3, temp8);
	element_init_GT(veri2, sys_param->pairing);
	element_div(veri2, div2, temp9);

	element_clear(el);
	element_clear(div2);
	element_clear(temp8);
	element_clear(temp9);
	element_clear(temp3);	
	element_clear(temp4);
	element_clear(temp5);
	element_clear(temp6);
	element_clear(temp7);
	if(!element_cmp(veri1, veri2))
		cout << RED << "file " << wanted[i] << "  match" << endl << WHITE;

	element_clear(veri1);
	element_clear(veri2);
}

void Server::search() {
size_t w;
net_to_user.receive(reinterpret_cast<char*>(&w), sizeof(size_t));
size_t wanted[w];
vector<size_t> S(w);
net_to_user.receive(reinterpret_cast<char*>(&wanted), w * sizeof(size_t));
for (int i = 0; i < w; ++i) S[i] = wanted[i];

Trapdoor tr;
tr.init(sys_param->pairing);
size_t buffer_size;
net_to_user.receive(reinterpret_cast<char*>(&buffer_size), sizeof(size_t));
char buf[buffer_size];
net_to_user.receive(buf, buffer_size);
tr.string_to_trapdoor(buf, buffer_size);

auto start_search = chrono::system_clock::now();
    for(int i = 0; i < w; i++)
    {
        Trapdoor tr_adjust(tr);
        adjust(tr, S, i, tr_adjust);
        test(tr_adjust, S, i, cipher[S[i]]);
    }
auto end_search = chrono::system_clock::now();
chrono::duration<double> time_search = end_search - start_search;
cout << "search generte time " << GREEN << time_search.count() << WHITE << " s" <<endl;
cout << "------------------------------------------" <<endl;
}
