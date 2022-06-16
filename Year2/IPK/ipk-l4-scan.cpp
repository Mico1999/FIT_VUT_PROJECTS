
/**
* @file ipk-14-scan.cpp
* @brief Provides UDP/TCP scannig of ports on the entered interface and domain/IP
* @author Marek Miček, xmicek08
* @date 10.4.2021
*/

#include "ipk-l4-scan.hpp"

/**
 * @brief Prints hint for user in case user entered invalid argument option
 */
void Print_Help() {
    
    cout << "Program ipk-l4-scan is simple TCP/UDP port scanner\n"
            "--usage of arguments:\n"
            "[-i interface | --interface interface] : specifies interface where ports will be scanned\n"
            "[-u port-ranges | --pu port-ranges] : specifies ports scanned by UDP scanning\n"
            "[-t port-ranges | --pt port-ranges] : specifies ports scanned by TCP scanning\n"
            "{-w timeout} : specifies time in ms application will be wait for host response\n"
            "[domain-name | ip-address] : specifies target of scanning\n";
}

/**
* @brief Fills IPv4 header data structure
*/
void Scanner::Create_IP_Header() {
    
    memset(this->header_ip, 0, sizeof(struct iphdr));
    (this->header_ip)->version = 4;
    (this->header_ip)->ihl = 5;
    (this->header_ip)->tos = 0;
    (this->header_ip)->tot_len = sizeof (struct ip) + sizeof (struct tcphdr);
    (this->header_ip)->id = htons (54321);	
    (this->header_ip)->frag_off = htons(16384);
    (this->header_ip)->ttl = 64;
    (this->header_ip)->protocol = IPPROTO_TCP;
    (this->header_ip)->check = 0;		//Set to 0 before calculating checksum

    if (inet_pton(AF_INET, this->Interface_IP, &(this->header_ip)->saddr) != 1) {
        this->Print_Error("Failed to create source address in IP header !\n");
    }
    if (inet_pton(AF_INET, this->Target_IP.c_str(), &(this->header_ip)->daddr) != 1) {
        this->Print_Error("Failed to create destination address in IP header !\n");
    }

    (this->header_ip)->check = Get_Checksum((uint16_t *) this->Datagram, (this->header_ip)->tot_len >> 1);
}

/**
* @brief Fills IPv6 header data structure
*/
void Scanner::Create_IP6_Header() {

    memset(this->header_ip6, 0, sizeof(struct ip6_hdr));
    (this->header_ip6)->ip6_ctlun.ip6_un2_vfc = 0x60;
    (this->header_ip6)->ip6_ctlun.ip6_un1.ip6_un1_flow = htonl(6<<28);
    (this->header_ip6)->ip6_ctlun.ip6_un1.ip6_un1_hlim = 0xFFU;
    (this->header_ip6)->ip6_ctlun.ip6_un1.ip6_un1_nxt = IPPROTO_TCP;
    (this->header_ip6)->ip6_ctlun.ip6_un1.ip6_un1_plen = htons(sizeof(struct tcphdr));
    (this->header_ip6)->ip6_src = ((struct sockaddr_in6 *)&(this->Src_data))->sin6_addr;
    (this->header_ip6)->ip6_dst = ((struct sockaddr_in6 *)&(this->Dst_data))->sin6_addr;

}

/**
* @brief Fills TCP header with all required data
* @param header Pointer to actula TCP header to be filled
* @param src_Port Number of source port choosen randomly
* @param dst_Port Number of destination port entered by user
*/
void Scanner::Create_TCP_Header(struct tcphdr **header, int src_Port, int dst_Port) {

    (*header)->source = htons(src_Port);
	(*header)->dest = htons(dst_Port);
	(*header)->seq = htonl(1105024978);
	(*header)->ack_seq = 0;
	(*header)->doff = sizeof(struct tcphdr) / 4;		//Size of tcp header
	(*header)->fin=0;
	(*header)->syn=1;
	(*header)->rst=0;
	(*header)->psh=0;
	(*header)->ack=0;
	(*header)->urg=0;
	(*header)->window = htons ( 14600 );	// maximum allowed window size
	(*header)->check = 0; 
	(*header)->urg_ptr = 0;
}

