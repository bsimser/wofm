//**********************************************************************
//
// File:	Fraction.cpp
// Author:	David H. Hovemeyer <daveho@cs.umd.edu>
//
// This code is in the public domain.
//
// Description:
//
//     The Fraction class is used to represent and compare simple
//     integer fractions.
//
//**********************************************************************


#include <cassert>
#include "Fraction.h"


////////////////////////////////////////////////////////////////////////
// Default constructor.
////////////////////////////////////////////////////////////////////////
Fraction::
Fraction()
    : m_numerator(1)
    , m_denominator(1)
{
}

////////////////////////////////////////////////////////////////////////
// Constructor from numerator and denominator.
////////////////////////////////////////////////////////////////////////
Fraction::
Fraction(int numerator, int denominator)
    : m_numerator(numerator)
    , m_denominator(denominator)
{
    assert(denominator != 0);
    Normalize();
}

////////////////////////////////////////////////////////////////////////
// Constructor from an integer.
////////////////////////////////////////////////////////////////////////
Fraction::
Fraction(int intVal)
    : m_numerator(intVal)
    , m_denominator(1)
{
}

////////////////////////////////////////////////////////////////////////
// Copy constructor.
////////////////////////////////////////////////////////////////////////
Fraction::
Fraction(const Fraction &other)
    : m_numerator(other.m_numerator)
    , m_denominator(other.m_denominator)
{
}

////////////////////////////////////////////////////////////////////////
// Assignment operator.
////////////////////////////////////////////////////////////////////////
Fraction& Fraction::
operator=(const Fraction &rhs)
{
    m_numerator   = rhs.m_numerator;
    m_denominator = rhs.m_denominator;
    return *this;
}

////////////////////////////////////////////////////////////////////////
// Test for equality.
////////////////////////////////////////////////////////////////////////
bool
operator==(const Fraction &lhs, const Fraction &rhs)
{
    // Since both fractions are in normalized form, a simple
    // equality test on numerator and denominator will suffice.
    return (lhs.m_numerator == rhs.m_numerator &&
	    lhs.m_denominator == rhs.m_denominator);
}

////////////////////////////////////////////////////////////////////////
// Test for inequality.
////////////////////////////////////////////////////////////////////////
bool
operator!=(const Fraction &lhs, const Fraction &rhs)
{
    // Since both fractions are in normalized form, a simple
    // inequality test on numerator and denominator will suffice.
    return (lhs.m_numerator != rhs.m_numerator &&
	    lhs.m_denominator != rhs.m_denominator);
}

////////////////////////////////////////////////////////////////////////
// Test if lhs is less than rhs.
////////////////////////////////////////////////////////////////////////
bool
operator<(const Fraction &lhs, const Fraction &rhs)
{
    Fraction lhsCopy(lhs), rhsCopy(rhs);
    Fraction::EqualizeDenominators(lhsCopy, rhsCopy);
    return lhsCopy.m_numerator < rhsCopy.m_numerator;
}

////////////////////////////////////////////////////////////////////////
// Test if lhs is greater than rhs.
////////////////////////////////////////////////////////////////////////
bool
operator>(const Fraction &lhs, const Fraction &rhs)
{
    Fraction lhsCopy(lhs), rhsCopy(rhs);
    Fraction::EqualizeDenominators(lhsCopy, rhsCopy);
    return lhsCopy.m_numerator > rhsCopy.m_numerator;
}

////////////////////////////////////////////////////////////////////////
// Normalize the fraction by dividing numerator and denominator
// by their greatest common divisor, and making sure that only
// the numerator is negative.
////////////////////////////////////////////////////////////////////////
void Fraction::
Normalize()
{
    int gcd = GCD(m_numerator, m_denominator);
    m_numerator /= gcd;
    m_denominator /= gcd;

    if (m_denominator < 0)
    {
	m_numerator   = -m_numerator;
	m_denominator = -m_denominator;
    }

    // Postconditions.
    assert(GCD(m_numerator, m_denominator) == 1);
    assert(m_denominator > 0);
}

////////////////////////////////////////////////////////////////////////
// Find greatest common divisor of given integers, using
// Euclid's algorithm.
////////////////////////////////////////////////////////////////////////
int Fraction::
GCD(int a, int b)
{
    // This code was shamelessly stolen off the 'net.
    int t;

    while (a > 0)
    {
	if (a < b)
	{
	    // swap a and b 
	    t = a;
	    a = b;
	    b = t;
	}  
	a = a - b;
    } 

    return b;
}

////////////////////////////////////////////////////////////////////////
// Make the denominators of given fractions equal, so that the
// fractions can be compared, added, or subtracted.
////////////////////////////////////////////////////////////////////////
void Fraction::
EqualizeDenominators(Fraction &a, Fraction &b)
{
    if (a.m_denominator != b.m_denominator)
    {
	int gcd = GCD(a.m_denominator, b.m_denominator);
	int t1, t2;

	t1 = b.m_denominator / gcd;
	t2 = a.m_denominator / gcd;

	a.m_numerator   *= t1;
	a.m_denominator *= t1;

	b.m_numerator   *= t2;
	b.m_denominator *= t2;

    }

    assert(a.m_denominator == b.m_denominator);
    // Denominators are now equal, but the fractions may not be
    // normalized, so don't return either one to the outside world.
}
