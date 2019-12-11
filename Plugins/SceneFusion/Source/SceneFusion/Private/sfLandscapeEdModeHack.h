#pragma once

#include <EdMode.h>
#include <LandscapeToolInterface.h>

/**
 * Hack to expose some members of the private class FEdModeLandscape from LandscapeEdMode.h. This class inherits
 * from the same base class as FEdModeLandscape and declares the first few members (or void* for pointers to
 * private types) of FEdModeLandscape in the same order.
 */
class sfLandscapeEdModeHack : public FEdMode
{
public:
    void* UISettings;
    void* CurrentToolMode;
    FLandscapeTool* CurrentTool;
    FLandscapeBrush* CurrentBrush;
    FLandscapeToolTarget CurrentToolTarget;
    FLandscapeBrush* GizmoBrush;
    int CurrentToolIndex;
};