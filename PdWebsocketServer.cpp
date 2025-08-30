#include "PdWebsocketServer.h"

PdWebsocketServer::PdWebsocketServer(t_pd* x)
    : WebsocketServer()
    , m_x(x)
{}


// threaded
void PdWebsocketServer::listening()
{    
    pd_queue_mess(&pd_maininstance, m_x, NULL, bindCb);    
}

void PdWebsocketServer::closed()
{
    pd_queue_mess(&pd_maininstance, m_x, NULL, unbindCb);
}

void PdWebsocketServer::clientConnected(void* client)
{
    pd_queue_mess(&pd_maininstance, m_x, NULL, clientConnectedCb);

    WebsocketServer::clientConnected(client);
}

void PdWebsocketServer::clientDisconnected(void* client)
{
    pd_queue_mess(&pd_maininstance, m_x, NULL, clientDisconnectedCb);

    WebsocketServer::clientDisconnected(client);
}

void PdWebsocketServer::received(const char* data, size_t size, void* client)
{
    auto str = new std::string(data, size);
    pd_queue_mess(&pd_maininstance, m_x, str, binaryDataCb);
}

void PdWebsocketServer::received(const std::string& msg, void* client)
{
    auto str = new std::string(msg);
    pd_queue_mess(&pd_maininstance, m_x, str, textDataCb);
}
