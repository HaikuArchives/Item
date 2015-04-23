/* ListColors.cpp */

#include "ListColors.h"
#include "ColorPrefWatcher.h"

static ColorPrefWatcher bgndWatcher("listBgndColor", &ListColors::bgndColor,
                                    ListColors::defaultBgndColor);
static ColorPrefWatcher selectedWatcher("listSelectedBgndColor",
                                        &ListColors::selectedBgndColor,
                                        ListColors::defaultSelectedBgndColor);
static ColorPrefWatcher hilitedWatcher("listHilitedBgndColor",
                                       &ListColors::hilitedBgndColor,
                                       ListColors::defaultHilitedBgndColor);
static ColorPrefWatcher textWatcher("listTextColor", &ListColors::textColor,
                                    ListColors::defaultTextColor);
static ColorPrefWatcher numArticlesWatcher("listNumArticlesColor",
                                           &ListColors::numArticlesColor,
                                           ListColors::defaultNumArticlesColor);

rgb_color ListColors::bgndColor;
rgb_color ListColors::selectedBgndColor;
rgb_color ListColors::hilitedBgndColor;
rgb_color ListColors::textColor;
rgb_color ListColors::numArticlesColor;
const rgb_color ListColors::defaultBgndColor = { 255, 255, 255, 255 };
const rgb_color ListColors::defaultSelectedBgndColor = { 192, 192, 192, 255 };
const rgb_color ListColors::defaultHilitedBgndColor = { 192, 192, 255, 255 };
const rgb_color ListColors::defaultTextColor = { 0, 0, 0, 255 };
const rgb_color ListColors::defaultNumArticlesColor = { 128, 128, 128, 255 };

