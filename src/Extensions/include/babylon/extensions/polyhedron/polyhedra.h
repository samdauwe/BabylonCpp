#ifndef BABYLON_EXTENSIONS_POLYHEDRON_POLYHEDRA_H
#define BABYLON_EXTENSIONS_POLYHEDRON_POLYHEDRA_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>

namespace BABYLON {
namespace Extensions {

/**
 * @brief Contains the data from the website "Virtual Polyhedra: The
 * Encyclopedia of Polyhedra" by George W. Hart :
 * http://www.georgehart.com/virtual-polyhedra/vp.html
 * George Hart Wikipedia article : https://en.wikipedia.org/wiki/George_W._Hart
 * These data were stored by George Hart in WRL files, so please read the terms
 * of use : http://www.georgehart.com/virtual-polyhedra/copyright.html
 *
 * These data were then converted to JSON by Lee Stemkoski
 * for this ThreeJS demo : http://stemkoski.github.io/Three.js/Polyhedra.html
 * Stemkoski's original data file is here :
 * http://stemkoski.github.io/Three.js/js/polyhedra.js
 *
 * This file contains a list of 126 Polyhedron objects.
 */
class BABYLON_SHARED_EXPORT Polyhedra {

public:
  Polyhedra();
  ~Polyhedra(); // = default

  const std::array<Polyhedron, 126>& models() const;

private:
  /* Platonic Solids */
  static const Polyhedron Tetrahedron;
  static const Polyhedron Cube;
  static const Polyhedron Octahedron;
  static const Polyhedron Dodecahedron;
  static const Polyhedron Icosahedron;

  /* Archimedean Solids */
  static const Polyhedron TruncatedTetrahedron;
  static const Polyhedron TruncatedCube;
  static const Polyhedron TruncatedOctahedron;
  static const Polyhedron TruncatedDodecahedron;
  static const Polyhedron TruncatedIcosahedron;
  static const Polyhedron Cuboctahedron;
  static const Polyhedron TruncatedCubocahedron;
  static const Polyhedron Rhombicubocahedron;
  static const Polyhedron SnubCuboctahedron;
  static const Polyhedron Icosidodecahedron;
  static const Polyhedron TruncatedIcosidodecahedron;
  static const Polyhedron Rhombicosidodecahedron;
  static const Polyhedron SnubIcosidodecahedron;

  /* Prisms */
  static const Polyhedron TriangularPrism;
  static const Polyhedron SquarePrism;
  static const Polyhedron PentagonalPrism;
  static const Polyhedron HexagonalPrism;
  static const Polyhedron HeptagonalPrism;
  static const Polyhedron OctagonalPrism;
  static const Polyhedron EnneagonalPrism;
  static const Polyhedron DecagonalPrism;

  /* Antiprisms */
  // Triangular Antiprism == Octahedron
  static const Polyhedron TriangularAntiprism;
  static const Polyhedron SquareAntiPrism;
  static const Polyhedron PentagonalAntiPrism;
  static const Polyhedron HexagonalAntiprism;
  static const Polyhedron HeptagonalAntiprism;
  static const Polyhedron OctagonalAntiprism;
  static const Polyhedron EnneagonalAntiprism;
  static const Polyhedron DecagonalAntiprism;

  /* Johnson Solids */
  static const Polyhedron J1;
  static const Polyhedron J2;
  static const Polyhedron J3;
  static const Polyhedron J4;
  static const Polyhedron J5;
  static const Polyhedron J6;
  static const Polyhedron J7;
  static const Polyhedron J8;
  static const Polyhedron J9;
  static const Polyhedron J10;
  static const Polyhedron J11;
  static const Polyhedron J12;
  static const Polyhedron J13;
  static const Polyhedron J14;
  static const Polyhedron J15;
  static const Polyhedron J16;
  static const Polyhedron J17;
  static const Polyhedron J18;
  static const Polyhedron J19;
  static const Polyhedron J20;
  static const Polyhedron J21;
  static const Polyhedron J22;
  static const Polyhedron J23;
  static const Polyhedron J24;
  static const Polyhedron J25;
  static const Polyhedron J26;
  static const Polyhedron J27;
  static const Polyhedron J28;
  static const Polyhedron J29;
  static const Polyhedron J30;
  static const Polyhedron J31;
  static const Polyhedron J32;
  static const Polyhedron J33;
  static const Polyhedron J34;
  static const Polyhedron J35;
  static const Polyhedron J36;
  static const Polyhedron J37;
  static const Polyhedron J38;
  static const Polyhedron J39;
  static const Polyhedron J40;
  static const Polyhedron J41;
  static const Polyhedron J42;
  static const Polyhedron J43;
  static const Polyhedron J44;
  static const Polyhedron J45;
  static const Polyhedron J46;
  static const Polyhedron J47;
  static const Polyhedron J48;
  static const Polyhedron J49;
  static const Polyhedron J50;
  static const Polyhedron J51;
  static const Polyhedron J52;
  static const Polyhedron J53;
  static const Polyhedron J54;
  static const Polyhedron J55;
  static const Polyhedron J56;
  static const Polyhedron J57;
  static const Polyhedron J58;
  static const Polyhedron J59;
  static const Polyhedron J60;
  static const Polyhedron J61;
  static const Polyhedron J62;
  static const Polyhedron J63;
  static const Polyhedron J64;
  static const Polyhedron J65;
  static const Polyhedron J66;
  static const Polyhedron J67;
  static const Polyhedron J68;
  static const Polyhedron J69;
  static const Polyhedron J70;
  static const Polyhedron J71;
  static const Polyhedron J72;
  static const Polyhedron J73;
  static const Polyhedron J74;
  static const Polyhedron J75;
  static const Polyhedron J76;
  static const Polyhedron J77;
  static const Polyhedron J78;
  static const Polyhedron J79;
  static const Polyhedron J80;
  static const Polyhedron J81;
  static const Polyhedron J82;
  static const Polyhedron J83;
  static const Polyhedron J84;
  static const Polyhedron J85;
  static const Polyhedron J86;
  static const Polyhedron J87;
  static const Polyhedron J88;
  static const Polyhedron J89;
  static const Polyhedron J90;
  static const Polyhedron J91;
  static const Polyhedron J92;

  /* List of 126 Polyhedron objects */
  static const std::array<Polyhedron, 126> _models;

}; // end of class Polyhedra

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_POLYHEDRON_POLYHEDRA_H
