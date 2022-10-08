#pragma once

#include "GUI/GUIState.h"

class GUIStateDrawer {
	public:
		void draw(GUIState &);
	private:
		void drawPlots(GUIState &);
};
