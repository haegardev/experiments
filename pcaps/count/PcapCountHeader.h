#ifndef PCAPCOUNTHEADER_H
#define PCAPCOUNTHEADER_H
#include <stdint.h>
#include <string>
#include <map>
using namespace std;
#define VERSION "1.0"
#define DESCRIPTION "PcapCount File"

class PcapCountHeader {
public:
    // Serialization function template declaration
    template<class Archive>
     void serialize(Archive &ar, const unsigned int ) {
        ar & description;
        ar & pcapCountVersion;
        ar & source;
        ar & target;
        ar & target_srcip_file;
        ar & flag_src_ips;
        ar & cnt_ip_src;
        ar & cnt_ip_dst;
        ar & cnt_proto;
    }

    // Attributes
    string description = DESCRIPTION;
    string pcapCountVersion = VERSION;
    map <uint32_t, uint32_t> cnt_ip_src;
    //Count IP protocols observed
    map <uint8_t,uint32_t> cnt_proto;
    // Count IP destinations observed
    map <uint32_t, uint32_t> cnt_ip_dst;
    string source;
    string target;
    string target_srcip_file;
    bool flag_src_ips;
    uint32_t firstSeen;
    uint32_t lastSeen;
};

#endif /* PCAPCOUNTHEADER_H */

