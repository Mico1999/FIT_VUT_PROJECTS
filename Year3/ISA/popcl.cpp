/**
 * @file popcl.cpp
 * @brief POP3 client application for downloading messages from server
 * @author Marek Micek, xmicek08
 * @date 30.9.2021
 */ 

#include "popcl.hpp"

/**
 * @brief Prints hint how to use program, in case of incorrect using by user
 */
void PrintHelp() {

    cout << "Popcl is a client app for downloading messages from POP3 server\n"
            "--running the program:\n "
            "popcl <server> [-p <port>] [-T|-S [-c <certfile>] [-C <certaddr>]] [-d] [-n] -a <auth_file> -o <out_dir>\n"
            "--usage of arguments:\n"
            "<server>: Required paramether specefing host name/IP addres of server\n"
            "[-p port]: Optional paramether, port number on server side\n"
            "[-T]: Optional paramether, turns on an ecryption of communication\n"
            "[-S]: Optional paramether, starts with unecrypted version and with STLS command switches to encrypted version\n"
            "[-c <certfile>]: File with certificates for validation of certificate for SSL/TLS communication, optional but only with -T or -S paramethers\n"
            "[-C <certaddr>]: Directory where to find certificates for encrypted communication, optional but only with -T or -S paramethers\n"
            "[-d]: Sends to server a command to delete messages\n"
            "[-n]: Sends to server a command to download only new messages\n"
            "[-a <auth_file>] Required argument with data for authetification\n"
            "[-o <out_dir>]: Specifies directory where to download messages from server\n";
} 

/**
 * @brief Prints error message on stdrr in case of error was detected
 * @param ErrMsg Error message written on stderr
 */
void PrintError(string ErrMsg) {

    cerr << ErrMsg << endl;
    exit(1);
}

/**
 * @brief Parses arguments from command line
 */
void ParseArgs(int argc, char *argv[]) {

    bool AuthFileEntered = false;
    bool OutFileEntered = false;
    bool Server_Set = false;
   
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) { //< print hint for user
        PrintHelp();
        exit(0);
    }

    if (argc < 2) {
        PrintHelp();
        PrintError("\n\nInvalid number of arguments !!!");
    }

    for (int i = 1; i <= argc -1; i++)
    {
       if (strcmp(argv[i], "-p") == 0)
       {
           try
           {
               Popcl_args.Port = stoi(argv[i+1]);   //< take argument of -p
               i++;                                 //< step over argument of -p
           }
           catch(const invalid_argument &error)
           {
               PrintError("Exception occured when converting port number to int, probably not enter int number !!!");
           } 
       }
       else if (strcmp(argv[i], "-T") == 0)
       {
           Popcl_args.TLS_Entered = true;
       }
       else if (strcmp(argv[i], "-S") == 0)
       {
           Popcl_args.SSL_Entered = true;
       }
       else if (strcmp(argv[i], "-c") == 0)
       {
           Popcl_args.CertificateFile = argv[i+1];
           i++;
       }
       else if (strcmp(argv[i], "-C") == 0)
       {
           Popcl_args.CertificateAddr = argv[i+1];
           i++;
       }
       else if (strcmp(argv[i], "-d") == 0)
       {
           Popcl_args.Delete_Entered = true;
       }
       else if (strcmp(argv[i], "-n") == 0)
       {
           Popcl_args.New_Entered = true;
       }
       else if (strcmp(argv[i], "-a") == 0)
       {
           Popcl_args.AuthFile = argv[i+1];
           i++;
           AuthFileEntered = true;
       }
       else if (strcmp(argv[i], "-o") == 0)
       {
           Popcl_args.OutDir = argv[i+1];
           i++;
           OutFileEntered = true;
       }
       
       else if (!Server_Set)
       {
           Popcl_args.ServerName = argv[i];
           Server_Set = true;
       }
       else     //< invalid option from user was set
       {
           PrintHelp();
           PrintError("\n\nInvalid option in command line arguments !!!");
       }
    }

    // checking if authen. and output file were entered
    if (!AuthFileEntered || !OutFileEntered)
    {
        PrintHelp();
        PrintError("\n\nMissing authentification or output file !!!");
    }
    // checking if -d and -n was not entered together
    if (Popcl_args.Delete_Entered && Popcl_args.New_Entered) {
        PrintHelp();
        PrintError("\n\nYou can not enter paramethers -d and -n together !!!");
    }
}

// Constructor for POP3 Client
POP3_Client::POP3_Client() {
    
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    SSL_library_init();
}

// Destructor for POP3 Client
POP3_Client::~POP3_Client() {

}

