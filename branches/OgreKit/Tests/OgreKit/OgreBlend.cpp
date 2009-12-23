/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2009 Erwin Coumans & Charlie C
-------------------------------------------------------------------------------
 This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
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
#include "autogenerated/blender.h"
#include "OgreBlend.h"
#include "gkEngine.h"
#include "gkUserDefs.h"
#include "gkSceneObjectManager.h"
#include "gkSceneObject.h"
#include "gkGameObject.h"
#include "gkLightObject.h"
#include "gkCameraObject.h"
#include "gkEntityObject.h"

#include "btBulletDynamicsCommon.h"

// internal converters
#include "gkBlendFile.h"
#include "gkBlenderDefines.h"
#include "gkObjectLoader.h"
#include "gkMeshLoader.h"
#include "gkSkeletonLoader.h"
#include "gkLoaderUtils.h"

#include "Constraints/gkLimitRotConstraint.h"
#include "Constraints/gkLimitLocConstraint.h"

#include "Logic/gkLogicManager.h"
#include "Logic/gkLogicTree.h"
#include "Logic/gkLogicNode.h"
#include "Logic/gkMotionNode.h"
#include "Logic/gkKeyNode.h"
#include "Logic/gkMathNode.h"
#include "Logic/gkPrintNode.h"
#include "Logic/gkAnimationNode.h"


// Ogre objects
#include "OgreLight.h"


// Converted objects:
// camera	= gkCameraObject
// lamp		= gkLampObject
// empty	= gkGameObject
// mesh		= gkEntityObject + Ogre::Mesh
// armature	= gkGameObject + Ogre::Skeleton
// others	= undefined and skipped



class OgreMotionState : public btMotionState
{
private:

	gkGameObject *m_object;


public:

	OgreMotionState() : m_object(0) {}
	virtual ~OgreMotionState() {}

	void setObject(gkGameObject *ob) {m_object = ob;}


	void getWorldTransform(btTransform& worldTrans ) const
	{
		worldTrans.setIdentity();

		if (m_object != 0)
		{
			Ogre::Quaternion rot = m_object->getWorldOrientation();
			Ogre::Vector3 loc = m_object->getWorldPosition();

			worldTrans.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
			worldTrans.setOrigin(btVector3(loc.x, loc.y, loc.z));
		}
	}

	void setWorldTransform(const btTransform& worldTrans)
	{
		if (m_object != 0)
		{
			const btQuaternion &rot = worldTrans.getRotation();
			const btVector3 &loc = worldTrans.getOrigin();


			Ogre::SceneNode *node = m_object->getNode();
			if (node)
			{
                // apply directly to the scene node
				node->setOrientation(Ogre::Quaternion(rot.w(), rot.x(), rot.y(), rot.z()));
				node->setPosition(Ogre::Vector3(loc.x(), loc.y(), loc.z()));

				m_object->_stateUpdated();
			}
		}
	}

};


// ----------------------------------------------------------------------------
gkCameraObject* createPivotCamera(gkSceneObject* ob, const Ogre::Vector3 &root, const Ogre::Vector3 &offset, const Ogre::Vector3& ori)
{
	gkGameObject *gob= ob->createObject("createPivotCamera::Ogre::Root");
	gkCameraObject* cam= ob->createCamera("createPivotCamera::Camera");

	cam->setParent(gob);

	gob->getProperties().position= root;
	cam->getProperties().position= root + offset;
	cam->getProperties().orientation= gkMathUtils::getQuatFromEuler(ori);

	/// TODO: node sockets need enumeration values
	/// (So you don't have to count indices!)

	/// setup gamelogic
	gkLogicTree *ltree= gkLogicManager::getSingleton().create();

	gkLogicNode *mouse= ltree->createNode(NT_MOUSE);
	gkLogicNode *motion= ltree->createNode(NT_MOTION);

	/// Mouse RelX -> Motion Z
	gkLogicSocket *rx= mouse->getOutputSocket(1);
	gkLogicSocket *mz= motion->getInputSocket(3);

	/// flag update : If Motion -> Update
	motion->getInputSocket(0)->link(mouse->getOutputSocket(0));


	/// scale RelX mouse a bit ScaleX
	mouse->getInputSocket(0)->setValue(-0.3f);

	/// add Q exit hook
	gkLogicNode *q= ltree->createNode(NT_KEY);
	gkLogicNode *ex= ltree->createNode(NT_EXIT);
	q->getInputSocket(0)->setValue(true);

	gkKeyNode *kn= (gkKeyNode*)q;
	kn->setKey(KC_QKEY);

	// Exit -> QKey
	ex->getInputSocket(0)->link(q->getOutputSocket(1));

	mz->link(rx);

	/// attach it to root pivot
	gob->attachLogic(ltree);
	return cam;
}

