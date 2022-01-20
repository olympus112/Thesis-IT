#pragma once

#include <utility>

template <typename T>
struct List {
private:
	std::size_t size_;
	std::size_t capacity_;

	T* data;

public:
	List() {
		size_ = 0;
		capacity_ = 1;
		data = new T[capacity_];
	}

	void add(const T& patch) {
		if (size_ == capacity_) {
			std::size_t newCapacity = 2 * capacity_;
			T* newData = new T[newCapacity];

			for (std::size_t index = 0; index < size_; index++)
				newData[index] = data[index];

			delete[] data;

			capacity_ = newCapacity;
			data = newData;
		}

		data[size_] = patch;
		size_++;
	}

	bool contains(const T& element) const {
		for (std::size_t index = 0; index < size_; index++) {
			if (data[index] == element)
				return true;
		}

		return false;
	}

	T& operator[](std::size_t index) {
		if (index < size_)
			return data[index];

		throw "Out of bounds";
	}

	const T& operator[](std::size_t index) const {
		if (index < size_)
			return data[index];

		throw "Out of bounds";
	}

	std::size_t size() const {
		return size_;
	}

	std::size_t capacity() const {
		return capacity_;
	}

};
