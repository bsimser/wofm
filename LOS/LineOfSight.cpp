//**********************************************************************
//
// File:	LineOfSight.cpp
// Author:	David H. Hovemeyer <daveho@cs.umd.edu>
//
// This code is in the public domain.
//
// Description:
//
//     The LineOfSight class calculates the visibility of map cells
//     from an observer's standpoint.
//
//     The class is implemented as an abstract class with a single
//     virtual method, which determines which map cells are
//     transparent.  To use the LineOfSight class, derive a class
//     which implements this method for the map in question.
//
//**********************************************************************


#include "Fraction.h"
#include "LineOfSight.h"
#include "..\coord.h"

////////////////////////////////////////////////////////////////////////
// Destructor.
////////////////////////////////////////////////////////////////////////
LineOfSight::
~LineOfSight()
{
}

////////////////////////////////////////////////////////////////////////
// Determine if the map cell at given coordinates is visible,
// by tracing a line of sight from the source to the destination
// cell.
////////////////////////////////////////////////////////////////////////
bool LineOfSight::
IsCellVisible(int srcX, int srcY, int destX, int destY)
{
    // Special case: if destination is the same as the source, the
    // cell is visible trivially.
    if (destX == srcX && destY == srcY)
		return true;

    // Figure out which direction (horizontal and vertical) the
    // destination cell lies in.
    int dirX = (destX >= srcX) ? 1 : -1;
    int dirY = (destY >= srcY) ? 1 : -1;

    // Special case: vertical path to destination.
    if (srcX == destX)
    {
		int curY = srcY;

		for (;;)
		{
			curY += dirY;

			if (curY == destY)
				return true;

			if (!IsCellTransparent(destX, curY))
				return false;
		}
    }

    // Scale source and destination by a factor of 2.
    srcX *= 2;
    srcY *= 2;
    destX *= 2;
    destY *= 2;

    // In order to determine the visibility of the destination cell,
    // we trace a line of sight to three different points on the
    // destination cell:
    //
    //  1. the nearest corner
    //  2. the midpoint of the nearest vertical edge
    //  3. the midpoint of the nearest horizontal edge
    //
    // If we can reach the destination cell by any of these lines,
    // then the cell is visible.

    // Note that since the algorithm used by TracePath() translates
    // coordinates to the (+x,+y) quadrant when performing slope
    // calculations, we take the absolute value of the x and y
    // differences when calculating the slopes.

    Fraction slope1(abs((destY - dirY) - srcY), abs((destX - dirX) - srcX));
    Fraction slope2(abs(destY - srcY), abs((destX - dirX) - srcX));
    Fraction slope3(abs((destY - dirY) - srcY), abs(destX - srcX));

    return ( TracePath(slope1, dirX, dirY, srcX, srcY, destX, destY) ||
	     TracePath(slope2, dirX, dirY, srcX, srcY, destX, destY) ||
	     TracePath(slope3, dirX, dirY, srcX, srcY, destX, destY) );

}

////////////////////////////////////////////////////////////////////////
// Trace a line from the source to the destination cell, following
// the line indicated by the given slope.  Return true if all
// intervening cells are transparent, false otherwise.
////////////////////////////////////////////////////////////////////////
bool LineOfSight::
TracePath(const Fraction &slope,
	  int dirX, int dirY,
	  int srcX, int srcY,
	  int destX, int destY) 
{
    int curX = srcX;
    int curY = srcY;

    for (;;)
    {
		// Calculate the slope of the line connecting the source with
		// the corner of the current cell.  If this slope is greater
		// than the LOS slope, then the LOS path increases in the
		// horizontal direction.  If this slope is less than the LOS
		// slope, then the LOS path increases in the vertical
		// direction.
		Fraction testSlope(abs((curY + dirY) - srcY),
				abs((curX + dirX) - srcX));

		if (testSlope > slope)
		{
			curX += 2 * dirX;
		}
		else if (testSlope < slope)
		{
			curY += 2 * dirY;
		}
		else
		{
			curX += 2 * dirX;
			curY += 2 * dirY;
		}

		// If we reached the destination, then the destination
		// is visible.
		if (curX == destX && curY == destY)
			return true;

		// We haven't reached the destination yet, so in order to
		// continue, this cell has to be transparent.
		if (!IsCellTransparent(curX / 2, curY / 2))
			return false;	
    }

    // We never reach here.
}
