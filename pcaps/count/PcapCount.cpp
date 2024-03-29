#include "PcapCountHeader.h"
#include "PcapCount.h"
#include <map>
PcapCount::PcapCount(){
    pch.flag_src_ips = false;
    // Put constructor stuff here
    pch.firstSeen = 0xFFFFFFFF;
    pch.lastSeen = 0;
    pch.pcapCountVersion = VERSION;
    pch.description = DESCRIPTION;
}

void PcapCount::setTarget(const char* tg)
{
    pch.target=string(tg);
    pch.target_srcip_file = string(tg);
    pch.target_srcip_file.append("/src_ip.cnt");
    // TODO generate all the other counted fields such as proto, source port, destination port here
}

void PcapCount::setSource(const char* sr)
{
    pch.source = string(sr);
}

void PcapCount::read_from_stdin(void)
{
    char *buf;
    int i,j;
    struct in_addr addr;
    uint32_t ts,ip_src, ip_dst,proto;
    char *ptr;
    buf = (char*)calloc(BUFSIZE,1);
    char *token;
    if (!buf) {
        cerr<<"[ERROR] Could not allocate memory, abort"<<endl;
        abort();
    }

    while( fgets(buf, BUFSIZE , stdin) ) {
        i=1;
        token = strtok(buf, ",");
        while(token != NULL)
        {
            switch (i) {
                case 1:
                        ptr = token;
                        for (j=0; j<(int)strlen(ptr); ++j){
                            if (ptr[j]=='.'){
                                ptr[j]=0;
                                break;
                            }
                        }
                        ts = atoi(ptr);
                        // Record first and last seen
                        if (ts > pch.lastSeen) {
                            pch.lastSeen = ts;
                        }
                        if ( ts < pch.firstSeen) {
                            pch.firstSeen = ts;
                        }
                    break;
                case 2:
                    if (inet_pton(AF_INET, token, &addr) < 0){
                        // Something went wrong
                        ip_src = 0;
                    }else{
                        ip_src = ntohl(addr.s_addr);
                        }
                    break;

                case 3:
                    if (inet_pton(AF_INET, token, &addr) < 0){
                        // Something went wrong
                        ip_dst = 0;
                    }else{
                        ip_dst = ntohl(addr.s_addr);
                        }
                    break;
                case 4:
                    proto = atoi(token);
                    break;
                // add here the next fields that are parsed
            }
            token = strtok(NULL, ",");
            i++;
        }
        // Count IP addresses and other fields
        if (pch.flag_src_ips) {
            ++pch.cnt_ip_src[ip_src];
        }
        if (pch.flag_ip_dst) {
            ++pch.cnt_ip_dst[ip_dst];
        }
        if (pch.flag_proto) {
            ++pch.cnt_proto[proto];
        }
    }
    free(buf);
}


/* FIMXE add clean header and say what data it is
 *format: size_t size of the map  followed by series of uint32_t ip uint32_t frequency
 */

void PcapCount::store_ip_cnt_map(void) {
    ofstream file(pch.target_srcip_file, ios::binary);
    if (file.is_open()) {
        boost::archive::binary_oarchive oa(file);
        oa << pch;
        file.close();
    } else {
    cerr << "Error: Unable to open file " << pch.target_srcip_file << " for writing." << std::endl;
    }
}

void PcapCount::usage(void){
    cout << "countpcapitems -source source_filename -target target_directory." <<endl;
    cout << "Count various items in pcap files. The items are counted and the counted results are stored in a target directory" << endl <<endl;
    cout << "OPTIONS" <<endl <<endl;
    cout << "--help\t\t-h\tshows this screen"<<endl;
    cout << "--source\t-s\tSpecify the source filename"<<endl;
    cout << "--target\t-t\tSpecify the target directory"<<endl;
    cout << endl;
    cout << "--ips\t\t-i\tCount source IP addresses" <<endl;
}

bool PcapCount::check_target_dir(string target)
{
    if (filesystem::exists(target) && filesystem::is_directory(target)) {
        return true;
    } else {
        cerr<< "Target directory does not exists"<<endl;
    }
    return false;
}

void PcapCount::load_ip_cnt_map(const string& filename)
{
// Load the serialized data
    std::ifstream file(filename, std::ios::binary);
    if (file.is_open()) {
        boost::archive::binary_iarchive ia(file);
        ia >> pch.cnt_ip_src;
        file.close();
        std::cout << "Data deserialized successfully." << std::endl;

        // Print the deserialized map
        for (const auto& entry : pch.cnt_ip_src) {
            std::cout << "Key: " << entry.first << ", Value: " << entry.second << std::endl;
        }
    } else {
        std::cerr << "Error: Unable to open file " << filename << " for reading." << std::endl;
    }
}
// Returns a vector with textual representations with maps found in the serialized data
vector<string> PcapCount::getFlagsText(PcapCountHeader pch)
{
    vector<string> out;
    if (pch.flag_src_ips) {
        out.push_back("ip_src");
    }
    if (pch.flag_ip_dst) {
        out.push_back("ip_dst");
    }
    if (pch.flag_proto) {
        out.push_back("proto");
    }
    return out;
}


//Returns a copy of the source ip map sorted by IP with the moist packets
vector<pair<uint32_t, uint32_t>> PcapCount::sortSourceIPsbyOccurence(const PcapCountHeader &pch)
{
    vector<std::pair<uint32_t, uint32_t>> out(pch.cnt_ip_src.begin(), pch.cnt_ip_src.end());
    // Sort the vector by values (second element of pair)
    sort(out.begin(), out.end(), [](const auto& lhs, const auto& rhs) {
    return lhs.second > rhs.second;
    });
    return out;
}

//Returns a copy of the destination ip map sorted by IP with the moist packets
vector<pair<uint32_t, uint32_t>> PcapCount::sortDestinationIPsbyOccurence(const PcapCountHeader &pch)
{
    vector<std::pair<uint32_t, uint32_t>> out(pch.cnt_ip_dst.begin(), pch.cnt_ip_dst.end());
    // Sort the vector by values (second element of pair)
    sort(out.begin(), out.end(), [](const auto& lhs, const auto& rhs) {
    return lhs.second > rhs.second;
    });
    return out;
}

//Returns a copy of the proto map sorted by IP with the moist packets
vector<pair<uint32_t, uint32_t>> PcapCount::sortProtosbyOccurence(const PcapCountHeader &pch)
{
    vector<std::pair<uint32_t, uint32_t>> out(pch.cnt_proto.begin(), pch.cnt_proto.end());
    // Sort the vector by values (second element of pair)
    sort(out.begin(), out.end(), [](const auto& lhs, const auto& rhs) {
    return lhs.second > rhs.second;
    });
    return out;
}

void PcapCount::sumMap(const map <uint32_t, uint32_t> &myMap) {
   for (auto it = myMap.begin(); it != myMap.end(); ++it) {
        sumData[it->first]+=it->second;
    }
}

vector<pair<uint32_t, uint32_t>> PcapCount::sortSumedData(void)
{
    vector<std::pair<uint32_t, uint32_t>> out(sumData.begin(), sumData.end());
    // Sort the vector by values (second element of pair)
    sort(out.begin(), out.end(), [](const auto& lhs, const auto& rhs) {
    return lhs.second > rhs.second;
    });
    return out;
}
