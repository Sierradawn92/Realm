#pragma once

#include <EdMode.h>
#include <LandscapeToolInterface.h>

#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 23
#include <LandscapeEdit.h>
#endif

/**
 * Hack to expose some members of the private class FEdModeLandscape from LandscapeEdMode.h. This class inherits
 * from the same base class as FEdModeLandscape and declares the first few members (or void* for pointers to
 * private types) of FEdModeLandscape in the same order.
 */
class sfLandscapeEdModeHack : public FEdMode
#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 23
    , public ILandscapeEdModeInterface
#endif
{
public:
    void* UISettings;
#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 23
    FText ErrorReasonOnMouseUp;
#endif
    void* CurrentToolMode;
    FLandscapeTool* CurrentTool;
    FLandscapeBrush* CurrentBrush;
    FLandscapeToolTarget CurrentToolTarget;
    FLandscapeBrush* GizmoBrush;
    int CurrentToolIndex;
};