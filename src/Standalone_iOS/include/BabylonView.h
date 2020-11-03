#pragma once

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

@interface BabylonView : NSObject

- (instancetype)initWithSize:(CGSize)size;
- (void)setSize:(CGSize)size;
- (void)render;
- (void)update;

@end
