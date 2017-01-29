// Fill out your copyright notice in the Description page of Project Settings.

// #include "TestCamera.h"
// #include "ScriptPluginPrivatePCH.h"
#define _includefile "FirstPerson.h"
#include  "FirstPerson.h" 
#include "TableUtil.h"
#include "UObject/UObjectThreadContext.h"
#include "GeneratedScriptLibraries.inl"

DEFINE_LOG_CATEGORY(LuaLog);

lua_State* UTableUtil::L = nullptr;
TMap<FString, TMap<FString, UProperty*>> UTableUtil::propertyMap;
#ifdef LuaDebug
TMap<FString, int> UTableUtil::countforgc;
#endif
// FLuaGcObj UTableUtil::gcobjs;
void UTableUtil::InitClassMap()
{
	for (TObjectIterator<UClass> uClass; uClass; ++uClass)
	{
		UClass* theClass = *uClass;
		FString className = FString::Printf(TEXT("%s%s"), theClass->GetPrefixCPP(), *theClass->GetName());
		TMap<FString, UProperty*> m;
		for (TFieldIterator<UProperty> PropertyIt(theClass); PropertyIt; ++PropertyIt)
		{
			UProperty* Property = *PropertyIt;
			m.Add(Property->GetName(), Property);
		}
		propertyMap.Add(className ,m);
	}
}

UProperty* UTableUtil::GetPropertyByName(FString classname, FString propertyname)
{
	return propertyMap[classname][propertyname];
}

UProperty* UTableUtil::GetPropertyByName(UClass *Class, FString propertyname)
{
	FString namecpp = FString::Printf(TEXT("%s%s"), Class->GetPrefixCPP(), *Class->GetName());
	return GetPropertyByName(namecpp, propertyname);
}

static void* LuaAlloc(void *Ud, void *Ptr, size_t OldSize, size_t NewSize)
{
	if (NewSize != 0)
	{
		return FMemory::Realloc(Ptr, NewSize);
	}
	else
	{
		FMemory::Free(Ptr);
		return NULL;
	}
}

static int32 LuaPanic(lua_State *L)
{
	UTableUtil::log(FString::Printf(TEXT("PANIC: unprotected error in call to Lua API(%s)"), ANSI_TO_TCHAR(lua_tostring(L, -1))));
	return 0;
}

void UTableUtil::init()
{
	if (L != nullptr)
		return;
	InitClassMap();
	auto l = lua_newstate(LuaAlloc, nullptr);
	if (l) lua_atpanic(l, &LuaPanic);
	luaL_openlibs(l);
	L = l;
	FString gameDir = FPaths::GameDir();
	FString projectname = _includefile;
	projectname.RemoveAt(projectname.Len() - 1, 2);
	FString luaDir = gameDir /TEXT("LuaSource");
	FString mainFilePath = luaDir / TEXT("main.lua");
	if (luaL_dofile(l, TCHAR_TO_ANSI(*mainFilePath)))
	{
		UTableUtil::log(FString(lua_tostring(L, -1)));
	}
	else
	{
		//set table for index exist userdata
		lua_newtable(L);
		lua_newtable(L);
		lua_pushstring(L, "v");
		lua_setfield(L, -2, "__mode");
		lua_setmetatable(L, -2);
		lua_setfield(L, LUA_REGISTRYINDEX, "_existuserdata");

		//set table for need Destroy data
		lua_newtable(L);
		lua_setfield(L, LUA_REGISTRYINDEX, "_needgcdata");

		push(luaDir);
		lua_setfield(L, LUA_GLOBALSINDEX, "_luadir");
		//register all function
		LuaRegisterExportedClasses(L);
		executeFunc("Init", 0, 0);
	}
}

void UTableUtil::shutdown()
{
	if (L != nullptr)
	{
		Call_void("Shutdown");
		lua_close(L);
		L = nullptr;
	}
}

