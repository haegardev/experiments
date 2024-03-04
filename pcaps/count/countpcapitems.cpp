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

    // Attributes
    map <uint32_t, uint32_t> counted_data;
    string source;
    string target;
    string target_srcip_file;
    bool cnt_src_ips;
};



map <uint32_t, uint32_t> ip_cnt_map;


PcapCount::PcapCount(){
    this->cnt_src_ips = false;
    // Put constructor stuff here
}

void PcapCount::setTarget(const char* tg)
{
    this->target=string(tg);
    this->target_srcip_file = string(tg);
    this->target_srcip_file.append("/src_ip.cnt");
    // TODO generate all the other counted fields such as proto, source port, destination port here
}

void PcapCount::setSource(const char* sr)
{
    this->source = string(sr);
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
        fprintf(stderr,"ERROR, could not allocate memory, abort");
        abort();
    }

    while( fgets(buf, BUFSIZE , stdin) ) {
        //printf("%s",buf);
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
                            }
                            ts = atoi(ptr);
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
        // Count IP addresses
        if (this->cnt_src_ips) {
            ++ip_cnt_map[ip_src];
        }
    }
    free(buf);
}


/* FIMXE add clean header and say what data it is
 *format: size_t size of the map  followed by series of uint32_t ip uint32_t frequency
 */

void PcapCount::store_ip_cnt_map(void) {
    ofstream file(this->target_srcip_file, ios::binary);
    if (file.is_open()) {
        boost::archive::binary_oarchive oa(file);
        oa << ip_cnt_map;
        file.close();
    } else {
    cerr << "Error: Unable to open file " << this->target_srcip_file << " for writing." << std::endl;
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
    cout << "--ips\t\t\-i\tCount source IP addresses" <<endl;
}

bool check_target(string target)
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
        ia >> ip_cnt_map;
        file.close();
        std::cout << "Data deserialized successfully." << std::endl;

        // Print the deserialized map
        for (const auto& entry : counted_data) {
            std::cout << "Key: " << entry.first << ", Value: " << entry.second << std::endl;
        }
    } else {
        std::cerr << "Error: Unable to open file " << filename << " for reading." << std::endl;
    }
}


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
                fprintf(stderr, "[ERROR] Invalid command line was specified\n");
        }
    }
    cout<<"Source " <<pc.source <<endl;
    cout<<"Target SRC "<<pc.target_srcip_file << endl;
    cout<<"Target " <<pc.target <<endl;

    if (check_target(pc.target) == false){
        return EXIT_FAILURE;
    }
    pc.read_from_stdin();
    cout<<"Start to serailize" <<endl;
    pc.store_ip_cnt_map();
    return 0;
}