/**
* @brief Fills UDP header with all required data
* @param header Pointer to actula UDP header to be filled
* @param src_Port Number of source port choosen randomly
* @param dst_Port Number of destination port entered by user
*/
void Scanner::Create_UDP_Header(struct udphdr **header, int src_Port, int dst_Port) {

    (*header)->source = htons(src_Port);
    (*header)->dest = htons(dst_Port);
    (*header)->len = htons(8);
    (*header)->check = 0;
}

/**
 * @brief Computes checksum for TCP header
 * @param buffer Points to header for which is checksum computed
 * @param len Number of bytes to be calculated
 * <a href=https://stackoverflow.com/questions/8845178/c-programming-tcp-checksum">Source code inspiration</a>
*/
unsigned short Get_Checksum(unsigned short *buffer, int len) {

    unsigned long chsum = 0;
    while (len > 1) {
        chsum += *buffer++;
        len -= 2;
    }
    if (len == 1)
        chsum += *(u_char*)buffer;

    chsum = (chsum >> 16) + (chsum & 0xffff);
    chsum += (chsum >> 16);
    return (u_short)(~chsum);
}

/**
* @brief Prints error message for user in case of error
* @param ErrorMessage Message to be printed to stderr
*/
void Scanner::Print_Error(string ErrorMessage) {

    cerr << ErrorMessage;
    exit(1);
}

/**
* @brief Parses arguments of command line
*/
void Scanner::Parse_Arguments(int argc, char *argv[]) {

    int opts, index;
    enum Args{interface, pu, pt, w, domain, IP_adress};

    static struct option long_options[] ={
        {"interface", optional_argument, 0, 'i'},
        {"pu", required_argument, 0, 'u'},
        {"pt", required_argument, 0, 't'},
        {"wait", required_argument, 0, 'w'}
    };

    while ((opts = getopt_long(argc, argv, "i::u:t:w:", long_options, &index)) != -1) {
        switch (opts) 
        {
            case 'i':
            //-i has optional argument, getopt_long not recognize opt args
            // we check for it on the basis of first char in next argument
                if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') {
                    this->InterfaceEntered = true;
                    this->InterfaceName = argv[optind];
                }
                break;
            
            case 'u':
                this->Check_Ports(optarg, true);    //< check format of UDP ports
                break;

            case 't':
                this->Check_Ports(optarg, false);   //< check format of TCP ports
                break;

            case 'w':
                this->wait_timeout = stof(optarg);  //< store timeout as float in case user not entered int
                if (this->wait_timeout < 0)
                {
                    this->Print_Error("Negative timeout !\n");
                }
                break;
            default:
                Print_Help();
                exit(1);
        }
    }
    // no interface was entered, check num of arguments and print all interfaces
    if (!this->InterfaceEntered) {
        if ((optind + 1) != argc)
        {
            Print_Help();
           this->Print_Error("Invalid number of arguments !\n"); 
        }
        this->Print_Interfaces();
        exit(0);
    }
    // interface entered, ckeck only num of arguments
    else if ((optind + 1) != argc - 1) {
        Print_Help();
        this->Print_Error("Invalid number of arguments !\n");
    }

    this->Check_Target(argv[optind + 1]);           //< find data of target
    this->Check_Interface(this->InterfaceName);     //< find data of interface
    
    cout << "Interesting ports on " << argv[optind+1] << endl;
    cout << "PORT STATE" << endl;
}

/**
* @brief Prints all active interfaces in case no interface entered by user
*/
void Scanner::Print_Interfaces() {

    pcap_if_t *devices;
    if (pcap_findalldevs(&devices, this->Error_Buffer) != 0) {
        this->Print_Error("Failed to get list of all devices !\n");
    }

    if (devices == NULL) {
        cout << "No active device was found !\n";
    }

    // print name of each active interface
    for (pcap_if_t dev = *devices; dev.next != NULL; dev = *dev.next) {
        cout << dev.name << endl;
    }
    pcap_freealldevs(devices);
}

