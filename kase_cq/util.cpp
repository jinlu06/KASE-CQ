#include "util.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstring>
using namespace std;
void Params::init(char* file_name, int n_, int m_, int number) {
    file_name = "kase_cq/a.param";
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
    n = n_;
    m = m_;
    Hash = element_from_hash;
if (number == 0) {
    element_t a;
    element_init_G1(this->g, pairing);
    element_init_Zr(a, pairing);
    element_random(this->g);
    element_random(a);
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
} else {
    element_init_G1(this->g, pairing);
    gn = new element_t[(n*2)+1];
    element_init_G1(gn[0], pairing);
    for(int i = 1; i<((2*n) +1); i++)
    {
        if(i != n+1){
        element_init_G1(gn[i], pairing);
        }
        else{
        element_init_G1(gn[i], pairing);
        }
    }
}
auto end_set= chrono::system_clock::now();
chrono::duration<double> time_set = end_set - start_set;
cout << "setup generte time " << GREEN << time_set.count() << WHITE << " s"<<endl;
cout << "------------------------------------------" <<endl;
}

size_t Params::to_string(char* buf) {
    if (buf == NULL) return ((2*n) +1) * ELEMENT_SIZE;
    element_snprint(buf, ELEMENT_SIZE, g);
    for(int i = 1; i<((2*n) +1); i++) {
        element_snprint(buf + i * ELEMENT_SIZE, ELEMENT_SIZE, gn[i]);
    }
    return ((2*n) +1) * ELEMENT_SIZE;
}

size_t Params::string_to_param(char* buf) {
    if (buf == NULL) return ((2*n) +1) * ELEMENT_SIZE;
    element_set_str(g, buf, 0);
    for(int i = 1; i<((2*n) +1); i++) {
        element_set_str(gn[i], buf + i * ELEMENT_SIZE, 0);
    }
    return ((2*n) +1) * ELEMENT_SIZE;
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

// 5 + m ELEMENT_SIZE
size_t Cipher::to_string(char* buf) {
    if (buf == NULL) return ((5 + m) * ELEMENT_SIZE);
    element_snprint(buf, ELEMENT_SIZE, c1);
    element_snprint(buf + ELEMENT_SIZE, ELEMENT_SIZE, c2);
    element_snprint(buf + ELEMENT_SIZE * 2, ELEMENT_SIZE, c3);
    element_snprint(buf + ELEMENT_SIZE * 3, ELEMENT_SIZE, c4);
    element_snprint(buf + ELEMENT_SIZE * 4, ELEMENT_SIZE, c5);
    for (int i = 0; i < m; ++i) {
        element_snprint(buf + ELEMENT_SIZE * (5 + i), ELEMENT_SIZE, Cw[i]);
    }
    return ((5 + m) * ELEMENT_SIZE);
}

size_t Cipher::string_to_cipher(char* buf) {
    if (buf == NULL) return ((5 + m) * ELEMENT_SIZE);
    element_set_str(c1, buf, 0);
    element_set_str(c2, buf + ELEMENT_SIZE, 0);
    element_set_str(c3, buf + ELEMENT_SIZE * 2, 0);
    element_set_str(c4, buf + ELEMENT_SIZE * 3, 0);
    element_set_str(c5, buf + ELEMENT_SIZE * 4, 0);
    for (int i = 0; i < m; ++i) {
        element_set_str(Cw[i], buf + ELEMENT_SIZE * (5 + i), 0);
    }
    return ((5 + m) * ELEMENT_SIZE);
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

// 2 * ELEMENT_SIZE + t3.size() * sizeof(size_t)
size_t Trapdoor::to_string(char* buf) {
    if (buf == NULL) return (2 * ELEMENT_SIZE + t3.size() * sizeof(size_t));
    element_snprint(buf, ELEMENT_SIZE, t1);
    element_snprint(buf + ELEMENT_SIZE, ELEMENT_SIZE, t2);
    size_t tmp[t3.size()];
    for (int i = 0; i < t3.size(); ++i) {
        tmp[i] = t3[i];
    }
    memcpy(buf + 2 * ELEMENT_SIZE, tmp, t3.size() * sizeof(size_t));
    return (2 * ELEMENT_SIZE + t3.size() * sizeof(size_t));
}

size_t Trapdoor::string_to_trapdoor(char* buf, size_t len) {
    if (buf == NULL) return (2 * ELEMENT_SIZE + t3.size() * sizeof(size_t));
    size_t t3_size = (len - 2 * ELEMENT_SIZE) / sizeof(size_t);
    std::cout << t3_size << std::endl;
    element_set_str(t1, buf, 0);
    element_set_str(t2, buf + ELEMENT_SIZE, 0);
    size_t tmp[t3_size];
    memcpy(tmp, buf + 2 * ELEMENT_SIZE, t3_size * sizeof(size_t));
    t3.resize(t3_size);
    for (int i = 0; i < t3_size; ++i) {
       t3[i] = tmp[i];
    }
    return len;
}