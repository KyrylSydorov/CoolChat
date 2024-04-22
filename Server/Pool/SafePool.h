// Kyryl Sydorov, 2024

#pragma once

#include <cassert>
#include <memory>
#include <vector>
#include <xutility>

#include "SharedFromThis.h"

using std::move;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using std::weak_ptr;

template <typename T>
class SafePool : public SharedFromThis<SafePool<T>>
{
	friend class SharedFromThis<SafePool<T>>;

public:
	class Proxy
	{
	private:
		friend class SafePool<T>;
		Proxy(weak_ptr<SafePool<T>> pool, unique_ptr<T> obj = nullptr);

		weak_ptr<SafePool<T>> _pool;
		unique_ptr<T> _object;

	public:
		~Proxy();

		Proxy(Proxy&&) = default;
		Proxy& operator=(Proxy&&) = default;

		T* operator->();
		T& operator*();

		operator bool() const;

		Proxy(const Proxy&) = delete;
		Proxy& operator=(const Proxy&) = delete;
	};

	~SafePool();

	// Delete copying and moving
	SafePool(const SafePool&) = delete;
	SafePool(SafePool&&) = delete;
	SafePool& operator=(const SafePool&) = delete;
	SafePool& operator=(SafePool&&) = delete;

	void push(unique_ptr<T> object);
	Proxy pull();

protected:
	SafePool(const int size);

private:
	vector<unique_ptr<T>> _objects;
};

template<typename T>
SafePool<T>::Proxy::Proxy(weak_ptr<SafePool<T>> pool, unique_ptr<T> object)
	: _pool(pool)
	, _object(move(object))
{
}

template<typename T>
SafePool<T>::Proxy::~Proxy()
{
	if (_object)
	{
		if (!_pool.expired())
		{
			_pool.lock()->push(move(_object));
		}
	}
}

template<typename T>
T* SafePool<T>::Proxy::operator->()
{
	return _object.get();
}

template<typename T>
T& SafePool<T>::Proxy::operator*()
{
	return *_object;
}

template<typename T>
SafePool<T>::Proxy::operator bool() const
{
	return _object != nullptr;
}

template <typename T>
SafePool<T>::SafePool(const int size)
{
	_objects.reserve(size);
	for (int i = 0; i < size; ++i)
	{
		_objects.push_back(unique_ptr<T>(new T()));
	}
}

template <typename T>
SafePool<T>::~SafePool()
{
}

template <typename T>
void SafePool<T>::push(unique_ptr<T> object)
{
	assert(object != nullptr);
	if (_objects.size() < _objects.capacity())
	{
		_objects.push_back(move(object));
	}
}

template <typename T>
typename SafePool<T>::Proxy SafePool<T>::pull()
{
	if (_objects.empty())
	{
		return { this->asWeak(), nullptr };
	}

	unique_ptr<T> object = move(_objects.back());
	_objects.pop_back();

	return { this->asWeak(), move(object) };
}