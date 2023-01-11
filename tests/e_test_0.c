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

    e_client_step(pClient1, dt);


    /* Do some drawing on the graphics engine. */
    e_graphics_present_surface(pVulkanGraphics, pVulkanSurface);

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

    return E_SUCCESS;
}

static e_window_vtable gTestWindowVTable =
{
    test_game_window_event
};


int main(int argc, char** argv)
{
    e_result result;
    e_engine_config engineConfig;
    e_engine* pEngine;
    e_client_config clientConfig;
    e_config_file_config configFileConfig;
    e_config_file configFile;
    e_log* pLog;
    int resX = 0;   /* Will be set to defaults below. */
    int resY = 0;

    result = e_log_init(NULL, &pLog);
    if (result != E_SUCCESS) {
        printf("Failed to initialize log.\n");
        return -1;
    }


    configFileConfig = e_config_file_config_init_filepath("config.lua");
    configFileConfig.pLog = pLog;

    result = e_config_file_init(&configFileConfig, NULL, &configFile);
    if (result != E_SUCCESS) {
        printf("Failed to initialize config file.\n");
    } else {
        e_config_file_get_int(&configFile, "graphics", "resolutionX", &resX);
        e_config_file_get_int(&configFile, "graphics", "resolutionY", &resY);

        if (resX == 0) {
            resX = 1280;
        }
        if (resY == 0) {
            resY = 720;
        }
    }


    engineConfig = e_engine_config_init(argc, argv, 0, &gTestEngineVTable);
    engineConfig.pLog = pLog;
    
    result = e_engine_init(&engineConfig, NULL, &pEngine);
    if (result != E_SUCCESS) {
        printf("Failed to initialize engine.\n");
        return -1;
    }

    /* We need a client for the game. */
    clientConfig = e_client_config_init(pEngine, "Test Game 3", 0, NULL);

    result = e_client_init(&clientConfig, NULL, &pClient1);
    if (result != E_SUCCESS) {
        printf("Failed to initialize client.\n");
        return -1;
    }

#if 0

    /* Testing for Vulkan graphics. */
    #if 1
    {
        e_window_config windowConfig;

        windowConfig = e_window_config_init(pEngine, "Vulkan", 0, 0, resX, resY, 0, &gTestWindowVTable);

        result = e_window_init(&windowConfig, NULL, &pVulkanWindow);
        if (result != E_SUCCESS) {
            printf("Failed to create Vulkan window.");
            return -1;
        }



        e_graphics_config graphicsConfig;

        graphicsConfig = e_graphics_config_init(pEngine);
        graphicsConfig.backend = E_GRAPHICS_BACKEND_VULKAN;

        result = e_graphics_init(&graphicsConfig, NULL, &pVulkanGraphics);
        if (result != E_SUCCESS) {
            printf("Failed to initialize Vulkan graphics sub-system.");
            return -1;
        }



        e_graphics_device_info deviceInfos[16];
        size_t deviceInfoCount = E_COUNTOF(deviceInfos);

        result = e_graphics_get_devices(pVulkanGraphics, NULL, &deviceInfoCount, deviceInfos);
        if (result != E_SUCCESS) {
            printf("Failed to retrieve graphics devices.");
            return -1;
        }

        size_t iDevice;
        for (iDevice = 0; iDevice < deviceInfoCount; iDevice += 1) {
            printf("Graphics Device (id = %u): %s\n", deviceInfos[iDevice].id, deviceInfos[iDevice].name);
        }



        e_graphics_surface_config surfaceConfig = e_graphics_surface_config_init(pVulkanGraphics, pVulkanWindow);

        result = e_graphics_surface_init(&surfaceConfig, NULL, &pVulkanSurface);
        if (result != E_SUCCESS) {
            printf("Failed to create Vulkan surface.");
            return -1;
        }
    }
    #endif

#endif

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
