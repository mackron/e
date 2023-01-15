#include <vector>   /* This needs to be include first because Lua (from inside e.c) defines "next" macros which conflict and cause compiler errors. */

#include "e.hpp"
#include "../../e.c"


#include <stdio.h>

namespace e
{
    static e_result e_engine_vtable__on_step(void* pUserData, e_engine* pEngine, double dt)
    {
        (void)pEngine;
        return ((e::engine*)pUserData)->on_step(dt);
    }

    e_engine_vtable e_gEngineVTable =
    {
        e_engine_vtable__on_step
    };



    static e_result e_client_vtable__on_step(void* pUserData, e_client* pClient, double dt)
    {
        (void)pClient;
        return ((e::client*)pUserData)->on_step(dt);
    }

    e_client_vtable e_gClientVTable =
    {
        e_client_vtable__on_step
    };
}




/////////////////////////////////////////////////////////////////
class my_client;


class my_engine : public e::engine
{
public:

    my_engine(int argc, char** argv)
        : e::engine(e::engine_config(argc, argv, 0))
    {
    }

    void register_client(my_client &client)
    {
        m_clients.push_back(&client);
    }

    void unregister_client(my_client &client)
    {
        m_clients.erase(std::remove(m_clients.begin(), m_clients.end(), &client), m_clients.end());
    }

    e::result on_loop_iteration(double dt);

private:

    std::vector<my_client*> m_clients;
};


class my_client : public e::client
{
public:

    my_client(my_engine &engine, const e::client_config &config)
        : e::client(config), m_engine(engine)
    {
        engine.register_client(*this);
    }

    my_client(my_engine &engine)
        : my_client(engine, e::client_config(engine, "game"))
    {
    }

    ~my_client()
    {
        m_engine.unregister_client(*this);
    }

    e::result on_step(double dt)
    {
        printf("Client step: dt = %f\n", dt);
        return E_SUCCESS;
    }

private:

    my_engine &m_engine;
};


e::result my_engine::on_loop_iteration(double dt)
{
    // Every client needs to be stepped.
    for (auto client : m_clients) {
        client->step(dt);
    }

    return E_SUCCESS;
}


int main(int argc, char** argv)
{
    my_engine engine(argc, argv);
    my_client client(engine);

    engine.run();

    return 0;
}
