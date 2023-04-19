#include "User.h"
#include "Server.h"
#include "DataOwner.h"
#include "util.h"
#include <stdlib.h>
using namespace std;
int main(int argc, char **argv) {
    Params param;

    param.init(argv[1], atoi(argv[2]), atoi(argv[3]));

    Server server(&param);
    DataOwner dow(&param);
    User user(&param);
    server.keygen();
    dow.keygen();
    dow.receive_pks_from_server(server.send_pks_to_dow());
    user.receive_pk1_from_dow(dow.send_pk1_to_user());
    dow.encrypt(server.receive_index_from_dow());

    while(1)
    {
        //控制查询
        cout << "search input 1, exit input 0: ";
        bool contro;
        cin >> contro;
        if(! contro) break;
    element_t agg;
    vector<size_t> wanted;
    Trapdoor tr;
        tr.init(param.pairing);
        // while(1)
        // {
        //     cin.clear();
        //     cin.ignore();
        //     int a;
        //     cout << "please input you wanted files" << endl;
        //     cout << "if input = -1, end" << endl;
        //     cin >> a;
        //     if(a != -1) wanted.push_back(a);
        //     else break;
        // }
        size_t wanted_size;
        cout << "please input you share file number: ";
        cin.clear();
        cin.ignore();
        cin >> wanted_size;
        for(size_t i = 0; i<wanted_size; i++)
        {
            wanted.push_back(i+1);
        }
        dow.extract(wanted, agg);
        user.gen_trapdoor(agg, tr);
        server.search(wanted, tr);
    }
}