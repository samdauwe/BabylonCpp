#import <AppKit/AppKit.h>
#define PlatformViewBase NSOpenGLView
#define PlatformViewController NSViewController
#define PlatformGLContext NSOpenGLContext

@interface OpenGLView : PlatformViewBase

@end

@interface OpenGLViewController : PlatformViewController

@end
