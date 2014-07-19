// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------


#if !defined(AFX_OPENGLSCENEGEN_H__AD83E990_77EB_443B_93FE_FEF2F0BCB075__INCLUDED_)
#define AFX_OPENGLSCENEGEN_H__AD83E990_77EB_443B_93FE_FEF2F0BCB075__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
//#include <gl\glaux.h>		// Header File For The Glaux Library

#include "FreeTypeFont.h"		// Header for our little font library.

#include "SceneGenerator.h"

#include "DungeonLevel.h" //character map

class OpenGLSceneGen : public SceneGenerator  
{
public:
	OpenGLSceneGen();

	GLvoid ReSizeGLScene(GLsizei width, GLsizei height);

	GLvoid KillGLWindow(GLvoid)	;
	GLvoid KillFont(GLvoid)	;
	int DrawGLScene(GLvoid)	;
	int InitGL(GLvoid);
	GLvoid BuildFont(GLint fnt);
	GLvoid glPrint(const char *fmt, ...);
	GLvoid glwPrint(const wchar_t *fmt, ...);

	BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag);

	void SwapBuffers();

	bool FullScreen() {return fullscreen;};
	void FullScreen(bool fullscr) {fullscreen=fullscr;};
	void DisplayMap(bool _drawMap) {drawMap=_drawMap;};

	void DrawBackgroundArea();
	void DrawMap();
	//void DrawBigMap();
	void DrawTextLines();
	void DrawTextDisplay();

	DungeonLevel *dLevel;

	float convert255(int c1);

	void ShowAll(bool show_all){showAll = show_all;};

	void AddShadows(bool add_shadows){addShadows = add_shadows;};
	void ShadowStrength(float strength){shadowStrength = strength;};

	//symbol and colour map

	void CreateOffset(Coord pos);
	

private:

    int calcX(int x);
    int calcY(int y);

	bool fullscreen;
	bool drawMap;
	bool showAll;
	bool addShadows;
	float shadowStrength;
	int width_scr; 
	int height_scr;
	int height_scr_offset;


	Coord offset; //base offset for large maps (fit portion of map into screen) //this is generally players position


	//fonts
	GLuint	base;							// Base Display List For The Font Set
	GLfloat	cnt1;							// 1st Counter Used To Move Text & For Coloring
	GLfloat	cnt2;							// 2nd Counter Used To Move Text & For Coloring

	HFONT	font;									// Windows Font ID
	HFONT	oldfont;								// Used For Good House Keeping

	
	// This holds all the information for the font that we are going to create.
	freetype::font_data our_font;
	freetype::font_data map_font;	



};

#endif // !defined(AFX_OPENGLSCENEGEN_H__AD83E990_77EB_443B_93FE_FEF2F0BCB075__INCLUDED_)
