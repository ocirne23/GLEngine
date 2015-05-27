#pragma once

#include "Core.h"
#include "3rdparty/rde/hash_map.h"
#include <functional>

BEGIN_NAMESPACE(ListenerUtils)

template <typename Object, typename ReturnType, typename... Args>
static std::function<ReturnType(Args...)> bindFunction(ReturnType(Object::*MemPtr)(Args...), Object* obj)
{
	return [=](Args... args) -> ReturnType { return (obj->*MemPtr)(args...); };
}

END_NAMESPACE(ListenerUtils)

#define DECLARE_LISTENER(NAME, ...) _DECLARE_LISTENER_IMPL(NAME, void, __VA_ARGS__)

#define DECLARE_LISTENER_CONSUME(NAME, ...) _DECLARE_LISTENER_IMPL(NAME, bool, __VA_ARGS__)

#define CALL_LISTENERS(NAME, ...) \
	for (auto& listener : m_##NAME##Listeners) \
		listener.second(__VA_ARGS__);

#define CALL_LISTENERS_CONSUME(NAME, ...) \
	for (auto& listener : m_##NAME##Listeners) \
		if (listener.second(__VA_ARGS__)) \
			break;

#define _DECLARE_LISTENER_IMPL(NAME, RETTYPE, ...) \
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