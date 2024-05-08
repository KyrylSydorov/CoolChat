// Kyryl Sydorov, 2024

#pragma once

#include <cassert>
#include <memory>
#include <mutex>
#include <vector>
#include <xutility>

using std::move;
using std::unique_ptr;
using std::vector;

/**
 * Thread-safe implementation of a SimplestPool
 */
template <typename T>
class SimplestPoolTS
{
public:
	class Proxy
	{
	private:
		friend class SimplestPoolTS<T>;
		Proxy(SimplestPoolTS<T>& pool, unique_ptr<T> obj = nullptr);

		SimplestPoolTS<T>& _pool;
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

	SimplestPoolTS(const int size);
	~SimplestPoolTS();

	// Delete copying and moving
	SimplestPoolTS(const SimplestPoolTS&) = delete;
	SimplestPoolTS(SimplestPoolTS&&) = delete;
	SimplestPoolTS& operator=(const SimplestPoolTS&) = delete;
	SimplestPoolTS& operator=(SimplestPoolTS&&) = delete;

	void push(unique_ptr<T> object);
	Proxy pull();

private:
	vector<unique_ptr<T>> _objects;
	std::mutex _mutex;
};

template<typename T>
SimplestPoolTS<T>::Proxy::Proxy(SimplestPoolTS<T>& pool, unique_ptr<T> object)
	: _pool(pool)
	, _object(move(object))
{
}

template<typename T>
SimplestPoolTS<T>::Proxy::~Proxy()
{
	if (_object)
	{
		_pool.push(move(_object));
	}
}

template<typename T>
T* SimplestPoolTS<T>::Proxy::operator->()
{
	return _object.get();
}

template<typename T>
T& SimplestPoolTS<T>::Proxy::operator*()
{
	return *_object;
}

template<typename T>
SimplestPoolTS<T>::Proxy::operator bool() const
{
	return _object != nullptr;
}

template <typename T>
SimplestPoolTS<T>::SimplestPoolTS(const int size)
{
	_objects.reserve(size);
	for (int i = 0; i < size; ++i)
	{
		_objects.push_back(unique_ptr<T>(new T()));
	}
}

template <typename T>
SimplestPoolTS<T>::~SimplestPoolTS()
{
}

template <typename T>
void SimplestPoolTS<T>::push(unique_ptr<T> object)
{
	assert(object != nullptr);
	std::lock_guard lock(_mutex);
	if (_objects.size() < _objects.capacity())
	{
		_objects.push_back(move(object));
	}
}

template <typename T>
typename SimplestPoolTS<T>::Proxy SimplestPoolTS<T>::pull()
{
	std::lock_guard lock(_mutex);
	if (_objects.empty())
	{
		return { *this, nullptr };
	}

	unique_ptr<T> object = move(_objects.back());
	_objects.pop_back();

	return { *this, move(object) };
}