#pragma once

/*  
* NOTE:
* This file contains core libraries
* to be used by clientside applications ONLY
*
* NEVER include this file anywhere in the core codebase!
*/

// Core 
#include <ScamPA/Core/Application.h>
//#include <ScamPA/Core/Logger.h>
#include <ScamPA/Core/Layer.h>
//#include <ScamPA/Core/Timestep.h>

// Input
#include <ScamPA/Input/Input.h>

// Utilities
#include <ScamPA/Utilities/Timer.h>
#include <ScamPA/Utilities/Random.h>


// Events
#include <ScamPA/Events/Event.h>
#include <ScamPA/Events/ApplicationEvent.h>
#include <ScamPA/Events/KeyEvent.h>
#include <ScamPA/Events/MouseEvent.h>

// Renderer
#include <ScamPA/Renderer/Renderer.h>
#include <ScamPA/Renderer/VulkanContext.h>
#include <ScamPA/Renderer/VulkanSwapchain.h>
#include <ScamPA/Renderer/VulkanImage.h>

// ImGui
#include <imgui.h>

// Entry point
// #include <ScamPA/Core/ClientEntryPoint.h> // INCLUDE THIS IN CLIENTSIDE APPLICATIONS
