/*
This demonstrates the low level API. It's a bit verbose. A high level API to encapsulate all of this is planned.

If you're compiling with MSVC, you'll need to compile with _CRT_SECURE_NO_WARNINGS. It is not enough to
define this in-code - it must be via the command line.

To close the program you'll need to force kill it. Window event handling is not yet exposed via the
client (it's planned).
*/
#include "../e.c"

static e_engine* g_pEngine;
static e_client* g_pClient;

static e_result on_engine_step(void* pUserData, e_engine* pEngine, double dt)
{
    E_ASSERT(pEngine != NULL);
    E_UNUSED(pUserData);
    E_UNUSED(pEngine);

    /* We just step our clients from the engine step callback. */
    e_client_step(g_pClient, dt);

    return E_SUCCESS;
}

static e_engine_vtable g_engineVTable =
{
    on_engine_step
};


int main(int argc, char** argv)
{
    e_result result;
    e_engine_config engineConfig;
    e_client_config clientConfig;


    /* Need an engine first. */
    engineConfig = e_engine_config_init(argc, argv, 0, &g_engineVTable, NULL);
    
    result = e_engine_init(&engineConfig, NULL, &g_pEngine);
    if (result != E_SUCCESS) {
        printf("Failed to initialize engine.");
        return -1;
    }


    /* Once we have the engine we can initialize a client. This is where the window is created. */
    clientConfig = e_client_config_init(g_pEngine, "game");

    result = e_client_init(&clientConfig, NULL, &g_pClient);
    if (result != E_SUCCESS) {
        return -1;
    }


    /* There's only a single main loop which fires events from all windows. We therefore need to run the loop from the engine. */
    e_engine_run(g_pEngine);


    /* Teardown. */
    e_client_uninit(g_pClient, NULL);
    e_engine_uninit(g_pEngine, NULL);

    return 0;
}
