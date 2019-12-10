#include "sfSessionInfo.h"
#include "../Public/SceneFusion.h"

sfSessionInfo::sfSessionInfo() :
    ProjectName{ "Default" },
    Creator{ "Test" },
    LevelName{ "Level" },
    CanStop{ false },
    RequiredVersion{ SceneFusion::Version() },
    RoomInfoPtr{ nullptr }
{
    Time = FDateTime::Now();
}