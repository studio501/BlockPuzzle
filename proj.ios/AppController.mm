#import "AppController.h"
#import "EAGLView.h"
#import "cocos2d.h"
#import "AppDelegate.h"
#import "RootViewController.h"
#import "Flurry.h"
#import "Chartboost.h"

#define FLURRY_KEY @"VNR6RNFT8K2HRCVKPFBZ"
#define ADMOB_ID @"a152aabd6a51f5d"
#define CHARTBOOST_KEY @"5295e579f8975c76af4312d2"
#define CHARTBOOST_SIGNATURE @"99fb9edb3ccdab8d62eaa8e2e59cd37723ce33a5"
#define APP_ID @"789589238"

@interface AppController () <ChartboostDelegate>
@end

@implementation AppController

#pragma mark -
#pragma mark Application lifecycle

// cocos2d application instance
static AppDelegate s_sharedApplication;
static id instance;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    instance = self;

    [Flurry startSession:FLURRY_KEY];
    [Flurry logEvent:@"Start of Block Puzzle"];

    // Override point for customization after application launch.

    // Add the view controller's view to the window and display.
    window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
    
    // Init the EAGLView
    EAGLView *__glView = [EAGLView viewWithFrame: [window bounds]
                                     pixelFormat: kEAGLColorFormatRGB565
                                     depthFormat: GL_DEPTH24_STENCIL8_OES
                              preserveBackbuffer: NO
                                      sharegroup: nil
                                   multiSampling: NO
                                 numberOfSamples: 0];

    // Use RootViewController manage EAGLView 
    viewController = [[RootViewController alloc] initWithNibName:nil bundle:nil];
    viewController.wantsFullScreenLayout = YES;
    viewController.view = __glView;
    
    self.adBanner = [[GADBannerView alloc] initWithAdSize:kGADAdSizeSmartBannerPortrait];//kGADAdSizeBanner  kGADAdSizeSmartBannerPortrait
    self.adBanner.adUnitID = ADMOB_ID;
    self.adBanner.rootViewController = viewController;
    [viewController.view addSubview:self.adBanner];
    [self.adBanner loadRequest:[GADRequest request]];
    
    self.adMRect = [[GADBannerView alloc] initWithAdSize:kGADAdSizeMediumRectangle];
    self.adMRect.adUnitID = ADMOB_ID;
    self.adMRect.rootViewController = viewController;
    self.adMRect.hidden = YES;
    CGRect frame = self.adMRect.frame;
    frame.origin.x = (__glView.frame.size.width - self.adMRect.frame.size.width) / 2;
    frame.origin.y = (__glView.frame.size.height - self.adMRect.frame.size.height) / 2;
    self.adMRect.frame = frame;
    [viewController.view addSubview:self.adMRect];
    [self.adMRect loadRequest:[GADRequest request]];

    Chartboost *cb = [Chartboost sharedChartboost];
    cb.appId = CHARTBOOST_KEY;
    cb.appSignature = CHARTBOOST_SIGNATURE;
    cb.delegate = self;
    [cb startSession];
    [cb showInterstitial];
    [cb cacheMoreApps];
    
    // Set RootViewController to window
    if ( [[UIDevice currentDevice].systemVersion floatValue] < 6.0)
    {
        // warning: addSubView doesn't work on iOS6
        [window addSubview: viewController.view];
    }
    else
    {
        // use this method on ios6
        [window setRootViewController:viewController];
    }
    
    [window makeKeyAndVisible];
    
    [[UIApplication sharedApplication] setStatusBarHidden:true];
    
    cocos2d::CCApplication::sharedApplication()->run();
    
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
    cocos2d::CCDirector::sharedDirector()->pause();
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
    
    cocos2d::CCDirector::sharedDirector()->resume();
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
    cocos2d::CCApplication::sharedApplication()->applicationDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
    cocos2d::CCApplication::sharedApplication()->applicationWillEnterForeground();
}

- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */

    [Flurry logEvent:@"Termination of Block Puzzle"];
}


#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
}

- (void)dealloc {
    [window release];
    [super dealloc];
}

