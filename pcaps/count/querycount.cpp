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
#include <ctime>
#include "PcapCount.h"
namespace fs = std::filesystem;

class QueryCount {
public:
    // Constructor
    QueryCount();
    void usage(void);
    void load_ip_cnt_map(const string& filename);
    void setIPaddress(const char* addr);
    void listFilesRecursive(const fs::path& dirPath);
    void printMetaData(const string &filename);
    string epochToDateTimeString(time_t epochTime);
    void printSortedSourceIPs(void);
    void printSortedDestinationIPs(void);
    void printSortedProtos(void);
    void printSortedSums(void);

    // Attributes
    string strIPaddress;
    uint32_t ip;
    string rootDir;
    PcapCountHeader pch;
    PcapCount pc;
    // Various flags for retrieving some data
    bool flag_ip;
    bool flag_list;
    bool flag_metadata;
    void setListOption(const char* optarg);
    bool lflag_ip_src;
    bool lflag_ip_dst;
    bool lflag_proto;
    bool lflag_sum;
};

QueryCount::QueryCount(){
    // Put constructor stuff here
}

void QueryCount::usage(void)
{
    cout << "querycount -r root_directory -d day -i ip_address in dotted decimal notion" <<endl;
}

string QueryCount::epochToDateTimeString(time_t epochTime)
{
    // Convert epoch time to a tm structure representing local time
    struct tm *timeInfo = std::gmtime(&epochTime);

    // Format the date-time string
    char buffer[20]; // Sufficient buffer size for year-month-day hour:minute:second
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);

    return std::string(buffer);
}


void QueryCount::printMetaData(const string& filename)
{
     cout << "Metadata of the archive: "<< filename << endl;
     cout << "Description: " << pch.description <<endl;
     cout << "Version: " << pch.pcapCountVersion << endl;
     cout << "Source: " <<pch.source <<endl;
     cout << "Target: " <<pch.target <<endl;
     cout << "Target file: " <<pch.target_srcip_file << endl;
     cout << "Oldest IP address: "<<pch.firstSeen << " (" <<this->epochToDateTimeString(pch.firstSeen)<<") (UTC)"<<endl;
     cout << "Newest IP address: "<<pch.lastSeen << " (" <<this->epochToDateTimeString(pch.lastSeen)<<") (UTC)"<<endl;
     cout << "Included maps listed below" <<endl;
     for (const auto& flag : this->pc.getFlagsText(this->pch)) {
        cout << flag << std::endl;
    }
}

void QueryCount::printSortedSourceIPs(void)
{
    struct in_addr addr;
    // Output the sorted vector
    vector<std::pair<uint32_t, uint32_t>> vec = this->pc.sortSourceIPsbyOccurence(pch);
    cout <<"Source IP address,Occurence"<<endl;
    for (const auto& pair : vec) {
        addr.s_addr = htonl(pair.first);
        cout << inet_ntoa(addr) << ","<< pair.second << endl;
    }
}

void QueryCount::printSortedDestinationIPs(void)
{
    struct in_addr addr;
    // Output the sorted vector
    vector<std::pair<uint32_t, uint32_t>> vec = this->pc.sortDestinationIPsbyOccurence(pch);
    cout <<"Destination IP address,Occurence"<<endl;
    for (const auto& pair : vec) {
        addr.s_addr = htonl(pair.first);
        cout << inet_ntoa(addr) << ","<< pair.second << endl;
    }
}

