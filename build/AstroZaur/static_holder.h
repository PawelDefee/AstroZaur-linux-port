/*
    Copyright (C) 2013 Pawel Defee <pawel.defee@iki.fi>
*/

#ifndef _STATIC_HOLDER_H_
#define _STATIC_HOLDER_H_

#include "program.h"
#include "division.h"
#include "lua.h"

using namespace std;

class CStaticHolder {
private:
    static CLua*		lua_;
    static CDivision*     division_;
    static CProgram*	program_;

public:
    CStaticHolder();
    ~CStaticHolder();

    void Set(CLua* aLua);
    void Set(CDivision* aDivision);
    void Set(CProgram* aProgram);

    static CLua& GetLua();
    static CDivision& GetDivision();
    static CProgram& GetProgram();
};

#endif // _STATIC_HOLDER_H_
