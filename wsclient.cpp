#include "wsclient.h"
#include "PdWebsocketClient.h"


#ifdef __cplusplus
extern "C"{
#endif

static t_class *ws_client_pd_class;


// ws callbacks (scheduler thread)
void openedCb(t_pd *obj, void *data)
{
    if (obj != NULL)
    {
        t_ws_client_pd *x = (t_ws_client_pd *)obj;

        if (!x->connected)
        {
            outlet_float(x->connected_out, 1);
            x->connected = true;
        }
    }
    else
    {
        // open got cancled
    }
}

void closedCb(t_pd *obj, void *data)
{
    if (obj != NULL)
    {
        t_ws_client_pd *x = (t_ws_client_pd *)obj;

        if (x->connected)
        {
            outlet_float(x->connected_out, 0);
            x->connected = false;
        }
    }
    else
    {
        // close got cancled
    }
}

void errorCb(t_pd *obj, void *data)
{
    std::string* str = (std::string*)data;

    if (obj != NULL)
    {
        t_ws_client_pd *x = (t_ws_client_pd *)obj;
        if (str)
        {
            pd_error(x, "%s", str->c_str());
        }
    }
    else
    {
        // error got cancled
    }

    if (str)
    {
        delete str;
    }
}

void outData(t_outlet* outlet, const std::string& str)
{
    std::vector<t_atom> list(str.length());
    for (size_t i=0; i<str.length(); i++)
    {
        list[i].a_type = A_FLOAT;
        list[i].a_w.w_float = (int)str.at(i);
    }

    outlet_list(outlet, gensym("list"), list.size(), list.data());
}

void binaryDataCb(t_pd *obj, void *data)
{
    std::string* str = (std::string*)data;

    if (obj != NULL)
    {
        t_ws_client_pd *x = (t_ws_client_pd *)obj;
        if (str)
        {
            outData(x->binary_data_out, *str);
        }
    }
    else
    {
        // data got cancled
    }

    if (str)
    {
        delete str;
    }
}

void textDataCb(t_pd *obj, void *data)
{
    std::string* str = (std::string*)data;

    if (obj != NULL)
    {
        t_ws_client_pd *x = (t_ws_client_pd *)obj;
        if (str)
        {
            outData(x->string_data_out, *str);
        }
    }
    else
    {
        // data got cancled
    }

    if (str)
    {
        delete str;
    }
}


void wsclient_connect(t_ws_client_pd *x, t_symbol* host)
{
    x->client->connect(host->s_name);
}

void wsclient_disconnect(t_ws_client_pd *x)
{
    x->client->disconnect();
}


void wsclient_list(t_ws_client_pd *x, t_symbol *s, int argc, t_atom *argv)
{
    if (!x->client ||
        !x->client->isConnected())
    {
        return;
    }

    if (x->client->binary())
    {
        // send binary data
        std::vector<char> data(argc);

        for (int i = 0; i<argc; i++)
        {
            if (argv[i].a_type == A_FLOAT)
            {
                t_int itemp = atom_getintarg(i, argc, argv);

                if (itemp > 255)
                {
                    pd_error(x, "invalid data in packet");
                    return;
                }

                data[i] = (char)itemp;
            }
            else
            {
                pd_error(x, "malformed data");
                return;
            }
        }

        x->client->send(data);
    }
    else
    {
        // send text data
        std::string msg;

        for (int i = 0; i<argc; i++)
        {
            if (argv[i].a_type == A_SYMBOL)
            {
                msg.append(argv[i].a_w.w_symbol->s_name);
            }
            else if (argv[i].a_type == A_FLOAT)
            {
                t_float val = argv[i].a_w.w_float;
                if (val == int(val))
                {
                    msg.append(std::to_string(int(val)));
                }
                else
                {
                    msg.append(std::to_string(val));
                }
            }

            if (i < argc-1)
            {
                msg.append(" ");
            }
        }

        x->client->send(msg);
    }

}

void *wsclient_new(t_symbol* s, int argc, t_atom *argv)
{
    t_ws_client_pd *x = (t_ws_client_pd *)pd_new(ws_client_pd_class);

    x->connected = false;

    x->binary_data_out = outlet_new(&x->x_obj, &s_list);
    x->string_data_out = outlet_new(&x->x_obj, &s_list);
    x->connected_out = outlet_new(&x->x_obj, &s_float);

    x->client = new PdWebsocketClient((t_pd*)x);


    for (int i = 0; i < argc; ++i)
    {
        if (argv[i].a_type == A_SYMBOL)
        {
            if (argv[i].a_w.w_symbol == gensym("-b"))
            {
                x->client->binary(true);
            }
            else if (argv[i].a_w.w_symbol == gensym("-t"))
            {
                x->client->binary(false);
            }
            else if (argv[i].a_w.w_symbol == gensym("-noverify"))
            {
                x->client->verifyPeer(false);
            }
            else
            {
                pd_error(x, "invalid argument: %s", s->s_name);
            }
        }
        else
        {
            pd_error(x, "invalid argument");
        }
    }

    return (void *)x;
}

void wsclient_free(t_ws_client_pd *x)
{
    if (x->client)
    {
        delete x->client;
        x->client = nullptr;
    }
}

void wsclient_setup(void) {
    ws_client_pd_class = class_new(gensym("wsclient"),
                                   (t_newmethod)wsclient_new,
                                   (t_method)wsclient_free,
                                   sizeof(t_ws_client_pd),
                                   CLASS_DEFAULT,
                                   A_GIMME,
                                   0);

    class_addlist(ws_client_pd_class, (t_method)wsclient_list);

    class_addmethod(ws_client_pd_class, (t_method)wsclient_connect, gensym("connect"), A_SYMBOL, A_NULL);
    class_addmethod(ws_client_pd_class, (t_method)wsclient_disconnect, gensym("disconnect"), A_NULL);
}

#ifdef __cplusplus
} // extern "C"
#endif
