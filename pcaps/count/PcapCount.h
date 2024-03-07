#ifndef PCAPCOUNT_H
#define PCAPCOUNT_H
using namespace std;
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <hiredis/hiredis.h>
#include <stdint.h>
#include <arpa/inet.h>
#define BUFSIZE 1024
#include <cstring>
#include <stdint.h>
#include <cstdlib>
#include <map>
#include <vector>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <filesystem>

#include "PcapCountHeader.h"

class PcapCount {
public:
    // Constructor
    PcapCount();
    void usage(void);
    void setTarget(const char* target);
    void setSource(const char* source);
    void read_from_stdin(void);
    void store_ip_cnt_map(void);
    void load_ip_cnt_map(const string& filename);
    bool check_target_dir(string target);
    map <uint32_t, uint32_t> sumData;
    void sumMap(const map <uint32_t, uint32_t> &map);

    vector<string> getFlagsText(PcapCountHeader pch);
    vector<pair<uint32_t, uint32_t>> sortSourceIPsbyOccurence(const PcapCountHeader& pch);
    vector<pair<uint32_t, uint32_t>> sortDestinationIPsbyOccurence(const PcapCountHeader &pch);
    vector<pair<uint32_t, uint32_t>> sortProtosbyOccurence(const PcapCountHeader &pch);
    vector<pair<uint32_t, uint32_t>> sortSumedData(void);

    PcapCountHeader pch;
};


#endif /* PCAPCOUNT_H */

