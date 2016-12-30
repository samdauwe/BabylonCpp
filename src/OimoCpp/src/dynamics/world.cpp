#include <oimo/dynamics/world.h>

#include <oimo/collision/broadphase/aabb.h>
#include <oimo/collision/broadphase/broad_phase.h>
#include <oimo/collision/broadphase/brute_force_broad_phase.h>
#include <oimo/collision/broadphase/dbvt/dbvt_broad_phase.h>
#include <oimo/collision/broadphase/proxy.h>
#include <oimo/collision/broadphase/sap/sap_broad_phase.h>
#include <oimo/collision/narrowphase/box_box_collision_detector.h>
#include <oimo/collision/narrowphase/box_cylinder_collision_detector.h>
#include <oimo/collision/narrowphase/cylinder_cylinder_collision_detector.h>
#include <oimo/collision/narrowphase/sphere_box_collision_detector.h>
#include <oimo/collision/narrowphase/sphere_cylinder_collision_detector.h>
#include <oimo/collision/narrowphase/sphere_sphere_collision_detector.h>
#include <oimo/collision/narrowphase/tetra_tetra_collision_detector.h>
#include <oimo/collision/shape/shape.h>
#include <oimo/constraint/contact/contact.h>
#include <oimo/constraint/contact/contact_constraint.h>
#include <oimo/constraint/contact/contact_link.h>
#include <oimo/constraint/contact/contact_manifold.h>
#include <oimo/constraint/joint/joint.h>
#include <oimo/constraint/joint/joint_link.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

float World::WORLD_SCALE = 100.f;
float World::INV_SCALE   = 0.01f;

World::World(float _timeStep, BroadPhase::Type broadPhaseType,
             unsigned int _numIterations, bool noStat)
    : timeStep{_timeStep}
    , numIterations{_numIterations}
    , performance{Performance(this)}
    , isNoStat{noStat}
    , rigidBodies{nullptr}
    , numRigidBodies{0}
    , contacts{nullptr}
    , unusedContacts{nullptr}
    , numContacts{0}
    , numContactPoints{0}
    , joints{nullptr}
    , numJoints{0}
    , numIslands{0}
    , gravity{Vec3(0.f, -9.80665f, 0.f)}
    , randX{65535}
    , randA{98765}
    , randB{123456789}
{
  // Broad phase
  switch (broadPhaseType) {
    case BroadPhase::Type::BR_BRUTE_FORCE:
      broadPhase = new BruteForceBroadPhase();
      break;
    case BroadPhase::Type::BR_SWEEP_AND_PRUNE:
      broadPhase = new SAPBroadPhase();
      break;
    case BroadPhase::Type::BR_BOUNDING_VOLUME_TREE:
      broadPhase = new DBVTBroadPhase();
      break;
    default:
    case BroadPhase::Type::BR_NULL:
      broadPhase = new SAPBroadPhase();
  }
  // Convert enums to integers
  unsigned int shapeBoxType = static_cast<unsigned int>(Shape::Type::SHAPE_BOX);
  unsigned int shapeCylinderType
    = static_cast<unsigned int>(Shape::Type::SHAPE_CYLINDER);
  unsigned int shapeSphereType
    = static_cast<unsigned int>(Shape::Type::SHAPE_SPHERE);
  unsigned int shapeTetraType
    = static_cast<unsigned int>(Shape::Type::SHAPE_TETRA);
  // Detectors
  // SPHERE add
  detectors[shapeSphereType][shapeSphereType]
    = new SphereSphereCollisionDetector();
  detectors[shapeSphereType][shapeBoxType]
    = new SphereBoxCollisionDetector(false);
  // BOX add
  detectors[shapeBoxType][shapeSphereType]
    = new SphereBoxCollisionDetector(true);
  detectors[shapeBoxType][shapeBoxType] = new BoxBoxCollisionDetector();
  // CYLINDER add
  detectors[shapeCylinderType][shapeCylinderType]
    = new CylinderCylinderCollisionDetector();
  detectors[shapeCylinderType][shapeBoxType]
    = new BoxCylinderCollisionDetector(true);
  detectors[shapeBoxType][shapeCylinderType]
    = new BoxCylinderCollisionDetector(false);
  detectors[shapeCylinderType][shapeSphereType]
    = new SphereCylinderCollisionDetector(true);
  detectors[shapeSphereType][shapeCylinderType]
    = new SphereCylinderCollisionDetector(false);
  // TETRA add
  detectors[shapeTetraType][shapeTetraType] = new TetraTetraCollisionDetector();
}

World::~World()
{
}

void World::clear()
{
  randX = 65535;
  while (joints != nullptr) {
    removeJoint(joints);
  }
  while (contacts != nullptr) {
    removeContact(contacts);
  }
  while (rigidBodies != nullptr) {
    removeRigidBody(rigidBodies);
  }

  Shape::s_nextID  = 0;
  Proxy::s_proxyID = 0;
}

