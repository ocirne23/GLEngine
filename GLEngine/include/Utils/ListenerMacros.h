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

#define DECLARE_LISTENER(NAME, RETTYPE, ...) \
	public:	\
		void NAME##ListenerRegister(void* ownerPtr, std::function<RETTYPE(__VA_ARGS__)> func) \
		{ \
			m_##NAME##Listeners.insert({ ownerPtr, func }); \
		} \
		void NAME##ListenerUnregister(void* ownerPtr) \
		{ \
			m_##NAME##Listeners.erase(ownerPtr); \
		} \
	private: \
		rde::hash_map<void*, std::function<RETTYPE(__VA_ARGS__)>> m_##NAME##Listeners;

#define REGISTER_LISTENER(LISTENEE, LISTENEEFUNC, LISTENER, LISTENERFUNC) \
	{ \
		ListenerUtils::bindFunction(LISTENEEFUNC, LISTENEE)(LISTENER, ListenerUtils::bindFunction(LISTENERFUNC, LISTENER)); \
	}

#define UNREGISTER_LISTENER(LISTENEE, LISTENEEFUNC, LISTENER) \
	{ \
		ListenerUtils::bindFunction(LISTENEEFUNC, LISTENEE)(LISTENER); \
	}