#pragma once
// This file is for including & compiling all of the necessary standard libraries being used one time
// and for including other important things globally (i.e., logging, debugging tools, etc.)

#define SPA_BUILD_VERSION "v1.0.0a" // TODO: Move this to a "Version.h" or some such

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <iomanip>
#include <cstdint>

#include <fstream>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <chrono>

#include <ScamPA/Core/PlatformDetection.h>
#include <ScamPA/Core/Logger.h>
#include <ScamPA/Core/Assert.h>
#include <ScamPA/Debug/Instrumentor.h>
//#include <ScamPA/Core/Events/Event.h>
