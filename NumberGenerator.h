// NumberGenerator.h: interface for the NumberGenerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUMBERGENERATOR_H__8F93E679_F7BB_4428_93A5_1AF31F278037__INCLUDED_)
#define AFX_NUMBERGENERATOR_H__8F93E679_F7BB_4428_93A5_1AF31F278037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace Random
{

//class NumberGenerator  
//{
//public:
	//NumberGenerator();
//	virtual ~NumberGenerator();

	int getInt(int max, int min);

	void reseed();

	static int primeNumbers[20] = {2,3,5,7,9,11,13,17,19,23,27,29,31,37,41,47,53,59,61,67};

//}; 
/*
     71 
     73     79     83     89     97    101    103    107    109    113 
    127    131    137    139    149    151    157    163    167    173 
    179    181    191    193    197    199    211    223    227    229 
    233    239    241    251    257    263    269    271    277    281 
	*/
}
#endif // !defined(AFX_NUMBERGENERATOR_H__8F93E679_F7BB_4428_93A5_1AF31F278037__INCLUDED_)
