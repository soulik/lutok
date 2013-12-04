#if !defined(LUTOK_LOBJECT_HPP)
#define LUTOK_LOBJECT_HPP

#include <cassert>
#include <string>
#include <utility>
#include <map>

#include <lua/lua.hpp>
#include <string>
#include <tuple>
#include <type_traits>
#include <mutex>

#define LOBJECT_ADD_PROPERTY(CLASSNAME, TYPENAME, LUANAME, GETTER, SETTER) this->properties[(LUANAME)] = LObject<CLASSNAME, TYPENAME>::PropertyPair(&CLASSNAME::GETTER, &CLASSNAME::SETTER)
#define LOBJECT_ADD_METHOD(CLASSNAME, LUANAME, METHOD) this->methods[(LUANAME)] = &CLASSNAME::METHOD
#define LOBJECT_ADD_OPERATOR(CLASSNAME, LUAOPERATORNAME) this->methods["operator_"#LUAOPERATORNAME] = &CLASSNAME::operator_##LUAOPERATORNAME

#define LOBJECT_DEFINE_CLASS(CLASSNAME, TYPENAME, LUANAME) CLASSNAME(lutok::state & state) : LObject<CLASSNAME, TYPENAME>::LObject( state, (LUANAME))
#define LOBJECT_INSTANCE(CLASSNAME) (CLASSNAME::getInstance(state))
#define LOBJECT_METHOD(METHODNAME, TYPEDEF) METHODNAME(lutok::state & state, TYPEDEF)
#define LOBJECT_OPERATOR(OPERATORNAME, TYPEDEF) operator_##OPERATORNAME(lutok::state & state, TYPEDEF)

#define thread_local __declspec(thread)

namespace lutok {

template <class C>
class LSingleton {
public:
	virtual ~LSingleton() {};
	static inline C & getInstance(lutok::state & state);
private:
	static std::unique_ptr<C> m_instance;
	static std::once_flag m_onceFlag;
	LSingleton& operator=(const C& rhs);
protected:
	LSingleton(void) {};
	LSingleton(const C& src) {};
};

/*
class LSingleton {
public:
	template <class C> static inline C & getInstance(lutok::state & state);
	virtual ~LSingleton() {};
private:
	static std::unique_ptr<LSingleton> m_instance;
	static std::once_flag m_onceFlag;

	LSingleton& operator=(const LSingleton& rhs);
protected:
	LSingleton(void) {};
	LSingleton(const LSingleton& src) {};
};
*/


template <class C, typename T>
class LObject: public LSingleton<C> {
protected:
	lutok::state state;
public:
	typedef std::weak_ptr<C> classWeakPtr;
	typedef std::shared_ptr<C> classSharedPtr;

	typedef std::weak_ptr<T> typeWeakPtr;
	typedef std::shared_ptr<T> typeSharedPtr;

	typedef int (C::*Method) (lutok::state &, T);
	typedef std::tuple< C *, T, bool> LObjectTuple;
	typedef struct std::pair< Method, Method > PropertyPair;

    typedef std::map< std::string, PropertyPair > PropertyType;
	typedef std::map< std::string, Method > FunctionType;

	typedef std::map< int, const PropertyPair *> PropertyCacheType;
	typedef std::map< int, Method > MethodCacheType;

	std::string className;

	int null_method(lutok::state &, T){
		return 0;
	}

	~LObject(){

	}

protected:
	PropertyType properties;
	FunctionType methods;

	LObject(lutok::state & state, const std::string & className){
		this->state = state;
		this->className = className;
	}

private:
	PropertyCacheType PropertyCache;
	MethodCacheType MethodCache;

