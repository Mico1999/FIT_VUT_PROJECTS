/**
 * @file popcl.hpp
 * @brief Headers file for pop3 client application
 * @author Marek Micek, xmicek08
 * @date 30.9.2021
 */ 


#include <iostream>
#include <fstream>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

/**
 * @brief Stores arguments from command line
 */
struct popcl_args
{
    string ServerName;
    string CertificateFile;
    string CertificateAddr;
    string AuthFile;
    string OutDir;
    int Port = -1;
    bool TLS_Entered = false;
    bool SSL_Entered = false;
    bool Delete_Entered = false;
    bool New_Entered = false;

} Popcl_args;

/**
 * @brief Prints hint how to use program, in case of incorrect using by user
 */
void PrintHelp();

/**
 * @brief Prints error message on stdrr in case of error was detected
 * @param ErrMsg Error message written on stderr
 */
void PrintError(string ErrMsg);

/**
 * @brief Parses arguments from command line
 */
void ParseArgs(int argc, char *argv[]);

class POP3_Client {

    public:
        POP3_Client();
        ~POP3_Client();

        // required variables for POP3s communication
        SSL_CTX *ctx;
        SSL *ssl;
        BIO *bio;

        string Server_Response;
        string User;
        string Password;
        int Message_Counter;            //< stores number of messages on server
        bool IsRETR = false;            //< flag to find out if client sends RETR command
        bool Server_Is_Ipv6 = false;    //< flag to find out if server is IPv6 or not
        bool UIDL_Supported = false;    //< indicates server support UIDL command
        bool SSL_Active = false;
        int socketIpv6;

        /**
        * @brief Connect to server in unencrypted mode (for Ipv4 address)
        */
        void Connect_Server();

        /**
        * @brief Connect to server in unencrypted mode (for Ipv6 address)
        */
        void Connect_Server_IPv6();

        /**
        * @brief Connect to server in encrypted POP3s mode (paramether -T was set)
        */
        void Connect_Server_Sec();

        /**
        * @brief Connect to server in unencrypted mode and then sends command StartTLS to switch into encrypted mode (paramether -S was set)
        */
        void Connect_Server_StartTLS();

        /**
         * @brief Loads certificates for POP3s communication
         */
        void Load_Certificates();

        /**
        * @brief Gets server responses 
        */
        void Get_Server_Response();

        /**
        * @brief Checks if server send possitive answer
        * @return True if "OK" was detected, otherwise False 
        */
        bool Is_Possitive();

        /**
        * @brief Sends commnad to server
        * @param commnad This command is send to server 
        */
        void Send_Server_Command(string commnad);

        /**
        * @brief Provides login to server with user name and password 
        */
        void Login_Server();

        /**
         * @brief Gets number of messeges on server
         */
        void Get_STAT();

        /**
         * @brief Downloads all messages from server
         */
        void Download_Messages();

        /**
         * @brief Downloads all new messages from server
         */
        void Download_New_Messages();

        /**
         * @brief Parses UID from server response on command UIDL
         * @return UIDL of current message in string format
         */
        string Parse_UIDL();

        /**
        * @brief Checks if server supports UIDL and
        * stores ID of last message to DB to distinguish new messages
        */
        void Get_UIDL(); 

        /**
        * @brief Downloads all messages from server to output dir
        */
        void Retr_Commnad(int MessageNumber);

        /**
        * @brief Deletes all messages from server
        */
        void Del_Command();

        /**
        * @brief Quites connection with server
        */
        void Quit_Command();
};