void World::addRigidBody(RigidBody* rigidBody)
{
  if (rigidBody->parent != nullptr) {
    // It is not possible to be added to more than one world one of the rigid
    // body
    return;
  }
  rigidBody->parent = this;
  rigidBody->awake();
  for (Shape* shape = rigidBody->shapes; shape != nullptr;
       shape        = shape->next) {
    addShape(shape);
  }
  if (rigidBodies != nullptr) {
    rigidBodies->prev       = rigidBody;
    rigidBodies->prev->next = rigidBodies;
  }
  rigidBodies = rigidBody;
  ++numRigidBodies;
}

void World::removeRigidBody(RigidBody* rigidBody)
{
  RigidBody* _remove = rigidBody;
  if (_remove->parent != this) {
    return;
  }
  _remove->awake();
  JointLink* js = _remove->jointLink;
  while (js != nullptr) {
    Joint* joint = js->joint;
    js           = js->next;
    removeJoint(joint);
  }
  for (Shape* shape = rigidBody->shapes; shape != nullptr;
       shape        = shape->next) {
    removeShape(shape);
  }
  RigidBody* prevBody = _remove->prev;
  RigidBody* nextBody = _remove->next;
  if (prevBody != nullptr) {
    prevBody->next = nextBody;
  }
  if (nextBody != nullptr) {
    nextBody->prev = prevBody;
  }
  if (rigidBodies == _remove) {
    rigidBodies = nextBody;
  }
  _remove->prev   = nullptr;
  _remove->next   = nullptr;
  _remove->parent = nullptr;
  --numRigidBodies;
}

void World::addShape(Shape* shape)
{
  if ((shape->parent == nullptr) || (shape->parent->parent == nullptr)) {
    // It is not possible to be added alone to shape world
    return;
  }
  shape->proxy = broadPhase->createProxy(shape);
  shape->updateProxy();
  broadPhase->addProxy(shape->proxy);
}

void World::removeShape(Shape* shape)
{
  broadPhase->removeProxy(shape->proxy);
  shape->proxy = nullptr;
}

void World::addJoint(Joint* joint)
{
  if (joint->parent != nullptr) {
    // It is not possible to be added to more than one world one of the joint
    return;
  }
  if (joints != nullptr) {
    joints->prev = joint;
    joints->next = joints;
  }
  joints        = joint;
  joint->parent = this;
  ++numJoints;
  joint->awake();
  joint->attach();
}

void World::removeJoint(Joint* joint)
{
  Joint* _remove   = joint;
  Joint* prevJoint = _remove->prev;
  Joint* nextJoint = _remove->next;
  if (prevJoint != nullptr) {
    prevJoint->next = nextJoint;
  }
  if (nextJoint != nullptr) {
    nextJoint->prev = prevJoint;
  }
  if (joints == _remove) {
    joints = nextJoint;
  }
  _remove->prev = nullptr;
  _remove->next = nullptr;
  --numJoints;
  _remove->awake();
  _remove->detach();
  _remove->parent = nullptr;
}

void World::setWorldscale(float scale)
{
  World::WORLD_SCALE = scale;
  World::INV_SCALE   = 1.f / World::WORLD_SCALE;
}

void World::addContact(Shape* s1, Shape* s2)
{
  Contact* newContact = nullptr;
  if (unusedContacts != nullptr) {
    newContact     = unusedContacts;
    unusedContacts = unusedContacts->next;
  }
  else {
    newContact = new Contact();
  }
  newContact->attach(s1, s2);
  newContact->detector = detectors[static_cast<unsigned int>(s1->type)]
                                  [static_cast<unsigned int>(s2->type)];
  if (contacts != nullptr) {
    contacts->prev       = newContact;
    contacts->prev->next = contacts;
  }
  contacts = newContact;
  ++numContacts;
}

void World::removeContact(Contact* contact)
{
  Contact* prevContact = contact->prev;
  Contact* nextContact = contact->next;
  if (nextContact != nullptr) {
    nextContact->prev = prevContact;
  }
  if (prevContact != nullptr) {
    prevContact->next = nextContact;
  }
  if (contacts == contact) {
    contacts = nextContact;
  }
  contact->prev = nullptr;
  contact->next = nullptr;
  contact->detach();
  contact->next  = unusedContacts;
  unusedContacts = contact;
  --numContacts;
}

bool World::checkContact(const std::string& name1, const std::string& name2)
{
  std::string n1, n2;
  Contact* contact = contacts;
  while (contact != nullptr) {
    n1 = contact->body1->name;
    n2 = contact->body2->name;
    if ((n1 == name1 && n2 == name2) || (n2 == name1 && n1 == name2)) {
      if (contact->touching) {
        return true;
      }
      else {
        return false;
      }
    }
    else {
      contact = contact->next;
    }
  }
  return false;
}