/**
* @brief Checks interface family and ID and stores this data for next using
* @param Interface Interface string parsed from argument parser
*/
void Scanner::Check_Interface(string Interface) {

    struct ifaddrs *ifaddr, *tmp;

    if (getifaddrs(&ifaddr) == -1) {
        this->Print_Error("Unable to get interface adress !\n");
    }

    tmp = ifaddr;

    // search for interface entered by user, must have same IP family as target
    while (ifaddr != NULL) {
        if (strcasecmp(ifaddr->ifa_name, Interface.c_str()) == 0 && ifaddr->ifa_addr->sa_family == this->Target_IP_Family)
        {
            break;
        }
        ifaddr = ifaddr->ifa_next;
    }

    if (ifaddr == NULL) {
        this->Print_Error("Given interface was not found or has different IP family than target !\n");
    }
    
    this->Interface_IP_Family = ifaddr->ifa_addr->sa_family;
    struct sockaddr *sAddres = ifaddr->ifa_addr;

    //get interface IP
    if (this->Interface_IP_Family == AF_INET)
    {
        if ((getnameinfo(sAddres, sizeof(struct sockaddr_in), this->Interface_IP, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) < 0))
        {
            this->Print_Error("Unable to fetch interface IP !\n");
        }
        // we store interface IP also to sockaddr_storage structure, cause later it is more simple when filling headers for packet
        memcpy(&(this->Src_data), (struct sockaddr_in *)ifaddr->ifa_addr, sizeof(struct sockaddr_in));
    }
    else 
    {
        // we store interface IP also to sockaddr_storage structure, cause later it is more simple when filling headers for packet
        memcpy(&(this->Src_data), (struct sockaddr_in6 *)ifaddr->ifa_addr, sizeof(struct sockaddr_in6));
        inet_ntop(AF_INET6, ((struct sockaddr_in6 *) &(this->Src_data))->sin6_addr.s6_addr, this->Interface_IP, 128);
    }

    freeifaddrs(tmp);
}

/**
* @brief Ckecks target family and IP and stores this data for next using
* @param Target IP/domain main entered by user
*/
void Scanner::Check_Target(string Target) {

    struct addrinfo hints;
    struct addrinfo *res;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;                    
    hints.ai_socktype = SOCK_RAW;

    if ((getaddrinfo(Target.c_str(), NULL, &hints, &res)) != 0) {
        this->Print_Error("Unable to fetch target service !\n");
    }

    void *Add_Src;
    char Target_IP[100];

    // get target IP family
    switch (res->ai_family)
    {
        case AF_INET:
            Add_Src = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
            this->Target_IP_Family = AF_INET;
            break;

        case AF_INET6:
            Add_Src = &((struct sockaddr_in6 *)res->ai_addr)->sin6_addr;
            this->Target_IP_Family = AF_INET6;
            break;

        default:
            this->Print_Error("Invalid IP family of target !\n");
            break;
    }

    //get target IP
    inet_ntop(res->ai_family, Add_Src, Target_IP, 100);
    this->Target_IP = Target_IP;

    // we store target IP also to sockaddr_storage structure, cause later it is more simple when filling headers for packet
    memcpy(&(this->Dst_data), res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);
}

/**
* @brief Explodes port range on the basis of delimeter
* @param Ports Range of ports entered by user, separated by '-' or ','
* @param Delimeter Defines char which explodes string
* @param is_range Defines variant of port range, whether written with ',' or '-'
* @return Int vector with exploded port numbers which are ready to pass to scannig functions
*/
vector<int> Scanner::Explode_Ports(string Ports, char delimeter, bool is_range) {

    istringstream iss(Ports);
    vector<string> tokens;
    vector<int> result;

    // explode input string of ports
    for (string line; getline(iss, line, delimeter);)
    {
        tokens.push_back(move(line));
    }

    // variant where ports seperated by ','
    if (!is_range) {
        //convert to int vector
        for (unsigned i = 0; i < tokens.size(); i++)
        {
            result.push_back(atoi(tokens[i].c_str()));
        }
    }
    // variant with port range (1-65355), we must fill numbers in middle
    else {
        for (int i = atoi(tokens[0].c_str()); i <= atoi(tokens[1].c_str()); i++)
        {
            result.push_back(i);
        }
    }

    return result;
}