/**
* @brief Connect to server in encrypted POP3s mode (paramether -T was set)
*/
void POP3_Client::Connect_Server_Sec() {

    // load SSl certificates
    Load_Certificates();

    if (!Server_Is_Ipv6) //< server is IPv4
    {
        //set bio object and auto handshake in case server wants to perform it during read/write operations
        bio = BIO_new_ssl_connect(ctx);
        BIO_get_ssl(bio, & ssl);
        SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

        // set bio host name
        string Bio_Host = Popcl_args.ServerName + ":" + to_string(Popcl_args.Port);
        BIO_set_conn_hostname(bio, Bio_Host.c_str());

        if(BIO_do_connect(bio) <= 0) {

            ERR_print_errors_fp(stderr);
            PrintError("\nFailed to connect to bio !!!");
        }
    }
    else    //< server is IPv6
    {
        Connect_Server_IPv6();
        ssl = SSL_new(ctx);
        SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
        BIO_set_ssl(bio, ssl, BIO_CLOSE);
        SSL_set_fd(ssl, socketIpv6);

        if (SSL_connect(ssl) != 1) {
            PrintError("Failed to make ssl connection !!!");
        }
        if (SSL_do_handshake(ssl) != 1) {
            PrintError("Failed to make ssl handshake !!!");
        }
    }

    if( SSL_get_peer_certificate(ssl) == NULL) {
        PrintError("No certificate received from server !!!");
    }

    if (SSL_get_verify_result(ssl) != X509_V_OK){
        cout << SSL_get_verify_result(ssl) << endl;
        PrintError("Certificate for encrypted communication was not valid !!!");
    }

    Get_Server_Response();
    if (!Is_Possitive())    //< look for "+OK" response from server
    {
        PrintError("Connection with server was not set !!!");
    }
}

/**
* @brief Connect to server in unencrypted mode and then sends
*        command StartTLS to switch into encrypted mode (paramether -S was set)
*/
void POP3_Client::Connect_Server_StartTLS() {

    Load_Certificates();

    if (!Server_Is_Ipv6) {      //< server is IPv4
        Connect_Server();       //< connect with unecrypted channel first
    }
    else {  //< server is IPv6
        Connect_Server_IPv6();
    }
    Send_Server_Command("STLS\r\n");
    Get_Server_Response();
    if (!Is_Possitive()) {
        PrintError("Negative response from server after STLS command !!!");
    }
    
    ssl = SSL_new(ctx);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
    SSL_Active = true;
    SSL_set_bio(ssl, bio, bio);

     if (SSL_connect(ssl) != 1) {
            PrintError("Failed to make ssl connection !!!");
        }
    if (SSL_do_handshake(ssl) != 1) {
        PrintError("Failed to make ssl handshake !!!");
    }
    if(SSL_get_peer_certificate(ssl) == NULL) {
        PrintError("No certificate received from server !!!");
    }
    if (SSL_get_verify_result(ssl) != X509_V_OK){
        PrintError("Certificate for encrypted communication was not valid !!!");
    }
}

/**
*  @brief Connects to server in unencrypted mode (for IPv4 address)
*/
void POP3_Client::Connect_Server() {

    // set bio host name
    string Bio_Host = Popcl_args.ServerName + ":" + to_string(Popcl_args.Port);
    bio = BIO_new_connect(Bio_Host.c_str());
    if(bio == NULL) {
        PrintError("Failed to conncet bio object !!!");
    }

    if(BIO_do_connect(bio) <= 0) {
        PrintError("Conncetion with server is not set !!!");   
    }

    Get_Server_Response();
    if (!Is_Possitive())    //< look for "+OK" response from server
    {
        PrintError("Connection with server was not set !!!");
    }
}

/**
* @brief Connect to server in unencrypted mode (for Ipv6 address)
*/
void POP3_Client::Connect_Server_IPv6() {

    if ((socketIpv6 = socket(PF_INET6, SOCK_STREAM, 0)) < 0) {
        PrintError("Fialed to create socket for IPv6 unsecured communication !!!");
    }
    struct sockaddr_in6 Sock_struct;
    memset(&Sock_struct, 0, sizeof(Sock_struct));
    Sock_struct.sin6_family = AF_INET6;
    Sock_struct.sin6_addr = in6addr_any;
    Sock_struct.sin6_port = htons(Popcl_args.Port);
    inet_pton(AF_INET6, Popcl_args.ServerName.c_str(), (void *)&Sock_struct.sin6_addr.s6_addr);

    if (connect(socketIpv6, (struct sockaddr *)&Sock_struct, sizeof(Sock_struct)) < 0) {
        PrintError("Failed to connect to IPv6 server !!!");
    }

    if (Popcl_args.TLS_Entered) {   //< encrypted version for IPv6
        bio = BIO_new(BIO_f_ssl());
    }
    else {
        bio = BIO_new(BIO_s_socket());  //< unecrypted version for IPV6
    }
    BIO_set_fd(bio, socketIpv6, BIO_CLOSE);

    Get_Server_Response();
    if (!Is_Possitive())    //< look for "+OK" response from server
    {
        PrintError("Connection with server was not set !!!");
    }
}

