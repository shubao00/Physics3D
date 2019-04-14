#pragma once

#include "part.h"
#include "physical.h"
#include <vector>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <iterator>

class PhysicalContainer {
private:
	size_t capacity;
	void movePhysical(size_t origin, size_t destination);
	void movePhysical(Physical* origin, Physical* destination);
	void swapPhysical(size_t first, size_t second);
	void swapPhysical(Physical* first, Physical* second);
public:
	size_t physicalCount;
	size_t freePhysicalsOffset;
	size_t partCount;
	size_t anchoredPartsCount;
	Physical* physicals;
	Part** parts;

	PhysicalContainer(size_t initialCapacity);
	void ensureCapacity(size_t capacity);
	void add(Physical& p);
	void addAnchored(Physical& p);
	void remove(size_t index);
	void remove(Physical* p);
	void anchor(size_t index);
	void anchor(Physical* p);
	void unanchor(size_t index);
	void unanchor(Physical* p);
	bool isAnchored(size_t index);
	bool isAnchored(const Physical* index) const;

	void add(Part* part, bool anchored);


	inline Physical& operator[](size_t index) {
		return physicals[index];
	}
	inline const Physical& operator[](size_t index) const {
		return physicals[index];
	}
	inline Physical* begin() {
		return physicals;
	}
	inline const Physical* begin() const {
		return physicals;
	}
	inline Physical* end() {
		return physicals + physicalCount;
	}
	inline const Physical* end() const {
		return physicals + physicalCount;
	}

	template<typename T>
	struct Iter {
		T* first; T* last;
		T* begin() { return first; }
		T* end() { return last; }
	};
	template<typename T>
	struct ConstIter {
		const T* first; const T* last;
		const T* begin() const { return first; }
		const T* end() const { return last; }
	};

	Iter<Physical> iterPhysicals() { return Iter<Physical>{physicals, physicals+physicalCount}; }
	ConstIter<Physical> iterPhysicals() const { return ConstIter<Physical>{physicals, physicals + physicalCount}; }

	Iter<Physical> iterAnchoredPhysicals() { return Iter<Physical>{physicals, physicals + freePhysicalsOffset}; }
	ConstIter<Physical> iterAnchoredPhysicals() const { return ConstIter<Physical>{physicals, physicals + freePhysicalsOffset}; }

	Iter<Physical> iterUnAnchoredPhysicals() { return Iter<Physical>{physicals + freePhysicalsOffset, physicals + physicalCount}; }
	ConstIter<Physical> iterUnAnchoredPhysicals() const { return ConstIter<Physical>{physicals + freePhysicalsOffset, physicals + physicalCount}; }
};

struct QueuedPart {
	Part* p;
	bool anchored;
};

class ConstPartIterator {
	const Physical* current;

public:
	inline ConstPartIterator(const Physical* current) : current(current) {}
	inline const Part& operator*() const {
		return current->part;
	}

	inline ConstPartIterator& operator++() {
		current++;
		return *this;
	}

	inline bool operator!=(ConstPartIterator& other) { return this->current != other.current; }
	inline bool operator==(ConstPartIterator& other) { return this->current == other.current; }
};

class PartIterator {
	Physical* current;

public:
	inline PartIterator(Physical* current) : current(current) {}
	inline Part& operator*() const {
		return current->part;
	}

	inline PartIterator& operator++() {
		current++;
		return *this;
	}

	inline bool operator!=(PartIterator& other) {return this->current != other.current;}
	inline bool operator==(PartIterator& other) {return this->current == other.current;}
};

class ConstPartIteratorFactory {
	const Physical* start;
	const Physical* finish;
public:
	inline ConstPartIteratorFactory(const Physical* start, const Physical* finish) : start(start), finish(finish) {}
	inline ConstPartIterator begin() const { return ConstPartIterator(start); }
	inline ConstPartIterator end() const { return ConstPartIterator(finish); }
};

class PartIteratorFactory {
	Physical* start;
	Physical* finish;
public:
	inline PartIteratorFactory(Physical* start, Physical* finish) : start(start), finish(finish) {}
	inline PartIterator begin() const { return PartIterator(start); }
	inline PartIterator end() const { return PartIterator(finish); }
};

class World {
private:
	std::queue<QueuedPart> newPartQueue;
	size_t getTotalVertexCount();
	void processQueue();
	void processColissions();

	void addPartUnsafe(Part* p, bool anchored);
public:
	PhysicalContainer physicals;
	mutable std::shared_mutex lock;
	mutable std::mutex queueLock;

	Physical* selectedPhysical = nullptr;
	Vec3 localSelectedPoint;
	Vec3 magnetPoint;

	World();

	World(const World&) = delete;
	World(World&&) = delete;
	World& operator=(const World&) = delete;
	World& operator=(World&&) = delete;

	void tick(double deltaT);

	void addObject(Part* p, bool anchored);
	void addObject(Part* p);
	inline void addObject(Part p, bool anchored = false) { addObject(new Part(p), anchored); }

	void removePart(Part* p);

	inline PartIterator begin() {return PartIterator(physicals.physicals);}
	inline PartIterator end() {return PartIterator(physicals.physicals + physicals.physicalCount);}

	inline ConstPartIterator begin() const { return ConstPartIterator(physicals.physicals); }
	inline ConstPartIterator end() const { return ConstPartIterator(physicals.physicals + physicals.physicalCount); }

	inline PartIteratorFactory iterAnchoredParts() { return PartIteratorFactory(physicals.physicals, physicals.physicals + physicals.freePhysicalsOffset); }
	inline ConstPartIteratorFactory iterAnchoredParts() const { return ConstPartIteratorFactory(physicals.physicals, physicals.physicals + physicals.freePhysicalsOffset); }

	inline PartIteratorFactory iterFreeParts() { return PartIteratorFactory(physicals.physicals + physicals.freePhysicalsOffset, physicals.physicals + physicals.physicalCount); }
	inline ConstPartIteratorFactory iterFreeParts() const { return ConstPartIteratorFactory(physicals.physicals + physicals.freePhysicalsOffset, physicals.physicals + physicals.physicalCount); }

	virtual void applyExternalForces();
};
