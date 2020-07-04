
#include "../include/tcp_client.h"
#include <stdio.h>
#include <string>
#include <algorithm>
#include <sstream>


pipe_ret_t TcpClient::connectTo(const std::string & address, int port) {
    m_sockfd = 0;
    pipe_ret_t ret;

    m_sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if (m_sockfd == -1) { //socket failed
        ret.success = false;
        ret.msg = strerror(errno);
        return ret;
    }

    int inetSuccess = inet_aton(address.c_str(), &m_server.sin_addr);

    if(!inetSuccess) { // inet_addr failed to parse address
        // if hostname is not in IP strings and dots format, try resolve it
        struct hostent *host;
        struct in_addr **addrList;
        if ( (host = gethostbyname( address.c_str() ) ) == NULL){
            ret.success = false;
            ret.msg = "Failed to resolve hostname";
            return ret;
        }
        addrList = (struct in_addr **) host->h_addr_list;
        m_server.sin_addr = *addrList[0];
    }
    m_server.sin_family = AF_INET;
    m_server.sin_port = htons( port );

    int connectRet = connect(m_sockfd , (struct sockaddr *)&m_server , sizeof(m_server));
    if (connectRet == -1) {
        ret.success = false;
        ret.msg = strerror(errno);
        return ret;
    }
    m_receiveTask = new std::thread(&TcpClient::ReceiveTask, this);
    ret.success = true;
    return ret;
}


pipe_ret_t TcpClient::sendMsg(const char * msg, size_t size) {
    pipe_ret_t ret;
    int numBytesSent = send(m_sockfd, msg, size, 0);
    if (numBytesSent < 0 ) { // send failed
        ret.success = false;
        ret.msg = strerror(errno);
        return ret;
    }
    if ((uint)numBytesSent < size) { // not all bytes were sent
        ret.success = false;
        char msg[100];
        sprintf(msg, "Only %d bytes out of %lu was sent to client", numBytesSent, size);
        ret.msg = msg;
        return ret;
    }
    ret.success = true;
    return ret;
}

void TcpClient::subscribe(const client_observer_t & observer) {
    m_subscibers.push_back(observer);
}

void TcpClient::unsubscribeAll() {
    m_subscibers.clear();
}

/*
 * Publish incoming client message to observer.
 * Observers get only messages that originated
 * from clients with IP address identical to
 * the specific observer requested IP
 */
void TcpClient::publishServerMsg(std::string msg, size_t msgSize) {
    for (uint i=0; i<m_subscibers.size(); i++) {
        if (m_subscibers[i].incoming_packet_func != NULL) {
            (*m_subscibers[i].incoming_packet_func)(msg, msgSize);
        }
    }
}

/*
 * Publish client disconnection to observer.
 * Observers get only notify about clients
 * with IP address identical to the specific
 * observer requested IP
 */
void TcpClient::publishServerDisconnected(const pipe_ret_t & ret) {
    for (uint i=0; i<m_subscibers.size(); i++) {
        if (m_subscibers[i].disconnected_func != NULL) {
            (*m_subscibers[i].disconnected_func)(ret);
        }
    }
}

void TcpClient::handleMessageComplete(std::string msg) {
    publishServerMsg(msg, msg.length());
}

/*
 * Receive server packets, and notify user
 */
void TcpClient::ReceiveTask() {
    while(!stop) {
        ReceiveData();
    }
}

std::string TcpClient::ReceiveDataBack()
{
    char buffer[40960];

    memset(buffer, 0, 40960);

    int bytes = recv(m_sockfd, buffer, 40960 - 1, 0);

    if (bytes > 0)
        return std::string(buffer);
    else {
        pipe_ret_t ret;
        ret.success = false;
        stop = true;
        ret.msg = "Server closed connection";
        publishServerDisconnected(ret);
        finish();
    }
    return "";
}

void TcpClient::ReceiveData() {
    std::string buffer = ReceiveDataBack();
    std::string line;
    std::istringstream iss(buffer);
    while(getline(iss, line))
    {
        if (line.find("\n") != std::string::npos)
            line = line.substr(0, line.size() - 1);
        handleMessageComplete(line);
    }
}

pipe_ret_t TcpClient::finish(){
    stop = true;
    terminateReceiveThread();
    pipe_ret_t ret;
    if (close(m_sockfd) == -1) { // close failed
        ret.success = false;
        ret.msg = strerror(errno);
        return ret;
    }
    ret.success = true;
    return ret;
}

void TcpClient::terminateReceiveThread() {
    if (m_receiveTask != nullptr) {
        m_receiveTask->detach();
        delete m_receiveTask;
        m_receiveTask = nullptr;
    }
}

TcpClient::~TcpClient() {
    terminateReceiveThread();
}