/**
* @brief Ckecks vailidity of port range entered by user
* @param Ports Range of ports entered by user, separated by '-' or ','
* @param Port_Family Defines whether ports belong to UDP or TCP
*/
void Scanner::Check_Ports(string Ports, bool is_UDP) {

    regex_t Port_regex1, Port_regex2;
    if (regcomp(&Port_regex1, "^[0-9]{1,5}+-[0-9]{1,5}+$", REG_EXTENDED) != 0 || regcomp(&Port_regex2, "^[0-9]{1,5}(,[0-9]{1,5})*$", REG_EXTENDED) != 0)
    {
        this->Print_Error("Failed to make regex compilation !\n");
    }

    // regex for ports separated by '-'
    int start, end;
    if (regexec(&Port_regex1, Ports.c_str(), 0, NULL, 0) == 0)
    {
        if (is_UDP) {
            this->UDP_ports = this->Explode_Ports(Ports, '-', true);    //< separate ports for UDP scanning
            start = this->UDP_ports[0];
            end = this->UDP_ports[1];
        }
        else {
            this->TCP_ports = this->Explode_Ports(Ports, '-', true);    //< separate ports for TCP scanning
            start = this->TCP_ports[0];
            end = this->TCP_ports[1];
        }
        regfree(&Port_regex1);

        // check left port is smaller than right, and if both belong to range 0-65535
        if (start > end || start < 0 || end > 65535) {
            this->Print_Error("Invalid port range !\n");
        }
    }

    // regex for ports separated by ','
    else if (regexec(&Port_regex2, Ports.c_str(), 0, NULL, 0) == 0)
    {
        if (is_UDP) {
            this->UDP_ports = this->Explode_Ports(Ports, ',', false);   //< separate ports for UDP scanning
            for (unsigned i = 0; i < this->UDP_ports.size(); i++)
            {
                if (this->UDP_ports[i] < 0 || this->UDP_ports[i] > 65535) {     //< change range of each port
                    this->Print_Error("Invalid UDP port number !\n");
                }
            }
        }
        else {
            this->TCP_ports = this->Explode_Ports(Ports, ',', false);   //< separate ports for TCP scanning
            for (unsigned i = 0; i < this->TCP_ports.size(); i++)
            {
                if (this->TCP_ports[i] < 0 || this->TCP_ports[i] > 65535) {     //< change range of each port
                    this->Print_Error("Invalid TCP port number !\n");
                }
            }
        }
        regfree(&Port_regex2);
    }

    else {
        this->Print_Error("Invalid format of UDP/TCP ports !\n");
    }
}

/*********************************  UDP SNIFFING  ******************************************************************************/

