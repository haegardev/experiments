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
#define VERSION "1.0"
#define DESCRIPTION "PcapCount File"
#include <cstring>
#include <stdint.h>
#include <cstdlib>
#include <map>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <filesystem>

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

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);

    // Attributes
    string description = DESCRIPTION;
    string version = VERSION;
    map <uint32_t, uint32_t> counted_data;
    string source;
    string target;
    string target_srcip_file;
    bool cnt_src_ips;
};


#endif /* PCAPCOUNT_H */

