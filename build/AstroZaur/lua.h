/*
    AstroZaur - Universal astrologic processor
    Copyright (C) 2006 Belousov Oleg <belousov.oleg@gmail.com>
    http://www.strijar.ru/astrozaur

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _LUA_H_
#define _LUA_H_

extern "C" {
    #include <lua.h>
    #include <lualib.h>
}

class CAstro;

class CLua {
private:
    lua_State		*iL;
    
public:
    CLua();
    ~CLua();

    void Start();
    void Stop();
    
    bool Load(char* name);
    int Call(int arg);
    void Exec(char* cmd);
    
    // High level

    void NewTable(char* name);
    void NewTable(int index);
    void EndTable();
    
    void SetField(const char* name, char* str);
    void SetField(const char* name, double data);
    void SetField(const char* name, bool data);
    void SetField(const char* name, CAstro* data);

    void SetField(int index, char* str);

    bool GetNumber(double *res);
    
    void PrintType(int index);
    void PrintStack(const char* info = NULL);
    
    void InitMeta(CAstro *obj);
    void ClearTable();
    
    void Method(const char* name, lua_CFunction func, CAstro *obj);
    CAstro* GetObj();
    
    // Low level
    
    void NewTable();
    void SetTable(int index);
    void GetTable(int index);
    void RawSet(int index);
    void RawGetI(int index, int n);
    
    void SetGlobal(char* name);
    void GetGlobal(char* name);
    
    void PushString(const char* str);
    void PushNumber(double data);
    void PushValue(int data);
    void PushNil();
    void PushFunc(void *data);
    void PushBool(bool data);

    bool Next(int index);
    
    void NewMetaTable(char* name);
    
    void Pop(int index);
    void Remove(int index);
    int GetTop();
    
    const char* ToString(int index);
    const double ToNumber(int index);
    const bool ToBool(int index);
    const void* ToUser(int index);
    
    int Type(int index);
    
    bool LoadString(char *str);
    
    int Ref();
    void Unref(int ref);
    lua_State* State();
};

class CLuaChunk {
private:
    int		iRef;
    char	*iChunk;
    
public:
    CLuaChunk();
    ~CLuaChunk();
    
    bool Set(char *text);
    void Push();
};

#endif
