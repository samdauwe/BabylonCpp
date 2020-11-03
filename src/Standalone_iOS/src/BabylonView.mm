#import "BabylonView.h"
#import "BabylonManager.h"

@interface BabylonView()
@property (nonatomic, readonly) BabylonManager* launcher;
@end

@implementation BabylonView

- (instancetype)initWithSize:(CGSize)size
{
  if(self = [super init])
  {
    _launcher = new BabylonManager(size.width, size.height);
  }
  return self;
}

- (void)dealloc
{
  delete _launcher;
}

- (void)setSize:(CGSize)size
{
  _launcher->setSize(size.width, size.height);
}

- (void)render
{
  _launcher->render();
}

- (void)update
{
}

@end
