// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------


#if !defined(AFX_SPECIALGUARDIANS_H__33DD115A_0CD5_4160_8FCB_1BB192712871__INCLUDED_)
#define AFX_SPECIALGUARDIANS_H__33DD115A_0CD5_4160_8FCB_1BB192712871__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Monster.h"

class SpecialGuardians : public Monster  
{
public:
	SpecialGuardians();
	virtual ~SpecialGuardians();
	int Create(int type,int level);

private:
	int createGuard(int level);

};

#endif // !defined(AFX_SPECIALGUARDIANS_H__33DD115A_0CD5_4160_8FCB_1BB192712871__INCLUDED_)
