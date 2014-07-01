// OpenGLSceneGen.cpp: implementation of the OpenGLSceneGen class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>		// Header File For Windows
#include "OpenGLSceneGen.h"
#include "WorldBuilder.h"


//fonts
#include <math.h>						// Header File For Windows Math Library		( not used ) 
#include <stdio.h>						// Header File For Standard Input/Output
#include <stdarg.h>						// Header File For Variable Argument Routines

//#define SHOW_ALL 

long WorldBuilder::turns;

#define _UNICODE
#define UNICODE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HDC			hDC = NULL;		// Private GDI Device Context
HGLRC		hRC = NULL;		// Permanent Rendering Context
HWND		hWnd = NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

OpenGLSceneGen::OpenGLSceneGen()
{
    drawMap = true;
    showAll = false;
    addShadows = false;
    shadowStrength = 1.0f;

    width_scr = 80;  //assumes 640x480
    height_scr = 39;
    height_scr_offset = 465; //465 
}

int OpenGLSceneGen::InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
    glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
    glClearDepth(1.0f);									// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

    our_font.init("text.fnt", 10);					    //Build the freetype font 
    map_font.init("map.fnt", 12);					    //Build the freetype font

    return true;										// Initialization Went OK  
}

GLvoid OpenGLSceneGen::ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
    if (height == 0)										// Prevent A Divide By Zero By
    {
        height = 1;										// Making Height Equal One
    }
    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glLoadIdentity();									// Reset The Projection Matrix

    glViewport(0, 0, width, height);						// Reset The Current Viewport

    // Calculate The Aspect Ratio Of The Window
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
    glLoadIdentity();									// Reset The Modelview Matrix

    ///////////DISPLAY////////////

    width_scr = (int)(width / 9.2f);   //rough estimates
    height_scr = (int)(height / 16.0f);

    height_scr_offset = height - 15;
}

GLvoid OpenGLSceneGen::BuildFont(GLint fnt)			// Build Our Bitmap Font
{
    char font_name[32];
    int size;

    if (fnt == 0)
    {
        size = 18;
        strcpy(font_name, "Courier New");
        //	wcscpy(font_name,"Tahoma");
        //strcpy(font_name,"Lucida Console");
    }
    else
    {
        size = 16;
        strcpy(font_name, "Tahoma");
    }

    base = glGenLists(96);							// Storage For 96 Characters ( NEW )

    font = CreateFont(size,						// Height Of Font ( NEW )
        0,							// Width Of Font
        0,							// Angle Of Escapement
        0,							// Orientation Angle
        FW_NORMAL,					// Font Weight
        false,						// Italic
        false,						// Underline
        false,						// Strikeout
        ANSI_CHARSET,				// Character Set Identifier
        OUT_TT_PRECIS,				// Output Precision
        CLIP_DEFAULT_PRECIS,		// Clipping Precision
        ANTIALIASED_QUALITY,		// Output Quality
        FF_DONTCARE | DEFAULT_PITCH,	// Family And Pitch
        font_name);					// Font Name

    oldfont = (HFONT)SelectObject(hDC, font);		// Selects The Font We Want

    wglUseFontBitmaps(hDC, 32, 96, base);			// Builds 96 Characters Starting At Character 32

    SelectObject(hDC, oldfont);						// Selects The Font We Want
    DeleteObject(font);								// Delete The Font
}

GLvoid OpenGLSceneGen::glPrint(const char *fmt, ...)				// Custom GL "Print" Routine
{
    char		text[256];				// Holds Our String
    va_list		ap;						// Pointer To List Of Arguments

    if (fmt == NULL)					// If There's No Text
        return;							// Do Nothing

    va_start(ap, fmt);					// Parses The String For Variables
    vsprintf(text, fmt, ap);			// And Converts Symbols To Actual Numbers
    va_end(ap);							// Results Are Stored In Text

    glPushAttrib(GL_LIST_BIT);				// Pushes The Display List Bits		( NEW )
    glListBase(base - 32);					// Sets The Base Character to 32	( NEW )

    glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text	( NEW )
    glPopAttrib();						// Pops The Display List Bits	( NEW )
}

