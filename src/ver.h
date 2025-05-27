/**
 * @file ver.h
 * @author hayasita04@gmail.com
 * @brief Version information header
 * @version 0.1
 * @date 2025-05-20
 * 
 * @copyright Copyright (c) 2025
 */
#pragma once

// Version information

#define SW_VERSION_MAJOR 0
#define SW_VERSION_MINOR 1
#define SW_VERSION_PATCH 0

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define SW_VERSION TOSTRING(SW_VERSION_MAJOR) "." TOSTRING(SW_VERSION_MINOR) "." TOSTRING(SW_VERSION_PATCH)

