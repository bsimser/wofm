//**********************************************************************
//
// File:	LineOfSight.h
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

#ifndef LINEOFSIGHT_H
#define LINEOFSIGHT_H

#include <math.h>
#include "..\coord.h"
class Fraction;

class LineOfSight
{
public:
    // Public methods
    virtual ~LineOfSight();

    bool IsCellVisible(int srcX, int srcY, int destX, int destY) ;

    // This method must be implemented by the subclass to determine
    // which map cells can be seen through.
    virtual bool IsCellTransparent(int x, int y)  = 0;

private:
    bool TracePath(const Fraction &slope,
		   int dirX, int dirY,
		   int srcX, int srcY,
		   int destX, int destY) ;
};

#endif // LINEOFSIGHT_H
