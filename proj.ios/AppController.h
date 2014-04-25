#import <UIKit/UIKit.h>
#import "GADBannerView.h"

@class RootViewController;

@interface AppController : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    RootViewController    *viewController;
}

@property(nonatomic, strong) GADBannerView *adBanner;
@property(nonatomic, strong) GADBannerView *adMRect;

- (int)getAdMobBannerHeight;
- (void)onMoreClicked;
- (void)setAdMobBannerPosition:(bool)top;
- (void)showAdMobBanner:(bool)show;
- (void)showAdMobMRect:(bool)show;
- (void)showInterstitial;
- (void)rateApp;

+ (AppController *)getCurrentInstance;
@end

