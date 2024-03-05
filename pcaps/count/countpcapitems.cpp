using namespace std;
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "PcapCount.h"

int main(int argc, char* argv[]) {
    int opt;
    PcapCount pc;
    while ((opt = getopt(argc, argv, "hs:t:i")) != -1) {
        switch (opt) {
            case 'h':
                pc.usage();
                return EXIT_SUCCESS;
            case 'i':
                pc.cnt_src_ips=true;
                break;
            case 's':
                pc.setSource(optarg);
                break;
            case 't':
                pc.setTarget(optarg);
                break;
            default: /* '?' */
                cerr<<"[ERROR] Invalid command line was specified"<<endl;
        }
    }
    cout<<"Source " <<pc.source <<endl;
    cout<<"Target SRC "<<pc.target_srcip_file << endl;
    cout<<"Target " <<pc.target <<endl;

    if (pc.check_target_dir(pc.target) == false){
        return EXIT_FAILURE;
    }
    pc.read_from_stdin();
    cout<<"Start to serailize" <<endl;
    pc.store_ip_cnt_map();
    return 0;
}