GLvoid OpenGLSceneGen::glwPrint(const wchar_t *fmt, ...)				// Custom GL "Print" Routine
{
    wchar_t		text[256];				// Holds Our String
    va_list		ap;						// Pointer To List Of Arguments

    if (fmt == NULL)					// If There's No Text
        return;							// Do Nothing

    va_start(ap, fmt);					// Parses The String For Variables
    vswprintf(text, fmt, ap);			// And Converts Symbols To Actual Numbers
    va_end(ap);							// Results Are Stored In Text

    glPushAttrib(GL_LIST_BIT);				// Pushes The Display List Bits		( NEW )
    glListBase(base - 32);					// Sets The Base Character to 32	( NEW )

    glCallLists(wcslen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text	( NEW )
    glPopAttrib();						// Pops The Display List Bits	( NEW )
}

int OpenGLSceneGen::DrawGLScene(GLvoid)				// Here's Where We Do All The Drawing
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

    //DrawBackgroundArea();

    if (drawMap)
    {
        //if(WorldBuilder::State()!=sMap)
        DrawMap();
        //else
        //	DrawBigMap();
    }
    else
        DrawTextDisplay();

    DrawTextLines();

    return 1;
}

//using TT font
void OpenGLSceneGen::DrawTextDisplay()
{
    // Red text
    glColor3ub(0xff, 0, 0);

    glPushMatrix();
    glLoadIdentity();
    //skip space 0
    freetype::print(our_font, 5, height_scr_offset - (1 * 14.0f), WorldBuilder::textManager.GetDisplayLine(0)->c_str());
    freetype::print(our_font, 5, height_scr_offset - (2 * 14.0f), WorldBuilder::textManager.GetDisplayLine(1)->c_str());

    glColor3ub(0xff, 0xff, 64);
    for (int i = 2; i < 39; i++)
    {
        freetype::print(our_font, 5.0f, height_scr_offset - ((i + 1)*14.0f), WorldBuilder::textManager.GetDisplayLine(i)->c_str());
    }

    glColor3ub(0xff, 0, 0);
    freetype::print(our_font, 5.0f, 56.0f, WorldBuilder::textManager.GetDisplayLine(39)->c_str());

    glPopMatrix();
}


void OpenGLSceneGen::DrawBackgroundArea()
{
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, 0.0f);

    glBegin(GL_QUADS);									// Draw A Quad
    glColor3f(.9f, .9f, .9f);
    int x = width_scr;
    glVertex3f(-0.55f, -0.35f, -1.0f);				// Top Left
    glVertex3f(0.55f, -0.35f, -1.0f);				// Top Right
    glVertex3f(0.55f, -0.41f, -1.0f);				// Bottom Right
    glVertex3f(-0.55f, -0.41f, -1.0f);				// Bottom Left
    glEnd();
}

void OpenGLSceneGen::CreateOffset(coord pos)
{
    offset.x = pos.x;
    offset.y = pos.y;
}

int OpenGLSceneGen::calcX(int x)
{
    const float fontW = 9;

    return (x * fontW + x);
}

int OpenGLSceneGen::calcY(int y)
{ 
    const int fontH = 14;

    return (height_scr_offset - y * fontH);
}

