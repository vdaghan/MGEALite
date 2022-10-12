#include "GUI/ImPlotRewrites.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"

//#include "implot_items.cpp"

//namespace DPlot {
//	using namespace ImPlot;
//
//	template <typename _Getter1, typename _Getter2>
//	struct FitterBarH {
//		FitterBarH(const _Getter1 & getter1, const _Getter2 & getter2, double height) :
//			Getter1(getter1),
//			Getter2(getter2),
//			HalfHeight(height * 0.5) {
//		}
//		void Fit(ImPlotAxis & x_axis, ImPlotAxis & y_axis) const {
//			int count = ImMin(Getter1.Count, Getter2.Count);
//			for (int i = 0; i < count; ++i) {
//				ImPlotPoint p1 = Getter1(i); p1.y -= HalfHeight;
//				ImPlotPoint p2 = Getter2(i); p2.y += HalfHeight;
//				x_axis.ExtendFitWith(y_axis, p1.x, p1.y);
//				y_axis.ExtendFitWith(x_axis, p1.y, p1.x);
//				x_axis.ExtendFitWith(y_axis, p2.x, p2.y);
//				y_axis.ExtendFitWith(x_axis, p2.y, p2.x);
//			}
//		}
//		const _Getter1 & Getter1;
//		const _Getter2 & Getter2;
//		const double    HalfHeight;
//	};
//
//	template <typename T>
//	struct IndexerIdx {
//		IndexerIdx(const T * data, int count, int offset = 0, int stride = sizeof(T)) :
//			Data(data),
//			Count(count),
//			Offset(count ? ImPosMod(offset, count) : 0),
//			Stride(stride) {
//		}
//		template <typename I> IMPLOT_INLINE double operator()(I idx) const {
//			return (double)IndexData(Data, idx, Count, Offset, Stride);
//		}
//		const T * Data;
//		int Count;
//		int Offset;
//		int Stride;
//	};
//
//	struct IndexerLin {
//		IndexerLin(double m, double b) : M(m), B(b) {}
//		template <typename I>
//		double operator()(I idx) const {
//			return M * idx + B;
//		}
//		const double M;
//		const double B;
//	};
//
//	struct IndexerConst {
//		IndexerConst(double ref) : Ref(ref) {}
//		template <typename I>
//		double operator()(I) const { return Ref; }
//		const double Ref;
//	};
//
//	template <typename _IndexerX, typename _IndexerY>
//	struct GetterXY {
//		GetterXY(_IndexerX x, _IndexerY y, int count) : IndxerX(x), IndxerY(y), Count(count) {}
//		template <typename I> ImPlotPoint operator()(I idx) const {
//			return ImPlotPoint(IndxerX(idx), IndxerY(idx));
//		}
//		const _IndexerX IndxerX;
//		const _IndexerY IndxerY;
//		const int Count;
//	};
//
//	template <class _Renderer>
//	void RenderPrimitivesEx(const _Renderer & renderer, ImDrawList & draw_list, const ImRect & cull_rect) {
//		unsigned int prims = renderer.Prims;
//		unsigned int prims_culled = 0;
//		unsigned int idx = 0;
//		renderer.Init(draw_list);
//		while (prims) {
//			// find how many can be reserved up to end of current draw command's limit
//			unsigned int cnt = ImMin(prims, (MaxIdx<ImDrawIdx>::Value - draw_list._VtxCurrentIdx) / renderer.VtxConsumed);
//			// make sure at least this many elements can be rendered to avoid situations where at the end of buffer this slow path is not taken all the time
//			if (cnt >= ImMin(64u, prims)) {
//				if (prims_culled >= cnt)
//					prims_culled -= cnt; // reuse previous reservation
//				else {
//					// add more elements to previous reservation
//					draw_list.PrimReserve((cnt - prims_culled) * renderer.IdxConsumed, (cnt - prims_culled) * renderer.VtxConsumed);
//					prims_culled = 0;
//				}
//			} else {
//				if (prims_culled > 0) {
//					draw_list.PrimUnreserve(prims_culled * renderer.IdxConsumed, prims_culled * renderer.VtxConsumed);
//					prims_culled = 0;
//				}
//				cnt = ImMin(prims, (MaxIdx<ImDrawIdx>::Value - 0/*draw_list._VtxCurrentIdx*/) / renderer.VtxConsumed);
//				// reserve new draw command
//				draw_list.PrimReserve(cnt * renderer.IdxConsumed, cnt * renderer.VtxConsumed);
//			}
//			prims -= cnt;
//			for (unsigned int ie = idx + cnt; idx != ie; ++idx) {
//				if (!renderer.Render(draw_list, cull_rect, idx))
//					prims_culled++;
//			}
//		}
//		if (prims_culled > 0)
//			draw_list.PrimUnreserve(prims_culled * renderer.IdxConsumed, prims_culled * renderer.VtxConsumed);
//	}
//
//	template <template <class> class _Renderer, class _Getter, typename ...Args>
//	void RenderPrimitives1(const _Getter & getter, Args... args) {
//		ImDrawList & draw_list = *GetPlotDrawList();
//		const ImRect & cull_rect = GetCurrentPlot()->PlotRect;
//		RenderPrimitivesEx(_Renderer<_Getter>(getter, args...), draw_list, cull_rect);
//	}
//
//	template <template <class, class> class _Renderer, class _Getter1, class _Getter2, typename ...Args>
//	void RenderPrimitives2(const _Getter1 & getter1, const _Getter2 & getter2, Args... args) {
//		ImDrawList & draw_list = *GetPlotDrawList();
//		const ImRect & cull_rect = GetCurrentPlot()->PlotRect;
//		RenderPrimitivesEx(_Renderer<_Getter1, _Getter2>(getter1, getter2, args...), draw_list, cull_rect);
//	}
//
//	template <typename Getter1, typename Getter2>
//	void PlotBarsHEx(const char * label_id, const Getter1 & getter1, const Getter2 & getter2, double height, ImPlotBarsFlags flags) {
//		if (BeginItemEx(label_id, DPlot::FitterBarH<Getter1, Getter2>(getter1, getter2, height), flags, ImPlotCol_Fill)) {
//			const ImPlotNextItemData & s = GetItemData();
//			const ImU32 col_fill = ImGui::GetColorU32(s.Colors[ImPlotCol_Fill]);
//			const ImU32 col_line = ImGui::GetColorU32(s.Colors[ImPlotCol_Line]);
//			bool rend_fill = s.RenderFill;
//			bool rend_line = s.RenderLine;
//			if (rend_fill) {
//				RenderPrimitives2<RendererBarsFillH>(getter1, getter2, col_fill, height);
//				if (rend_line && col_fill == col_line)
//					rend_line = false;
//			}
//			if (rend_line) {
//				RenderPrimitives2<RendererBarsLineH>(getter1, getter2, col_line, height, s.LineWeight);
//			}
//			EndItem();
//		}
//	}
//}

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

	IMPLOT_TMP void PlotBars(std::string label_id, std::vector<T> values, double bar_size, double shift, ImPlotBarsFlags flags, int offset, int stride) {
			ImPlot::PlotBars(label_id.c_str(), &values[0], values.size(), bar_size, shift, flags, offset, stride);
	}
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