void QueryCount::printSortedProtos(void)
{

    std::unordered_map<uint8_t, std::string> protocolMap = {
        {0, "HOPOPT"}, {1, "ICMP"}, {2, "IGMP"}, {3, "GGP"}, {4, "IPv4"},
        {6, "TCP"}, {8, "EGP"}, {12, "PUP"}, {17, "UDP"}, {20, "HMP"},
        {22, "XNS-IDP"}, {27, "RDP"}, {29, "ISO-TP4"}, {36, "XTP"}, {37, "DDP"},
        {41, "IPv6"}, {43, "IPv6-Route"}, {44, "IPv6-Frag"}, {46, "RSVP"}, {47, "GRE"},
        {50, "ESP"}, {51, "AH"}, {58, "IPv6-ICMP"}, {59, "IPv6-NoNxt"}, {60, "IPv6-Opts"},
        {89, "OSPF"}, {94, "IPIP"}, {103, "PIM"}, {132, "SCTP"}, {255, "Reserved"}
    };


    // Output the sorted vector
    vector<std::pair<uint32_t, uint32_t>> vec = this->pc.sortProtosbyOccurence(pch);
    cout <<"IP protocol,Occurence"<<endl;
    for (const auto& pair : vec) {
        cout << protocolMap[pair.first] << ","<< pair.second << endl;
    }
}


void QueryCount::setListOption(const char* optarg)
{
    string option(optarg);
    if (option == "ip_src") {
        this->lflag_ip_src = true;
        return;
    }

    if (option == "ip_dst") {
        this->lflag_ip_dst = true;
        return;
    }

    if (option == "proto") {
        this->lflag_proto = true;
        return;
    }

    cerr<<"Invalid option specified to list maps" <<endl;
}
void QueryCount::load_ip_cnt_map(const string& filename)
{
// Load the serialized data
    std::ifstream file(filename, std::ios::binary);
    if (file.is_open()) {
        boost::archive::binary_iarchive ia(file);
        ia >> pch;
        file.close();
        //FIXME check version and description
        if (this->flag_ip) {
            cout << filename<<","<<this->strIPaddress<<","<<pch.cnt_ip_src[this->ip]<<endl;
        }
        if (this->flag_metadata){
            this->printMetaData(filename);
        }
        if (lflag_ip_src == true) {
            if (lflag_sum == true) {
                cout <<"[INFO] doing sum of "<<filename <<endl;
                pc.sumMap(pch.cnt_ip_src);
            } else {
                this->printSortedSourceIPs();
            }
        }
        if (lflag_ip_dst == true) {
            this->printSortedDestinationIPs();
        }
        if (lflag_proto == true) {
            this->printSortedProtos();
        }
    } else {
        std::cerr << "Error: Unable to open file " << filename << " for reading." << std::endl;
    }
}


void QueryCount::listFilesRecursive(const fs::path& dirPath) {
    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        if (fs::is_regular_file(entry.path())) {
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

void QueryCount::printSortedSums(void)
{
    bool isIP=false;
    struct in_addr addr;

    if (lflag_sum==true) {
        if (lflag_ip_src) {
            cout<< "Source IP, Occurence" <<endl;
            isIP = true;
       }
    }
    // print the sorted data
    vector<std::pair<uint32_t, uint32_t>> vec = this->pc.sortSumedData();
    for (const auto& pair : vec) {
        if (isIP) {
            addr.s_addr = htonl(pair.first);
            cout << inet_ntoa(addr) << ","<< pair.second << endl;
        } else{
            cout << pair.first << ","<< pair.second << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    std::string directoryPath = "";
    int opt;
    QueryCount qc;
    while ((opt = getopt(argc, argv, "sl:mhi:r:")) != -1) {
        switch (opt) {
            case 'h':
                return EXIT_SUCCESS;
            case 'i':
                qc.flag_ip = true;
                qc.setIPaddress(optarg);
                break;
            case 'r':
                qc.rootDir = string(optarg);
                break;
            case 'm':
                qc.flag_metadata = true;
                break;
            case 'l':
                qc.setListOption(optarg);
                break;
            case 's':
                qc.lflag_sum = true;
                break;
            default: /* '?' */
                fprintf(stderr, "[ERROR] Invalid command line was specified\n");
        }
    }

    // Call the function to list files recursively
    qc.listFilesRecursive(qc.rootDir);
    if (qc.lflag_sum){
        qc.printSortedSums();
    }
    return 0;
}

