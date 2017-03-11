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
const std::array<std::string, 4> World::Btypes
  = {{"None", "BruteForce", "Sweep & Prune", "Bounding Volume Tree"}};

World::World(float _timeStep, BroadPhase::Type iBroadPhaseType,
             unsigned int _numIterations, bool noStat)
    : timeStep{_timeStep}
    , timerate{timeStep * 1000.f}
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
  switch (iBroadPhaseType) {
    case BroadPhase::Type::BR_BRUTE_FORCE:
      // BruteForce
      broadPhase     = make_unique<BruteForceBroadPhase>();
      broadPhaseType = Btypes[1];
      break;
    case BroadPhase::Type::BR_SWEEP_AND_PRUNE:
      // Sweep & Prune
      broadPhase     = make_unique<SAPBroadPhase>();
      broadPhaseType = Btypes[2];
      break;
    case BroadPhase::Type::BR_BOUNDING_VOLUME_TREE:
      // Bounding Volume Tree
      broadPhase     = make_unique<DBVTBroadPhase>();
      broadPhaseType = Btypes[3];
      break;
    default:
    case BroadPhase::Type::BR_NULL:
      broadPhase     = make_unique<SAPBroadPhase>();
      broadPhaseType = Btypes[0];
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
    = make_unique<SphereSphereCollisionDetector>();
  detectors[shapeSphereType][shapeBoxType]
    = make_unique<SphereBoxCollisionDetector>(false);
  // BOX add
  detectors[shapeBoxType][shapeSphereType]
    = make_unique<SphereBoxCollisionDetector>(true);
  detectors[shapeBoxType][shapeBoxType]
    = make_unique<BoxBoxCollisionDetector>();
  // CYLINDER add
  detectors[shapeCylinderType][shapeCylinderType]
    = make_unique<CylinderCylinderCollisionDetector>();
  detectors[shapeCylinderType][shapeBoxType]
    = make_unique<BoxCylinderCollisionDetector>(true);
  detectors[shapeBoxType][shapeCylinderType]
    = make_unique<BoxCylinderCollisionDetector>(false);
  detectors[shapeCylinderType][shapeSphereType]
    = make_unique<SphereCylinderCollisionDetector>(true);
  detectors[shapeSphereType][shapeCylinderType]
    = make_unique<SphereCylinderCollisionDetector>(false);
  // TETRA add
  detectors[shapeTetraType][shapeTetraType]
    = make_unique<TetraTetraCollisionDetector>();
}

World::~World()
{
}

