// Kyryl Sydorov, 2024

#pragma once

#include <cassert>
#include <memory>
#include <vector>
#include <xutility>
#include <type_traits>

using std::move;
using std::shared_ptr;
using std::weak_ptr;

template <typename T>
class SharedFromThis
{
private:
	weak_ptr<T> _weakThis;

public:
	shared_ptr<T> asShared();
	weak_ptr<T> asWeak();

	template <typename... Args>
	static shared_ptr<T> create(Args&&... args);

	~SharedFromThis() = default;

	// Delete copying and moving
	SharedFromThis(const SharedFromThis&) = delete;
	SharedFromThis(SharedFromThis&&) = delete;
	SharedFromThis& operator=(const SharedFromThis&) = delete;
	SharedFromThis& operator=(SharedFromThis&&) = delete;

protected:
	// Intentionally hide constructor, so that only create() can be used
	SharedFromThis() = default;
};

template <typename T>
template <typename... Args>
shared_ptr<T> SharedFromThis<T>::create(Args&&... args)
{
	shared_ptr<T> result = shared_ptr<T>(new T(args...));
	result->_weakThis = result;
	return result;
}

template <typename T>
shared_ptr<T> SharedFromThis<T>::asShared()
{
	return _weakThis.lock();
}

template <typename T>
weak_ptr<T> SharedFromThis<T>::asWeak()
{
	return _weakThis;
}