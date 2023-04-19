#ifndef __SERVER_H__
#define __SERVER_H__
#include "util.h"
#include <iostream>
#include <vector>
using namespace std;
class Server {
public:
    Server(Params* param) {
        sys_param = param;
        cipher = new Cipher[sys_param->n + 1];
        for (size_t i = 0; i < sys_param->n + 1; ++i) {
            cipher[i].init(sys_param->pairing, sys_param->m);
        }
    };
    ~Server();
    void keygen();
    element_t* send_pks_to_dow() {return &pks;};
    Cipher* receive_index_from_dow() {return cipher;};
    void adjust(Trapdoor& tr_in, vector<size_t>& S, int i, Trapdoor& tr_out);
    void test(Trapdoor& tr_in, vector<size_t>& S, int i, Cipher& cipher);
    void search(vector<size_t>& S, Trapdoor& tr);

private:
    Params* sys_param;
    element_t pks;
    element_t sks;
    Cipher* cipher;
};
#endif