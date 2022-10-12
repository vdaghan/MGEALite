#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"

#include <vector>
#include <string>

namespace ImPlot {
	IMPLOT_API void SetupAxisTicks(ImAxis axis, std::vector<double>, std::vector<std::string> labels = {}, bool keep_default = false);
	IMPLOT_TMP void PlotBars(std::string label_id, std::vector<T> values, double bar_size = 0.67, double shift = 0, ImPlotBarsFlags flags = 0, int offset = 0, int stride = sizeof(T));
}
