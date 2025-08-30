#include <stdlib.h>
#include <stdio.h>

#include "wsserver.h"
#include "PdWebsocketServer.h"

using namespace std;


#ifdef __cplusplus
extern "C"{
#endif

static t_class *ws_server_pd_class;

// ws callbacks (scheduler thread)
void bindCb(t_pd *obj, void *data)
{
    if (obj != NULL)
    {
        t_ws_server_pd *x = (t_ws_server_pd *)obj;
        outlet_float(x->bound_out, 1);
    }
}

void unbindCb(t_pd *obj, void *data)
{
    if (obj != NULL)
    {
        t_ws_server_pd *x = (t_ws_server_pd *)obj;
        outlet_float(x->bound_out, 0);
    }
}

void clientConnectedCb(t_pd *obj, void *data)
{
    if (obj != NULL)
    {
        t_ws_server_pd *x = (t_ws_server_pd *)obj;
        x->clients++;
        outlet_float(x->client_count_out, x->clients);
    }
}

void clientDisconnectedCb(t_pd *obj, void *data)
{
    if (obj != NULL)
    {
        t_ws_server_pd *x = (t_ws_server_pd *)obj;
        x->clients--;

        if (x->clients < 0)
        {
            pd_error(x, "invalid client count < 0");
            x->clients = 0;
        }

        outlet_float(x->client_count_out, x->clients);
    }
}

static void outData(t_outlet* outlet, const string& str)
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
    string* str = (string*)data;

    if (obj != NULL)
    {
        t_ws_server_pd *x = (t_ws_server_pd *)obj;
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
    string* str = (string*)data;

    if (obj != NULL)
    {
        t_ws_server_pd *x = (t_ws_server_pd *)obj;
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

void wsserver_parse_bind(t_ws_server_pd *x, int argc, t_atom *argv)
{
    uint16_t port = 0;
    t_symbol* address = nullptr;

    if (argc &&
        argv->a_type == A_FLOAT)
    {
        port = atom_getintarg(0, argc, argv);
        argc--;
        argv++;
    }


    if (argc &&
        argv->a_type == A_SYMBOL)
    {
        address = atom_getsymbolarg(0, argc, argv);
    }

    x->server->listen(port, address != nullptr ? address->s_name : "");
}

void wsserver_listen(t_ws_server_pd *x, t_symbol *s, int argc, t_atom *argv)
{
    x->server->close();

    wsserver_parse_bind(x, argc, argv);
}


void wsserver_list(t_ws_server_pd *x, t_symbol *s, int argc, t_atom *argv)
{
    if (!x->server ||
        !x->server->isListening())
    {
        return;
    }

    if (x->server->binary())
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

        x->server->sendToAll(data);
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

        x->server->sendToAll(msg);
    }

}


void *wsserver_new(t_symbol *s)
{
    t_ws_server_pd *x = (t_ws_server_pd *)pd_new(ws_server_pd_class);

    x->clients = 0;

    x->binary_data_out = outlet_new(&x->x_obj, &s_list);
    x->string_data_out = outlet_new(&x->x_obj, &s_list);
    x->client_count_out = outlet_new(&x->x_obj, &s_float);
    x->bound_out = outlet_new(&x->x_obj, &s_float);

    // create server
    x->server = new PdWebsocketServer((t_pd*)x);

    if (s && s != gensym(""))
    {
        if (s == gensym("-b"))
        {
            x->server->binary(true);
        }
        else if (s == gensym("-t"))
        {
            x->server->binary(false);
        }
        else
        {
            pd_error(x, "invalid argument: %s", s->s_name);
        }
    }


    return (void *)x;
}

void wsserver_free(t_ws_server_pd *x)
{
    if (x->server)
    {
        delete x->server;
        x->server = nullptr;
    }

    outlet_free(x->binary_data_out);
    outlet_free(x->string_data_out);
    outlet_free(x->client_count_out);
    outlet_free(x->bound_out);
}

void wsserver_setup(void) {
    ws_server_pd_class = class_new(gensym("wsserver"),
                                   (t_newmethod)wsserver_new,
                                   (t_method)wsserver_free,
                                   sizeof(t_ws_server_pd),
                                   CLASS_DEFAULT,
                                   A_DEFSYM,
                                   0);

    class_addlist(ws_server_pd_class, (t_method)wsserver_list);

    class_addmethod(ws_server_pd_class, (t_method)wsserver_listen, gensym("listen"), A_GIMME, A_NULL);
}

#ifdef __cplusplus
} // extern "C"
#endif
