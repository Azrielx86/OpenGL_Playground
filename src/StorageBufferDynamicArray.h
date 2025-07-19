//
// Created by tohka on 7/18/25.
//

#ifndef STORAGEBUFFER_H
#define STORAGEBUFFER_H

#include <GL/glew.h>
#include <vector>

template <typename T>
class StorageBufferDynamicArray
{
	static_assert(std::is_class_v<T>);
	GLuint ssbo{};
	unsigned int bindingIndex;
	std::vector<T> data;

  public:
	explicit StorageBufferDynamicArray(unsigned int bindingIndex);
	void Add(const T &item);
	T &operator[](size_t index);
	void UpdateIndex(size_t index);
	void Remove(size_t index);
	size_t Size();
};

template <typename T>
StorageBufferDynamicArray<T>::StorageBufferDynamicArray(const unsigned int bindingIndex) : bindingIndex(bindingIndex)
{
	glCreateBuffers(1, &ssbo);
	glNamedBufferData(ssbo, sizeof(T) * data.size(), data.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, ssbo);
}

template <typename T>
void StorageBufferDynamicArray<T>::Add(const T &item)
{
	data.push_back(item);
	glNamedBufferData(ssbo, sizeof(T) * data.size(), data.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, ssbo);
}

template <typename T>
T &StorageBufferDynamicArray<T>::operator[](size_t index)
{
	return data[index];
}

template <typename T>
void StorageBufferDynamicArray<T>::UpdateIndex(const size_t index)
{
	glNamedBufferSubData(ssbo, static_cast<GLintptr>(sizeof(T) * index), sizeof(T), &data[index]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, ssbo);
}

template <typename T>
void StorageBufferDynamicArray<T>::Remove(size_t index)
{
	data.erase(data.begin() + index);
	glNamedBufferData(ssbo, sizeof(T) * data.size(), data.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, ssbo);
}

template <typename T>
size_t StorageBufferDynamicArray<T>::Size()
{
	return data.size();
}

#endif // STORAGEBUFFER_H
