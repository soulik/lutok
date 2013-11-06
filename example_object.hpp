#ifndef LUA_EXAMPLE_OBJECT_H
#define LUA_EXAMPLE_OBJECT_H

namespace Example {

	class ExampleObject {
		public:
			int x;
	};

	class LuaExampleObject : public lutok::LObject<LuaExampleObject, ExampleObject *> {
	public:
		LOBJECT_DEFINE_CLASS(LuaExampleObject, ExampleObject *, "ExampleObject type") {
			LOBJECT_ADD_OPERATOR(LuaExampleObject, add);
			
			//property consists of two methods: getter & setter
			LOBJECT_ADD_PROPERTY(LuaExampleObject, ExampleObject *, "x", getX, setX);

			LOBJECT_ADD_METHOD(LuaExampleObject, "doSomething", doSomething);
			LOBJECT_ADD_METHOD(LuaExampleObject, "doSomething2", doSomething2);
		}

		void destructor(lutok::state & s, ExampleObject* object){
			delete object;
		}

		int LOBJECT_OPERATOR(add, ExampleObject* objectA){
			Lua_Box2D_Vector2 * wrapper = LOBJECT_INSTANCE(Lua_Box2D_Vector2);

			ExampleObject * objectT = new ExampleObject(*objectA);
			ExampleObject * objectB = _object->check(2);
			(*objectT) += (*objectB);
			wrapper->push(objectT);
			return 1;
		}

		int inline LOBJECT_METHOD(doSomething, ExampleObject * object){
			state.push_string("Something has to be done here!");
			return 1;
		}

		int LOBJECT_METHOD(doSomething2, ExampleObject * object);

		int inline LOBJECT_METHOD(getX, ExampleObject * object){
			state.push_integer(object->x);
			return 1;
		}
		
		int inline LOBJECT_METHOD(setX, ExampleObject * object){
			object->x = static_cast<int>(state.to_number(1));
			return 0;
		}

	};
}