/**
* @brief Loads certificates for POP3s communication
*/
void POP3_Client::Load_Certificates() {

    ctx = SSL_CTX_new(SSLv23_client_method());
    if (ctx == NULL) {
        PrintError("Failed to create context for POP3s communication !!!");
    }

    if (Popcl_args.CertificateFile != "") {     //< check if user enterd file with certificates
        if (SSL_CTX_load_verify_locations(ctx, Popcl_args.CertificateFile.c_str(), NULL) == 0) {
            PrintError("Failed to load certificates from entered file !!!");
        }
    }
    else if (Popcl_args.CertificateAddr != "") {    //< check if user enterd dir with certificates
        if (SSL_CTX_load_verify_locations(ctx, NULL, Popcl_args.CertificateAddr.c_str()) == 0) {
            PrintError("Failed to load certificates from entered directory !!!");
        }
    }
    else if (!SSL_CTX_set_default_verify_paths(ctx)){
        PrintError("Failed to set default path for POP3s certificates !!!");
    }
}

/**
* @brief Checks if server send possitive answer
* @return True if "OK" was detected, otherwise False 
*/
bool POP3_Client::Is_Possitive() {
    
    if (Server_Response.find("+OK") != string::npos) {
        return true;
    }
    return false;
}

/**
* @brief Gets server responses 
*/
void POP3_Client::Get_Server_Response() {

    Server_Response.clear();    //< clear variable for each new calling of this func
    char *buffer = new char[1024];
    memset(buffer, 0, 1024);
    size_t End_Of_Response;
    int bytes;

    while(1) {
        if (Popcl_args.SSL_Entered && SSL_Active) {   //< STLS was send
            bytes = SSL_read(ssl, buffer, 1024);
        }
        else {                                      //< STLS was not send
            bytes = BIO_read(bio, buffer, 1024);
        }
        //< check number of returned bytes from server        
        if (bytes == 0) {
            PrintError("Connection lost while getting response from server !!!");
        }
        else if (bytes < 0) {
            if (!Popcl_args.SSL_Entered)    //< we try again if STLS was not set
            {
                if(!BIO_should_retry(bio)) {
                    PrintError("Connection with server lost while getting response !!!");
                }
            }
            else {
                PrintError("Connection lost when receiving data in STLS mode !!!");
            }
        }
        //< check for end of response from server
        Server_Response += buffer;
        if (!IsRETR) {
            End_Of_Response = Server_Response.find("\r\n");     //< detect end of response which is not for RETR
        }
        else {
            End_Of_Response = Server_Response.find("\r\n.\r\n");    //< detect end of RETR response
        }
        if (End_Of_Response != string::npos)    //< end of response detected
        {
            break;
        }
    }
    delete [] buffer;
}

/**
* @brief Sends commnad to server
* @param commnad This command is send to server 
*/
void POP3_Client::Send_Server_Command(string commnad) {

    int bytes;
    if (Popcl_args.SSL_Entered && SSL_Active) {               //< STLS was send and ssl is not null
        bytes = SSL_write(ssl, commnad.c_str(), (int)commnad.length());
    }
    else {
        bytes = BIO_write(bio, commnad.c_str(), (int)commnad.length());
    }
    
    if (bytes <= 0) {
        if (Popcl_args.SSL_Entered) {
        PrintError("Connection lost when sending command to server !!!");
        }

        else if (!BIO_should_retry(bio)) {
            PrintError("Failed to send command to server !!!");
        }
    }
}

