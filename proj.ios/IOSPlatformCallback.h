//
//  PlatformCallback.h
//  blockpuzzle
//
//  Created by fu xiaoling on 13-11-25.
//
//

#ifndef __IOS_PLATFORM_CALLBACK_H__
#define __IOS_PLATFORM_CALLBACK_H__

#ifdef __cplusplus
extern "C" {
#endif
    int ios_getAdMobBannerHeight();
    void ios_onMoreClicked();
    void ios_setAdMobBannerPosition(bool top);
    void ios_showAdMobBanner(bool show);
    void ios_showAdMobMRect(bool show);
    void ios_showInterstitial();
    void ios_rateApp();
    
#ifdef __cplusplus
} 
#endif

#endif
