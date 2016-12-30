#ifndef OIMO_CONSTRAINT_CONTACT_CONTACT_LINK_H
#define OIMO_CONSTRAINT_CONTACT_CONTACT_LINK_H

namespace OIMO {

class RigidBody;
class Contact;
class Shape;

/**
 * @brief A link list of contacts.
 */
struct ContactLink {

  ContactLink(Contact* _contact)
      : prev{nullptr}
      , next{nullptr}
      , shape{nullptr}
      , body{nullptr}
      , contact{_contact}
  {
  }

  // The previous contact link.
  ContactLink* prev;
  // The next contact link.
  ContactLink* next;
  // The shape of the contact.
  Shape* shape;
  // The other rigid body.
  RigidBody* body;
  // The contact of the link.
  Contact* contact;

}; // end of struct ContactLink

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_CONTACT_CONTACT_LINK_H
