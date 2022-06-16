/*
* @file ipk-l4-scan.hpp
* @brief Header file for main program of tcp/udp port scanner
* @author Marek Miček, xmicek08
* @date 7.4.2021
*/

#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <getopt.h>
#include <pcap.h>
#include <vector>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netdb.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <regex.h>
#include <pcap.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <mutex>
#include <thread>
#include <cmath>
#include <net/ethernet.h>

using namespace std;

/* Represent possible states of port for TCP scanning */
#define TCP_PORT_OPEN 1
#define TCP_PORT_CLOSED 2
#define TCP_PORT_FILTERED 3

/* UDP scanning global variables*/
pcap_t *UDP_SNIFFER_HANDLE;     //< UDP filter handle
bool Port_Closed;               //< signalize whether port is close or not, UDP callback modifies this var
mutex UDP_LOCK;                 //< prevents data race when more thread accesing var Port_Closed

/* TCP scanning global variables */
int TCP_Port_State;             //< stores state of actual scanned port, TCP callbacks modifies this var
mutex TCP_LOCK;                 //< prevents data race when more thread accesing var TCP_PORT_STATE

/**
 * @brief Pseudo header for IPv4 TCP checksum calculation
 * <a href="https://tools.ietf.org/html/rfc793">Source code inspiration</a> 
 */
struct TCP_PSEUDO_HEADER
{
    unsigned int Src;
    unsigned int Dst;
    unsigned char Reserved;
    unsigned char Protocol;
    unsigned short TCP_Len;

    tcphdr TCP_HEADER;  //< checksum calculated over pseudo header and TCP segment
};

/**
 * @brief Pseudo header for IPv6 TCP checksum calculation
 * <a href="https://tools.ietf.org/html/rfc2460">Source code inspiration</a> 
 */
struct TCP_v6_PSEUDO_HEADER
{
    struct in6_addr Src;
    struct in6_addr Dst;
    unsigned int protocol_length;
    unsigned char zero[3];
    unsigned char next_header;

    struct tcphdr TCP_HEADER;
};

/**
 * @brief Pseudo header for IPv6 UDP checksum calculation
 *  <a href="https://tools.ietf.org/html/rfc2460">Source code inspiration</a>
 */
struct UDP_v6_PSEUDO_HEADER
{
    struct in6_addr Src;
    struct in6_addr Dst;
    unsigned int protocol_length;
    unsigned char zero[3];
    unsigned char next_header;

    struct udphdr UDP_HEADER;
};


/**
 * @brief Computes checksum for TCP header
 * @param buffer Points to header for which is checksum computed
 * @param len Number of bytes to be calculated
 * <a href=https://stackoverflow.com/questions/8845178/c-programming-tcp-checksum">Source code inspiration</a>
*/
unsigned short Get_Checksum(unsigned short *buffer, int len);

/**
 * @brief Callback for TCP RST filter
 *        This callback is called in case filter catch packet with RST response
 *        For more details about callback and it's arguments see link below
 * 
 * <a href="http://homes.di.unimi.it/~gfp/SiRe/2002-03/progetti/libpcap-tutorial.html">Callback explanation</a>
 */
void Callback_TCP_RST(u_char *args, const struct pcap_pkthdr *pkthdr, const u_char *packet);

/**
 * @brief Callback for TCP SYN filter
 *        This callback is called in case filter catch packet with ACK-SYN response
 *        For more details about callback and it's arguments see link below
 * 
 * <a href="http://homes.di.unimi.it/~gfp/SiRe/2002-03/progetti/libpcap-tutorial.html">Callback explanation</a>
 */
void Callback_TCP_SYN(u_char *args, const struct pcap_pkthdr *pkthdr, const u_char *packet);

/**
 * @brief Callback for UDP filter
 *        This callback is called in case filter catch packet with ICMP 3 message
 *        For more details about callback and it's arguments see link below
 * 
 * <a href="http://homes.di.unimi.it/~gfp/SiRe/2002-03/progetti/libpcap-tutorial.html">Callback explanation</a>
 */
void Callback_UDP(u_char *args, const struct pcap_pkthdr *pkthdr, const u_char *packet);

/**
 * @brief Prints hint for user in case user entered invalid argument option
 */
void Print_Help();

/**
 * @brief Stores all necessary data and functions use for port scanning
 *        Allows us to make more complex and flexible program
 */
class Scanner {

    public:

        /* Storage for interface */
        string InterfaceName;
        bool InterfaceEntered = false;
        char Interface_IP[128];
        int Interface_IP_Family;

