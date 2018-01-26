#pragma once

namespace
{
	template <int... Is>
	struct index {};

	template <int N, int... Is>
	struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};

	template <int... Is>
	struct gen_seq<0, Is...> : index<Is...> {};
}
/*
template<typename F, typename... Args>
constexpr auto MakeDeferredFunction(F&& f, Args&&... args)
-> DeferredFunction<decltype(f(args...)), Args...>
{
	return{ std::forward<F>(f), std::forward<Args>(args)... };
}
*/
template <typename Ret, typename... Ts>
class DeferredFunction
{
private:
	Ret(*f)(Ts...);
	std::tuple<Ts...> args;
public:

	template <typename F, typename... Args>
	DeferredFunction(F&& func, Args&&... args)
		: f(std::forward<F>(func)),
		args(std::forward<Args>(args)...)
	{}

	template <typename F, typename... Args>
	DeferredFunction(DeferredFunction<F, Args...>& copy)
	{
		f = copy.f;
		args = copy.args;
	}

	template <typename... Args, int... Is>
	Ret func(std::tuple<Args...>& tup, index<Is...>)
	{
		return f(std::get<Is>(tup)...);
	}

	template <typename... Args>
	Ret func(std::tuple<Args...>& tup)
	{
		return func(tup, gen_seq<sizeof...(Args)>{});
	}

	Ret operator()()
	{
		return func(args);
	}
};

template<typename F, typename... Args>
static constexpr auto makeDeferredFunction(F&& f, Args&&... args)
-> DeferredFunction<decltype(f(args...)), Args...>
{
	return{ std::forward<F>(f), std::forward<Args>(args)... };
}