using namespace std;
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <cstring>
#include <stdint.h>
#include <cstdlib>
#include <map>
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <filesystem>
namespace fs = std::filesystem;

class QueryCount {
public:
    // Constructor
    QueryCount();
    void usage(void);
    void load_ip_cnt_map(const string& filename);
    void setIPaddress(const char* addr);
    void listFilesRecursive(const fs::path& dirPath);
    // Attributes
    map <uint32_t, uint32_t> counted_data;
    string strIPaddress;
    uint32_t ip;
    string rootDir;
};

QueryCount::QueryCount(){
    // Put constructor stuff here
}

void QueryCount::usage(void)
{
    cout << "querycount -r root_directory -d day -i ip_address in dotted decimal notion" <<endl;
}

void QueryCount::load_ip_cnt_map(const string& filename)
{
// Load the serialized data
    std::ifstream file(filename, std::ios::binary);
    if (file.is_open()) {
        boost::archive::binary_iarchive ia(file);
        ia >> this->counted_data;
        file.close();
        cout << filename<<","<<this->strIPaddress<<","<<counted_data[this->ip]<<endl;
        // Go through the deserialized map
        //for (const auto& entry : counted_data) {
        //    if (entry.first == this->ip) {
        //        std::cout << filename <<","<< this->strIPaddress << entry.first << ",Count: " << entry.second << std::endl;
        //    }
        //}
    } else {
        std::cerr << "Error: Unable to open file " << filename << " for reading." << std::endl;
    }
}


void QueryCount::listFilesRecursive(const fs::path& dirPath) {
    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        if (fs::is_regular_file(entry.path())) {
            this->counted_data.clear();
            this->load_ip_cnt_map(entry.path().string());
        }
    }
}

void QueryCount::setIPaddress(const char* addr)
{
    struct in_addr saddr;
    this->strIPaddress=string(addr);
    if (inet_pton(AF_INET, addr, &saddr) < 0){
        // Something went wrong
        cerr<<"[ERROR] Invalid IP address."<<addr<<endl;
        // FIXME to some exception handling
        std::exit(1);
    } else {
        this->ip = ntohl(saddr.s_addr);
   }
}

int main(int argc, char* argv[]) {
    std::string directoryPath = "";
    int opt;
    QueryCount qc;
    while ((opt = getopt(argc, argv, "hi:r:")) != -1) {
        switch (opt) {
            case 'h':
                return EXIT_SUCCESS;
            case 'i':
                qc.setIPaddress(optarg);
                break;
            case 'r':
                qc.rootDir = string(optarg);
                break;
            default: /* '?' */
                fprintf(stderr, "[ERROR] Invalid command line was specified\n");
        }
    }

    // Call the function to list files recursively
    qc.listFilesRecursive(qc.rootDir);
    return 0;
}