/**
* @brief Provides UDP scanning of each port specified by user
*        For each port is filled UDP header, set filter expression and sent packet to destination entered by user
*        Results of UDP scanning for each port are written out
*/
void Scanner::Scan_UDP() {

    struct udphdr * UDP_HEADER = (struct udphdr *) (this->Datagram + (this->Target_IP_Family == AF_INET ? sizeof(struct iphdr) : sizeof(struct ip6_hdr)));
    struct UDP_v6_PSEUDO_HEADER PSD_v6_Header;
    struct bpf_program UDP_bpf;
    bpf_u_int32 Net = {0};
    char Filter_Exp[256];
    memset(UDP_HEADER, 0, sizeof(udphdr));
    memset(&PSD_v6_Header, 0, sizeof(UDP_v6_PSEUDO_HEADER));
    int src_Port = rand() % 16383 + 49152;      //< source port is choosen from temporary ports (49152-65535)


    for (unsigned i = 0; i < this->UDP_ports.size(); i++)
    {
        Port_Closed = false;    //< firstly we are positive and consider port to be opened :)
        this->Create_UDP_Header(&UDP_HEADER, src_Port, this->UDP_ports[i]);

        if (this->Target_IP_Family == AF_INET6) //< must compute checksum over pseudoheader for IPv6
        {
            PSD_v6_Header.Src = ((struct sockaddr_in6 *)&(this->Src_data))->sin6_addr;
            PSD_v6_Header.Dst = ((struct sockaddr_in6 *)&(this->Dst_data))->sin6_addr;
            PSD_v6_Header.next_header = IPPROTO_UDP;
            PSD_v6_Header.protocol_length = htons(sizeof(struct udphdr));
            memcpy(&PSD_v6_Header.UDP_HEADER, UDP_HEADER, sizeof(struct udphdr));

            UDP_HEADER->check = Get_Checksum((unsigned short*) &PSD_v6_Header , sizeof (struct UDP_v6_PSEUDO_HEADER));
        }

        // compile and set filter for ICMP 3 messages
        snprintf(Filter_Exp, sizeof(Filter_Exp), 
            "dst host %s and src host %s and icmp [icmptype]=icmp-unreach and icmp [icmpcode]=3", this->Interface_IP, this->Target_IP.c_str());

        
        if (pcap_compile(UDP_SNIFFER_HANDLE, &UDP_bpf, Filter_Exp, 0, Net) != 0) {
            pcap_close(UDP_SNIFFER_HANDLE);
            this->Print_Error("Failed to compile UDP filter !\n");
        }

        if (pcap_setfilter(UDP_SNIFFER_HANDLE, &UDP_bpf) == -1) {
            pcap_close(UDP_SNIFFER_HANDLE);
            this->Print_Error("Failed to set filter for UDP sniffing !\n");
        }

        // send first packet
        if (sendto(this->sniff_Socket, this->Datagram, this->Target_IP_Family == AF_INET ? sizeof(struct iphdr) + sizeof(struct udphdr)
        : sizeof(struct ip6_hdr) + sizeof(struct udphdr), 0, (struct sockaddr *) &(this->Dst_data), sizeof(this->Dst_data)) < 0)
        {
            pcap_close(UDP_SNIFFER_HANDLE);
            this->Print_Error("Failed to send socket while UDP sniffing !\n");
        }

       
        thread UDP_Loop_Sniffer(&Scanner::Loop_UDP, this);  //< create catching thread
        sleep(this->wait_timeout/1000);                     //< wait for port response

        UDP_LOCK.lock();
        if (Port_Closed)
        {
            cout << this->UDP_ports[i] << "/udp closed\n";
        }
        else
        {
            cout << this->UDP_ports[i] << "/udp open\n";
        }
        UDP_LOCK.unlock();

        pcap_breakloop(UDP_SNIFFER_HANDLE);             //< break loop when actual port is scanned
        UDP_Loop_Sniffer.join();    
    }
    pcap_freecode(&UDP_bpf);
}

 /**
* @brief Function is called with new thread in UPD scanning
*        Starts infinite pcap_loop for catching ICMP 3 message, is broken when actual UDP port entered by user is scanned
*/
void Scanner::Loop_UDP() {

    pcap_loop(UDP_SNIFFER_HANDLE, -1, Callback_UDP, NULL);
}

/**
 * @brief Callback for UDP filter
 *        This callback is called in case filter catch packet with ICMP 3 message
 *        For more details about callback and it's arguments see link below
 * 
 * <a href="http://homes.di.unimi.it/~gfp/SiRe/2002-03/progetti/libpcap-tutorial.html">Callback explanation</a>
 */
void Callback_UDP(u_char *args, const struct pcap_pkthdr *pkthdr, const u_char *packet) {

    UDP_LOCK.lock();
    Port_Closed = true;     //< filter recognized ICMP 3 message so port is closed
    UDP_LOCK.unlock();
}

/*******************************************        TCP Sniffing       **************************************************************************************/