int32 indexFunc(lua_State* L)
{
	lua_getmetatable(L, 1);
	lua_pushvalue(L, 2);
	lua_rawget(L,-2);
	if (lua_isnil(L, -1))
	{
		FString property = FString(lua_tostring(L, 2));
		lua_pop(L, 1);
		FString propertyKey = FString::Printf(TEXT("Get_%s"), *property);
		UTableUtil::push(propertyKey);
		lua_rawget(L, -2);
		if (!lua_isnil(L, -1))
		{
			lua_pushvalue(L, 1);
			lua_pcall(L, 1, 1, 0);
		}
	}
	return 1;
}

int32 newindexFunc(lua_State* L)
{
	if (lua_isuserdata(L, 1))
	{
		lua_getmetatable(L, 1);
		FString property = FString(lua_tostring(L, 2));
		FString propertyKey = FString::Printf(TEXT("Set_%s"), *property);
		UTableUtil::push(propertyKey);
		lua_rawget(L, -2);
		if (!lua_isnil(L, -1))
		{
			lua_pushvalue(L, 1);
			lua_pushvalue(L, 3);
			lua_call(L, 2, 1);
		}
	}
	else if (lua_istable(L, 1))
	{
		lua_rawset(L, -3);
	}
	return 0;
}

int32 cast(lua_State* L)
{
	if (lua_isnil(L, 2))
	{
		UTableUtil::log(FString("cast error, nil"));
		return 1;
	}
	lua_pushstring(L, "classname");
	lua_rawget(L, 1);
	luaL_getmetatable(L, lua_tostring(L, -1));
	lua_setmetatable(L, 2);
	lua_pushvalue(L, 2);
	return 1;
}

int32 gcfunc(lua_State *L)
{
	auto u = (void**)lua_touserdata(L, -1);
	lua_getfield(L, LUA_REGISTRYINDEX, "_needgcdata");
	lua_pushlightuserdata(L, *u);
	lua_rawget(L, -2);
	if ( !lua_isnil(L, -1) )
	{
		lua_pop(L, 1);
		lua_pushlightuserdata(L, *u);
		lua_pushnil(L);
		lua_rawset(L, -3);
		lua_getmetatable(L, 1);
		lua_getfield(L, -1, "Destroy");
		if (lua_iscfunction(L, -1))
		{
			lua_getmetatable(L, 1);
			lua_getfield(L, -1, "classname");
			FString n = lua_tostring(L, -1);
			lua_pop(L, 2);
#ifdef LuaDebug
			UTableUtil::countforgc[n]--;
#endif
			lua_pushvalue(L, 1);
			if (lua_pcall(L, 1, 0, 0))
			{
				UTableUtil::log(FString(lua_tostring(L, -1)));
			}
		}
	}
	return 0;
}

void UTableUtil::initmeta()
{
	lua_pushstring(L, "__index");
	lua_pushcfunction(L, indexFunc);
	lua_rawset(L, -3);
	lua_pushstring(L, "cast");
	lua_pushcfunction(L, cast);
	lua_rawset(L, -3);
	lua_pushstring(L, "__newindex");
	lua_pushcfunction(L, newindexFunc);
	lua_rawset(L, -3);
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, gcfunc);
	lua_rawset(L, -3);
	lua_pushstring(L, "__iscppclass");
	lua_pushboolean(L, true);
	lua_rawset(L, -3);
}

void UTableUtil::addmodule(const char* name)
{
	lua_getglobal(L, name);
	if (lua_istable(L, -1))
	{
		lua_pop(L, 1);
		return;
	}
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, name);
	luaL_newmetatable(L, name);
	initmeta();
	lua_pushstring(L, "classname");
	lua_pushstring(L, name);
	lua_rawset(L, -3);
	lua_rawset(L, -3);
	lua_pop(L, 2);
}

void UTableUtil::openmodule(const char* name)
{
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	lua_pushstring(L, name);
	lua_rawget(L, -2);
}


void UTableUtil::addfunc(const char* name, luafunc f)
{
	lua_pushstring(L, name);
	lua_pushcfunction(L, f);
	lua_rawset(L, -3);
}

