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
        // Count IP addresses
        if (pch.flag_src_ips) {
            ++pch.counted_data[ip_src];
        }
        // TODO add switches to enable
        ++pch.cnt_proto[proto];
        ++pch.cnt_ip_dst[ip_dst];
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
        ia >> pch.counted_data;
        file.close();
        std::cout << "Data deserialized successfully." << std::endl;

        // Print the deserialized map
        for (const auto& entry : pch.counted_data) {
            std::cout << "Key: " << entry.first << ", Value: " << entry.second << std::endl;
        }
    } else {
        std::cerr << "Error: Unable to open file " << filename << " for reading." << std::endl;
    }
}

