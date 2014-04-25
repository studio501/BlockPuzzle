//
//  PlatformCallback.c
//  blockpuzzle
//
//  Created by fu xiaoling on 13-11-25.
//
//

#import "IOSPlatformCallback.h"
#import "AppController.h"

int ios_getAdMobBannerHeight()
{
    return [[AppController getCurrentInstance] getAdMobBannerHeight];
}

void ios_onMoreClicked()
{
    [[AppController getCurrentInstance] onMoreClicked];
}

void ios_setAdMobBannerPosition(bool top)
{
    [[AppController getCurrentInstance] setAdMobBannerPosition:top];
}

void ios_showAdMobBanner(bool show)
{
    [[AppController getCurrentInstance] showAdMobBanner:show];
}

void ios_showAdMobMRect(bool show)
{
    [[AppController getCurrentInstance] showAdMobMRect:show];
}

void ios_showInterstitial()
{
    [[AppController getCurrentInstance] showInterstitial];
}

void ios_rateApp()
{
    [[AppController getCurrentInstance] rateApp];
}