	LObjectTuple * getObjPair(T obj, bool managed){
		return new LObjectTuple(reinterpret_cast<C*>(this), obj, managed);
	}
public:

/*
  @ check
  Arguments:
    * L - Lua State
    * narg - Position to check

  Description:
    Retrieves a wrapped class from the arguments passed to the func, specified by narg (position).
    This func will raise an exception if the argument is not of the correct type.
*/
    T check(int narg)
	{
		LObjectTuple ** obj = state.check_userdata<LObjectTuple *>(narg, className);
		if ( obj 
			//&& (std::is_class<std::get<1>(**obj)>::value == true)
			//&& (std::is_class<(*obj)->second>::value == true)
		){
			return std::get<1>(**obj);		// pointer to T object
		}else
			return nullptr; // lightcheck returns nullptr if not found.
	}

/*
  @ Register
  Arguments:
    * L - Lua State
    * namespac - Namespace to load into

  Description:
    Registers your class with Lua.  Leave namespac "" if you want to load it into the global space.
*/

	void refresh_methods(int metatable = 0){
		int i=0, index=0;
		for (PropertyType::const_iterator							// Register some properties in it
			iter = properties.begin(); iter != properties.end(); iter++) {

			state.push_string((*iter).first);
			state.push_integer(i);
			state.set_table(metatable);
			LObject::PropertyCache[i] = &((*iter).second);
			i++;
		}

		i=0;
		for (FunctionType::const_iterator							// Register some functions in it
			iter = methods.begin(); iter != methods.end(); iter++) {
			index = i | ( 1 << 8 );
			state.push_string((*iter).first);
			state.push_integer(index);
			state.set_table(metatable);
			LObject::MethodCache[i] = (*iter).second;
			i++;
		}
	}

    // REGISTER CLASS AS A GLOBAL TABLE 
    void Register(const std::string & namespac = "", bool create_constructor = false) {

		if (create_constructor){
			if ( !namespac.empty() ){
				state.get_global(namespac);
				if( state.is_nil() ) // Create namespace if not present
				{
					state.new_table();
					state.push_value();
					state.set_global(namespac);
				}
				state.push_cxx_function(&constructor);
				state.set_field(-2, className);
				state.pop(1);
			} else {
				state.push_cxx_function(&constructor);
				state.set_global(className);
			}
		}
		
		state.new_metatable(className);
		int             metatable = state.get_top();
		
		state.push_literal("__gc");
		state.push_cxx_function(&gc_obj);
		state.set_table(metatable);
		
		state.push_literal("__tostring");
		state.push_cxx_function(&to_string);
		state.set_table(metatable);

		state.push_literal("__index");
		state.push_cxx_function(&property_getter);
		state.set_table(metatable);

		state.push_literal("__newindex");
		state.push_cxx_function(&property_setter);
		state.set_table(metatable);

		state.push_literal("__add");
		state.push_cxx_function(&operator_add);
		state.set_table(metatable);
		state.push_literal("__sub");
		state.push_cxx_function(&operator_sub);
		state.set_table(metatable);
		state.push_literal("__mul");
		state.push_cxx_function(&operator_mul);
		state.set_table(metatable);
		state.push_literal("__div");
		state.push_cxx_function(&operator_div);
		state.set_table(metatable);
		state.push_literal("__mod");
		state.push_cxx_function(&operator_mod);
		state.set_table(metatable);
		state.push_literal("__pow");
		state.push_cxx_function(&operator_pow);
		state.set_table(metatable);
		state.push_literal("__unm");
		state.push_cxx_function(&operator_unm);
		state.set_table(metatable);
		state.push_literal("__concat");
		state.push_cxx_function(&operator_concat);
		state.set_table(metatable);
		state.push_literal("__len");
		state.push_cxx_function(&operator_len);
		state.set_table(metatable);
		state.push_literal("__eq");
		state.push_cxx_function(&operator_eq);
		state.set_table(metatable);
		state.push_literal("__lt");
		state.push_cxx_function(&operator_lt);
		state.set_table(metatable);
		state.push_literal("__le");
		state.push_cxx_function(&operator_le);
		state.set_table(metatable);
		state.push_literal("__call");
		state.push_cxx_function(&operator_call);
		state.set_table(metatable);

		refresh_methods(metatable);
	}


/*
  @ constructor (internal)
  Arguments:
    * L - Lua State
*/
    static int constructor(lutok::state & s){
		return 0;
    }