/**
* @brief Provides TCP scanning of each port specified by user
*        For each port is filled TCP header, set filter expression and sent packet to destination entered by user
*        Results of TCP scanning for each port are written out
*/
void Scanner::Scan_TCP() {

    struct tcphdr * TCP_HEADER = (struct tcphdr *) (this->Datagram + (this->Target_IP_Family == AF_INET ? sizeof(struct iphdr) : sizeof(struct ip6_hdr)));
    struct TCP_PSEUDO_HEADER PSD_Header;
    struct TCP_v6_PSEUDO_HEADER PSD_v6_Header;
    bpf_u_int32 Net = {0};
    memset(TCP_HEADER, 0, sizeof(tcphdr));
    memset(&PSD_Header, 0, sizeof(TCP_PSEUDO_HEADER));
    memset(&PSD_v6_Header, 0, sizeof(TCP_v6_PSEUDO_HEADER));
    int src_Port = rand() % 16383 + 49152;      //< source port is choosen from temporary ports (49152-65535)

    char SYN_FIL_EXP[512];
    char RST_FIL_EXP[512];
    struct bpf_program TCP_SYN_bpf;
    struct bpf_program TCP_RST_bpf;

    for (unsigned i = 0; i < this->TCP_ports.size(); i++)
    {
        this->Create_TCP_Header(&TCP_HEADER, src_Port, this->TCP_ports[i]);

        // IPv4 and IPv6 have different psudo headers for checksum computation
        if (this->Target_IP_Family == AF_INET)
        {
            if (inet_pton(AF_INET, this->Interface_IP, &PSD_Header.Src) != 1) {
                this->Print_Error("Failed to get destination for TCP pseudo header !\n");
            } 
            if (inet_pton(AF_INET, this->Target_IP.c_str(), &PSD_Header.Dst) != 1) {
                this->Print_Error("Failed to get destination for TCP pseudo header !\n");
            }
            PSD_Header.Protocol = IPPROTO_TCP;
            PSD_Header.Reserved = 0;
            PSD_Header.TCP_Len = htons(sizeof(struct tcphdr));
            memcpy(&PSD_Header.TCP_HEADER, TCP_HEADER, sizeof(struct tcphdr));

            TCP_HEADER->check = Get_Checksum((unsigned short*) &PSD_Header, sizeof(struct TCP_PSEUDO_HEADER));   
        }
        else
        {
            PSD_v6_Header.Src = ((struct sockaddr_in6 *)&(this->Src_data))->sin6_addr;
            PSD_v6_Header.Dst = ((struct sockaddr_in6 *)&(this->Dst_data))->sin6_addr;
            PSD_v6_Header.protocol_length = htons(sizeof(struct tcphdr));
            PSD_v6_Header.next_header = IPPROTO_TCP;
            memcpy(&PSD_v6_Header.TCP_HEADER, TCP_HEADER, sizeof(struct tcphdr));

            TCP_HEADER->check = Get_Checksum((unsigned short*) &PSD_v6_Header, sizeof(struct TCP_v6_PSEUDO_HEADER));    
        }

        TCP_Port_State = TCP_PORT_FILTERED;     //< firsly we consider port to be filtered

        if (this->Target_IP_Family == AF_INET)
        {
            snprintf(RST_FIL_EXP, sizeof(RST_FIL_EXP),"tcp[tcpflags] & (tcp-rst) != 0 and src host %s and dst host %s and tcp src port %d and tcp dst port %d",
            this->Target_IP.c_str(), this->Interface_IP, this->TCP_ports[i], src_Port);

            snprintf(SYN_FIL_EXP, sizeof(SYN_FIL_EXP),"tcp[tcpflags] & (tcp-syn|tcp-ack) != 0 or tcp[tcpflags] & (tcp-syn) != 0 and tcp[tcpflags] & (tcp-rst) = 0 and src host %s and dst host %s and tcp src port %d and tcp dst port %d",
            this->Target_IP.c_str(), this->Interface_IP, this->TCP_ports[i], src_Port);
        }
        else
        {
            snprintf(RST_FIL_EXP, sizeof(RST_FIL_EXP), "((ip6[6] == 6 && ip6[53] & 0x04 == 0x04) || (ip6[6] == 6 && tcp[13] & 0x04 == 0x04)) and src host %s and dst host %s and tcp src port %d and tcp dst port %d",
            this->Target_IP.c_str(), this->Interface_IP, this->TCP_ports[i], src_Port);

            snprintf(SYN_FIL_EXP, sizeof(SYN_FIL_EXP), "((tcp[13] & 0x12 == 0x12) || (ip6[6] == 6 && ip6[53] & 0x12 == 0x12)) || ((tcp[13] & 0x02 == 0x02) || (ip6[6] == 6 && ip6[53] & 0x02 == 0x02)) and src host %s and dst host %s and tcp src port %d and tcp dst port %d",
            this->Target_IP.c_str(), this->Interface_IP, this->TCP_ports[i], src_Port);
        }

        // compile and set both filters before scanning port
        if (pcap_compile(TCP_SYN_SNIFFER, &TCP_SYN_bpf, SYN_FIL_EXP, 0, Net) == -1) {
            pcap_close(TCP_SYN_SNIFFER);
            this->Print_Error("Failed to compile TCP SYN filter !\n");
        }
        if (pcap_setfilter(TCP_SYN_SNIFFER, &TCP_SYN_bpf) == -1) {
            pcap_close(TCP_SYN_SNIFFER);
            this->Print_Error("Failed to set filter for TCP SYN sniffing !\n");
        }

        if (pcap_compile(TCP_RST_SNIFFER, &TCP_RST_bpf, RST_FIL_EXP, 0, Net) == -1) {
            pcap_close(TCP_RST_SNIFFER);
            this->Print_Error("Failed to compile TCP RST filter !\n");
        }
        if (pcap_setfilter(TCP_RST_SNIFFER, &TCP_RST_bpf) == -1) {
            pcap_close(TCP_RST_SNIFFER);
            this->Print_Error("Failed to set filter for TCP RST sniffing !\n");
        }
        
        // send first packet
        if (sendto(this->sniff_Socket, this->Datagram, this->Target_IP_Family == AF_INET ? sizeof(struct iphdr) + sizeof(struct tcphdr)
        : sizeof(struct ip6_hdr) + sizeof(struct tcphdr), 0, (struct sockaddr *) &(this->Dst_data), sizeof(this->Dst_data)) < 0)
        {
            this->Print_Error("Failed to send socket while TCP sniffing !\n");
        }

        thread RST_Thread(&Scanner::Loop_TCP_RST, this);    //< create thread for RST responses
        thread SYN_Thread(&Scanner::Loop_TCP_SYN, this);    //< create thread for ACK-SYN responses

        sleep(this->wait_timeout/1000);                     //< wait for port response

        TCP_LOCK.lock();
        if (TCP_Port_State == TCP_PORT_OPEN)
        {
            cout << TCP_ports[i] << "/tcp open\n";
            TCP_LOCK.unlock();
        }
        else if (TCP_Port_State == TCP_PORT_CLOSED)
        {
            cout << TCP_ports[i] << "/tcp closed\n";
            TCP_LOCK.unlock();
        }
        // try one more time to confirm filtered, so send second packet
        else
        {
            TCP_LOCK.unlock();
            if (sendto(this->sniff_Socket, this->Datagram, this->Target_IP_Family == AF_INET ? sizeof(struct iphdr) + sizeof(struct tcphdr)
            : sizeof(struct ip6_hdr) + sizeof(struct tcphdr), 0, (struct sockaddr *) &(this->Dst_data), sizeof(this->Dst_data)) < 0)
            {
                this->Print_Error("Failed to send socket while TCP sniffing !\n");
            }

            sleep(this->wait_timeout/1000);

            TCP_LOCK.lock();
            if (TCP_Port_State == TCP_PORT_OPEN)
            {
                cout << TCP_ports[i] << "/tcp open\n";
            }
            else if (TCP_Port_State == TCP_PORT_CLOSED)
            {
                cout << TCP_ports[i] << "/tcp closed\n";
            }
            // port is definitely filtered
            else
            {
                cout << TCP_ports[i] << "/tcp filtered\n";
            }
            TCP_LOCK.unlock();
        }

        // break both loops when actual port is scanned
        pcap_breakloop(this->TCP_RST_SNIFFER);
        pcap_breakloop(this->TCP_SYN_SNIFFER);
        RST_Thread.join();
        SYN_Thread.join();
    }
    pcap_freecode(&TCP_SYN_bpf);
    pcap_freecode(&TCP_RST_bpf);
}