/**
* @brief Provides login to server with user name and password 
*/
void POP3_Client::Login_Server() {

    ifstream AuthFile(Popcl_args.AuthFile);
    if (!AuthFile.is_open()) {
        PrintError("Failed to open authentification file !!!");
    }

    string user_line = "username = ";   //< expected start of first line in file
    string pass_line = "password = ";   //< expected start of second line in file
    string line;

    getline(AuthFile, line);
    if (line.find(user_line) == 0) {
        User = line.substr(user_line.length());     //< store user name to class attribute
    }
    else {
        PrintError("Incorrect structure of authentification file !!!");
    }

    getline(AuthFile, line);
    if (line.find(pass_line) == 0) {
        Password = line.substr(pass_line.length());     //< store password to class attribute
    }
    else {
        PrintError("Incorrect structure of authentification file !!!");
    }
    // check for eof
    getline(AuthFile, line);
    if (!AuthFile.eof()) {
        PrintError("Missing error on third line in auth file !!!");
    }

    // send USER commnad
    Send_Server_Command("USER " + User + "\r\n");
    Get_Server_Response();
    if (!Is_Possitive()) {
        PrintError("Negative answer from server when sending USER commnad !!!");
    }
    // send PASS command
    Send_Server_Command("PASS " + Password + "\r\n");
    Get_Server_Response();
    if (!Is_Possitive()) {
        PrintError("Negative answer when sending PASS commnad !!!");
    }

    AuthFile.close();
}

/**
* @brief Gets number of messeges on server
 */
void POP3_Client::Get_STAT() {

    Send_Server_Command("STAT\r\n");
    Get_Server_Response();
    if (!Is_Possitive()) {    //< STAT returned -ERR
        PrintError("STAT command returned -ERR !!!");
    }

    Message_Counter = stoi(Server_Response.substr(4, Server_Response.find_last_of(" ")));   //< +OK numMessages numofOctates
}

/**
 * @brief Provides RETR command
 * @param MessageNumber ID of message to be downloaded
 */
void POP3_Client::Retr_Commnad(int MessageNumber) {

    IsRETR = true;
    Send_Server_Command("RETR " + to_string(MessageNumber) + "\r\n");
    Get_Server_Response();
    if (!Is_Possitive()) {
        PrintError("Negative answer from server after RETR command !!!");
    }
    IsRETR = false;     //< no longer in RETR state
}

/**
* @brief Downloads all messages from server to output dir
*/
void POP3_Client::Download_Messages() {

    Get_STAT(); //< get num of all messages on server
    for (int i = 1; i <= Message_Counter; i++)
    {
        Retr_Commnad(i);
        ofstream OutFile(Popcl_args.OutDir + "/" + User +"_message_" + to_string(i) + ".txt");  //< create new file for every message
        if (!OutFile.is_open()) {
            PrintError("Failed to open file for writing messages !!!");
        }
        // remove first line of message (+OK ...)
        Server_Response = Server_Response.erase(0, Server_Response.find_first_of("\r\n") + 2);
        // remove last line of message (.\r\n)
        size_t position = Server_Response.find("\r\n.\r\n");
        Server_Response = Server_Response.erase(position, position + 5);

        OutFile << Server_Response;
        OutFile.close();
    }
    cout << "Number of messages which were downloaded: " + to_string(Message_Counter) << endl;
    // get UIDL of last downloaded message
    // to be able download only new messages in future
    Get_UIDL();
}

/**
* @brief Downloads all new messages from server
*/
void POP3_Client::Download_New_Messages() {

    int New_messg_counter = 0;
    string Current_UIDL;
    string DB_Content;       //< UIDL/ID of last message stored in database file

    ifstream ID_Database_File("ID_Database_File.txt");
    if (ID_Database_File.is_open()) {               //< file exists
        getline(ID_Database_File, DB_Content);
        ID_Database_File.close();
    }
    // check if server supports UIDL
    Send_Server_Command("UIDL\r\n");
    Get_Server_Response();
    if (Is_Possitive()) {
        UIDL_Supported = true;
    }

    // stoi to empty string would cause exception...see below
    if (!UIDL_Supported && strcmp(DB_Content.c_str(), "") == 0) {
        DB_Content = "-1";
    }

    Get_STAT(); //< get num of all messages on server

    for (int i = Message_Counter; i > 0; i--) {
        if (UIDL_Supported)
        {
            Send_Server_Command("UIDL " + to_string(i) + "\r\n");
            Get_Server_Response();
            if (!Is_Possitive()) {
                PrintError("Unexpected error occured when receiving response on UIDL commnad");
            }
            Current_UIDL = Parse_UIDL();
            if (strcmp(Current_UIDL.c_str(), DB_Content.c_str()) == 0) {
                break;  //< message is not new
            }
            
        }
        // UIDL not supported, must work with MessageID
        else {
            if (stoi(DB_Content) == i) {   //< message is not new
                break;
            }
        }
        // message is new
        Retr_Commnad(i);
        ofstream OutFile(Popcl_args.OutDir + "/" + User +"_new_message_" + to_string(New_messg_counter+1) + ".txt");  //< create new file for every message
        if (!OutFile.is_open()) {
            PrintError("Failed to open file for writing messages !!!");
        }
        // remove first line of message (+OK ...)
        Server_Response = Server_Response.erase(0, Server_Response.find_first_of("\r\n") + 2);
        // remove last line of message (.\r\n)
        size_t position = Server_Response.find("\r\n.\r\n");
        Server_Response = Server_Response.erase(position, position + 5);
        OutFile << Server_Response;

        New_messg_counter++;
    }
    cout << "Number of new messages which were downloaded: " + to_string(New_messg_counter) << endl;
    // get UIDL of last downloaded message
    // to be able download only new messages in future
    Get_UIDL();
}

