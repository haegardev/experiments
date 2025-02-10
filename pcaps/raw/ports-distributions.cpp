// Skeleton created by AI, manually completed
#include <iostream>
#include <pcap.h>
#include <zlib.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <unordered_map>

// Data structure to store source IP -> destination port frequency
std::unordered_map<uint32_t, std::unordered_map<uint16_t, int>> connectionCount;


void printConnectionStats() {
    struct in_addr addr;
    std::cout << "\n=== Connection Statistics ===\n";
    for (const auto &entry : connectionCount) {
        const uint32_t srcIP = entry.first;
        addr.s_addr = srcIP;
        std::cout << "Source IP: " << inet_ntoa(addr) << " " << std::endl;
        for (const auto &portCount : entry.second) {
            std::cout << "\t\tDest Port: " << portCount.first << "  Count: " << portCount.second << std::endl;
        }
    }
}



// Function to handle each packet
void packetHandler(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    struct ip *ipHeader;
    struct tcphdr *tcpHeader=NULL;
    struct udphdr *udpHeader=NULL;
    uint16_t destPort = 0;
    uint32_t srcIP = 0;
    ipHeader = (struct ip *)(packet + 14);  // Skip Ethernet header (14 bytes)

    // Print timestamp
    //std::cout << "Timestamp: " << pkthdr->ts.tv_sec << "." << pkthdr->ts.tv_usec << "  ";

    // Check protocol
    if (ipHeader->ip_p == IPPROTO_TCP) {
        tcpHeader = (struct tcphdr *)(packet + 14 + (ipHeader->ip_hl * 4));
        destPort = ntohs(tcpHeader->th_dport);
    } else if (ipHeader->ip_p == IPPROTO_UDP) {
        udpHeader = (struct udphdr *)(packet + 14 + (ipHeader->ip_hl * 4));
        destPort = ntohs(udpHeader->uh_dport);
    }
    srcIP = ipHeader->ip_src.s_addr;
    connectionCount[srcIP][destPort]++;
}

// Function to open a possibly compressed pcap file
pcap_t* open_pcap_file(const char* filename, char* errbuf) {
    pcap_t* handle = nullptr;

    // Check if the file has a .gz extension (compressed)
    if (strlen(filename) > 3 && strcmp(filename + strlen(filename) - 3, ".gz") == 0) {

        gzFile gzfp = gzopen(filename, "rb");
        if (!gzfp) {
            std::cerr << "Error opening compressed file: " << filename << std::endl;
            return nullptr;
        }

        FILE* temp_fp = tmpfile(); // Create a temporary file
        if (!temp_fp) {
            std::cerr << "Error creating temporary file for decompression!" << std::endl;
            gzclose(gzfp);
            return nullptr;
        }

        // Decompress to temp file
        char buffer[8192];
        int bytesRead;
        while ((bytesRead = gzread(gzfp, buffer, sizeof(buffer))) > 0) {
            fwrite(buffer, 1, bytesRead, temp_fp);
        }

        // Close gzip file
        gzclose(gzfp);

        // Rewind temp file for reading
        rewind(temp_fp);

        // Open pcap file from temp file stream
        handle = pcap_fopen_offline(temp_fp, errbuf);
        if (!handle) {
            std::cerr << "Error reading decompressed pcap file: " << errbuf << std::endl;
            fclose(temp_fp);
            return nullptr;
        }
    } else {
        // Open a normal (uncompressed) pcap file
        handle = pcap_open_offline(filename, errbuf);
        if (!handle) {
            std::cerr << "Error opening pcap file: " << errbuf << std::endl;
            return nullptr;
        }
    }

    return handle;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <pcap file or .gz>" << std::endl;
        return 1;
    }

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* handle = open_pcap_file(argv[1], errbuf);
    if (!handle) {
        return 1;
    }

    pcap_loop(handle, 0, packetHandler, nullptr);

    pcap_close(handle);
    //printConnectionStats();
    return 0;
}

