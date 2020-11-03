#import "OpenGLViewController.h"
#import <Foundation/Foundation.h>
#import <OpenGLES/ES3/gl.h>
#import "BabylonView.h"

@implementation OpenGLView

+ (Class) layerClass
{
  return [CAEAGLLayer class];
}

@end

@implementation OpenGLViewController
{
  OpenGLView* _view;
  EAGLContext* _context;
  GLuint _defaultFrameBuffer;
  GLuint _colorRenderBuffer;
  GLuint _depthRenderBuffer;
  CAEAGLLayer* _eaglLayer;
  CADisplayLink* _displayLink;
  BabylonView* _babylonView;
}

- (void)viewDidLoad
{
  [super viewDidLoad];
  _view = (OpenGLView *)self.view;
  [self prepareView];
  [self makeCurrentContext];
}

- (void)draw:(id)sender
{
  [EAGLContext setCurrentContext:_context];
  
  glBindFramebuffer(GL_FRAMEBUFFER, _defaultFrameBuffer);
  [_babylonView render];
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
  
  [_context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)makeCurrentContext
{
  [EAGLContext setCurrentContext:_context];
}

- (void)setupDisplayLink
{
  // Create the display link so that we render at 60FPS
  _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(draw:)];
  _displayLink.preferredFramesPerSecond = 60;
  // Have the display link run on the default runn loop (and the main thread)
  [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)setupLayer
{
  // Get the layer
  _eaglLayer = (CAEAGLLayer *)self.view.layer;
  _eaglLayer.drawableProperties = @{kEAGLDrawablePropertyRetainedBacking : @NO, kEAGLDrawablePropertyColorFormat : kEAGLColorFormatRGBA8 };
  _eaglLayer.opaque = YES;
}

- (void)setupContext
{
  _context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
  NSAssert(_context, @"Could Not Create OpenGL ES Context");
  BOOL isSetCurrent = [EAGLContext setCurrentContext:_context];
  NSAssert(isSetCurrent, @"Could not make OpenGL ES context current");
}

- (void)setupRenderBuffer:(CGSize)viewSize
{
  glGenRenderbuffers(1, &_depthRenderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, viewSize.width, viewSize.height);
  
  glGenRenderbuffers(1, &_colorRenderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
  
  // On iOS & tvOS you must create an FBO and attach a CoreAnimation allocated drawable texture to use as the "defaultFBO" for a view
  glGenFramebuffers(1, &_defaultFrameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, _defaultFrameBuffer);
  
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBuffer);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer);
}

- (void)prepareView
{
  [self setupLayer];
  [self setupContext];
  CGSize viewSize = [self drawableSize];
  _babylonView = [[BabylonView alloc] initWithSize:viewSize];
  [self setupRenderBuffer:viewSize];
  [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(id<EAGLDrawable>)_view.layer];
  [self setupDisplayLink];
}

- (CGSize)drawableSize
{
  return [UIScreen mainScreen].bounds.size;
}

- (void)resizeDrawable
{
  [self makeCurrentContext];
  CGSize drawSize = [self drawableSize];
  
  // Update size to Babylon
  [_babylonView setSize:drawSize];
  
  // Ensure we've actually got a render buffer first;
  assert(_colorRenderBuffer != 0);
  glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
  [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(id<EAGLDrawable>)_view.layer];
}

- (void)viewDidLayoutSubviews
{
  [self resizeDrawable];
}

- (void)viewDidAppear:(BOOL)animated
{
  [self resizeDrawable];
}

@end