	void destructor(lutok::state & s, T obj){
    }

/*
  @ createNew
  Arguments:
    * L - Lua State
	T*	- Instance to push

  Description:
    Loads an instance of the class into the Lua stack, and provides you a pointer so you can modify it.
*/
    void push(T obj, bool managed = true){
		int top = state.get_top();
		if (obj){
			top = state.get_top(); //0
			LObjectTuple ** a = state.new_userdata<LObjectTuple *>(); // Create userdata
			top = state.get_top(); //1
			*a = getObjPair(obj, managed);
		
			top = state.get_top(); //1
			state.get_metatable(className);
			top = state.get_top(); //2

			const char * tt = state.typeName(-1);
			if (state.is_nil()){
				state.pop(1);
				top = state.get_top(); //1
				Register();
				top = state.get_top(); //2
			}
		
			state.set_metatable();
			top = state.get_top(); //1
		}
    }

/*
  @ property_getter (internal)
  Arguments:
    * L - Lua State
*/
    static int property_getter(lutok::state & s){
		s.get_metatable(1);
		s.push_value(2);
		s.raw_get();
		
		if (s.is_number()) { // Check if we got a valid index
			
			int _index = s.to_integer();

			LObjectTuple ** obj = s.to_userdata<LObjectTuple *>(1);
			C * thisobj = std::get<0>(**obj);
			s.push_value(3);
			
			if( _index & ( 1 << 8 ) ){ // A func
				s.push_integer(_index ^ ( 1 << 8 )); // Push the right func index
				s.push_lightuserdata(obj);
				s.push_cxx_closure(function_dispatch, 2);
				return 1; // Return a func
			}
			
			s.pop(2);			// Pop metatable and _index
			s.remove(1);		// Remove userdata
			s.remove(1);		// Remove [key]
			
			const PropertyPair * fpairs = thisobj->PropertyCache[_index];
			assert(std::get<1>(**obj));
			return (thisobj->*(fpairs->first)) (s, std::get<1>(**obj));
		}
		
		return 1;
    }

/*
  @ property_setter (internal)
  Arguments:
    * L - Lua State
*/
    static int property_setter(lutok::state & s){
		s.get_metatable(1);		// Look up the index from name
		s.push_value(2);
		s.raw_get();
		
		if ( s.is_number() ) // Check if we got a valid index
		{
			
			int _index = s.to_integer();
			
			LObjectTuple ** obj = s.to_userdata<LObjectTuple *>(1);
			C * thisobj = std::get<0>(**obj);

			if( !obj || !*obj ){
				s.error("Internal error, no object given!");
				return 0;
			}
			
			if( _index >> 8 ){ // Try to set a func
				char c[128];
				sprintf( c , "Trying to set the method [%s] of class [%s]" , s.to_string(2).c_str() , thisobj->className );
				s.error( c );
				return 0;
			}
			
			s.pop(2);			// Pop metatable and _index
			s.remove(1);		// Remove userdata
			s.remove(1);		// Remove [key]

			const PropertyPair * fpairs = thisobj->PropertyCache[_index];
			assert(std::get<1>(**obj));
			return (thisobj->*(fpairs->second)) (s, std::get<1>(**obj));
		}
		
		return 0;
    }

/*
  @ operator_ (internal)
  Arguments:
    * L - Lua State
*/
	static int operator_global(lutok::state & s, const char * name){
		s.get_metatable(1);
		s.push_string(name);
		s.raw_get();
		if (s.is_number()) {
			int _index = s.to_integer();
			LObjectTuple ** obj = s.to_userdata<LObjectTuple *>(1);
			C * thisobj = std::get<0>(**obj);
			if( !obj || !*obj ){
				s.error("Internal error, no object given!");
				return 0;
			}
			if( _index >> 8 ){
				s.pop(2);
				Method method = thisobj->MethodCache[_index ^ ( 1 << 8 )];
				if (method){
					assert(std::get<1>(**obj));
					return (thisobj->*(method)) (s, std::get<1>(**obj));
				}else{
					char c[128];
					sprintf( c , "Trying to use unset operator [%s] of class [%s]" , name, thisobj->className );
					s.error( c );
					return 0;
				}
			}else{
				char c[128];
				sprintf( c , "Trying to use unset operator [%s] of class [%s]" , name, thisobj->className );
				s.error( c );
				return 0;
			}
		}
		return 0;
	}

