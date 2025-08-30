#ifndef PDWEBSOCKETSERVER_H
#define PDWEBSOCKETSERVER_H

#include <m_pd.h>
#include <WebsocketServer.h>

#include "wsserver.h"

using namespace scaryws;

class PdWebsocketServer : public WebsocketServer
{
public:
    PdWebsocketServer(t_pd* x);

public:
    // IServerSessionListener
    virtual void listening() override;
    virtual void closed() override;
    virtual void clientConnected(void* client) override;
    virtual void clientDisconnected(void* client) override;
    virtual void received(const char* data, size_t size, void* client) override;
    virtual void received(const std::string& msg, void* client) override;

private:
    t_pd* m_x{nullptr};
};

#endif // PDWEBSOCKETSERVER_H
