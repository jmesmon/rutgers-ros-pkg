/*
 * vector.h
 *
 *  Created on: Nov 1, 2010
 *      Author: asher
 */

#ifndef VECTOR_H_
#define VECTOR_H_
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace ROS {

/* XXX: This is poorly named: the ros documentation refers to them as "Fixed
 * or variable sized arrays (lists)". Additionally, there is no reason to use
 * the same class for both Fixed and Variable sized, in fact it makes it
 * rather confusing in dealing with the serialize/deserialize methods.
 */
template<class T> class vector {
public:

	vector(void)
	{}

	vector(uint16_t size)
	{
		fixed = 1;
		setMaxLength(size);
		length = size;
	}

	void setMaxLength(uint16_t L)
	{
		max_length = L;
		data = malloc( L* sizeof(T));
	}

	T& operator[] (int i)
	{
		return data[i];
	}

	void push_back(T item)
	{
		if (length < max_length) {
			data[length] = item;
			length++;
		}
	}

	T pop_back()
	{
		length--;
		return data[length+1];
	}

	~vector()
	{
		free(data);
	}

	uint16_t serialize(uint8_t * buffer);
	uint16_t deserialize(uint8_t * buffer);

	/* FIXME: choose better name. 'Size' vs 'Bytes' */
	uint16_t bytes(void)// output size to store in bytes
	{
		return length*sizeof(T);
	}

	uint16_t size(void)
	{
		return length;
	}

	/* FIXME: should this be public? */
	bool fixed;
private:
	T* data;
	uint16_t length;
	uint16_t max_length;

	void setSize(uint16_t size)
	{
		length = size;
	}

	uint16_t serializeFixed(uint8_t * buffer);
	uint16_t serializeVariable(uint8_t* buffer);
	uint16_t deserializeFixed(uint8_t *buffer);
	uint16_t deserializeVariable(uint8_t * buffer);
};

template<class T>
uint16_t vector<T>::serializeFixed(uint8_t * buffer)
{
	memcpy(buffer,data, this->bytes());
	return this->bytes();
}


template<class T>
uint16_t vector<T>::serializeVariable(uint8_t * buffer)
{
	memcpy(buffer, &length, 2);
	serializeFixed(buffer+4);
	return this->bytes()+4;
}

template<class T>
uint16_t vector<T>::deserializeFixed(uint8_t * buffer)
{
	memcpy(data,buffer, this->bytes());
	return this->bytes();
}

template<class T>
uint16_t vector<T>::deserializeVariable(uint8_t * buffer)
{
	uint32_t size;
	memcpy(&size, buffer, 4);
	setSize(size);
	deserializeFixed(buffer+4);
	return this->bytes()+4;
}

template<class T>
uint16_t vector<T>::serialize(uint8_t * buffer)
{
	if (fixed) return serializeFixed(buffer);
	else return this->serializeVariable(buffer);
}

template<class T>
uint16_t vector<T>::deserialize(uint8_t* buffer)
{
	if (fixed) return deserializeFixed(buffer);
	else return deserializeVariable(buffer);
}

} /* namespace ROS */

#endif /* VECTOR_H_ */
