#ifndef __CVAR_H
#define __CVAR_H

#pragma warning(disable : 4172)

#define CMAP_MAX_PER_TYPE 512

class CVarError {};
class UninitializedCVar : public CVarError {};
class CVarOutOfBounds : public CVarError {};

class CVar;

enum CType {
	CType_int = 0,
	CType_float = 1,
	CType_vec2 = 2,
};

enum CMutable {
	IMMUTABLE = 0,
	MUTABLE = 1,
};

class CMap {
	static CMap* m_instance;

	int* integers;
	bool* integers_mask;
	int* integers_ref;

	float* floats;
	bool* floats_mask;
	int* floats_ref;

	vec2* vec2s;
	bool* vec2s_mask;
	int* vec2s_ref;

	boost::unordered_map<std::string, int> db;
	boost::unordered_map<std::string, CType> db_type;

	public:
	CMap();
	~CMap();
	static CMap* instance() {
		if(m_instance == 0)
			m_instance = new CMap();
		return m_instance;
	}
	void BindVar(CVar* var);
	void UnbindVar(CVar* var);
	int& GetInt(int index);
	float& GetFloat(int index);
	vec2& GetVec2(int index);
	bool DoesExist(char* name);
	CType GetType(char* name);
	std::list<std::string> GetVars();
	CType GetTypeFromStr(char* str);
	int GetRef(CVar* var);
};

class CVar {
	public:
	CType type;
	char* name;
	int index;
};

class CInt : public CVar {
	bool isMutable;
	bool noScope;

	public:
	CInt(char* name_n, bool mutable_=true, bool noscope_=false) {
		name = name_n;
		type = CType_int;
		index = -1; // uninitialized
		isMutable = mutable_;
		noScope = noscope_;
		CMap::instance()->BindVar(this);
	}

	~CInt() {
		if(!noScope) CMap::instance()->UnbindVar(this);
	}
	
	int& CInt::operator*() {
		if(index == -1) throw UninitializedCVar();
		if(isMutable)
			return CMap::instance()->GetInt(index);
		else {
			int x = CMap::instance()->GetInt(index);
			return x;
		}
	}
};

class CFloat : public CVar {
	bool isMutable;
	bool noScope;

	public:
	CFloat(char* name_n, bool mutable_=true, bool noscope_=false) {
		name = name_n;
		type = CType_float;
		index = -1; // uninitialized
		isMutable = mutable_;
		noScope = noscope_;
		CMap::instance()->BindVar(this);
	}

	~CFloat() {
		if(!noScope) CMap::instance()->UnbindVar(this);
	}
	
	float& CFloat::operator*() {
		if(index == -1) throw UninitializedCVar();
		if(isMutable)
			return CMap::instance()->GetFloat(index);
		else {
			float x = CMap::instance()->GetFloat(index);
			return x;
		}
	}
};

class CVec2 : public CVar {
	bool isMutable;
	bool noScope;

	public:
	CVec2(char* name_n, bool mutable_=true, bool noscope_=false) {
		name = name_n;
		type = CType_vec2;
		index = -1; // uninitialized
		isMutable = mutable_;
		noScope = noscope_;
		CMap::instance()->BindVar(this);
	}

	~CVec2() {
		if(!noScope) CMap::instance()->UnbindVar(this);
	}
	
	vec2& CVec2::operator*() {
		if(index == -1) throw UninitializedCVar();
		if(isMutable)
			return CMap::instance()->GetVec2(index);
		else {
			vec2 x = CMap::instance()->GetVec2(index);
			return x;
		}
	}
};


#endif