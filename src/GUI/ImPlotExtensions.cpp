#include "GUI/ImPlotExtensions.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"

namespace ImPlot {
	//int Formatter_Default(double value, char * buff, int size, void * data) {
	//	char * fmt = (char *)data;
	//	return ImFormatString(buff, size, fmt, value);
	//}

	void AddTicksCustom(std::vector<double> values, std::vector<std::string> labels, int n, ImPlotTicker & ticker, ImPlotFormatter formatter, void * data) {
		for (int i = 0; i < n; ++i) {
			if (!labels.empty() and labels.size() > i)
				ticker.AddTick(values[i], false, 0, true, labels[i].c_str());
			else
				ticker.AddTick(values[i], false, 0, true, formatter, data);
		}
	}

	IMPLOT_API void SetupAxisTicks(ImAxis idx, std::vector<double> values, std::vector<std::string> labels, bool show_default) {
		IM_ASSERT_USER_ERROR(GImPlot->CurrentPlot != NULL && !GImPlot->CurrentPlot->SetupLocked,
			"Setup needs to be called after BeginPlot and before any setup locking functions (e.g. PlotX)!");
		ImPlotPlot & plot = *GImPlot->CurrentPlot;
		ImPlotAxis & axis = plot.Axes[idx];
		IM_ASSERT_USER_ERROR(axis.Enabled, "Axis is not enabled! Did you forget to call SetupAxis()?");
		axis.ShowDefaultTicks = show_default;
		AddTicksCustom(values,
			labels,
			values.size(),
			axis.Ticker,
			axis.Formatter ? axis.Formatter : Formatter_Default,
			(axis.Formatter && axis.FormatterData) ? axis.FormatterData : axis.HasFormatSpec ? axis.FormatSpec : (void *)IMPLOT_LABEL_FORMAT);
		}

	/*IMPLOT_TMP void PlotBars(std::string label_id, std::vector<T> values, double bar_size, double shift, ImPlotBarsFlags flags, int offset, int stride) {
			ImPlot::PlotBars(label_id.c_str(), &values[0], values.size(), bar_size, shift, flags, offset, stride);
	}*/
	//template <typename T>
	//void PlotBars(std::string label_id, std::vector<T> values, double bar_size, double shift, ImPlotBarsFlags flags, int offset, int stride) {
	//	int count = values.size();
	//	if (ImHasFlag(flags, ImPlotBarsFlags_Horizontal)) {
	//		GetterXY<IndexerIdx<T>, IndexerLin> getter1(IndexerIdx<T>(values, count, offset, stride), IndexerLin(1.0, shift), count);
	//		GetterXY<IndexerConst, IndexerLin>  getter2(IndexerConst(0), IndexerLin(1.0, shift), count);
	//		PlotBarsHEx(label_id, getter1, getter2, bar_size, flags);
	//	} else {
	//		GetterXY<IndexerLin, IndexerIdx<T>> getter1(IndexerLin(1.0, shift), IndexerIdx<T>(values, count, offset, stride), count);
	//		GetterXY<IndexerLin, IndexerConst>  getter2(IndexerLin(1.0, shift), IndexerConst(0), count);
	//		PlotBarsVEx(label_id, getter1, getter2, bar_size, flags);
	//	}
	//}
}
