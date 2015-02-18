#pragma once

#include "Core.h"
#include "rde/hash_map.h"
#include <functional>

BEGIN_NAMESPACE(ListenerUtils)

template <typename Object, typename ReturnType, typename... Args>
static std::function<ReturnType(Args...)> bindFunction(ReturnType(Object::*MemPtr)(Args...), Object* obj)
{
	return [=](Args... args) -> ReturnType { return (obj->*MemPtr)(args...); };
}

END_NAMESPACE(ListenerUtils)

#define DECLARE_LISTENER_H(NAME, RETTYPE, ...) \
	public:	\
		void NAME##ListenerRegister(void* ownerPtr, std::function<RETTYPE(__VA_ARGS__)> func); \
		void NAME##ListenerUnregister(void* ownerPtr); \
	private: \
		rde::hash_map<void*, std::function<RETTYPE(__VA_ARGS__)>> m_##NAME##Listeners;

#define DECLARE_LISTENER_CPP(NAME, RETTYPE, ...) \
	void Input::##NAME##ListenerRegister(void* a_ownerPtr, std::function<RETTYPE(__VA_ARGS__)> a_func) \
	{ \
		m_##NAME##Listeners.insert({ a_ownerPtr, a_func }); \
	} \
	void Input::##NAME##ListenerUnregister(void* a_ownerPtr) \
	{ \
		m_##NAME##Listeners.erase(a_ownerPtr); \
	}

#define REGISTER_LISTENER(LISTENEE, LISTENEEFUNC, LISTENER, LISTENERFUNC) \
	{ \
		ListenerUtils::bindFunction(LISTENEEFUNC, LISTENEE)(LISTENER, ListenerUtils::bindFunction(LISTENERFUNC, LISTENER)); \
	}

#define UNREGISTER_LISTENER(LISTENEE, LISTENEEFUNC, LISTENER) \
	{ \
		ListenerUtils::bindFunction(LISTENEEFUNC, LISTENEE)(LISTENER); \
	}