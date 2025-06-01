#pragma once

#include "NovaMinimal.h"

// Core system includes
#include "Core/NovaLog.h"
#include "Core/NovaFileOperations.h"

// Additional includes moved from other files
#include "Menus/MainMenu.h"
#include "Menus/OptionsMenu.h"
#include "Menus/HelpMenu.h"
#include "Menus/InstallationMenu.h"
#include "Utils/CommandLineParsing.h"
#include "Utils/ConfigManager.h"
#include "Utils/Interactables.h"
#include "Utils/TerminalUtils.h"
#include "UnitTests/BaseUnitTest.h"
#include "UnitTests/UnitTestManager.h"

// Forward declarations to avoid circular dependencies
namespace Menus {
    class BaseMenu;
}

namespace Core::Helpers {
    class BrewInstallHelper;
    class PipInstallHelper;
    class RootAccessHelper;
}
