#include "../e.c"

#include <stdio.h>

e_client* pClient1;

e_window* pVulkanWindow;
e_graphics* pVulkanGraphics;
e_graphics_surface* pVulkanSurface;

static e_result test_game_engine_iteration(void* pUserData, e_engine* pEngine, double dt)
{
    E_ASSERT(pEngine != NULL);
    E_UNUSED(pUserData);

    (void)dt;

    e_client_step(pClient1, dt);


    /* Do some drawing on the graphics engine. */
    /*e_graphics_present_surface(pVulkanGraphics, pVulkanSurface);*/

    return E_SUCCESS;
}

static e_engine_vtable gTestEngineVTable =
{
    test_game_engine_iteration
};



static e_result test_game_window_event(void* pUserData, e_window* pWindow, e_window_event* pEvent)
{
    E_UNUSED(pUserData);

    switch (pEvent->type)
    {
        case E_WINDOW_EVENT_CLOSE:
        {
            return e_engine_exit(e_window_get_engine(pWindow), 0);
        } break;

        default: break;
    }

    return e_window_default_event_handler(pWindow, pEvent);
}

static e_window_vtable gTestWindowVTable =
{
    test_game_window_event
};



static e_result test_client_event_handler(void* pUserData, e_client* pClient, e_client_event* pEvent)
{
    (void)pUserData;

    switch (pEvent->type)
    {
        case E_CLIENT_EVENT_WINDOW_SIZE:
        {
            printf("EVENT: WINDOW_SIZE\n");
        } break;

        case E_CLIENT_EVENT_CURSOR_MOVE:
        {
            //printf("EVENT: CURSOR_MOVE: %d %d\n", pEvent->data.cursorMove.x, pEvent->data.cursorMove.y);
        } break;
    }

    return e_client_default_event_handler(pClient, pEvent);
}

static e_result test_client_step(void* pUserData, e_client* pClient, double dt)
{
    int cursorPosX;
    int cursorPosY;

    E_UNUSED(pUserData);
    E_UNUSED(pClient);
    E_UNUSED(dt);

    e_client_get_absolute_cursor_position(pClient, &cursorPosX, &cursorPosY);

    if (e_client_has_cursor_moved(pClient)) {
        printf("Cursor has moved: %d %d\n", cursorPosX, cursorPosY);
    }

    e_client_step_input(pClient);

    //printf("Cursor Position: %d %d\n", cursorPosX, cursorPosY);

    return E_SUCCESS;
}

static e_client_vtable gTestClientVTable =
{
    test_client_event_handler,
    test_client_step,
};


int main(int argc, char** argv)
{
    e_result result;
    e_engine_config engineConfig;
    e_engine* pEngine;
    e_client_config clientConfig;
    e_config_file configFile;
    e_log* pLog;
    int resX = 0;   /* Will be set to defaults below. */
    int resY = 0;

    result = e_log_init(NULL, &pLog);
    if (result != E_SUCCESS) {
        printf("Failed to initialize log.\n");
        return -1;
    }


    result = e_config_file_init(NULL, &configFile);
    if (result != E_SUCCESS) {
        printf("Failed to initialize config file.\n");
    } else {
        e_config_file_load_file(&configFile, NULL, "config.lua", NULL, pLog);
        e_config_file_load_file(&configFile, NULL, "mod.lua", NULL, pLog);

        e_config_file_get_int(&configFile, "game", "resolutionX", &resX);
        e_config_file_get_int(&configFile, "game", "resolutionY", &resY);

        if (resX == 0) {
            resX = 1280;
        }
        if (resY == 0) {
            resY = 720;
        }
    }


    engineConfig = e_engine_config_init(argc, argv, 0, &gTestEngineVTable, NULL);
    engineConfig.pLog = pLog;
    
    result = e_engine_init(&engineConfig, NULL, &pEngine);
    if (result != E_SUCCESS) {
        printf("Failed to initialize engine.\n");
        return -1;
    }

    /* We need a client for the game. */
    clientConfig = e_client_config_init(pEngine, "game");
    clientConfig.graphicsBackend = E_GRAPHICS_BACKEND_VULKAN;
    clientConfig.pVTable         = &gTestClientVTable;
    clientConfig.pVTableUserData = NULL;

    result = e_client_init(&clientConfig, NULL, &pClient1);
    if (result != E_SUCCESS) {
        printf("Failed to initialize client.\n");
        return -1;
    }


    /*
    There's only a single main loop so we need to run via the engine. This will run a single event handling
    loop from which every window will have their events processed.
    */
    result = e_engine_run(pEngine);
    if (result != E_SUCCESS) {
        printf("WARNING: e_engine_run() did not return successfully.");
    }

    printf("Shutting down.\n");

    /* Teardown. */
    e_client_uninit(pClient1, NULL);
    e_engine_uninit(pEngine, NULL);


    return 0;
}
