#include <bit>
#include <algorithm>
#include <stdexcept>

#pragma once

template <typename T>
class MyVector
{
public:
	MyVector(size_t capacity) {
		_capacity = capacity;
		_count = 0;
		_first = new T[capacity];
	}

	void Add(const T& item)
	{
		EnsureCapacity(_count);
		_first[_count++] = item;
	}


	void RemoveAt(size_t index)
	{
		if (index >= _count)
			throw std::out_of_range("Index out of range");

		std::move(_first + index + 1,
			_first + _count,
			_first + index);

		_count--;
	}

	void Clear()
	{
		_count = 0;
	}

	T& operator[](size_t index) {
		if (index >= _count) {
			throw std::invalid_argument("Index out of range.");
		}
		return _first[index];
	}

	~MyVector()
	{
		delete[] _first;
	}
private:
	T* _first;
	size_t _count;
	size_t _capacity;

	void EnsureCapacity(size_t index)
	{
		size_t newCapacity = std::bit_ceil(index + 1);

		if (_capacity < newCapacity)
		{
			T* newBuffer = new T[newCapacity];
			std::move(_first, _first + _count, newBuffer);
			delete[] _first;
			_first = newBuffer;
			_capacity = newCapacity;
		}
	}
};