//**********************************************************************
//
// File:	Fraction.h
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

#ifndef FRACTION_H
#define FRACTION_H


class Fraction
{
private:
    // Data members
    int m_numerator, m_denominator;

public:
    // Public methods
    Fraction();
    Fraction(int numerator, int m_denominator);
    Fraction(int intVal);
    Fraction(const Fraction &other);

    Fraction& operator=(const Fraction &rhs);

    // Accessors.
    int GetNumerator() const { return m_numerator; }
    int GetDenominator() const { return m_denominator; }

    // Comparison operators.
    friend bool operator==(const Fraction &lhs, const Fraction &rhs);
    friend bool operator!=(const Fraction &lhs, const Fraction &rhs);
    friend bool operator<(const Fraction &lhs, const Fraction &rhs);
    friend bool operator>(const Fraction &lhs, const Fraction &rhs);

private:
    // Private methods
    void Normalize();
    static int GCD(int a, int b);
    static void EqualizeDenominators(Fraction &a, Fraction &b);
};


#endif // FRACTION_H
