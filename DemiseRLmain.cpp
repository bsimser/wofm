
#include <windows.h>        // Header File For Windows
#include "WorldBuilder.h"
#include "OpenGLSceneGen.h"

//general
bool    keys[256];            // Array Used For The Keyboard Routine
bool    active = true;        // Window Active Flag Set To true By Default

bool    keypress;

WorldBuilder world; //the one and only world

LRESULT CALLBACK WndProc(HWND    hWnd,            // Handle For This Window
						 UINT    uMsg,            // Message For This Window
					 	 WPARAM    wParam,            // Additional Message Information
					 	 LPARAM    lParam)            // Additional Message Information
{
	// initial check for non-US keys
	switch (wParam)
	{

	case 60: // '<' 
		keys[wParam] = TRUE;                                  // key
		keypress = true;
		return 0;
	case 62: // '>'
		keys[wParam] = TRUE;                                  // key
		keypress = true;
		return 0;
	case 63: // '?'
		keys[wParam] = TRUE;                                  // help key
		keypress = true;
		return 0;
	}

    switch (uMsg)                                    // Check For Windows Messages
    {
    case WM_ACTIVATE:                            // Watch For Window Activate Message
    {
        if (!HIWORD(wParam))                    // Check Minimization State
        {
            active = true;                        // Program Is Active
        }
        else
        {
            active = false;                        // Program Is No Longer Active
        }

        return 0;                                // Return To The Message Loop
    }

    case WM_SYSCOMMAND:                            // Intercept System Commands
    {
		switch (wParam)                            // Check System Calls
		{
		case SC_SCREENSAVE:                  // Screensaver Trying To Start?
		case SC_MONITORPOWER:                // Monitor Trying To Enter Powersave?
			return 0;                            // Prevent From Happening
		}
		break;                                    // Exit
	}

    case WM_CLOSE:                                // Did We Receive A Close Message?
    {
        PostQuitMessage(0);                        // Send A Quit Message
        return 0;                                // Jump Back
    }

    case WM_KEYDOWN:                            // Is A Key Being Held Down?
    {
        keys[wParam] = true;                    // If So, Mark It As true
        keypress = true;
        return 0;                                // Jump Back
    }

    case WM_KEYUP:                                // Has A Key Been Released?
    {
        keys[wParam] = false;                    // If So, Mark It As false
        return 0;                                // Jump Back
    }

    case WM_SIZE:                                // Resize The OpenGL Window
    {
        world.Resize(lParam, wParam);

        return 0;                                // Jump Back
    }
    }

    // Pass All Unhandled Messages To DefWindowProc
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE    hInstance,    // Instance
    HINSTANCE    hPrevInstance,                // Previous Instance
    LPSTR        lpCmdLine,                    // Command Line Parameters
    int            nCmdShow)                    // Window Show State
{
    MSG        msg;                                    // Windows Message Structure
    BOOL    done = false;                                // Bool Variable To Exit Loop

    keypress = false;

    try
    {
        if (!world.Initialise("The Warlock of Firetop Mountain: Roguelike (c) 2014 Corremn"))
            return 0;
    }
    catch (std::exception & ex)
    {
        ::MessageBox(NULL, ex.what(), "Initialisation", MB_ICONERROR);

    }
    catch (...)
    {
        ::MessageBox(NULL, "Unknown Error", "Initialisation", MB_ICONERROR);
    }

	world.Run(); //display world

    while (!done)                                    // Loop That Runs While done=false
    {
        //Sleep(10);

        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))    // Is There A Message Waiting?
        {
            if (msg.message == WM_QUIT)                // Have We Received A Quit Message?
            {
                done = true;                            // If So done=true
            }
            else                                    // If Not, Deal With Window Messages
            {
                TranslateMessage(&msg);                // Translate The Message
                DispatchMessage(&msg);                // Dispatch The Message
            }
        }
        else                                        // If There Are No Messages
        {
            // Draw The Scene.  Watch For Quit Messages From DrawGLScene()

            // add key press handler here ??cjm
            if (active)                                // Program Active?
            {
                if (keypress)
                {
                    world.ProcessCommand(keys);
					world.Run();
					keypress = false;
				}
                else                                // Not Time To Quit, Update Screen
                {
					if (world.State() == sRunning || world.State() == sResting) //update while player is running or resting(HACK)
						world.Run();
                }
				Sleep(10);
            }

            if (keys[VK_F1])                // Is F1 Being Pressed?
            {
                keys[VK_F1] = false;            // If So Make Key false
                world.ToggleFullScreen(800, 600);
            }
            if (keys[VK_F2])                // Is F2 Being Pressed?
            {
                keys[VK_F2] = false;            // If So Make Key false

                world.ToggleFullScreen(1024, 768);

            }/*
            if (keys[VK_F3])                // Is F3 Being Pressed?
            {
            keys[VK_F3]=false;            // If So Make Key false

            world.ToggleFullScreen(1024,768 );

            }
            if (keys[VK_F4])                // Is F4 Being Pressed?
            {
            keys[VK_F4]=false;            // If So Make Key false

            world.ToggleFullScreen(1280,1024);

            }*/

        }
    }

    // Shutdown
    world.Stop();

    try{
        return 0;                            // Exit The Program
    }
    catch (...)
    {
        return 0;
    }
}