// ----------------------------------------------------------------------------
void createExitHook(gkSceneObject *sc)
{
	gkGameObject *ob = sc->createObject("createExitHook::Exit");

	gkLogicTree *ltree= gkLogicManager::getSingleton().create();

	gkLogicNode *q= ltree->createNode(NT_KEY);
	gkLogicNode *ex= ltree->createNode(NT_EXIT);
	q->getInputSocket(0)->setValue(true);

	gkKeyNode *kn= (gkKeyNode*)q;
	kn->setKey(KC_QKEY);

	// Exit -> QKey
	ex->getInputSocket(0)->link(q->getOutputSocket(1));

	ltree->solveOrder();
	ob->attachLogic(ltree);
}


// ----------------------------------------------------------------------------
gkCameraObject* createMouseLook(gkSceneObject *sc,  const Ogre::Vector3 &pos, const Ogre::Vector3& ori)
{
	gkGameObject *y = sc->createObject("createMouseLook::y");
	gkGameObject *z = sc->createObject("createMouseLook::z");

	gkCameraObject *mcam = sc->getMainCamera();

	gkCameraObject *cam = sc->createCamera("createMouseLook::cam");

	if (mcam)
	{
		cam->getCameraProperties() = mcam->getCameraProperties();


		Ogre::Vector3 neul = gkMathUtils::getEulerFromQuat(mcam->getWorldOrientation());
		Ogre::Vector3 zeul = Ogre::Vector3(0, 0, neul.z);

		Ogre::Quaternion& zrot= z->getProperties().orientation;
		Ogre::Quaternion& crot= cam->getProperties().orientation;

		zrot = gkMathUtils::getQuatFromEuler(zeul);
		zrot.normalise();
		crot = gkMathUtils::getQuatFromEuler(ori);
		crot.normalise();

		z->getProperties().position = mcam->getWorldPosition();
	}
	else
	{
		z->getProperties().position = pos;
		cam->getProperties().orientation = gkMathUtils::getQuatFromEuler(ori);
	}

	y->setParent(z);
	cam->setParent(y);

	// zrotation
	gkLogicTree *ztree= gkLogicManager::getSingleton().create();
	gkLogicNode *zmou = ztree->createNode(NT_MOUSE);
	gkLogicNode *zmot = ztree->createNode(NT_MOTION);
	zmou->getInputSocket(0)->setValue(-0.3f);

	zmot->getInputSocket(0)->link(zmou->getOutputSocket(0));
	zmot->getInputSocket(3)->link(zmou->getOutputSocket(1));

	// yrotation
	gkLogicTree *ytree= gkLogicManager::getSingleton().create();
	gkLogicNode *ymou = ytree->createNode(NT_MOUSE);
	gkLogicNode *ymot = ytree->createNode(NT_MOTION);
	ymou->getInputSocket(1)->setValue(-0.3f);

	ymot->getInputSocket(0)->link(ymou->getOutputSocket(0));
	ymot->getInputSocket(1)->link(ymou->getOutputSocket(2));

	/// setup 90, -90 clamp
	gkMotionNode *node = (gkMotionNode*)ymot;
	node->setMinX(-90);
	node->setMaxX(90);


	ytree->solveOrder();
	y->attachLogic(ytree);

	// keyboard controlls
	gkKeyNode *fwd = (gkKeyNode*)ztree->createNode(NT_KEY);
	gkKeyNode *bak = (gkKeyNode*)ztree->createNode(NT_KEY);
	gkKeyNode *lft = (gkKeyNode*)ztree->createNode(NT_KEY);
	gkKeyNode *rgt = (gkKeyNode*)ztree->createNode(NT_KEY);
	fwd->getInputSocket(0)->setValue(true);
	bak->getInputSocket(0)->setValue(true);
	lft->getInputSocket(0)->setValue(true);
	rgt->getInputSocket(0)->setValue(true);


	fwd->setKey(KC_UPARROWKEY);
	bak->setKey(KC_DOWNARROWKEY);
	lft->setKey(KC_LEFTARROWKEY);
	rgt->setKey(KC_RIGHTARROWKEY);

	const Ogre::Real scale = 0.5;

	gkMathNode *scale1 = (gkMathNode*)ztree->createNode(NT_MATH);
	gkMathNode *scale2 = (gkMathNode*)ztree->createNode(NT_MATH);
	scale1->setFunction(MTH_MULTIPLY);
	scale2->setFunction(MTH_MULTIPLY);
	scale1->getInputSocket(1)->setValue(scale);
	scale2->getInputSocket(1)->setValue(scale);



	/// math node
	// subtract (bak - fwd) out -> link to y movement
	gkMathNode *vecfwd = (gkMathNode*)ztree->createNode(NT_MATH);

	vecfwd->setFunction(MTH_SUBTRACT);
	vecfwd->getInputSocket(0)->link(fwd->getOutputSocket(0));
	vecfwd->getInputSocket(1)->link(bak->getOutputSocket(0));

	scale1->getInputSocket(0)->link(vecfwd->getOutputSocket(0));

	gkMotionNode *fwdmot = (gkMotionNode*)ztree->createNode(NT_MOTION);
	fwdmot->getInputSocket(2)->link(scale1->getOutputSocket(0));
	fwdmot->setMotionType(MT_TRANSLATION);

	// translate relitave to y GameObject
	fwdmot->setSpace(TRANSFORM_PARENT);
	fwdmot->setOtherObject("createMouseLook::y");


	/// math node
	// subtract (lft - rgt) out -> link to x movement
	gkMathNode *vecstrf = (gkMathNode*)ztree->createNode(NT_MATH);

	vecstrf->setFunction(MTH_SUBTRACT);
	vecstrf->getInputSocket(0)->link(rgt->getOutputSocket(0));
	vecstrf->getInputSocket(1)->link(lft->getOutputSocket(0));

	scale2->getInputSocket(0)->link(vecstrf->getOutputSocket(0));
	fwdmot->getInputSocket(1)->link(scale2->getOutputSocket(0));

	ztree->solveOrder();
	z->attachLogic(ztree);

	if (mcam->isLoaded())
		cam->load();
	return cam;
}


