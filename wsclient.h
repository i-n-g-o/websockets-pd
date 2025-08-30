#ifndef WSCLIENT_H
#define WSCLIENT_H

#include <m_pd.h>

#ifdef __cplusplus
extern "C"{
#endif

class PdWebsocketClient;

typedef struct _ws_client_pd
{
    t_object x_obj;

    PdWebsocketClient* client;

    t_outlet* binary_data_out;
    t_outlet* string_data_out;
    t_outlet* connected_out;

} t_ws_client_pd;


// websocket client callbacks
void openedCb(t_pd *obj, void *data);
void closedCb(t_pd *obj, void *data);
void errorCb(t_pd *obj, void *data);
void binaryDataCb(t_pd *obj, void *data);
void textDataCb(t_pd *obj, void *data);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // WSCLIENT_H
