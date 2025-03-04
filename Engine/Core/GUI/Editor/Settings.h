//
// Created by scion on 3/3/2025.
//

#pragma once

#include <Vendor/IMGUI/imgui.h>

#define GE_EDITOR_ASSET_PADDING 16
#define GE_EDITOR_ASSET_MIN_SCALE 64
#define GE_EDITOR_ASSET_MAX_SCALE 512

CONSTEXPR_GLOBAL const char* GE_EDITOR_ASSET_PAYLOAD = "ASSET";

#define GE_EDITOR_HIERARCHY_FLAGS ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth
#define GE_EDITOR_TABLE_FLAGS ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_Resizable
#define GE_EDITOR_TOOLTIP_FLAGS ImGuiHoveredFlags_Stationary | ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_AllowWhenDisabled
#define GE_EDITOR_INPUT_FLAGS ImGuiSliderFlags_AlwaysClamp
#define GE_EDITOR_COLOR_PICKER_FLAGS ImGuiColorEditFlags_Float | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreviewHalf