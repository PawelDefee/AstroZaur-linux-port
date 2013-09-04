/*
    Copyright (C) 2013 Pawel Defee <pawel.defee@iki.fi>
*/

#include "static_holder.h"
#include "utils.h"

CLua* CStaticHolder::lua_ = NULL;
CDivision* CStaticHolder::division_ = NULL;
CProgram* CStaticHolder::program_ = NULL;

CStaticHolder::CStaticHolder() {
}

CStaticHolder::~CStaticHolder() {
}

void  CStaticHolder::Set(CLua* aLua) {
	lua_ = aLua;
}

void  CStaticHolder::Set(CDivision* aDivision) {
	division_ = aDivision;
}

void  CStaticHolder::Set(CProgram* aProgram) {
	program_ = aProgram;
}

CLua& CStaticHolder::GetLua() {
	return *lua_;
}

CDivision& CStaticHolder::GetDivision() {
	return *division_;
}

CProgram& CStaticHolder::GetProgram() {
	return *program_;
}
