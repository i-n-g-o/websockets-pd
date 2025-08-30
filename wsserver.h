#ifndef WSSERVER_H
#define WSSERVER_H

#include <m_pd.h>

class PdWebsocketServer;

#ifdef __cplusplus
extern "C"{
#endif


typedef struct _ws_server_pd
{
    t_object x_obj;

    int clients;
    PdWebsocketServer* server;

    t_outlet* binary_data_out;
    t_outlet* string_data_out;
    t_outlet* client_count_out;
    t_outlet* bound_out;

} t_ws_server_pd;

void bindCb(t_pd *obj, void *data);
void unbindCb(t_pd *obj, void *data);
void clientConnectedCb(t_pd *obj, void *data);
void clientDisconnectedCb(t_pd *obj, void *data);
void binaryDataCb(t_pd *obj, void *data);
void textDataCb(t_pd *obj, void *data);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // WSSERVER_H
