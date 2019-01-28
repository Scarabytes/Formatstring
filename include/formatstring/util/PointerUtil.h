/** @file formatstring/util/PointerUtil.h
 *
 * Useful typedefs and helper methods for smart pointer.
 */

#ifndef FORMATSTRING_POINTERUTIL_H
#define FORMATSTRING_POINTERUTIL_H

#include <memory>


namespace fs
{

// typedefs of pointer type templates P, U, S, W
// 		...saves you a lot of typing :)

/**
 * P<T> defines a raw pointer.
 * A raw pointer _should never_ own the object it points to. It should only be
 * used to represent a nullable, non-owning reference.
 */
template <typename T>
using P = T*;

/**
 * U<T> defines a unique smart pointer.
 * A unique pointer _owns_ the object it points to.
 */
template <typename T>
using U = std::unique_ptr<T>;

/**
 * S<T> defines a shared smart pointer.
 * Multiple shared pointers _collectively own_ the object they point to.
 */
template <typename T>
using S = std::shared_ptr<T>;

/**
 * W<T> defines a weak smart pointer.
 * A weak pointer _does not_ own the object it points to, the corresponding
 * shared pointers do.
 */
template <typename T>
using W = std::weak_ptr<T>;

//------------------------------------------------------------------------------
// Creator utility methods

/**
 * Creates a unique pointer together with a new object of the given type T.
 * @tparam T	The type of the object / the pointer.
 * @param args	Arguments for the object's ctor. May be empty.
 * @return 		A unique pointer wrapping a newly created object.
 */
template <typename T, typename... Args>
inline U<T> mkU(Args&& ... args)
{
#if __cplusplus >= 201402l
	return std::make_unique<T>(std::forward<Args>(args)...);
#else
	return U<T>(new T(std::forward<Args>(args)...));
#endif
}

/**
 * Creates a shared pointer together with a new object of the given type T.
 * @tparam T	The type of the object / the pointer.
 * @param args	Arguments for the object's ctor. May be empty.
 * @return 		A shared pointer wrapping a newly created object.
 */
template <typename T, typename... Args>
inline S<T> mkS(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

//------------------------------------------------------------------------------
// Pointer casts

/**
 * Dynamically casts a shared pointer to a different type. If the cast fails,
 * the returned pointer will be empty (pointing to nullptr).
 * @tparam to	The type to cast the pointer to.
 * @tparam from The original type of the pointer. Can usually be left out to be
 * 				guessed by the compiler.
 * @param ptr 	The pointer to cast.
 * @return 		A new shared pointer that has been cast to the given type.
 */
template <typename to, typename from>
inline S<to> ptr_cast(const S<from>& ptr)
{
	return std::dynamic_pointer_cast<to>(ptr);
}

/**
 * Dynamically moves and casts a unique pointer to a different type. If the cast
 * succeeds, the original pointer is invalidated (release()'d) as ownership of
 * the object is transferred to the newly created pointer. If the cast fails,
 * the returned pointer will be empty (pointing to nullptr) and the original
 * pointer keeps ownership.
 * @tparam to	The type to cast the pointer to.
 * @tparam from The original type of the pointer. Can usually be left out to be
 * 				guessed by the compiler.
 * @param ptr 	The pointer to cast.
 * @return 		A new shared pointer that has been cast to the given type.
 */
template <typename to, typename from>
inline U<to> ptr_cast(U<from>& ptr)
{
	if (!ptr)
		return nullptr;
	U<to> result(dynamic_cast<to*>(ptr.get()));
	if (result)
		ptr.release();
	return result;
}

/**
 * Dynamically moves and casts an rvalue unique pointer to a different type. As
 * the original pointer is an rvalue, it is always lost after the cast. If the
 * cast fails, the returned pointer will be empty (pointing to nullptr) and the
 * object is deleted by the unique ptr.
 * @tparam to	The type to cast the pointer to.
 * @tparam from The original type of the pointer. Can usually be left out to be
 * 				guessed by the compiler.
 * @param ptr 	The rvalue pointer to cast.
 * @return 		A new shared pointer that has been cast to the given type.
 */
template <typename to, typename from>
inline U<to> ptr_cast(U<from>&& ptr)
{
	return ptr_cast<to>(ptr);
}

} // namespace pdfrw

#endif //FORMATSTRING_POINTERUTIL_H
