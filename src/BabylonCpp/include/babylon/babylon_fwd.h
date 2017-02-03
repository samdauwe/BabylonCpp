#ifndef BABYLON_FWD_H
#define BABYLON_FWD_H

#include <babylon/babylon_stl.h>

// --- Forward Declarations --- //

namespace BABYLON {
// --- Actions ---
class Action;
class ActionEvent;
class ActionManager;
class Condition;
// - Conditions
class PredicateCondition;
class StateCondition;
class ValueCondition;
// - DirectActions
class CombineAction;
class DoNothingAction;
class ExecuteCodeAction;
class IncrementValueAction;
class PlayAnimationAction;
class PlaySoundAction;
class SetParentAction;
class SetStateAction;
class SetValueAction;
class StopAnimationAction;
class StopSoundAction;
class SwitchBooleanAction;
// - ValueActions
class InterpolateValueAction;
// --- Animations ---
class Animatable;
class Animation;
class AnimationEvent;
class AnimationKey;
class AnimationRange;
class AnimationValue;
struct IAnimatable;
class PathCursor;
// - Easing
class BackEase;
class BezierCurveEase;
class BounceEase;
class CircleEase;
class CubicEase;
class EasingFunction;
class ElasticEase;
class ExponentialEase;
class IEasingFunction;
class PowerEase;
class QuadraticEase;
class QuarticEase;
class QuinticEase;
class SineEase;
// --- Audio ---
class Analyser;
class AudioEngine;
class Sound;
class SoundTrack;
// --- Bones ---
class Bone;
class BoneIKController;
class BoneLookController;
class Skeleton;
// --- Cameras ---
class ArcFollowCamera;
class ArcRotateCamera;
struct ArcRotateCameraInputsManager;
class Camera;
template <class TCamera>
class CameraInputsManager;
class FollowCamera;
class FreeCamera;
struct FreeCameraInputsManager;
template <class TCamera>
struct ICameraInput;
class TargetCamera;
// - Inputs
class ArcRotateCameraKeyboardMoveInput;
class ArcRotateCameraMouseWheelInput;
class ArcRotateCameraPointersInput;
class FreeCameraKeyboardMoveInput;
class FreeCameraMouseInput;
// - VR
struct VRFov;
// --- Collisions ---
struct BabylonMessage;
struct CollidePayload;
class CollideWorker;
class Collider;
class CollisionCache;
class CollisionCoordinatorLegacy;
class CollisionCoordinatorWorker;
class CollisionDetectorTransferable;
struct CollisionReplyPayload;
struct ICollisionCoordinator;
struct ICollisionDetector;
struct InitPayload;
class IntersectionInfo;
class PickingInfo;
struct SerializedColliderToWorker;
struct SerializedGeometry;
struct SerializedMesh;
struct SerializedSubMesh;
struct UpdatePayload;
class Worker;
struct WorkerReply;
// --- Core ---
struct Image;
struct NodeCache;
// - Logging
class LogChannel;
class LogMessage;
class Logger;
// --- Culling ---
class BoundingBox;
class BoundingInfo;
class BoundingSphere;
struct ICullable;
class Ray;
// - Octrees
template <class T>
struct IOctreeContainer;
template <class T>
class Octree;
template <class T>
class OctreeBlock;
// --- Debug ---
class AxesViewer;
class BoneAxesViewer;
class DebugLayer;
class SkeletonViewer;
// --- Engine ---
class Engine;
struct EngineCapabilities;
struct EngineOptions;
struct InstancingAttributeInfo;
class Node;
struct PointerEventTypes;
class PointerInfo;
class PointerInfoBase;
class PointerInfoPre;
struct RenderingGroupInfo;
class Scene;
// --- Interfaces ---
class ICanvas;
class ICanvasRenderingContext2D;
class IDisposable;
class IReflect;
namespace GL {
struct GLInfo;
class IGLFramebuffer;
class IGLBuffer;
class IGLProgram;
class IGLRenderBuffer;
class IGLRenderingContext;
class IGLShader;
class IGLShaderPrecisionFormat;
class IGLTexture;
class IGLUniformLocation;
} // end of namespace GL
class ILoadingScreen;
class IRenderableScene;
// --- Layer ---
struct GlowBlurPostProcess;
class HighlightLayer;
struct IHighlightLayerExcludedMesh;
struct IHighlightLayerMesh;
struct IHighlightLayerOptions;
class Layer;
// --- Lensflare ---
class LensFlare;
class LensFlareSystem;
// --- Lights ---
class DirectionalLight;
class HemisphericLight;
class IShadowLight;
class Light;
class PointLight;
class SpotLight;
// - Shadows
class ShadowGenerator;
// --- Loading ---
struct IRegisteredPlugin;
struct ISceneLoaderPlugin;
struct ISceneLoaderPluginExtensions;
class SceneLoader;
// - Plugins / babylon
struct BabylonFileLoader;
// --- Materials ---
// - Common
class ColorCurves;
class Effect;
class EffectFallbacks;
struct EffectIncludesShadersStore;
struct EffectShadersStore;
class FresnelParameters;
struct IMaterialDefines;
class Material;
struct MaterialDefines;
class MaterialHelper;
class MultiMaterial;
class PBRMaterial;
struct PBRMaterialDefines;
class ShaderMaterial;
struct ShaderMaterialOptions;
class StandardMaterial;
struct StandardMaterialDefines;
// - Textures
class BaseTexture;
class ColorGradingTexture;
class CubeTexture;
class DynamicTexture;
class HDRCubeTexture;
class MapTexture;
class MirrorTexture;
struct RawTexture;
class RefractionTexture;
class RenderTargetTexture;
class Texture;
// - Textures / Procedurals
class CustomProceduralTexture;
class ProceduralTexture;
// --- Math ---
class Angle;
class Arc2;
class Axis;
class BezierCurve;
class Color3;
class Color4;
class Curve3;
class Frustum;
struct ISize;
class MathTools;
class Matrix;
class Path2;
class Path3D;
class PathCursor;
class Plane;
class PositionNormalTextureVertex;
class PositionNormalVertex;
class Quaternion;
class Size;
class SphericalHarmonics;
class SphericalPolynomial;
class Tmp;
class Vector2;
class Vector3;
class Vector4;
class Viewport;
namespace SIMD {
struct Float32x4;
struct SIMDMatrix;
struct SIMDVector3;
} // end of namespace SIMD
// --- Mesh ---
class _InstancesBatch;
class _VisibleInstances;
class AbstractMesh;
class Buffer;
struct BufferPointer;
class Geometry;
class GroundMesh;
struct IGetSetVerticesData;
class InstancedMesh;
class LinesMesh;
class Mesh;
class MeshBuilder;
class MeshLODLevel;
class SubMesh;
class VertexBuffer;
class VertexData;
// - Options
class BoxOptions;
class CylinderOptions;
class DashedLinesOptions;
class DecalOptions;
class DiscOptions;
class ExtrudeShapeOptions;
class ExtrudeShapeCustomOptions;
class GroundFromHeightMapOptions;
class GroundOptions;
class IcoSphereOptions;
class LatheOptions;
class LinesOptions;
class LineSystemOptions;
class PlaneOptions;
class PolyhedronOptions;
class RibbonOptions;
class SphereOptions;
class TiledGroundOptions;
class TorusKnotOptions;
class TorusOptions;
class TubeOptions;
// - CSG
namespace CSG {
class CSG;
// class Node;
class Plane;
class Polygon;
class Vertex;
} // end of namespace CSG
// - Geometry Primitives
namespace GeometryPrimitives {
class _Primitive;
class Box;
class Cylinder;
class Disc;
class Ground;
class Plane;
class Ribbon;
class Sphere;
class TiledGround;
class Torus;
class TorusKnot;
} // end of namespace GeometryPrimitives
// - Polygon Mesh
class IndexedVector2;
struct Polygon;
class PolygonMeshBuilder;
struct PolygonPoints;
// - Simplification
class DecimationTriangle;
class DecimationVertex;
struct ISimplificationSettings;
struct ISimplificationTask;
class ISimplifier;
class QuadraticErrorSimplification;
class QuadraticMatrix;
class Reference;
class SimplificationQueue;
class SimplificationSettings;
// --- Particles ---
class ModelShape;
class Particle;
class ParticleSystem;
class SolidParticle;
class SolidParticleSystem;
// --- Physics ---
struct IBody;
struct IPhysicsEnabledObject;
struct IPhysicsEnginePlugin;
struct IWorld;
class PhysicsEngine;
class PhysicsImpostor;
struct PhysicsImpostorJoint;
struct PhysicsImpostorParameters;
struct PhysicsParams;
// - Joint
class DistanceJoint;
struct DistanceJointData;
class Hinge2Joint;
class HingeJoint;
struct IMotorEnabledJoint;
class MotorEnabledJoint;
class PhysicsJoint;
struct PhysicsJointData;
struct SpringJointData;
// - Plugins
class OimoPhysicsEnginePlugin;
// --- Post Process ---
class AnaglyphPostProcess;
class BlackAndWhitePostProcess;
class BlurPostProcess;
class ColorCorrectionPostProcess;
class ConvolutionPostProcess;
class DisplayPassPostProcess;
class FilterPostProcess;
class FxaaPostProcess;
class HDRRenderingPipeline;
class LensRenderingPipeline;
class PassPostProcess;
class PostProcess;
class PostProcessManager;
class RefractionPostProcess;
class SSAORenderingPipeline;
class StandardRenderingPipeline;
class StereoscopicInterlacePostProcess;
class TonemapPostProcess;
class VolumetricLightScatteringPostProcess;
class VRDistortionCorrectionPostProcess;
// - Render Pipeline
class PostProcessRenderEffect;
class PostProcessRenderPass;
class PostProcessRenderPipeline;
class PostProcessRenderPipelineManager;
// --- Probes ---
class ReflectionProbe;
// --- Rendering ---
class BoundingBoxRenderer;
class DepthRenderer;
class FaceAdjacencies;
class EdgesRenderer;
class OutlineRenderer;
class RenderingGroup;
class RenderingManager;
// --- Sprites ---
class Sprite;
class SpriteManager;
// --- States ---
namespace Internals {
class _AlphaState;
class _DepthCullingState;
class _StencilState;
} // end of namespace Internals
} // end of namespace BABYLON
// --- Tools ---
class PackedRect;
struct SerializationHelper;
struct RectPackingMap;
// - Optimization
class HardwareScalingOptimization;
class LensFlaresOptimization;
class MergeMeshesOptimization;
class ParticlesOptimization;
class PostProcessesOptimization;
class RenderTargetsOptimization;
class SceneOptimization;
struct SceneOptimizer;
class SceneOptimizerOptions;
class ShadowsOptimization;
class TextureOptimization;
namespace picojson {
class value;
typedef std::vector<value> array;
typedef std::map<std::string, value> object;
} // end of namespace picojson

namespace BABYLON {
namespace Json {
typedef picojson::value value;
typedef picojson::array array;
typedef picojson::object object;
} // end of namespace picojson
} // end of namespace BABYLON

#endif // BABYLON_FWD_H
