#ifndef PDWEBSOCKETCLIENT_H
#define PDWEBSOCKETCLIENT_H

#include <m_pd.h>

#include <WebsocketClient.h>

using namespace scaryws;

class PdWebsocketClient : public WebsocketClient
{
public:
    PdWebsocketClient(t_pd* x);
    ~PdWebsocketClient();

public:
    // ISessionListener
    void connected() override;
    void error(int code, const std::string& message) override;
    void disconnected(uint16_t code) override;
    void received(const char* data, size_t size) override;
    void received(const std::string& msg) override;

private:
    t_pd* m_x{nullptr};
};

#endif // PDWEBSOCKETCLIENT_H