void World::setGravity(const std::vector<float>& ar)
{
  gravity.fromArray(ar);
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
  for (auto shape = rigidBody->shapes; shape != nullptr; shape = shape->next) {
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
  auto _remove = rigidBody;
  if (_remove->parent != this) {
    return;
  }
  _remove->awake();
  auto js = _remove->jointLink;
  while (js != nullptr) {
    auto joint = js->joint;
    js         = js->next;
    removeJoint(joint);
  }
  for (auto shape = rigidBody->shapes; shape != nullptr; shape = shape->next) {
    removeShape(shape);
  }
  auto prevBody = _remove->prev;
  auto nextBody = _remove->next;
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
  broadPhase->addProxy(shape->proxy.get());
}

void World::removeShape(Shape* shape)
{
  broadPhase->removeProxy(shape->proxy.get());
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
  auto _remove   = joint;
  auto prevJoint = _remove->prev;
  auto nextJoint = _remove->next;
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
                                  [static_cast<unsigned int>(s2->type)]
                                    .get();
  if (contacts != nullptr) {
    contacts->prev       = newContact;
    contacts->prev->next = contacts;
  }
  contacts = newContact;
  ++numContacts;
}

void World::removeContact(Contact* contact)
{
  auto prevContact = contact->prev;
  auto nextContact = contact->next;
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
  auto contact = contacts;
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
  bool stat = !isNoStat ? true : false;

  if (stat) {
    performance.setTime(0);
  }

  auto body = rigidBodies;

  while (body != nullptr) {
    body->addedToIsland = false;
    if (body->sleeping) {
      body->testWakeUp();
    }
    body = body->next;
  }

  //----------------------------------------------------------------------------
  //   UPDATE BROADPHASE CONTACT
  //----------------------------------------------------------------------------

  // broad phase
  if (stat) {
    performance.setTime(1);
  }

  broadPhase->detectPairs();
  std::vector<Pair>& pairs = broadPhase->pairs;
  Shape *s1 = nullptr, *s2 = nullptr;
  ContactLink* link = nullptr;
  Contact* contact  = nullptr;
  bool exists       = false;
  for (unsigned int i = broadPhase->numPairs - 1; i-- > 0;) {
    auto pair = pairs[i];
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
    performance.calcBroadPhase();
  }

  //----------------------------------------------------------------------------
  //   UPDATE NARROWPHASE CONTACT
  //----------------------------------------------------------------------------

  // update & narrow phase
  numContactPoints = 0;
  contact          = contacts;
  RigidBody *b1 = nullptr, *b2 = nullptr;
  while (contact != nullptr) {
    if (!contact->persisting) {
      if (contact->shape1->aabb->intersectTest(*contact->shape2->aabb)) {
        auto nextContact = contact->next;
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
    performance.calcNarrowPhase();
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

  if (stat) {
    performance.setTime(1);
  }

  numIslands = 0;

  // build and solve simulation islands
  unsigned int islandNumRigidBodies = 0;
  unsigned int islandNumConstraints = 0;
  unsigned int stackCount           = 1;
  ContactLink* cs                   = nullptr;
  contact                           = nullptr;
  RigidBody* nextRigidBody          = nullptr;
  for (auto base = rigidBodies; base != nullptr; base = base->next) {

    if (base->addedToIsland || base->isStatic || base->sleeping) {
      // ignore
      continue;
    }

    if (base->isLonely()) { // update single body
      if (base->isDynamic) {
        base->linearVelocity.addScaledVector(gravity, timeStep);
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
        constraint = contact->constraint.get();
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
      for (auto js = body->jointLink; js != nullptr; js = js->next) {
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
    auto gVel = Vec3().addScaledVector(gravity, timeStep);
    for (unsigned int j = islandNumRigidBodies - 1; j-- > 0;) {
      body = islandRigidBodies[j];
      if (body->isDynamic) {
        body->linearVelocity.addEqual(gVel);
      }
    }

    // randomizing order
    if (enableRandomizer) {
      for (unsigned int j = islandNumRigidBodies - 1; j-- > 0;) {
        randX = ((randX * randA) + (randB & 0x7fffffff));
        float tmp
          = static_cast<float>(randX) / static_cast<float>(2147483648 * j);
        unsigned int swap       = static_cast<unsigned int>(tmp) | 0;
        constraint              = islandConstraints[j];
        islandConstraints[j]    = islandConstraints[swap];
        islandConstraints[swap] = constraint;
      }
    }

    // solve contraints
    for (unsigned int j = islandNumConstraints - 1; j-- > 0;) {
      // pre-solve
      islandConstraints[j]->preSolve(timeStep, invTimeStep);
    }
    for (unsigned int k = 0; k < numIterations; ++k) {
      for (unsigned int j = islandNumConstraints - 1; j-- > 0;) {
        // main-solve
        islandConstraints[j]->solve();
      }
    }
    for (unsigned int j = islandNumConstraints - 1; j-- > 0;) {
      islandConstraints[j]->postSolve(); // post-solve
      islandConstraints[j] = nullptr;    // gc
    }

    // sleeping check
    float sleepTime = 10.f;
    for (unsigned int j = islandNumRigidBodies - 1; j-- > 0;) {
      body = islandRigidBodies[j];
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
      for (unsigned int j = islandNumRigidBodies - 1; j-- > 0;) {
        islandRigidBodies[j]->sleep();
        islandRigidBodies[j] = nullptr; // gc
      }
    }
    else {
      // update positions
      for (unsigned int j = islandNumRigidBodies - 1; j-- > 0;) {
        islandRigidBodies[j]->updatePosition(timeStep);
        islandRigidBodies[j] = nullptr; // gc
      }
    }
    ++numIslands;
  }

  //----------------------------------------------------------------------------
  //   END SIMULATION
  //----------------------------------------------------------------------------

  if (stat) {
    performance.calcEnd();
  }
}

} // end of namespace OIMO