/**
* @brief Function is called with new thread in TCP scanning
*        Starts infinite pcap_loop for catching TCP RST packets, is broken when actual TCP port entered by user is scanned
*/
void Scanner::Loop_TCP_RST() {

    pcap_loop(this->TCP_RST_SNIFFER, -1, Callback_TCP_RST, NULL);
}

/**
* @brief Function is called with new thread in TCP scanning
*        Starts infinite pcap_loop for catching TCP SYN packets, is broken when actual TCP port entered by user is scanned
*/
void Scanner::Loop_TCP_SYN() {

    pcap_loop(this->TCP_SYN_SNIFFER, -1, Callback_TCP_SYN, NULL);
}

/**
 * @brief Callback for TCP RST filter
 *        This callback is called in case filter catch packet with RST response
 *        For more details about callback and it's arguments see link below
 * 
 * <a href="http://homes.di.unimi.it/~gfp/SiRe/2002-03/progetti/libpcap-tutorial.html">Callback explanation</a>
 */
void Callback_TCP_RST(u_char *args, const struct pcap_pkthdr *pkthdr, const u_char *packet) {

    TCP_LOCK.lock();
    TCP_Port_State = TCP_PORT_CLOSED;
    TCP_LOCK.unlock();
}

/**
 * @brief Callback for TCP SYN filter
 *        This callback is called in case filter catch packet with ACK-SYN response
 *        For more details about callback and it's arguments see link below
 * 
 * <a href="http://homes.di.unimi.it/~gfp/SiRe/2002-03/progetti/libpcap-tutorial.html">Callback explanation</a>
 */
