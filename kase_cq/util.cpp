#include "util.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstring>
using namespace std;
void Params::init(char* file_name, int n_, int m_) {
auto start_set = chrono::system_clock::now();	
    fstream fp(file_name);
    fp.seekg(0, std::ios::end);    // go to the end  
    int count = fp.tellg();           // report location (this is the length)  
    fp.seekg(0, std::ios::beg);    // go back to the beginning  
    char* param = new char[count];    // allocate memory for a buffer of appropriate dimension  
    fp.read(param, count);      // read the whole file into the buffer  
    fp.close();
    pairing_init_set_buf(this->pairing, param, count);
    delete[] param;

    element_t a;
    element_init_G1(this->g, pairing);
    element_init_Zr(a, pairing);
    element_random(this->g);
    element_random(a);

    n = n_;
    m = m_;

    gn = new element_t[(n*2)+1];
    element_init_G1(gn[0], pairing);
    for(int i = 1; i<((2*n) +1); i++)
    {
        if(i != n+1){
        mpz_t temp;
        element_t ai;
        mpz_init(temp);
        mpz_init_set_ui(temp, i);
        element_init_Zr(ai,pairing);	
        element_pow_mpz(ai, a, temp);
        element_init_G1(gn[i], pairing);
        element_pow_zn(gn[i], this->g, ai);
        mpz_clear(temp);
        element_clear(ai);
        }
        else{
        element_init_G1(gn[i], pairing);
        }
    }
    element_clear(a);

    Hash = element_from_hash;
auto end_set= chrono::system_clock::now();
chrono::duration<double> time_set = end_set - start_set;
cout << "setup generte time " << GREEN << time_set.count() << WHITE << " s"<<endl;
cout << "------------------------------------------" <<endl;
}

Params::~Params() {
    for(int i=0; i<2*n+1; i++)
    {
        element_clear(gn[i]);
    }
    delete[] gn;
    element_clear(g);
    pairing_clear(pairing);
}

Cipher::Cipher() {}

void Cipher::init(pairing_t& pairing, int m)
{
	this->m = m;
    element_init_G1(c1, pairing);
    element_init_G1(c2, pairing);
    element_init_GT(c3, pairing);
    element_init_G1(c4, pairing);
    element_init_GT(c5, pairing);
	Cw = new element_t[m];
    for (int i = 0; i < m; ++i) {
        element_init_G1(Cw[i], pairing);
    }
}

Cipher::~Cipher()
{
	if(m != 0)
	{
        element_clear(c1);
        element_clear(c2);
        element_clear(c3);
        element_clear(c4);
        element_clear(c5);
        for(int i=0; i<m; i++)
        {
            element_clear(Cw[i]);
        }
        delete[] Cw;
	}
}

Trapdoor::Trapdoor() {}

void Trapdoor::init(pairing_t& pairing)
{
    element_init_G1(t1, pairing);
    element_init_G1(t2, pairing);
}

Trapdoor::Trapdoor(Trapdoor& tr) : t3(tr.t3) {
    element_init_same_as(this->t1, tr.t1);
    element_set(this->t1, tr.t1);
    element_init_same_as(this->t2, tr.t2);
    element_set(this->t2, tr.t2);
}

Trapdoor::~Trapdoor()
{
    element_clear(t1);
    element_clear(t2);
}