// ----------------------------------------------------------------------------
void loadMomo(gkSceneObject* scene)
{

	/// create a pivot camera sys
	gkCameraObject *cam= createPivotCamera(scene, Ogre::Vector3::ZERO, Ogre::Vector3(-1, 1.5, 0), Ogre::Vector3(90, 0, 212));
	cam->setMainCamera(true);

	// load the blender data
	scene->load();

	// grab the mesh
	gkGameObject *ob= scene->getObject("MeshMomo");


	gkLogicTree *animtree= gkLogicManager::getSingleton().create();
	gkAnimationNode *anim= (gkAnimationNode*)animtree->createNode(NT_ANIM);
	anim->setFunction(gkAnimationNode::AF_LOOP);
	anim->setAnim("Momo_Run"); // the Action ID name


	gkLogicNode* mouse= animtree->createNode(NT_MOUSE);

	/// Tell the Node to update
	anim->getInputSocket(0)->setValue(true);	// Update

	// 1 == Blending
	anim->getInputSocket(2)->setValue(1.f);	 // Start
	anim->getInputSocket(3)->setValue(25.f);	// End
	anim->getInputSocket(4)->setValue(25.f);	// Speed

	// link RelX -> Ogre::Animation Speed
	//anim->getInputSocket(4)->link(mouse->getOutputSocket(1));	// Speed
	ob->attachLogic(animtree);
}



OgreBlend::OgreBlend(class btDynamicsWorld* destinationWorld) :
	BulletBlendReaderNew(destinationWorld),
	m_file(0), m_scene(0), m_blenScene(0)
{
	m_engine = new gkEngine();
	m_engine->initialize(false);
}


OgreBlend::~OgreBlend()
{
	delete m_file;
	delete m_engine;
}


