#include "util.h"
class DataOwner{
public:
    DataOwner(Params* param) {
        sys_param = param;
    };
    ~DataOwner();
    void receive_pks_from_server(element_t* ser_pk) {this->ser_pk = ser_pk;};
    void keygen();
    element_t* send_pk1_to_user() { return &pk1;};
    void encrypt(Cipher* cipher);
    void extract(std::vector<size_t>& wanted, element_t &agg);
    // void send_agg(Channel *channel, element_t& agg, vector<size_t>& S);
    
private:
    Params* sys_param;
    element_t pk1;
    element_t pk2;
    element_t sk1;
    element_t sk2;
    element_t* ser_pk;
};
