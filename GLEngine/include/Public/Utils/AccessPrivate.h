template<typename Tag, typename Tag::type M>
struct AccessPrivate {
	friend typename Tag::type get(Tag) {
		return M;
	}
};
template<typename Class, typename Type>
struct AccessPrivateTag
{
	typedef Type Class::*type;
	friend type get(AccessPrivateTag);
};

#define ACCESS_PRIVATE(Class, Type, Name) \
template struct AccessPrivate<AccessPrivateTag<Class, Type>, &Class::Name>; \
Type& get##Name(Class& owner) \
{ \
	return owner.*get(AccessPrivateTag<Class, Type>()); \
}