- (int)getAdMobBannerHeight {
    return self.adBanner.frame.size.height * [[UIScreen mainScreen] scale];
}

- (void)onMoreClicked {
    [[Chartboost sharedChartboost] showMoreApps];
}

- (void)setAdMobBannerPosition:(bool)top {
    CGRect screen = [UIScreen mainScreen].bounds;
    CGRect frame = self.adBanner.frame;
    frame.origin.y = (top ? 0 : screen.size.height - frame.size.height);
    self.adBanner.frame = frame;
}

- (void)showAdMobBanner:(bool)show {
    [self.adBanner setHidden:(show ? NO : YES)];
}

- (void)showAdMobMRect:(bool)show {
    [self.adMRect setHidden:(show ? NO : YES)];
}

- (void)showInterstitial {
    [[Chartboost sharedChartboost] showInterstitial];
}

- (void)rateApp {
    static NSString *const iOSAppStoreURLFormat = @"itms-apps://itunes.apple.com/app/id%@?at=10l6dK";
    static NSString *const iOS7AppStoreURLFormat = @"itms-apps://itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?type=Purple+Software&id=%@&at=10l6dK";
    
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:([[UIDevice currentDevice].systemVersion floatValue] >= 7.0f)? iOS7AppStoreURLFormat: iOSAppStoreURLFormat, APP_ID]];
    [[UIApplication sharedApplication] openURL:url];
}

+ (AppController *)getCurrentInstance {
    return (AppController *)instance;
}

/*
 * Chartboost Delegate Methods
 *
 * Recommended for everyone: shouldDisplayInterstitial
 */

- (BOOL)shouldRequestInterstitialsInFirstSession {
    return YES;
}

- (BOOL)shouldRequestInterstitial:(NSString *)location {
    return YES;
}

- (BOOL)shouldDisplayInterstitial:(NSString *)location {
    NSLog(@"about to display interstitial at location %@", location);
    return YES;
}

- (BOOL)shouldDisplayMoreApps {
    return YES;
}

- (BOOL)shouldDisplayLoadingViewForMoreApps {
    return YES;
}

- (void)didCloseInterstitial:(NSString *)location {
    
}

- (void)didFailToLoadInterstitial:(NSString *)location {
    NSLog(@"failure to load interstitial at location %@", location);
    
    // Show a house ad or do something else when a chartboost interstitial fails to load
}

/*
 * didCacheInterstitial
 *
 * Passes in the location name that has successfully been cached.
 *
 * Is fired on:
 * - All assets loaded
 * - Triggered by cacheInterstitial
 *
 * Notes:
 * - Similar to this is: cb.hasCachedInterstitial(String location)
 * Which will return true if a cached interstitial exists for that location
 */

- (void)didCacheInterstitial:(NSString *)location {
    NSLog(@"interstitial cached at location %@", location);
    
}

/*
 * didFailToLoadMoreApps
 *
 * This is called when the more apps page has failed to load for any reason
 *
 * Is fired on:
 * - No network connection
 * - No more apps page has been created (add a more apps page in the dashboard)
 * - No publishing campaign matches for that user (add more campaigns to your more apps page)
 *  -Find this inside the App > Edit page in the Chartboost dashboard
 */

- (void)didFailToLoadMoreApps {
    NSLog(@"failure to load more apps");
}


/*
 * didDismissInterstitial
 *
 * This is called when an interstitial is dismissed
 *
 * Is fired on:
 * - Interstitial click
 * - Interstitial close
 *
 * #Pro Tip: Use the delegate method below to immediately re-cache interstitials
 */

- (void)didDismissInterstitial:(NSString *)location {
    NSLog(@"dismissed interstitial at location %@", location);
    
    [[Chartboost sharedChartboost] cacheInterstitial:location];
}


/*
 * didDismissMoreApps
 *
 * This is called when the more apps page is dismissed
 *
 * Is fired on:
 * - More Apps click
 * - More Apps close
 *
 * #Pro Tip: Use the delegate method below to immediately re-cache the more apps page
 */

- (void)didDismissMoreApps {
    NSLog(@"dismissed more apps page, re-caching now");
    
    [[Chartboost sharedChartboost] cacheMoreApps];
}


@end
