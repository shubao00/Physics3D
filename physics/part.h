#pragma once

class Part;
class HardConstraint;
class RigidBody;
class Physical;
class ConnectedPhysical;
class MotorizedPhysical;
class WorldPrototype;
#include "geometry/shape.h"
#include "math/linalg/mat.h"
#include "math/position.h"
#include "math/globalCFrame.h"
#include "math/bounds.h"
#include "motion.h"

struct PartProperties {
	double density;
	double friction;
	double bouncyness;

	/*
		This is extra velocity that should be added to any colission
		if this part is anchored, this gives the velocity of another part sliding on top of it, with perfect friction

		In other words, this is the desired relative velocity for there to be no friction
	*/
	Vec3 conveyorEffect{0, 0, 0};
};

struct PartIntersection {
	bool intersects;
	Position intersection;
	Vec3 exitVector;

	PartIntersection() : intersects(false) {}
	PartIntersection(const Position& intersection, const Vec3& exitVector) :
		intersects(true),
		intersection(intersection),
		exitVector(exitVector) {}
};


class Part {
	friend class RigidBody;
	friend class Physical;
	friend class ConnectedPhysical;
	friend class MotorizedPhysical;
	friend class WorldPrototype;

	GlobalCFrame cframe;

public:
	bool isTerrainPart = false;
	Physical* parent = nullptr;
	Shape hitbox;
	double maxRadius;
	PartProperties properties;

	Part() = default;
	Part(const Shape& shape, const GlobalCFrame& position, const PartProperties& properties);
	Part(const Shape& shape, Part& attachTo, const CFrame& attach, const PartProperties& properties);
	~Part();

	Part(const Part& other) = delete;
	Part& operator=(const Part& other) = delete;
	Part(Part&& other);
	Part& operator=(Part&& other);


	PartIntersection intersects(const Part& other) const;
	void scale(double scaleX, double scaleY, double scaleZ);

	Bounds getStrictBounds() const;

	BoundingBox getLocalBounds() const;

	Position getPosition() const { return cframe.getPosition(); }
	double getMass() const { return hitbox.getVolume() * properties.density; }
	Vec3 getLocalCenterOfMass() const { return hitbox.getCenterOfMass(); }
	Position getCenterOfMass() const { return cframe.localToGlobal(this->getLocalCenterOfMass()); }
	SymmetricMat3 getInertia() const { return hitbox.getInertia() * properties.density; }
	const GlobalCFrame& getCFrame() const { return cframe; }
	void setCFrame(const GlobalCFrame& newCFrame);

	CFrame transformCFrameToParent(const CFrame& cframeRelativeToPart);

	Motion getMotion() const;

	bool isMainPart() const;
	void makeMainPart();

	void translate(Vec3 translation);

	double getWidth() const;
	double getHeight() const;
	double getDepth() const;

	void setWidth(double newWidth);
	void setHeight(double newHeight);
	void setDepth(double newDepth);

	void ensureHasParent();

	void attach(Part* other, const CFrame& relativeCFrame);
	void attach(Part* other, HardConstraint* constraint, const CFrame& attachToThis, const CFrame& attachToThat);
	void detach();

	bool isFixed() const;

	bool isValid() const;
};
