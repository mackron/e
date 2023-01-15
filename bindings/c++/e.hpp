/*
This is just some experimenting with some ideas for a C++ wrapper.
*/

#include "../../e.h"

namespace e
{
    typedef e_result result;    /* Will need to auto-generate an enum from the e_result enum */


    class exception : public std::exception
    {
    public:

        exception(e::result result)
            : std::exception("Error."), result(result)  // TODO: Do a proper error message based on the result code.
        {
        }

        e::result result;
    };

    void throw_exception(e::result result)
    {
    #if defined(__EXCEPTIONS)   // <-- This needs to be made cross-compiler. Currently only tested on MSVC.
        throw new e::exception(result);
    #else
        (void)result;
        abort();
    #endif
    }

    void throw_exception_if_error(e::result result)
    {
        if (result != E_SUCCESS) {
            throw_exception(result);
        }
    }


    void engine_config_init_inplace(int argc, char** argv, unsigned int flags, e_engine_vtable* pVTable, e_engine_config* pConfig)
    {
        *pConfig = e_engine_config_init(argc, argv, flags, pVTable);
    }

    class engine_config : public e_engine_config
    {
    public:

        engine_config(int argc, char** argv, unsigned int flags)
        {
            engine_config_init_inplace(argc, argv, flags, NULL, this);
        }
    };

    class engine_callbacks
    {
    public:

        virtual result on_step(double dt)
        {
            (void)dt;
            return E_SUCCESS;
        }
    };

    extern e_engine_vtable e_gEngineVTable;

    class engine : public engine_callbacks
    {
    public:

        engine(const engine_config &config)
            : m_pEngine(nullptr)
        {
            engine_config configCopy(config);
            configCopy.pVTable = &e_gEngineVTable;
            configCopy.pVTableUserData = this;

            throw_exception_if_error(e_engine_init(&configCopy, NULL, &m_pEngine));
        }

        ~engine()
        {
            e_engine_uninit(m_pEngine, NULL);
        }

        e::result run()
        {
            return (e::result)e_engine_run(m_pEngine);
        }

        operator       e_engine*()       { return m_pEngine; }
        operator const e_engine*() const { return m_pEngine; }


    private:

        e_engine* m_pEngine;
    };






    void client_config_init_inplace(engine &engine, const char* pConfigFileSection, e_client_config* pConfig)
    {
        *pConfig = e_client_config_init(engine, pConfigFileSection);
    }

    class client_config : public e_client_config
    {
    public:

        client_config(engine &engine, const char* pConfigFileSection)
        {
            client_config_init_inplace(engine, pConfigFileSection, this);
        }
    };

    class client_callbacks
    {
    public:

        virtual result on_step(double dt)
        {
            (void)dt;
            return E_SUCCESS;
        }
    };

    extern e_client_vtable e_gClientVTable;

    class client : public client_callbacks
    {
    public:

        client(const client_config &config)
            : m_pClient(nullptr)
        {
            client_config configCopy(config);
            configCopy.pVTable = &e_gClientVTable;
            configCopy.pVTableUserData = this;

            throw_exception_if_error(e_client_init(&configCopy, NULL, &m_pClient));
        }

        ~client()
        {
            e_client_uninit(m_pClient, NULL);
        }

        e::result step(double dt)
        {
            return (e::result)e_client_step(m_pClient, dt);
        }

        operator       e_client*()       { return m_pClient; }
        operator const e_client*() const { return m_pClient; }


    private:

        e_client* m_pClient;
    };
}