#pragma once

#include "main.h"

class CDrawing
{
public:
	void FillArea( int x , int y , int w , int h , BYTE r , BYTE g , BYTE b , BYTE a );
	void DrawBox( int x , int y , int w , int h , int linewidth , int r , int g , int b , int a );
};

extern CDrawing g_Drawing;