void UTableUtil::closemodule()
{
	lua_pop(L, 2);
}

void* UTableUtil::tousertype(const char* classname, int i)
{
	if (lua_isnil(L, i))
		return nullptr;
	else if (lua_istable(L, i))
	{
		lua_pushstring(L,  "_cppinstance_");
		lua_rawget(L, i);
		if (lua_isnil(L, -1))
		{
			lua_pushstring(L, "suck in tousertype");
			lua_error(L);
			return nullptr;
		}
		else
		{
			lua_replace(L, i);
			return tousertype(classname, i);
		}
	}
	else if (lua_isuserdata(L, i))
	{
		auto u = static_cast<void**>(lua_touserdata(L, i));
		return *u;
	}
	else
		return nullptr;
}

int UTableUtil::toint(int i)
{
	return lua_tointeger(L, i);
}

void UTableUtil::setmeta(const char* classname, int index)
{
	luaL_getmetatable(L, classname);
	if (lua_istable(L, -1))
	{
		lua_setmetatable(L, index-1);
	}
	else
	{
		lua_pop(L, 1);
		UTableUtil::addmodule(classname);
		luaL_getmetatable(L, classname);
		lua_setmetatable(L, index-1);
	}
}

void UTableUtil::pushclass(const char* classname, void* p, bool bgcrecord)
{
	if (p == nullptr)
	{
		lua_pushnil(L);
		return;
	}
	if (!existdata(p))
	{
		*(void**)lua_newuserdata(L, sizeof(void *)) = p;

		lua_getfield(L, LUA_REGISTRYINDEX, "_existuserdata");
		lua_pushlightuserdata(L, p);
		lua_pushvalue(L, -3);
		lua_rawset(L, -3);
		lua_pop(L, 1);

		if (bgcrecord)
		{
			lua_getfield(L, LUA_REGISTRYINDEX, "_needgcdata");
			lua_pushlightuserdata(L, p);
			lua_pushboolean(L, true);
			lua_rawset(L, -3);
			lua_pop(L, 1);
#ifdef LuaDebug
			if (countforgc.Contains(classname))  
				countforgc[classname]++;
			else
				countforgc.Add(classname, 1);
#endif
		}
	}
	setmeta(classname, -1);
}

UTableUtil::UTableUtil()
{
	
}

void UTableUtil::loadlib(const luaL_Reg funclist[], const char* classname)
{
	int i = 0;
	UTableUtil::addmodule(classname);
	UTableUtil::openmodule(classname);
	while (true)
	{
		luaL_Reg temp = funclist[i];
		if (temp.name == nullptr)
		{
			break;
		}
		else if (temp.func != nullptr)
		{
			UTableUtil::addfunc(temp.name, temp.func);
		}
		i++;
	}
	UTableUtil::closemodule();
}

void UTableUtil::addutil(const luaL_Reg funclist[], const char* tablename)
{
	lua_newtable(L);
	int i = 0;
	while (true)
	{
		const luaL_Reg &temp = funclist[i];
		if (temp.name == nullptr)
		{
			break;
		}
		else if (temp.func != nullptr)
		{
			lua_pushcfunction(L, temp.func);
			lua_setfield(L, -2, temp.name);
		}
		i++;
	}
	lua_setfield(L, LUA_GLOBALSINDEX, tablename);
}


void UTableUtil::loadEnum(const EnumItem list[], const char* enumname)
{
	lua_newtable(L);
	int i = 0;
	while (true)
	{
		EnumItem temp = list[i];
		if (temp.key == nullptr)
			break;
		else
		{
			lua_pushstring(L, temp.key);
			lua_pushinteger(L, temp.value);
			lua_rawset(L, -3);
		}
		++i;
	}
	lua_setfield(L, LUA_GLOBALSINDEX, enumname);
}

void UTableUtil::executeFunc(FString funcName, int n, int nargs)
{
	lua_getfield(L, LUA_GLOBALSINDEX, TCHAR_TO_ANSI(*funcName));
	if (nargs > 0)
		lua_insert(L, -nargs-1);
	if (lua_pcall(L, nargs, n, 0))
		log(lua_tostring(L, -1));
}