int OgreBlend::_readFile(char *fname)
{
	// TEMP: so manual loaders can be registered and freed
	// (mainly image loaders, which are internal to the mesh loader)

	m_file = new gkBlendFile(fname, "General", false);
	if (!m_file->_parseFile())
		return 0;

	// pass ownership
	m_blendFile = m_file->getInternalFile();
	m_curFile = fname;
	return 1;
}


void OgreBlend::load()
{
	if (m_scene)
	{

		if (m_curFile.find("momo_ogreSmallAnim.blend") != -1)
		{

			loadMomo(m_scene);
			m_scene->load();

		}
		else
		{
			m_scene->load();
			gkCameraObject *cam = createMouseLook(m_scene, Ogre::Vector3(0, -7, 2), Ogre::Vector3(90, 0,0));
			cam->setMainCamera(true);
			createExitHook(m_scene);
		}
	}
}

void OgreBlend::beginScene(Blender::Scene *scene)
{
	if (m_scene) return;

	m_blenScene = scene;

	// set scene
	m_scene = (gkSceneObject*)gkSceneObjectManager::getSingleton().create(scene->id.name + 2, "General").getPointer();

	if (m_destinationWorld)
	{
		m_scene->setDynamicsWorld(m_destinationWorld);
		m_destinationWorld->setGravity(btVector3(0,0,-10));
	}


	// needed for animation loop (Defined in render buttons)
	gkLoaderUtils::blender_anim_rate= scene->r.frs_sec;

	// apply to ogre animations as well
	m_engine->getUserDefs().animspeed= scene->r.frs_sec;

	if (scene->world)
	{
		// some sort of skybox system with world->mtex?
		Blender::World *world= scene->world;

		gkSceneProperties& props= m_scene->getProperties();

		if (m_destinationWorld)
			m_destinationWorld->setGravity(btVector3(0,0, -world->gravity));

		props.ambient.r= world->ambr;
		props.ambient.g= world->ambg;
		props.ambient.b= world->ambb;

		props.world_color.r= world->horr;
		props.world_color.g= world->horg;
		props.world_color.b= world->horb;
	}
}



void OgreBlend::convertLogicBricks()
{
}


void OgreBlend::createParentChildHierarchy()
{
	// apply the correct bullet hierarchy

	int i;
	btAlignedObjectArray<btCollisionObject*>children;

	for (i=0;i<this->m_destinationWorld->getNumCollisionObjects();i++)
	{
		btCollisionObject* childColObj = m_destinationWorld->getCollisionObjectArray()[i];
		BulletObjectContainer* childNode = (BulletObjectContainer*)childColObj->getUserPointer();
		if (!childNode)
			continue;
		Blender::Object* obj = (Blender::Object*)childNode->m_userPointer;
		if (!obj)
			continue;
		if (obj->parent)
			children.push_back(childColObj);
	}

	for (i=0;i<children.size();i++)
	{
		btCollisionObject* childColObj = children[i];
		m_destinationWorld->removeCollisionObject(childColObj);
		///disconnect the rigidbody update for child objects
		childColObj->setUserPointer(0);
	}
}


void* OgreBlend::createGraphicsObject(Blender::Object* tmpObject, class btCollisionObject* bulletObject)
{
	if (!m_scene)
	{
		printf("No scene present, cannot create objects!\n");
		return 0;
	}

	if (tmpObject->type != OB_MESH)
	{
		printf("Object Type = %i not %i?\n", tmpObject->type, OB_MESH);
		return 0;
	}


	gkMeshLoader *loader = new gkMeshLoader(m_file, tmpObject);

	// Create the ogre mesh
	m_scene->createMesh(tmpObject->id.name + 2, loader);
	m_file->_registerLoader(loader);


	gkEntityObject *obj = m_scene->createEntity(tmpObject->id.name + 2);
	applyObjectProperties(tmpObject, obj);


	gkEntityProperties& props= obj->getEntityProperties();
	props.source= tmpObject->id.name + 2;

	// find armature start pose
	if (tmpObject->parent)
	{
		Blender::Object *par= tmpObject->parent;
		Blender::bAction *act= par->action;
		if (!act && par->proxy_from)
			act= par->proxy_from->action;

		if (act)
			obj->_setStartPose(act->id.name + 2);
	}



	btRigidBody* body = bulletObject ? btRigidBody::upcast(bulletObject) : 0;
	OgreMotionState* newMotionState = 0;

	if (body)
	{
		if (!bulletObject->isStaticOrKinematicObject())
		{
			newMotionState = new OgreMotionState();

			newMotionState->setObject(obj);
			obj->attachRigidBody(body);
			body->setMotionState(newMotionState);
		}
	}

	tmpObject->vnode = bulletObject;
	BulletObjectContainer *cont = new BulletObjectContainer;
	cont->m_userPointer = tmpObject;
	return cont;
}