bool World::callSleep(RigidBody* body)
{
  if (!body->allowSleep) {
    return false;
  }
  if (body->linearVelocity.lengthSq() > 0.04f) {
    return false;
  }
  if (body->angularVelocity.lengthSq() > 0.25f) {
    return false;
  }
  return true;
}

void World::step()
{
  //std::time_t time0 = 0, time1 = 0, time2 = 0, time3 = 0;

  bool stat = !isNoStat ? true : false;

  if (stat) {
    //time0 = highres_time_now();
  }

  RigidBody* body = rigidBodies;

  while (body != nullptr) {
    body->addedToIsland = false;
    if (body->sleeping) {
      if (body->linearVelocity.testZero() || body->angularVelocity.testZero()
          || body->position.testDiff(body->sleepPosition)
          || body->orientation.testDiff(body->sleepOrientation)) {
        // awake the body
        body->awake();
      }
    }
    body = body->next;
  }

  //----------------------------------------------------------------------------
  //   UPDATE CONTACT
  //----------------------------------------------------------------------------

  // broad phase
  if (stat) {
    //time1 = highres_time_now();
  }

  broadPhase->detectPairs();
  std::vector<Pair>& pairs = broadPhase->pairs;
  Shape *s1 = nullptr, *s2 = nullptr;
  ContactLink* link = nullptr;
  Contact* contact  = nullptr;
  bool exists       = false;
  for (unsigned int i = broadPhase->numPairs; i > 0; --i) {
    Pair& pair = pairs[i - 1];
    if (pair.shape1->id < pair.shape2->id) {
      s1 = pair.shape1;
      s2 = pair.shape2;
    }
    else {
      s1 = pair.shape2;
      s2 = pair.shape1;
    }

    if (s1->numContacts < s2->numContacts) {
      link = s1->contactLink;
    }
    else {
      link = s2->contactLink;
    }

    exists = false;
    while (link != nullptr) {
      contact = link->contact;
      if (contact->shape1 == s1 && contact->shape2 == s2) {
        contact->persisting = true;
        exists              = true; // contact already exists
        break;
      }
      link = link->next;
    }
    if (!exists) {
      addContact(s1, s2);
    }
  }

  if (stat) {
    //time2                      = highres_time_now();
    //performance.broadPhaseTime = time2 - time1;
  }

  // update & narrow phase
  numContactPoints = 0;
  contact          = contacts;
  RigidBody *b1 = nullptr, *b2 = nullptr;
  while (contact != nullptr) {
    if (!contact->persisting) {
      if (contact->shape1->aabb->intersectTest(*contact->shape2->aabb)) {
        Contact* nextContact = contact->next;
        removeContact(contact);
        contact = nextContact;
        continue;
      }
    }
    b1 = contact->body1;
    b2 = contact->body2;
    if ((b1->isDynamic && !b1->sleeping) || (b2->isDynamic && !b2->sleeping)) {
      contact->updateManifold();
    }
    numContactPoints += contact->manifold->numPoints;
    contact->persisting                = false;
    contact->constraint->addedToIsland = false;
    contact                            = contact->next;
  }

  if (stat) {
    //time3                       = highres_time_now();
    //performance.narrowPhaseTime = time3 - time2;
  }

  //----------------------------------------------------------------------------
  //   SOLVE ISLANDS
  //----------------------------------------------------------------------------

  float invTimeStep = 1.f / timeStep;
  // var body;
  Joint* joint           = nullptr;
  Constraint* constraint = nullptr;
  for (joint = joints; joint != nullptr; joint = joint->next) {
    joint->addedToIsland = false;
  }

  // clear old island array
  islandRigidBodies.clear();
  islandConstraints.clear();
  islandStack.clear();

  //time1      = highres_time_now();
  numIslands = 0;

  // build and solve simulation islands
  unsigned int islandNumRigidBodies = 0;
  unsigned int islandNumConstraints = 0;
  unsigned int stackCount           = 1;
  ContactLink* cs                   = nullptr;
  contact                           = nullptr;
  RigidBody* nextRigidBody          = nullptr;
  for (RigidBody* base = rigidBodies; base != nullptr; base = base->next) {

    if (base->addedToIsland || base->isStatic || base->sleeping) {
      // ignore
      continue;
    }

    if (base->isLonely()) { // update single body
      if (base->isDynamic) {
        base->linearVelocity.addTime(gravity, timeStep);
      }
      if (callSleep(base)) {
        base->sleepTime += timeStep;
        if (base->sleepTime > 0.5f) {
          base->sleep();
        }
        else {
          base->updatePosition(timeStep);
        }
      }
      else {
        base->sleepTime = 0;
        base->updatePosition(timeStep);
      }
      ++numIslands;
      continue;
    }

    islandNumRigidBodies = 0;
    islandNumConstraints = 0;
    stackCount           = 1;
    // add rigid body to stack
    islandStack[0]      = base;
    base->addedToIsland = true;

    // build an island
    do {
      // get rigid body from stack
      body                    = islandStack[--stackCount];
      islandStack[stackCount] = nullptr;
      body->sleeping          = false;
      // add rigid body to the island
      islandRigidBodies[islandNumRigidBodies++] = body;
      if (body->isStatic) {
        continue;
      }

      // search connections
      for (cs = body->contactLink; cs != nullptr; cs = cs->next) {
        contact    = cs->contact;
        constraint = contact->constraint;
        if (constraint->addedToIsland || !contact->touching) {
          // ignore
          continue;
        }

        // add constraint to the island
        islandConstraints[islandNumConstraints++] = constraint;
        constraint->addedToIsland                 = true;
        nextRigidBody                             = cs->body;

        if (nextRigidBody->addedToIsland) {
          continue;
        }

        // add rigid body to stack
        islandStack[stackCount++]    = nextRigidBody;
        nextRigidBody->addedToIsland = true;
      }
      for (JointLink* js = body->jointLink; js != nullptr; js = js->next) {
        constraint = js->joint;
        if (constraint->addedToIsland) {
          // ignore
          continue;
        }
        // add constraint to the island
        islandConstraints[islandNumConstraints++] = constraint;
        constraint->addedToIsland                 = true;
        nextRigidBody                             = js->body;
        if (nextRigidBody->addedToIsland || !nextRigidBody->isDynamic) {
          continue;
        }
        // add rigid body to stack
        islandStack[stackCount++]    = nextRigidBody;
        nextRigidBody->addedToIsland = true;
      }
    } while (stackCount != 0);

    // update velocities
    Vec3 gVel = Vec3().addTime(gravity, timeStep);
    for (unsigned int j = islandNumRigidBodies; j > 0; --j) {
      body = islandRigidBodies[j - 1];
      if (body->isDynamic) {
        body->linearVelocity.addEqual(gVel);
      }
    }

    // randomizing order
    if (enableRandomizer) {
      // for(var j=1, l=islandNumConstraints; j<l; j++){
      unsigned int j = islandNumConstraints;
      while ((j--) > 0) {
        if (j != 0) {
          randX = ((randX * randA) + (randB & 0x7fffffff));
          float tmp
            = static_cast<float>(randX) / static_cast<float>(2147483648 * j);
          unsigned int swap       = static_cast<unsigned int>(tmp) | 0;
          constraint              = islandConstraints[j];
          islandConstraints[j]    = islandConstraints[swap];
          islandConstraints[swap] = constraint;
        }
      }
    }

    // solve contraints
    for (unsigned int j = islandNumConstraints; j > 0; --j) {
      // pre-solve
      islandConstraints[j - 1]->preSolve(timeStep, invTimeStep);
    }
    for (unsigned int k = numIterations; k > 0; --k) {
      for (unsigned int j = islandNumConstraints; j > 0; --j) {
        // main-solve
        islandConstraints[j - 1]->solve();
      }
    }
    for (unsigned int j = islandNumConstraints; j > 0; --j) {
      islandConstraints[j]->postSolve(); // post-solve
      islandConstraints[j] = nullptr;    // gc
    }

    // sleeping check
    float sleepTime = 10;
    for (unsigned int j = islandNumRigidBodies; j > 0; --j) {
      body = islandRigidBodies[j - 1];
      if (callSleep(body)) {
        body->sleepTime += timeStep;
        if (body->sleepTime < sleepTime) {
          sleepTime = body->sleepTime;
        }
      }
      else {
        body->sleepTime = 0.f;
        sleepTime       = 0.f;
        continue;
      }
    }
    if (sleepTime > 0.5f) {
      // sleep the island
      for (unsigned int j = islandNumRigidBodies; j > 0; --j) {
        islandRigidBodies[j - 1]->sleep();
        islandRigidBodies[j - 1] = nullptr; // gc
      }
    }
    else {
      // update positions
      for (unsigned int j = islandNumRigidBodies; j > 0; --j) {
        islandRigidBodies[j - 1]->updatePosition(timeStep);
        islandRigidBodies[j - 1] = nullptr; // gc
      }
    }
    ++numIslands;
  }

  if (stat) {
    //time2                   = highres_time_now();
    //performance.solvingTime = time2 - time1;

    //--------------------------------------------------------------------------
    //   END SIMULATION
    //--------------------------------------------------------------------------

    //time2 = highres_time_now();
    // fps update
    performance.upfps();
    //performance.totalTime = time2 - time0;
  }
}

} // end of namespace OIMO
