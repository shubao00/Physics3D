#pragma once

#include <string>

class ResourceManager;

#pragma region ResourceAllocator

//! ResourceAllocator
template<typename T>
class ResourceAllocator {
	friend ResourceManager;
	
protected:
	ResourceAllocator() {};

public:
	virtual T* load(const std::string& name, const std::string& path) = 0;
};

#pragma endregion

#pragma region Resource

//! Resource
enum class ResourceType {
	None = 0,
	Mesh,
	Font,
	Texture,
	Shader,
	OBJ
};

class Resource {
	friend ResourceManager;

protected:
	std::string path;
	std::string name;

	Resource(const std::string& path);
	Resource(const std::string& name, const std::string& path);

public:
	inline virtual ResourceType getType() const = 0;
	inline virtual std::string getTypeName() const = 0;
	inline virtual void close() = 0;

	inline std::string getName() const;
	inline std::string getPath() const;

	void setName(const std::string& name);
	void setPath(const std::string& path);

	template<typename T>
	static ResourceAllocator<T> getAllocator() {
		return T::getAllocator();
	}
};

#define DEFINE_RESOURCE(type, path) \
	inline static std::string getDefaultPath() { return path; } \
	inline static ResourceType getStaticType() { return ResourceType::type; } \
	inline static std::string getStaticTypeName() { return #type; } \
	inline virtual std::string getTypeName() const override { return getStaticTypeName(); } \
	inline virtual ResourceType getType() const override { return getStaticType(); }

#pragma endregion