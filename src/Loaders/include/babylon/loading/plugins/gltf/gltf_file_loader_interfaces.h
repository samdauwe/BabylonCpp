#ifndef BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_LOADER_INTERFACES_H
#define BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_LOADER_INTERFACES_H

#include <babylon/babylon_global.h>
#include <babylon/loading/plugins/gltf/gltf_file_loader_enums.h>

namespace BABYLON {

/**
 * Interfaces
 */
struct IGLTFChildRootProperty {
  std::string name;
};

struct IGLTFAccessor : IGLTFChildRootProperty {
  std::string bufferView;
  unsigned int byteOffset;
  unsigned int byteStride;
  size_t count;
  std::string type;
  std::string componentType;
  Float32Array max;
  Float32Array min;
  std::string name;
};

struct IGLTFBufferView : IGLTFChildRootProperty {
  std::string buffer;
  unsigned int byteOffset;
  unsigned int byteLength;
  unsigned int target;
};

struct IGLTFBuffer : IGLTFChildRootProperty {
  std::string uri;
  size_t byteLength;
  std::string type;
};

struct IGLTFShader : IGLTFChildRootProperty {
  std::string uri;
  unsigned int type;
};

struct IGLTFProgram : IGLTFChildRootProperty {
  std::vector<std::string> attributes;
  std::string fragmentShader;
  std::string vertexShader;
};

struct IGLTFTechniqueParameter {
  EParameterType type;
  size_t count;
  std::string semantic;
  std::string node;
  std::string value;
  std::string source;
  std::string babylonValue;
};

struct IGLTFTechniqueCommonProfile {
  std::string lightingModel;
  std::string texcoordBindings;
  std::vector<std::string> parameters;
};

struct IGLTFTechniqueStatesFunctions {
  Float32Array blendColor;
  Float32Array blendEquationSeparate;
  Float32Array blendFuncSeparate;
  std::vector<bool> colorMask;
  Float32Array cullFace;
};

struct IGLTFTechniqueStates {
  Uint32Array enable;
  IGLTFTechniqueStatesFunctions functions;
};

struct IGLTFTechnique : IGLTFChildRootProperty {
  std::string parameters;
  std::string program;
  std::string attributes;
  std::string uniforms;
  IGLTFTechniqueStates states;
};

struct IGLTFMaterial : IGLTFChildRootProperty {
  std::string technique;
  std::vector<std::string> values;
};

struct IGLTFMeshPrimitive {
  std::vector<std::string> attributes;
  std::string indices;
  std::string material;
  unsigned int mode;
};

struct IGLTFMesh : IGLTFChildRootProperty {
  std::vector<IGLTFMeshPrimitive> primitives;
};

struct IGLTFImage : IGLTFChildRootProperty {
  std::string uri;
};

struct IGLTFSampler : IGLTFChildRootProperty {
  unsigned int magFilter;
  unsigned int minFilter;
  unsigned int wrapS;
  unsigned int wrapT;
};

struct IGLTFTexture : IGLTFChildRootProperty {
  std::string sampler;
  std::string source;
  unsigned int format;
  unsigned int internalFormat;
  unsigned int target;
  unsigned int type;
  // Babylon.js values (optimize)
  Texture* babylonTexture;
};

struct IGLTFAmbienLight {
  Float32Array color;
};

struct IGLTFDirectionalLight {
  Float32Array color;
};

struct IGLTFPointLight {
  Float32Array color;
  float constantAttenuation;
  float linearAttenuation;
  float quadraticAttenuation;
};

struct IGLTFSpotLight {
  Float32Array color;
  float constantAttenuation;
  float fallOfAngle;
  float fallOffExponent;
  float linearAttenuation;
  float quadraticAttenuation;
};

struct IGLTFLight : IGLTFChildRootProperty {
  std::string type;
};

struct IGLTFCameraOrthographic {
  float xmag;
  float ymag;
  float zfar;
  float znear;
};

struct IGLTFCameraPerspective {
  float aspectRatio;
  float yfov;
  float zfar;
  float znear;
};

struct IGLTFCamera : IGLTFChildRootProperty {
  std::string type;
};

struct IGLTFAnimationChannelTarget {
  std::string id;
  std::string path;
};

struct IGLTFAnimationChannel {
  std::string sampler;
  IGLTFAnimationChannelTarget target;
};

struct IGLTFAnimationSampler {
  std::string input;
  std::string output;
  std::string interpolation;
};

struct IGLTFAnimation : IGLTFChildRootProperty {
  std::vector<IGLTFAnimationChannel> channels;
  std::string parameters;
  std::string samplers;
};

struct IGLTFNodeInstanceSkin {
  std::vector<std::string> skeletons;
  std::string skin;
  std::vector<std::string> meshes;
};

struct IGLTFSkins : IGLTFChildRootProperty {
  Float32Array bindShapeMatrix;
  std::string inverseBindMatrices;
  std::vector<std::string> jointNames;
  Skeleton* babylonSkeleton;
};

struct IGLTFNode : IGLTFChildRootProperty {
  std::string camera;
  std::vector<std::string> children;
  std::string skin;
  std::string jointName;
  std::string light;
  Float32Array matrix;
  std::string mesh;
  std::vector<std::string> meshes;
  Float32Array rotation;
  Float32Array scale;
  Float32Array translation;
  // Babylon.js values (optimize)
  Node* babylonNode;
};

struct IGLTFScene : IGLTFChildRootProperty {
  std::vector<std::string> nodes;
};

/**
 * Runtime
 */
struct IGLTFRuntime {
  std::unordered_map<std::string, picojson::object> objects;
  std::unordered_map<std::string, picojson::object> accessors;
  std::unordered_map<std::string, picojson::object> buffers;
  std::unordered_map<std::string, picojson::object> bufferViews;
  std::unordered_map<std::string, picojson::object> meshes;
  std::unordered_map<std::string, picojson::object> lights;
  std::unordered_map<std::string, picojson::object> cameras;
  std::unordered_map<std::string, IGLTFNode> nodes;
  std::unordered_map<std::string, picojson::object> images;
  std::unordered_map<std::string, picojson::object> textures;
  std::unordered_map<std::string, picojson::object> shaders;
  std::unordered_map<std::string, picojson::object> programs;
  std::unordered_map<std::string, picojson::object> samplers;
  std::unordered_map<std::string, picojson::object> techniques;
  std::unordered_map<std::string, picojson::object> materials;
  std::unordered_map<std::string, picojson::object> animations;
  std::unordered_map<std::string, picojson::object> skins;
  std::string currentScene;
  size_t buffersCount;
  size_t shaderscount;
  Scene* scene;
  std::string rootUrl;
  size_t loadedBuffers;
  size_t loadedShaders;
  std::string arrayBuffers;
  bool importOnlyMeshes;
  std::vector<std::string> importMeshesNames;
  std::vector<Node*> dummyNodes;
};

/**
 * Bones
 */
struct INodeToRoot {
  INodeToRoot(Bone* /*bone*/, const IGLTFNode& /*node*/,
              const std::string& /*id*/)
  {
  }

  Bone* bone;
  IGLTFNode node;
  std::string id;
}; // end of struct INodeToRoot

struct IJointNode {
  IJointNode()
  {
  }

  IJointNode(const IGLTFNode& /*node*/, const std::string& /*id*/)
  {
  }

  IGLTFNode node;
  std::string id;
  bool isNull;
}; // end of struct IJointNode

} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_PLUGINS_GLTF_GLTF_FILE_LOADER_INTERFACES_H
