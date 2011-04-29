#include "PhysicsEngine.h"
#include <OgreMath.h>
#include <OgrePlane.h>

using namespace Ogre;

// ========================================================================
// BaseObject Implementation
// ========================================================================

BaseObject::BaseObject(Vector3 position) :
	m_position(position), m_orientation(Quaternion::IDENTITY)
{
}

BaseObject::BaseObject(const BaseObject& copy) : m_position(copy.m_position), 
	m_orientation(copy.m_orientation)
{
}

BaseObject::BaseObject() : 
	m_position(0, 0, 0), m_orientation(Radian(1), Vector3(0, 0, 0))
{
}

void BaseObject::yaw(Radian radians)
{
	Quaternion q(Radian(radians), Vector3::UNIT_Y);
	setOrientation(m_orientation * q);
}

void BaseObject::roll(Radian radians)
{
	Quaternion q(Radian(radians), Vector3::UNIT_Z);
	setOrientation(m_orientation * q);
}

void BaseObject::pitch(Radian radians)
{
	Quaternion q(Radian(radians), Vector3::UNIT_X);
	setOrientation(m_orientation * q);
}

void BaseObject::setPosition(Vector3 position)
{
	m_position = position;
}

Vector3 BaseObject::getOffset(const BaseObject& other) {
	return other.getPosition() - getPosition();
}

Vector3 BaseObject::getPosition() const
{
	return m_position;
}

Vector3 BaseObject::getHeading() const
{
	return m_orientation * Vector3(0, 0, -1);
}

Vector3 BaseObject::getNormal() const
{
	return m_orientation * Vector3(0, 1, 0);
}

Quaternion BaseObject::getOrientation() const 
{
	return m_orientation;
}

void BaseObject::setOrientation(Quaternion orientation) {
	m_orientation = orientation;
	m_orientation.normalise();
}


// ========================================================================
// PhysicsObject Implementation
// ========================================================================

PhysicsObject::PhysicsObject(ObjectType type, Real mass, Vector3 position) :
	BaseObject(position), m_type(type), m_mass(mass), m_velocity(0, 0, 0),
	m_acceleration(0, 0, 0), m_force(0, 0, 0), m_tempForce(0, 0, 0)
{
}

PhysicsObject::PhysicsObject(ObjectType type, Real mass) : BaseObject(), m_type(type), m_mass(mass), 
	m_velocity(0, 0, 0), m_acceleration(0, 0, 0), m_force(0, 0 ,0), m_tempForce(0, 0, 0)
{
}

PhysicsObject::PhysicsObject(const PhysicsObject& copy) : BaseObject(copy), m_type(copy.m_type), 
	m_mass(copy.m_mass), m_velocity(copy.m_velocity), m_acceleration(copy.m_acceleration),
	m_force(copy.m_force), m_tempForce(copy.m_tempForce)
{
}

ObjectType PhysicsObject::getType() const
{
	return m_type;
}

Real PhysicsObject::getMass() const
{
	return m_mass;
}

void PhysicsObject::setVelocity(Vector3 velocity) 
{
	m_velocity = velocity;
}

void PhysicsObject::setAcceleration(Vector3 acceleration) 
{
	m_acceleration = acceleration;
}

Vector3 PhysicsObject::getVelocity() const
{
	return m_velocity;
}

Vector3 PhysicsObject::getAcceleration() const
{
	return m_acceleration;
}

Vector3 PhysicsObject::getForce() const
{
	return m_force;
}

Vector3 PhysicsObject::getTempForce() const
{
	return m_tempForce;
}

void PhysicsObject::applyForce(Vector3 force) 
{
	m_force = m_force + force;
}

void PhysicsObject::applyTempForce(Vector3 force) 
{
	m_tempForce = m_tempForce + force;
}

void PhysicsObject::clearForces() 
{
	m_force = Vector3(0, 0, 0);
	m_tempForce = Vector3(0, 0, 0);
}

void PhysicsObject::updatePhysics(Real timeElapsed) 
{
	// if(m_mass == 0) {
	// TODO: Throw exception
	// }
	
	m_acceleration = (m_force + m_tempForce) / m_mass;
	m_velocity = m_velocity + (m_acceleration * timeElapsed);
	setPosition(getPosition() + (m_velocity * timeElapsed));
	m_tempForce = Vector3(0, 0, 0);
}


// ========================================================================
// Constraint Implementation
// ========================================================================
Constraint::Constraint(PhysicsObject * startObject, PhysicsObject * endObject, Real distance) :
	m_startObject(startObject), m_endObject(endObject), m_distance(distance)
{
}

Constraint::Constraint(const Constraint& copy) :
	m_startObject(copy.m_startObject), m_endObject(copy.m_endObject), m_distance(copy.m_distance)
{
}

PhysicsObject * Constraint::getStartObject()
{
	return m_startObject;
}

PhysicsObject * Constraint::getEndObject()
{
	return m_endObject;
}

void Constraint::applyForces(Real timeElapsed)
{
	// Spring based constraint
	/*
	Real distance = m_startObject->getOffset(*m_endObject).length();
	Real appliedForce = Math::Pow((distance - m_distance), 2) * 1 + Math::Abs(distance - m_distance) * 3;
	if(distance < m_distance) {
		appliedForce = -appliedForce;
	}

	m_startObject->applyTempForce((m_startObject->getOffset(*m_endObject)).normalisedCopy()
		* appliedForce);

	m_endObject->applyTempForce((m_endObject->getOffset(*m_startObject)).normalisedCopy()
		* appliedForce);
	*/

	// Orbit constraint
	Vector3 normalVector = m_endObject->getOffset(*m_startObject);
	if(normalVector.length() > m_distance) {
		Plane normalPlane = Plane(normalVector, 0);
		normalPlane.normalise();
		Vector3 curVelocity = m_startObject->getVelocity() - m_endObject->getVelocity();
		Vector3 desiredVelocity = normalPlane.projectVector(curVelocity) + m_endObject->getVelocity();
		Vector3 velocityOffset = desiredVelocity - m_startObject->getVelocity();
	
		m_startObject->applyTempForce(((velocityOffset * m_startObject->getMass()) / timeElapsed));
	}
}


// ========================================================================
// SphereCollisionObject Implementation
// ========================================================================

SphereCollisionObject::SphereCollisionObject(ObjectType type, Real radius, Real mass, Vector3 position)
	: PhysicsObject(type, mass, position), m_radius(radius)
{
}

SphereCollisionObject::SphereCollisionObject(ObjectType type, Real radius, Real mass)
	: PhysicsObject(type, mass), m_radius(radius)
{
}

SphereCollisionObject::SphereCollisionObject(const SphereCollisionObject& copy)
	: PhysicsObject(copy), m_radius(copy.m_radius)
{
}

Real SphereCollisionObject::getRadius() const
{
	return m_radius;
}

bool SphereCollisionObject::checkCollision(const SphereCollisionObject& object) const
{ 
	return getPosition().squaredDistance(object.getPosition()) <= Math::Pow(getRadius() + object.getRadius(), 2);
}