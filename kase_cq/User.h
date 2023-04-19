#include "util.h"
#include <vector>
class User{
public:
public:
    User(Params* param) {
        sys_param = param;
    };
    ~User() {};
    void receive_pk1_from_dow(element_t* do_pk1) {this->do_pk1 = do_pk1;};
    void gen_trapdoor(element_t &agg, Trapdoor& trapdoor);
    // void send_trapdoor(Channel *channel, Trapdoor& trapdoor, vector<size_t>& S);
    // void receive_cipher(Channel *channel, vector<Cipher>& ciphers);

private:
    Params* sys_param;
    element_t* do_pk1;
};