/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Nestor Silveira.

    Contributor(s): none yet.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#ifndef _gkSteeringObject_h_
#define _gkSteeringObject_h_

#include "AbstractVehicle.h"
#include "SteerLibrary.h"

class gkGameObject;
class gkNavPath;

typedef OpenSteer::SteerLibraryMixin<OpenSteer::AbstractVehicle> SimpleSteering;

class gkSteeringObject : public SimpleSteering
{
public:

	enum STATE
	{
		UNKNOWN,
		AVOIDING,
		EVADING_AND_SEEKING_GOAL,
		IN_GOAL,
		SEEKING_GOAL,
		SEEKING_PATH,
		WAITTING_PATH_CLEAR
	};

	gkSteeringObject(gkGameObject* obj, gkScalar maxSpeed, const gkVector3& forward, const gkVector3& up, const gkVector3& side);
	virtual ~gkSteeringObject();

    OpenSteer::Vec3 side() const;
    OpenSteer::Vec3 up() const;
	OpenSteer::Vec3 forward() const;
	OpenSteer::Vec3 position() const;

    float mass() const {return m_mass;}
    float setMass(float m) {return m_mass = m;}

	OpenSteer::Vec3 velocity() const;

    float speed() const {return m_speed;}
    float setSpeed(float s) {return m_speed = s;}

    float radius() const {return m_radius;}
    float setRadius(float m) {return m_radius = m;}

    float maxForce() const {return m_maxForce;}
    float setMaxForce(float mf) {return m_maxForce = mf;}

    float maxSpeed(void) const {return m_maxSpeed;}
    float setMaxSpeed(float ms) {return m_maxSpeed = ms;}

	OpenSteer::Vec3 predictFuturePosition(const float predictionTime) const;

    gkScalar curvature () const {return m_curvature;}
	void measurePathCurvature (const float elapsedTime);

	virtual const gkVector3& getGoalPosition() const = 0;
	virtual gkScalar getGoalRadius() const = 0;
	virtual gkVector3 steering(STATE& newState, const float elapsedTime) = 0;
	virtual void notifyInGoal() = 0;
	virtual void applyForce(const gkVector3& force, const float elapsedTime) = 0;

	virtual void setGoalPosition(const gkVector3& position){};
	virtual void setGoalRadius(gkScalar radius){};

	bool update(gkScalar tick);

	int getState() const { return m_state; }

	gkString getDebugStringState() const;

protected:

	void reset();
	void applySteeringForce(const OpenSteer::Vec3& force, const float elapsedTime);
	void applyBrakingForce(const float rate, const float deltaTime);
	OpenSteer::Vec3 adjustRawSteeringForce(const OpenSteer::Vec3& force);
	bool clearPathToGoal(const gkVector3& goalPosition, gkGameObject* target = 0);
	gkScalar adjustObstacleAvoidanceLookAhead(bool clearPath, const gkVector3& goalPosition, gkScalar minPredictTime, gkScalar maxPredictTime);
	OpenSteer::Vec3 steerToEvadeOthers(const gkGameObject* target = 0);
	gkVector3 steerToFollowPathLinear(const int direction, const float predictionTime, const gkNavPath& path);
	gkScalar combinedLookAheadTime (float minTime, gkScalar minDistance) const;
	OpenSteer::Vec3 mapPointAndDirectionToTangent(gkNavPath const& pathway, OpenSteer::Vec3 const& point, int direction) const;
	bool isNearWaypoint(gkNavPath const& pathway, OpenSteer::Vec3 const& point) const;
	float maxSpeedForCurvature(gkScalar minimumTurningRadius) const;
	float maxSpeedForCurvature(const gkNavPath* path, gkScalar minimumTurningRadius, int pathFollowDirection) const;
	OpenSteer::Vec3 steerTowardHeading (const OpenSteer::Vec3& desiredGlobalHeading) const;
	float nonZeroCurvatureQQQ() const;
	OpenSteer::Vec3 adjustSteeringForMinimumTurningRadius (const OpenSteer::Vec3& steering, gkScalar minimumTurningRadius);

protected:

	gkGameObject* m_obj;

	typedef std::set<gkSteeringObject*> OTHERS;
	static OTHERS m_others;

    float m_mass;       // mass (defaults to unity so acceleration=force)

    float m_radius;     // size of bounding sphere, for obstacle avoidance, etc.

    float m_speed;      // speed along Forward direction.  Because local space
                       // is velocity-aligned, velocity = Forward * Speed

    float m_maxForce;   // the maximum steering force this vehicle can apply
                       // (steering force is clipped to this magnitude)

    float m_maxSpeed;   // the maximum speed this vehicle is allowed to move
                       // (velocity is clipped to this magnitude)

private:

	STATE m_state;

	OpenSteer::Vec3 m_smoothedAcceleration;

	gkVector3 m_forward;
	gkVector3 m_up;
	gkVector3 m_side;

	gkScalar m_curvature;
    OpenSteer::Vec3 m_lastForward;
	OpenSteer::Vec3 m_lastPosition;
    gkScalar m_smoothedCurvature;

};

#endif//_gkSteeringObject_h_