        /* Storage for target of scannig */
        string Target_IP;
        int Target_IP_Family;

        /* Vectores storing UDP and TCP ports entered by user */
        vector<int> UDP_ports;
        vector<int> TCP_ports;

        /* Specifies time to wait for port response...implicit value is 5000 ms*/
        float wait_timeout = 5000;

        /* Handles for TCP RST and TCP SYN filters */
        pcap_t *TCP_RST_SNIFFER;
        pcap_t *TCP_SYN_SNIFFER;

        /* Supportive structures which stores data of interface and target */
        /* Have no special meaning, just allow us to use more comfortable implementation when filling headers for packets */
        struct sockaddr_storage Dst_data;
        struct sockaddr_storage Src_data;

        /* Dtagram to be send in packet */
        uint8_t Datagram[4096];

        /* IP and IP6 header structures */ 
        struct iphdr *header_ip = (struct iphdr *) Datagram;
        struct ip6_hdr *header_ip6 = (struct ip6_hdr *) Datagram;

        /* File descriptor to endpoint communication which will be used when sending packets */
        int sniff_Socket;

        /* Error buffer used in case some error occures when creating pcap handle */
        char Error_Buffer[PCAP_ERRBUF_SIZE];

        /**
        * @brief Prints error message for user in case of error
        * @param ErrorMessage Message to be printed to stderr
        */
        void Print_Error(string ErrorMessage);

        /**
        * @brief Parses arguments of command line
        */
        void Parse_Arguments(int argc, char *argv[]);

        /**
        * @brief Checks interface family and ID and stores this data for next using
        * @param Interface Interface string parsed from argument parser
        */
        void Check_Interface(string Interface);

        /**
        * @brief Prints all active interfaces in case no interface entered by user
        */
        void Print_Interfaces();

        /**
        * @brief Ckecks target family and IP and stores this data for next using
        * @param Target IP/domain main entered by user
        */
        void Check_Target(string Target);

        /**
        * @brief Ckecks vailidity of port range entered by user
        * @param Ports Range of ports entered by user, separated by '-' or ','
        * @param Port_Family Defines whether ports belong to UDP or TCP
        */
        void Check_Ports(string Ports, bool is_UDP);

        /**
        * @brief Explodes port range on the basis of delimeter
        * @param Ports Range of ports entered by user, separated by '-' or ','
        * @param Delimeter Defines char which explodes string
        * @param is_range Defines variant of port range, whether written with ',' or '-'
        * @return Int vector with exploded port numbers which are ready to pass to scannig functions
        */
        vector<int> Explode_Ports(string Ports, char delimeter, bool is_range);

        /**
         * @brief Fills IPv4 header data structure
         */
        void Create_IP_Header();

        /**
         * @brief Fills IPv6 header data structure
         */
        void Create_IP6_Header();

        /**
         * @brief Fills TCP header with all required data
         * @param header Pointer to actula TCP header to be filled
         * @param src_Port Number of source port choosen randomly
         * @param dst_Port Number of destination port entered by user
         */
        void Create_TCP_Header(struct tcphdr **header, int src_Port, int dst_Port);

        /**
         * @brief Fills UDP header with all required data
         * @param header Pointer to actula UDP header to be filled
         * @param src_Port Number of source port choosen randomly
         * @param dst_Port Number of destination port entered by user
         */
        void Create_UDP_Header(struct udphdr **header, int src_Port, int dst_Port);

        /**
        * @brief Provides UDP scanning of each port specified by user
        *        For each port is filled UDP header, set filter expression and sent packet to destination entered by user
        *        Results of UDP scanning for each port are written out
        */
        void Scan_UDP();

        /**
        * @brief Provides TCP scanning of each port specified by user
        *        For each port is filled TCP header, set filter expression and sent packet to destination entered by user
        *        Results of TCP scanning for each port are written out
        */
        void Scan_TCP();

        /**
         * @brief Function is called with new thread in UPD scanning
         *        Starts infinite pcap_loop for catching ICMP 3 message, is broken when actual UDP port entered by user is scanned
         */
        void Loop_UDP();

        /**
         * @brief Function is called with new thread in TCP scanning
         *        Starts infinite pcap_loop for catching TCP RST packets, is broken when actual TCP port entered by user is scanned
         */
        void Loop_TCP_RST();

        /**
         * @brief Function is called with new thread in TCP scanning
         *        Starts infinite pcap_loop for catching TCP SYN packets, is broken when actual TCP port entered by user is scanned
         */
        void Loop_TCP_SYN();
};