void Callback_TCP_SYN(u_char *args, const struct pcap_pkthdr *pkthdr, const u_char *packet) {

    TCP_LOCK.lock();
    TCP_Port_State = TCP_PORT_OPEN;
    TCP_LOCK.unlock();
}


int main(int argc, char *argv[]) {

    Scanner scanner;
    scanner.Parse_Arguments(argc, argv);    //< parse arguments

    if (scanner.InterfaceEntered && scanner.UDP_ports.size() == 0 && scanner.TCP_ports.size() == 0) {
        scanner.Print_Error("Invalid usage of arguments !\n");      //< at least one port must be entered
    }
    
    memset(scanner.Datagram, 0, 4096);
    if (scanner.Target_IP_Family == AF_INET)    //< create IP header on the basis of target IP family
    {
        scanner.Create_IP_Header();
    }
    else
    {
        scanner.Create_IP6_Header();
    }

    // create raw socket
    scanner.sniff_Socket = socket(scanner.Target_IP_Family, SOCK_RAW, IPPROTO_RAW);
    if (scanner.sniff_Socket < 0) {
        scanner.Print_Error("Unable to make sniff socket !\n");
    }

    // allow us to create our IP header (IPv4/IPv6)
    const int option = 1;
    if (scanner.Target_IP_Family == AF_INET) {
        if (setsockopt(scanner.sniff_Socket, IPPROTO_IP, IP_HDRINCL, &option, sizeof(option)) < 0)
        {
            scanner.Print_Error("Failed to set socket option for IPV4 socket !\n");
        }
    }
    else {
        if (setsockopt(scanner.sniff_Socket, IPPROTO_IPV6, IP_HDRINCL, &option, sizeof(option)) < 0)
        {
            scanner.Print_Error("Failed to set socket option for IPV6 socket !\n");
        }
    }

    if (scanner.TCP_ports.size() != 0)  //< make TCP scan only if user entered some ports
    {
        // create fileter handles for RST and SYN responses
        scanner.TCP_RST_SNIFFER = pcap_open_live(scanner.InterfaceName.c_str(), BUFSIZ, 1, -1, scanner.Error_Buffer);
        scanner.TCP_SYN_SNIFFER = pcap_open_live(scanner.InterfaceName.c_str(), BUFSIZ, 1, -1, scanner.Error_Buffer);
        if (scanner.TCP_RST_SNIFFER == NULL || scanner.TCP_SYN_SNIFFER == NULL) 
        {
            close(scanner.sniff_Socket);
            scanner.Print_Error("Failed to open filter handle !\n");
        }

        scanner.Scan_TCP();
        pcap_close(scanner.TCP_RST_SNIFFER);
        pcap_close(scanner.TCP_SYN_SNIFFER);
    }

    if (scanner.UDP_ports.size() != 0)  //< make UDP scan only if user entered some ports
    {
        //we must change values in IP header we filled before
        if (scanner.Target_IP_Family == AF_INET) {
            scanner.header_ip->protocol = IPPROTO_UDP;
        }
        else {
            scanner.header_ip6->ip6_ctlun.ip6_un1.ip6_un1_nxt = IPPROTO_UDP;
        }

        // create filter handle for ICMP 3 messages
        UDP_SNIFFER_HANDLE = pcap_open_live(scanner.InterfaceName.c_str(), BUFSIZ, 1, -1, scanner.Error_Buffer);
        if (UDP_SNIFFER_HANDLE == NULL) 
        {
            close(scanner.sniff_Socket);
            scanner.Print_Error("Failed to open filter handle !\n");
        }
        scanner.Scan_UDP();
        pcap_close(UDP_SNIFFER_HANDLE);
    }

    close(scanner.sniff_Socket);
    return 0;
}