/**
 * @brief Checks if server supports UIDL and
 * stores ID of last message to DB to distinguish new messages
 */
void POP3_Client::Get_UIDL() {

    ofstream ID_Database_File("ID_Database_File.txt");  //< create new file for storing ID of last message (database)
    if (!ID_Database_File.is_open()) {
        PrintError("Failed to open database file for storing ID of last message !!!");
    }

    // store UIDL of last message to seperate new and old messages in future
    Send_Server_Command("UIDL " + to_string(Message_Counter) + "\r\n");
    Get_Server_Response();
    if (!Is_Possitive()) {      //< server probably not support UIDL command
        ID_Database_File << to_string(Message_Counter);
    }
    else {
        UIDL_Supported = true;
        ID_Database_File << Parse_UIDL();   //< server enables UIDL, get last message UIDL
    }

    ID_Database_File.close();
}

/**
* @brief Parses UID from server response on command UIDL
* @return UIDL of current message in string format
*/
string POP3_Client::Parse_UIDL() {

    string UIDL;
    size_t position = Server_Response.find_last_of(" ");    //< +OK messageID messageUIDL 
    if (position != string::npos) {
        UIDL = Server_Response.substr(position + 1);
        UIDL = UIDL.substr(0, UIDL.length()-2);         //< remove /r/n
    }
    return UIDL;
}

/**
* @brief Deletes all messages from server
*/
void POP3_Client::Del_Command() {

    Get_STAT();
    for (int i = 1; i <= Message_Counter; i++)
    {
        Send_Server_Command("DELE " + to_string(i) + "\r\n");
        Get_Server_Response();
        if (!Is_Possitive()) {
            PrintError("Negative answer from server when sending DEL command !!!");
        }
    }
}

/**
 * @brief Quites connection with server
 */
void POP3_Client::Quit_Command() {

    Send_Server_Command("QUIT\r\n");
    Get_Server_Response();
    if (!Is_Possitive()) {
        PrintError("Failed to close connection with server !!!");
    }
}

int main(int argc, char *argv[]) {

    ParseArgs(argc, argv);
    POP3_Client pop3_client = POP3_Client();

    //set port number according to user's arguments
    if (Popcl_args.Port < 0)    //< user did not entered port number
    {
        if (Popcl_args.TLS_Entered) {   //< if -T paramether was set
            Popcl_args.Port = 995;
        }
        else {
            Popcl_args.Port = 110;
        }
    }
    //find IP family of server
    if (Popcl_args.ServerName.find(":") != string::npos) {  //< IPv6 contains :
        pop3_client.Server_Is_Ipv6 = true;
    }

    if (Popcl_args.TLS_Entered && Popcl_args.SSL_Entered) {
        PrintError("You have to require either TLS or SSL communication, not both (either -T or -S paramether) !!!");
    }
    else if (Popcl_args.TLS_Entered)    //< -T was entered
    {
        pop3_client.Connect_Server_Sec();
    }
    else if (Popcl_args.SSL_Entered)    //< -S was entered
    {
        pop3_client.Connect_Server_StartTLS();
    }
    else    // unecrypted communication 
    {
        if (pop3_client.Server_Is_Ipv6) {           //< server is IPV6
            pop3_client.Connect_Server_IPv6();
        }
        else {
            pop3_client.Connect_Server();   //< server is IPV4
        }
    }

    pop3_client.Login_Server();

    if (Popcl_args.Delete_Entered)  //< user entered -d 
    {
        pop3_client.Del_Command();
    }
    else if (Popcl_args.New_Entered)    //< user entered -n
    {
        pop3_client.Download_New_Messages();
    }
    else {
        pop3_client.Download_Messages();    //< basic download of messages
    }

    pop3_client.Quit_Command(); //< close connection with server
    return 0;
}