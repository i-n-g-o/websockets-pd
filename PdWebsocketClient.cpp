#include "PdWebsocketClient.h"

#include "wsclient.h"

PdWebsocketClient::PdWebsocketClient(t_pd* x)
    : WebsocketClient()
    , m_x(x)
{
}

PdWebsocketClient::~PdWebsocketClient()
{
    pd_queue_cancel((t_pd*)m_x);
}


// threaded functions

void PdWebsocketClient::connected()
{
    pd_queue_mess(&pd_maininstance, m_x, NULL, openedCb);

    WebsocketClient::connected();
}

void PdWebsocketClient::error(int code, const std::string& message)
{
    auto str = new std::string(message);
    pd_queue_mess(&pd_maininstance, m_x, str, errorCb);

    WebsocketClient::error(code, message);
}

void PdWebsocketClient::disconnected(uint16_t code)
{    
    pd_queue_mess(&pd_maininstance, m_x, NULL, closedCb);

    WebsocketClient::disconnected(code);
}

void PdWebsocketClient::received(const char* data, size_t size)
{
    auto str = new std::string(data, size);
    pd_queue_mess(&pd_maininstance, m_x, str, binaryDataCb);

    WebsocketClient::received(data, size);
}

void PdWebsocketClient::received(const std::string& msg)
{
    auto str = new std::string(msg);
    pd_queue_mess(&pd_maininstance, m_x, str, textDataCb);

    WebsocketClient::received(msg);
}
