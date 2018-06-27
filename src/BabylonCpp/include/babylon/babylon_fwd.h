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
class AnimationGroup;
struct AnimationPropertiesOverride;
class AnimationRange;
class AnimationValue;
class IAnimatable;
struct IAnimationKey;
class PathCursor;
class RuntimeAnimation;
struct TargetedAnimation;
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
// --- Behaviors ---
template <class T>
struct Behavior;
template <class T>
struct IBehaviorAware;
// - Cameras
class AutoRotationBehavior;
class BouncingBehavior;
class FramingBehavior;
// - Mesh
class MultiPointerScaleBehavior;
class PointerDragBehavior;
struct PointerDragBehaviorOptions;
class SixDofDragBehavior;
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
class DeviceOrientationCamera;
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
struct DevicePose;
struct PoseControlled;
class VRCameraMetrics;
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
class DataView;
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
class DebugLayer;
class RayHelper;
namespace Debug {
class AxesViewer;
class BoneAxesViewer;
class PhysicsViewer;
class SkeletonViewer;
} // end of namespace Debug
// --- Engine ---
class ClickInfo;
struct DepthTextureCreationOptions;
class Engine;
struct EngineCapabilities;
struct EngineOptions;
struct IActiveMeshCandidateProvider;
struct InstancingAttributeInfo;
class Node;
class Scene;
// --- Events ---
struct KeyboardEventTypes;
class KeyboardInfo;
class KeyboardInfoPre;
class PointerInfo;
class PointerInfoBase;
class PointerInfoPre;
struct RenderingGroupInfo;
// --- Gamepad ---
class Gamepad;
struct GamepadButtonChanges;
class GamepadManager;
struct GamepadPose;
class GenericPad;
struct StickValues;
class Xbox360Pad;
// - Controllers
class DaydreamController;
class ExtendedGamepadButton;
class GearVRController;
class GenericController;
class MutableGamepadButton;
class OculusTouchController;
class PoseEnabledController;
struct PoseEnabledControllerHelper;
class ViveController;
class WebVRController;
class WindowsMotionController;
// --- Gizmos ---
class AxisDragGizmo;
class Gizmo;
class PositionGizmo;
// --- Helpers ---
class EnvironmentHelper;
struct IEnvironmentHelperOptions;
struct IParticleSystemData;
struct ISceneSize;
class ParticleHelper;
struct PhotoDomeOptions;
class PhotoDome;
// --- Instrumentation ---
struct _TimeToken;
class EngineInstrumentation;
class SceneInstrumentation;
// --- Interfaces ---
class IBrowserGamepad;
class ICanvas;
class ICanvasRenderingContext2D;
struct IDisposable;
class IReflect;
namespace GL {
struct GLInfo;
class IGLFramebuffer;
class IGLBuffer;
class IGLProgram;
class IGLQuery;
class IGLRenderBuffer;
class IGLRenderingContext;
class IGLShader;
class IGLShaderPrecisionFormat;
class IGLTexture;
class IGLUniformLocation;
class IGLVertexArrayObject;
} // end of namespace GL
class ILoadingScreen;
class IRenderableScene;
// --- Layer ---
class EffectLayer;
class GlowLayer;
class HighlightLayer;
struct IEffectLayerOptions;
struct IGlowLayerOptions;
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
class ShadowLight;
class SpotLight;
// - Shadows
class ShadowGenerator;
// --- Loading ---
struct IRegisteredPlugin;
struct ISceneLoaderPlugin;
struct ISceneLoaderPluginExtensions;
class ProgressEvent;
class SceneLoader;
class SceneLoaderProgressEvent;
// - Plugins / babylon
struct BabylonFileLoader;
// --- Materials ---
// - Common
class ColorCurves;
class Effect;
struct EffectCreationOptions;
class EffectFallbacks;
struct EffectIncludesShadersStore;
class EffectShadersStore;
class FresnelParameters;
struct IImageProcessingConfigurationDefines;
class ImageProcessingConfiguration;
struct IMaterialDefines;
class Material;
struct MaterialDefines;
struct MaterialHelper;
class MultiMaterial;
class PushMaterial;
class ShaderMaterial;
struct ShaderMaterialOptions;
class StandardMaterial;
struct StandardMaterialDefines;
class UniformBuffer;
// - Background
class BackgroundMaterial;
struct BackgroundMaterialDefines;
// - PBR
class PBRBaseMaterial;
class PBRBaseSimpleMaterial;
class PBRMaterial;
struct PBRMaterialDefines;
class PBRMetallicRoughnessMaterial;
class PBRSpecularGlossinessMaterial;
// - Textures
class BaseTexture;
class ColorGradingTexture;
class CubeTexture;
struct DummyInternalTextureTracker;
class DynamicTexture;
class HDRCubeTexture;
struct IInternalTextureTracker;
struct IMultiRenderTargetOptions;
class InternalTexture;
struct IRenderTargetOptions;
using RenderTargetCreationOptions = IRenderTargetOptions;
class MirrorTexture;
class MultiRenderTarget;
class RawTexture;
class RawTexture3D;
class RefractionTexture;
class RenderTargetTexture;
class Texture;
// - Textures / Procedurals
class CustomProceduralTexture;
class ProceduralTexture;
// --- Math ---
class Angle;
class Arc2;
struct Axis;
struct BezierCurve;
class Color3;
class Color4;
class Curve3;
class Frustum;
struct ISize;
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
struct Tmp;
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
class BaseSubMesh;
class Buffer;
struct BufferPointer;
struct FacetParameters;
class Geometry;
class GroundMesh;
struct IGetSetVerticesData;
class InstancedMesh;
class LinesMesh;
class Mesh;
class MeshBuilder;
class MeshLODLevel;
class SubMesh;
class TransformNode;
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
class PolygonOptions;
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
// - Primitive Geometries
class _PrimitiveGeometry;
class BoxGeometry;
class CylinderGeometry;
class DiscGeometry;
class GroundGeometry;
class PlaneGeometry;
class RibbonGeometry;
class SphereGeometry;
class TiledGroundGeometry;
class TorusGeometry;
class TorusKnotGeometry;
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
// --- Morph ---
class MorphTarget;
class MorphTargetManager;
// --- Particles ---
struct DepthSortedParticle;
class GPUParticleSystem;
struct IParticleSystem;
class ModelShape;
class Particle;
class ParticleSystem;
class SolidParticle;
class SolidParticleSystem;
// - EmitterTypes
class BoxParticleEmitter;
class ConeParticleEmitter;
struct IParticleEmitterType;
class SphereDirectedParticleEmitter;
class SphereParticleEmitter;
// --- Physics ---
struct IPhysicsBody;
struct IPhysicsEnabledObject;
struct IPhysicsEnginePlugin;
struct IWorld;
class PhysicsEngine;
class PhysicsImpostor;
struct PhysicsImpostorJoint;
struct PhysicsImpostorParameters;
struct PhysicsParams;
// - Helper
struct PhysicsForceAndContactPoint;
class PhysicsGravitationalFieldEvent;
struct PhysicsGravitationalFieldEventData;
class PhysicsHelper;
class PhysicsRadialExplosionEvent;
struct PhysicsRadialExplosionEventData;
class PhysicsUpdraftEvent;
struct PhysicsUpdraftEventData;
class PhysicsVortexEvent;
struct PhysicsVortexEventData;
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
class BloomEffect;
class BloomMergePostProcess;
class BlurPostProcess;
class ChromaticAberrationPostProcess;
class CircleOfConfusionPostProcess;
class ColorCorrectionPostProcess;
class ConvolutionPostProcess;
struct DepthOfFieldBlurPostProcess;
class DepthOfFieldEffect;
class DepthOfFieldMergePostProcess;
struct DepthOfFieldMergePostProcessOptions;
class DisplayPassPostProcess;
class ExtractHighlightsPostProcess;
class FilterPostProcess;
class FxaaPostProcess;
class GlowBlurPostProcess;
class GrainPostProcess;
class HighlightsPostProcess;
class ImageProcessingPostProcess;
class PassPostProcess;
class PostProcess;
class PostProcessManager;
class RefractionPostProcess;
class SharpenPostProcess;
class StereoscopicInterlacePostProcess;
class TonemapPostProcess;
class VolumetricLightScatteringPostProcess;
class VRDistortionCorrectionPostProcess;
// - Render Pipeline
class PostProcessRenderEffect;
class PostProcessRenderPipeline;
class PostProcessRenderPipelineManager;
// -- Pipelines
class DefaultRenderingPipeline;
class LensRenderingPipeline;
class SSAO2RenderingPipeline;
class SSAORenderingPipeline;
class StandardRenderingPipeline;
// --- Probes ---
class ReflectionProbe;
// --- Rendering ---
class BoundingBoxRenderer;
class DepthRenderer;
class EdgesRenderer;
class FaceAdjacencies;
class GeometryBufferRenderer;
class OutlineRenderer;
class RenderingGroup;
class RenderingManager;
class UtilityLayerRenderer;
// --- Sprites ---
class Sprite;
class SpriteManager;
// --- States ---
class _AlphaState;
class _DepthCullingState;
class _StencilState;
} // end of namespace BABYLON
// --- Tools ---
struct DDS;
struct DDSInfo;
class DDSTools;
class EventState;
struct IFileRequest;
class PerformanceMonitor;
class RollingAverage;
struct SerializationHelper;
// - HDR
struct CMGBoundinBox;
struct CubeMapInfo;
class CubeMapToSphericalPolynomialTools;
struct FileFaceOrientation;
struct HDRInfo;
class HDRTools;
class PanoramaToCubeMapTools;
template <typename ArrayBufferView>
class PMREMGenerator;
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
typedef ::std::vector<value> array;
typedef ::std::map<::std::string, value> object;
} // end of namespace picojson

namespace BABYLON {
namespace Json {
typedef picojson::value value;
typedef picojson::array array;
typedef picojson::object object;
} // namespace Json
} // end of namespace BABYLON

#endif // BABYLON_FWD_H