void OgreBlend::_convertConstraints(Blender::Object *tmpObject, gkGameObject *obj)
{
	for (Blender::bConstraint *bc = (Blender::bConstraint*)tmpObject->constraints.first; bc; bc = bc->next)
    {
        if (bc->enforce == 0.0) continue;

		if (bc->type == CONSTRAINT_TYPE_ROTLIMIT)
        {
			Blender::bRotLimitConstraint *lr = (Blender::bRotLimitConstraint*)bc->data;
            if (!lr->flag)
                continue;

			gkLimitRotConstraint *c = new gkLimitRotConstraint();
            if (lr->flag & LIMIT_XROT)
                c->setLimitX(Ogre::Vector2(lr->xmin, lr->xmax));
            if (lr->flag & LIMIT_YROT)
                c->setLimitY(Ogre::Vector2(lr->ymin, lr->ymax));
            if (lr->flag & LIMIT_ZROT)
                c->setLimitZ(Ogre::Vector2(lr->zmin, lr->zmax));


            if (bc->ownspace == CONSTRAINT_SPACE_LOCAL)
                c->setLocalSpace();

            c->setInfluence(bc->enforce);
            obj->addConstraint(c);
        }

        else if (bc->type == CONSTRAINT_TYPE_LOCLIMIT)
        {
            Blender::bLocLimitConstraint *ll = (Blender::bLocLimitConstraint*)bc->data;
            if (!ll->flag)
                continue;

            gkLimitLocConstraint *c = new gkLimitLocConstraint();

            if (ll->flag & LIMIT_XMIN) c->setMinX(ll->xmin);
            if (ll->flag & LIMIT_XMAX) c->setMaxX(ll->xmax);
            if (ll->flag & LIMIT_YMIN) c->setMinY(ll->ymin);
            if (ll->flag & LIMIT_YMAX) c->setMaxY(ll->ymax);
            if (ll->flag & LIMIT_ZMIN) c->setMinZ(ll->zmin);
            if (ll->flag & LIMIT_ZMAX) c->setMaxZ(ll->zmax);

            if (bc->ownspace == CONSTRAINT_SPACE_LOCAL)
                c->setLocalSpace();

            c->setInfluence(bc->enforce);
            obj->addConstraint(c);
        }
    }
}


void OgreBlend::addArmature(Blender::Object* tmpObject)
{
	if (!m_scene || !m_blenScene)
	{
		printf("No scene present, cannot create objects!\n");
		return;
	}

	if (m_scene->hasObject(tmpObject->id.name + 2))
	{
		printf("Duplicate object/conversion call on object %s\n", tmpObject->id.name + 2);
		return;
	}

	gkGameObject *obj= m_scene->createObject(tmpObject->id.name + 2);
	applyObjectProperties(tmpObject, obj);

	gkSkeletonLoader *client= new gkSkeletonLoader(m_file, tmpObject);
	m_file->_registerLoader(client);
	m_scene->createSkeleton(tmpObject->id.name + 2, client);
}


void OgreBlend::addEmpty(Blender::Object* tmpObject)
{
	if (!m_scene || !m_blenScene)
	{
		printf("No scene present, cannot create objects!\n");
		return;
	}

	if (m_scene->hasObject(tmpObject->id.name + 2))
	{
		printf("Duplicate object/conversion call on object %s\n", tmpObject->id.name + 2);
		return;
	}

	gkGameObject *obj= m_scene->createObject(tmpObject->id.name + 2);
	applyObjectProperties(tmpObject, obj);
}