void OpenGLSceneGen::DrawMap()
{
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -1); //restore

    float maxX = 0.49f;
    float maxY = 0.34f;

    float sizeX = .014f;
    float sizeY = .022f;

    int temp = 0;
    int h_MOD = 0;
    int W_MOD = 0;

    width_scr = 80;  //no scrolling
    height_scr = 39;

    /*int h_MOD=offset.y-height_scr/2; //Y-offset to centre character
    if(h_MOD < 0) h_MOD=0 ;
    else if(h_MOD > DUNGEON_SIZE_H-height_scr) h_MOD = DUNGEON_SIZE_H-height_scr;

    int W_MOD=offset.x-width_scr/2; //X-offset to centre character
    if(W_MOD < 0) W_MOD=0;
    else if(W_MOD > DUNGEON_SIZE_W-width_scr) W_MOD = DUNGEON_SIZE_W-width_scr;

    h_MOD = (h_MOD/5 * 5); //make screen move in steps (this calc works because it is using integers)
    W_MOD = (W_MOD/12 * 12);*/

    if (dLevel->getMapLight() == DungeonLevel::eNoFound)
         addShadows = false; 
    else
        addShadows = true; 

    for (int h = 0; h < height_scr; h++)
    {
        for (int w = 0; w < width_scr; w++)
        {
            if (WorldBuilder::GetCurrentLevel() == 0) // show all level
                ;
            else if (!showAll && !dLevel->map[w + W_MOD][h + h_MOD].terrain.found) //don't show
                continue;

            if ((dLevel->map[w + W_MOD][h + h_MOD].GetMonster() && dLevel->map[w + W_MOD][h + h_MOD].terrain.light)  //display creature if lit
                || (dLevel->map[w + W_MOD][h + h_MOD].GetMonster() && showAll))
            {
                Monster* monster = dLevel->map[w + W_MOD][h + h_MOD].GetMonster();
                glColor3ub(monster->color1, monster->color2, monster->color3);

                freetype::qprint(map_font, calcX(w), calcY(h), dLevel->map[w + W_MOD][h + h_MOD].GetMonster()->symbol);
            }
            else if (dLevel->map[w + W_MOD][h + h_MOD].getItem() && (addShadows || dLevel->map[w + W_MOD][h + h_MOD].terrain.light)) //display items
            {
                Item * item = dLevel->map[w + W_MOD][h + h_MOD].getItem();
                glColor3ub(item->color1, item->color2, item->color3);

                // update colour on screen if better than item held
                if (!item->hasBrand() && !item->hasResistance())
                {
                    if (item->type == weapon)
                    {
                        monsterData* player = WorldBuilder::monsterManager.Player();
                        Item * w = WorldBuilder::monsterManager.monsterItems.GetEquipment(player, weapon);

                        if (!w || item->getAverage_h2h() > w->getAverage_h2h())
                        {
                            glColor3ub(0, 128, 255);
                        }
                    }
                    else if (item->type == armour)
                    {
                        monsterData* player = WorldBuilder::monsterManager.Player();
                        Item * w = WorldBuilder::monsterManager.monsterItems.GetEquipment(player, armour);

                        if (!w || item->absorb_bonus > w->absorb_bonus)
                        {
                            glColor3ub(0, 128, 255);
                        }
                    }
                    else if (item->type == shield)
                    {
                        monsterData* player = WorldBuilder::monsterManager.Player();
                        Item * w = WorldBuilder::monsterManager.monsterItems.GetEquipment(player, shield);

                        if (!w || item->absorb_bonus > w->absorb_bonus)
                        {
                            glColor3ub(0, 128, 255);
                        }
                    }
                    else if (item->type == projectileWeapon)
                    {
                        monsterData* player = WorldBuilder::monsterManager.Player();
                        Item * w = WorldBuilder::monsterManager.monsterItems.GetEquipment(player, projectileWeapon);

                        if (!w || item->getAverage_thr() > w->getAverage_thr())
                        {
                            glColor3ub(0, 128, 255);
                        }
                    }

                }
                freetype::qprint(map_font, calcX(w), calcY(h), dLevel->map[w + W_MOD][h + h_MOD].getItem()->symbol);
            }
            else if (dLevel->map[w + W_MOD][h + h_MOD].terrain.light) //display terrain
            {
                glColor3ub((dLevel->map[w + W_MOD][h + h_MOD].terrain.color1),
                    (dLevel->map[w + W_MOD][h + h_MOD].terrain.color2),
                    (dLevel->map[w + W_MOD][h + h_MOD].terrain.color3));

                //special water  effect
                if (dLevel->map[w + W_MOD][h + h_MOD].terrain.type == deepWater && dLevel->map[w + W_MOD][h + h_MOD].terrain.light) //make water flow
                {
                    int flow = WorldBuilder::GetTurns();

                    if ((h + flow) % 3 == 0)
                        glColor3ub(0, 64, 128);
                    else if (((h + flow) + 1) % 3 == 0)
                        glColor3ub(0, 12, 128);
                    else
                        glColor3ub(0, 128, 255);
                }
                freetype::qprint(map_font, calcX(w), calcY(h), dLevel->map[w + W_MOD][h + h_MOD].terrain.symbol);
                //freetype::qprint(map_font, 0 + (w*9.1f), (float)height_scr_offset - (h*14), '.');

            }
            else if (addShadows)//add night effect
            {
                glColor3ub((GLubyte)(dLevel->map[w + W_MOD][h + h_MOD].terrain.color1*shadowStrength),
                    (GLubyte)(dLevel->map[w + W_MOD][h + h_MOD].terrain.color2*shadowStrength),
                    (GLubyte)(dLevel->map[w + W_MOD][h + h_MOD].terrain.color3*shadowStrength));

                freetype::qprint(map_font, calcX(w), calcY(h), dLevel->map[w + W_MOD][h + h_MOD].terrain.symbol);
                //freetype::qprint(map_font, 0 + (w*9.1f), (float)height_scr_offset - (h*14.5f), dLevel->map[w + W_MOD][h + h_MOD].terrain.symbol);
            }
            //add extra characters
            if (dLevel->map[w + W_MOD][h + h_MOD].show_target != 0) //show target
            {
                glColor3ub(0xff, 0xff, 0xff);
                freetype::qprint(map_font, calcX(w), calcY(h), 0);
            }
            if (dLevel->map[w + W_MOD][h + h_MOD].show_path != none) //show target path (overlay)
            {
                if (dLevel->map[w + W_MOD][h + h_MOD].show_path == clear)
                    glColor3ub(0xff, 0xff, 0xff);
                else if (dLevel->map[w + W_MOD][h + h_MOD].show_path == blocked)
                    glColor3ub(0xff, 0, 0);
                freetype::qprint(map_font, calcX(w), calcY(h) , '*');
            }
        }
    }
}
/*void OpenGLSceneGen::DrawBigMap()
{
glLoadIdentity();
glTranslatef(0.0f,0.0f,-1); //restore

float maxX = 0.49f;
float maxY = 0.34f;

float sizeX = .014f;
float sizeY = .022f;

int temp = 0;

int h_MOD=offset.y-height_scr/2;
if(h_MOD < 0) h_MOD=0 ;
else if(h_MOD > (DUNGEON_SIZE_H/MAP_SCALE)-height_scr) h_MOD = (DUNGEON_SIZE_H/MAP_SCALE)-height_scr;

int W_MOD=offset.x-width_scr/2;
if(W_MOD < 0) W_MOD=0;
else if(W_MOD > (DUNGEON_SIZE_W/MAP_SCALE)-width_scr) W_MOD = (DUNGEON_SIZE_W/MAP_SCALE)-width_scr;

for( int h=0;h<height_scr;h++)
{
for(int w=0;w<width_scr;w++)
{
if(h==offset.y-h_MOD && w==offset.x-W_MOD) //you are here
{
glColor3ub(200,200,40);
freetype::print(map_font,(float)(w*9.1f) , (float)height_scr_offset-(h*14.5f),"@" );
}
else //if(true) //display terrain
{
glColor3f(convert255(dLevel->bigmap[w+W_MOD][h+h_MOD].terrain.color1),
convert255(dLevel->bigmap[w+W_MOD][h+h_MOD].terrain.color2),
convert255(dLevel->bigmap[w+W_MOD][h+h_MOD].terrain.color3));

freetype::print(map_font,(float)0+(w*9.1f) , (float)height_scr_offset-(h*14.5f),"%c", dLevel->bigmap[w+W_MOD][h+h_MOD].terrain.symbol);

}
}
}*
}*/