	#define LUTOK_OPERATOR_CALLER_DEFINITION(OPERATORNAME) \
	static int OPERATORNAME(lutok::state & s){\
		return operator_global(s, # OPERATORNAME);\
	}

	LUTOK_OPERATOR_CALLER_DEFINITION(operator_add)
	LUTOK_OPERATOR_CALLER_DEFINITION(operator_sub)
	LUTOK_OPERATOR_CALLER_DEFINITION(operator_mul)
	LUTOK_OPERATOR_CALLER_DEFINITION(operator_div)
	LUTOK_OPERATOR_CALLER_DEFINITION(operator_mod)
	LUTOK_OPERATOR_CALLER_DEFINITION(operator_pow)
	LUTOK_OPERATOR_CALLER_DEFINITION(operator_unm)
	LUTOK_OPERATOR_CALLER_DEFINITION(operator_concat)
	LUTOK_OPERATOR_CALLER_DEFINITION(operator_len)
	LUTOK_OPERATOR_CALLER_DEFINITION(operator_eq)
	LUTOK_OPERATOR_CALLER_DEFINITION(operator_lt)
	LUTOK_OPERATOR_CALLER_DEFINITION(operator_le)
	LUTOK_OPERATOR_CALLER_DEFINITION(operator_call)

/*
  @ function_dispatch (internal)
  Arguments:
    * L - Lua State
*/
    static int function_dispatch(lutok::state & s) {
		int i = s.to_integer(s.upvalue_index(1));
		LObjectTuple ** obj = s.to_userdata<LObjectTuple *>(s.upvalue_index(2));
		C * thisobj = std::get<0>(**obj);
		assert(std::get<1>(**obj));
		return (thisobj->*(thisobj->MethodCache[i])) (s, std::get<1>(**obj));
    }

/*
  @ gc_obj (internal)
  Arguments:
    * L - Lua State
*/
    static int gc_obj(lutok::state & s)	{
		LObjectTuple ** obj = s.to_userdata<LObjectTuple *>();
		
		if( obj && *obj){
			C * thisobj = std::get<0>(**obj);
			//managed
			if ( std::get<2>(**obj) ){
				assert(std::get<1>(**obj));
				thisobj->destructor(s, std::get<1>(**obj));
			}
			delete(*obj);
		}
		return 0;
    }
	
	static int to_string(lutok::state & s) {
		LObjectTuple ** obj = s.to_userdata<LObjectTuple *>();
		C * thisobj = std::get<0>(**obj);
		
		if ( obj ) {
			char c[128];
			sprintf(c,"%s (%p)", thisobj->className.c_str(), (void*)std::get<1>(**obj));
			s.push_string(c);
		} else {
			s.push_string("Empty object");
		}
		
		return 1;
	}

};
/*
template <class C> C & LSingleton::getInstance(lutok::state & state){
	assert((std::is_base_of<LSingleton, C>::value == true));

	std::call_once(m_onceFlag,
		[& state] {
			m_instance.reset(new C(state));
	});
	return dynamic_cast<C &>(*m_instance.get());
}
*/

template <class C> std::unique_ptr<C> LSingleton<C>::m_instance;
template <class C> std::once_flag LSingleton<C>::m_onceFlag;

template <class C> C & LSingleton<C>::getInstance(lutok::state & state){
	assert((std::is_base_of<LSingleton<C>, C>::value == true));

	std::call_once(m_onceFlag,
		[& state] {
			m_instance.reset(new C(state));
	});
	return *m_instance.get();
}

}
#endif