// api for blueprint start:
void UTableUtil::Push_obj(UObject *p)
{
	auto Class = p->StaticClass();
	FString ClassName = FString::Printf(TEXT("%s%s"), Class->GetPrefixCPP(), *Class->GetName());
	pushclass(TCHAR_TO_ANSI(*ClassName), (void*)p, true);
}

void UTableUtil::Push_float(float value)
{
	lua_pushnumber(L, value);
}

void UTableUtil::Push_str(FString value)
{
	push(value);
}

void UTableUtil::Push_bool(bool isTrue)
{
	lua_pushboolean(L, isTrue);
}


void UTableUtil::Call_void(FString funcName, int count_param)
{
	if (count_param == -1)
		count_param = lua_gettop(L);
	executeFunc(funcName, 0, count_param);
}

float UTableUtil::Call_float(FString funcName, int count_param)
{
	if (count_param == -1)
		count_param = lua_gettop(L);
	executeFunc(funcName, 0, count_param);
	auto result = lua_tonumber(L, -1);
	return result;
}

UObject* UTableUtil::Call_obj(FString funcName, int count_param)
{
	if (count_param == -1)
		count_param = lua_gettop(L);
	executeFunc(funcName, 0, count_param);
	auto result = (UObject*)tousertype("", -1);
	return result;
}

FString UTableUtil::Call_str(FString funcName, int count_param)
{
	if (count_param == -1)
		count_param = lua_gettop(L);
	executeFunc(funcName, 0, count_param);
	FString result = ANSI_TO_TCHAR(luaL_checkstring(L, -1));
	return result;
}

bool UTableUtil::Call_bool(FString funcName, int count_param)
{
	if (count_param == -1)
		count_param = lua_gettop(L);
	executeFunc(funcName, 0, count_param);
	bool result = !!lua_toboolean(L, -1);
	return result;
}

bool UTableUtil::existdata(void * p)
{
	lua_getfield(L, LUA_REGISTRYINDEX, "_existuserdata");
	lua_pushlightuserdata(L, p);
	lua_rawget(L, -2);
	if (lua_isnil(L, -1))
	{
		lua_pop(L, 2);
		return false;
	}
	else
	{
		lua_replace(L, -2);
		return true;
	}
}

void UTableUtil::log(FString content)
{
	UE_LOG(LuaLog, Warning, TEXT("[lua error] %s"), *content);
}

void UTableUtil::CtorCpp(UObject* p, FString classpath)
{
	if (L == nullptr)
		init();
	pushclass("UObject", (void*)p, true);
	push(classpath);
	Call_void(FString("CtorCpp"));
}

UObject* UTableUtil::FObjectFinder( UClass* Class, FString PathName )
{
	auto& ThreadContext = FUObjectThreadContext::Get();
	UE_CLOG(!ThreadContext.IsInConstructor, LogUObjectGlobals, Fatal, TEXT("FObjectFinders can't be used outside of constructors to find %s"), *PathName);
	ConstructorHelpers::StripObjectClass(PathName, true);
	int32 PackageDelimPos = INDEX_NONE;
	PathName.FindChar(TCHAR('.'), PackageDelimPos);
	if (PackageDelimPos == INDEX_NONE)
	{
		int32 ObjectNameStart = INDEX_NONE;
		PathName.FindLastChar(TCHAR('/'), ObjectNameStart);
		if (ObjectNameStart != INDEX_NONE)
		{
			const FString ObjectName = PathName.Mid(ObjectNameStart + 1);
			PathName += TCHAR('.');
			PathName += ObjectName;
		}
	}
	Class->GetDefaultObject(); // force the CDO to be created if it hasn't already
	UObject * result = (UObject*)StaticLoadObject(Class, nullptr, *PathName);
	if (result)
	{
		result->AddToRoot();
	}
	return result;
}