void OpenGLSceneGen::DrawTextLines()
{
    //Text 3 lines

    glColor3ub(224, 224, 224);

    glPushMatrix();
    glLoadIdentity(); //22,5

    freetype::print(our_font, 5, 28, WorldBuilder::textManager.display_line1.c_str());
    freetype::print(our_font, 5, 8, WorldBuilder::textManager.GetMessageLine(0)->c_str());

    glPopMatrix();
}

float OpenGLSceneGen::convert255(int c1)
{
    float c = (float)c1;
    return c / 255.0f;

}

GLvoid OpenGLSceneGen::KillFont(GLvoid)						// Delete The Font List
{
    glDeleteLists(base, 96);				// Delete All 96 Characters ( NEW )
}




GLvoid OpenGLSceneGen::KillGLWindow(GLvoid)								// Properly Kill The Window
{
    //KillFont();
    our_font.clean();
    map_font.clean();

    if (fullscreen)										// Are We In Fullscreen Mode?
    {
        ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
        ShowCursor(true);								// Show Mouse Pointer
    }

    if (hRC)											// Do We Have A Rendering Context?
    {
        if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
        {
            MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        }

        if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
        {
            MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        }
        hRC = NULL;										// Set RC To NULL
    }

    if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
    {
        MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        hDC = NULL;										// Set DC To NULL
    }

    if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
    {
        MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        hWnd = NULL;										// Set hWnd To NULL
    }

    if (!UnregisterClass("OpenGL", hInstance))			// Are We Able To Unregister Class
    {
        MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        hInstance = NULL;									// Set hInstance To NULL
    }
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (true) Or Windowed Mode (false)	*/

BOOL OpenGLSceneGen::CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
    GLuint		PixelFormat;			// Holds The Results After Searching For A Match
    WNDCLASS	wc;						// Windows Class Structure
    DWORD		dwExStyle;				// Window Extended Style
    DWORD		dwStyle;				// Window Style
    RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
    WindowRect.left = (long)0;			// Set Left Value To 0
    WindowRect.right = (long)width;		// Set Right Value To Requested Width
    WindowRect.top = (long)0;				// Set Top Value To 0
    WindowRect.bottom = (long)height;		// Set Bottom Value To Requested Height

    fullscreen = fullscreenflag;			// Set The Global Fullscreen Flag

    hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
    wc.lpfnWndProc = (WNDPROC)WndProc;					// WndProc Handles Messages
    wc.cbClsExtra = 0;									// No Extra Window Data
    wc.cbWndExtra = 0;									// No Extra Window Data
    wc.hInstance = hInstance;							// Set The Instance
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
    wc.hbrBackground = NULL;									// No Background Required For GL
    wc.lpszMenuName = NULL;									// We Don't Want A Menu
    wc.lpszClassName = "OpenGL";								// Set The Class Name

    if (!RegisterClass(&wc))									// Attempt To Register The Window Class
    {
        MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return false;											// Return false
    }

    if (fullscreen)												// Attempt Fullscreen Mode?
    {
        DEVMODE dmScreenSettings;								// Device Mode
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);		// Size Of The Devmode Structure
        dmScreenSettings.dmPelsWidth = width;				// Selected Screen Width
        dmScreenSettings.dmPelsHeight = height;				// Selected Screen Height
        dmScreenSettings.dmBitsPerPel = bits;					// Selected Bits Per Pixel
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
        if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        {
            // If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
            if (MessageBox(NULL, "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", "OPEN GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
            {
                fullscreen = false;		// Windowed Mode Selected.  Fullscreen = false
            }
            else
            {
                // Pop Up A Message Box Letting User Know The Program Is Closing.
                MessageBox(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP);
                return false;									// Return false
            }
        }
    }

    if (fullscreen)												// Are We Still In Fullscreen Mode?
    {
        dwExStyle = WS_EX_APPWINDOW;								// Window Extended Style
        dwStyle = WS_POPUP;										// Windows Style
        ShowCursor(false);										// Hide Mouse Pointer
    }
    else
    {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
        dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style
    }

    AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);		// Adjust Window To True Requested Size

    // Create The Window
    if (!(hWnd = CreateWindowEx(dwExStyle,							// Extended Style For The Window
        "OpenGL",							// Class Name
        title,								// Window Title
        dwStyle |							// Defined Window Style
        WS_CLIPSIBLINGS |					// Required Window Style
        WS_CLIPCHILDREN,					// Required Window Style
        0, 0,								// Window Position
        WindowRect.right - WindowRect.left,	// Calculate Window Width
        WindowRect.bottom - WindowRect.top,	// Calculate Window Height
        NULL,								// No Parent Window
        NULL,								// No Menu
        hInstance,							// Instance
        NULL)))								// Dont Pass Anything To WM_CREATE
    {
        KillGLWindow();								// Reset The Display
        MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return false;								// Return false
    }

    static	PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
    {
        sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
        1,											// Version Number
        PFD_DRAW_TO_WINDOW |						// Format Must Support Window
        PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
        PFD_DOUBLEBUFFER,							// Must Support Double Buffering
        PFD_TYPE_RGBA,								// Request An RGBA Format
        bits,										// Select Our Color Depth
        0, 0, 0, 0, 0, 0,							// Color Bits Ignored
        0,											// No Alpha Buffer
        0,											// Shift Bit Ignored
        0,											// No Accumulation Buffer
        0, 0, 0, 0,									// Accumulation Bits Ignored
        16,											// 16Bit Z-Buffer (Depth Buffer)  
        0,											// No Stencil Buffer
        0,											// No Auxiliary Buffer
        PFD_MAIN_PLANE,								// Main Drawing Layer
        0,											// Reserved
        0, 0, 0										// Layer Masks Ignored
    };

    if (!(hDC = GetDC(hWnd)))							// Did We Get A Device Context?
    {
        KillGLWindow();								// Reset The Display
        MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return false;								// Return false
    }

    if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))	// Did Windows Find A Matching Pixel Format?
    {
        KillGLWindow();								// Reset The Display
        MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return false;								// Return false
    }

    if (!SetPixelFormat(hDC, PixelFormat, &pfd))		// Are We Able To Set The Pixel Format?
    {
        KillGLWindow();								// Reset The Display
        MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return false;								// Return false
    }

    if (!(hRC = wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
    {
        KillGLWindow();								// Reset The Display
        MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return false;								// Return false
    }

    if (!wglMakeCurrent(hDC, hRC))					// Try To Activate The Rendering Context
    {
        KillGLWindow();								// Reset The Display
        MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return false;								// Return false
    }

    ShowWindow(hWnd, SW_SHOW);						// Show The Window
    SetForegroundWindow(hWnd);						// Slightly Higher Priority
    SetFocus(hWnd);									// Sets Keyboard Focus To The Window
    ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

    if (!InitGL())									// Initialize Our Newly Created GL Window
    {
        KillGLWindow();								// Reset The Display
        MessageBox(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return false;								// Return false
    }

    return true;									// Success
}

void OpenGLSceneGen::SwapBuffers()
{
    ::SwapBuffers(hDC);
}

#undef _UNICODE
#undef UNICODE