void OgreBlend::addCamera(Blender::Object* tmpObject)
{
	if (!m_scene || !m_blenScene)
	{
		printf("No scene present, cannot create objects!\n");
		return;
	}
	if (m_scene->hasObject(tmpObject->id.name + 2))
	{
		printf("Duplicate object/conversion call on object %s\n", tmpObject->id.name + 2);
		return;
	}
	gkCameraObject *obj= m_scene->createCamera(tmpObject->id.name + 2);
	applyObjectProperties(tmpObject, obj);

	Blender::Camera* camera= static_cast<Blender::Camera*>(tmpObject->data);

	gkCameraProperties &props= obj->getCameraProperties();

	props.clipend	=	camera->clipend;
	props.clipstart	=	camera->clipsta;
	props.fov		=   Ogre::Degree(camera->angle);

	if (camera->angle == 0.0)
		props.fov = Ogre::Degree(Ogre::Real(360) * Ogre::Math::ATan(Ogre::Real(16) / camera->lens).valueRadians() / gkPi);

	if (m_blenScene->camera == tmpObject)
		props.start = true;
}

void OgreBlend::addLight(Blender::Object* tmpObject)
{
	if (!m_scene)
	{
		printf("No scene present, cannot create objects!\n");
		return;
	}
	if (m_scene->hasObject(tmpObject->id.name + 2))
	{
		printf("Duplicate object/conversion call on object %s\n", tmpObject->id.name + 2);
		return;
	}

	gkLightObject *obj= m_scene->createLight(tmpObject->id.name + 2);
	applyObjectProperties(tmpObject, obj);

	// convert/set light properties


	gkLightProperties &props= obj->getLightProperties();
	Blender::Lamp* la= static_cast<Blender::Lamp*>(tmpObject->data);

	props.diffuse= Ogre::ColourValue(la->r, la->g, la->b);
	if (la->mode & LA_NO_DIFF)
		props.diffuse= Ogre::ColourValue::Black;

	props.specular= Ogre::ColourValue(la->r, la->g, la->b);
	if (la->mode & LA_NO_SPEC)
		props.specular= Ogre::ColourValue::Black;

	props.power= la->energy;
	if (la->mode & LA_NEG)
		props.power= -props.power;

	props.linear= la->att1 / la->dist;
	props.constant= 1.0;
	props.quadratic= la->att2 / (la->dist * la->dist);

	props.type= Ogre::Light::LT_POINT;
	if (la->type != LA_LOCAL)
		props.type= la->type == LA_SPOT ? Ogre::Light::LT_SPOTLIGHT : Ogre::Light::LT_DIRECTIONAL;

	// lamp casts shadows
	props.casts= true;

	props.spot_inner= Ogre::Radian(la->spotblend).valueDegrees();
	props.spot_outer= la->spotsize > 128 ? 128 : la->spotsize;
	props.falloff=   128.0 * la->spotblend;

	if (props.spot_inner > props.spot_outer)
		props.spot_inner= props.spot_outer;
}


void OgreBlend::applyObjectProperties(Blender::Object* tmpObject, gkGameObject *obj)
{
	if (!tmpObject || !obj)
	{
		printf("Missing object arguments\n");
		return;
	}

	Ogre::Quaternion quat;
	Ogre::Vector3 loc, scale;

	Ogre::Matrix4 obmat = gkMathUtils::getFromFloat(tmpObject->obmat);

	if (!tmpObject->parent)
		gkMathUtils::extractTransform(obmat, loc, quat, scale);
	else
	{
		Ogre::Matrix4 parent = gkMathUtils::getFromFloat(tmpObject->parent->obmat);

		obmat = parent.inverse() * obmat;
		gkMathUtils::extractTransform(obmat, loc, quat, scale);

		// blender lists are sorted internally
		// so parents will be first in the base loop
		if (m_scene->hasObject(tmpObject->parent->id.name + 2))
		{
			gkGameObject *parob= m_scene->getObject(tmpObject->parent->id.name + 2);
			obj->setParent(parob);
		}
	}

	gkGameObjectProperties &props= obj->getProperties();
	props.position= loc;
	props.orientation= quat;
	props.scale= scale;


	// not needed, because calling loop blocks this
	obj->setActiveLayer(true);


	// apply local gkConstraints
	_convertConstraints(tmpObject, obj);
}
