#pragma once

#include "GUI/GUIStateDrawer.h"
#include "GUI/Initialisation.h"
#include "SharedSynchronisation.h"

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

template <typename State> void draw(State);

template <typename State>
class GUI {
	public:
		GUI(SharedSynchronisationToken && sST) : guiInitialisation(std::move(sST.createToken())), guiStateDrawer(std::move(sST)) {};
		~GUI() { stopLoop(); }

		void startLoop() {
			endLoop = false;
			auto loopLambda = [&](){
				guiInitialisation.initialise();
				guiStateDrawer.initialise(guiInitialisation.window);
				guiInitialisation.GUILoop(std::bind_front(&GUI::loop, this), endLoop);
			};
			loopThread = std::thread(loopLambda);
		}
		void stopLoop() {
			endLoop = true;
			if (loopThread.joinable()) {
				loopThread.join();
			}
		}
		State state;
	private:
		GUIInitialisation guiInitialisation;
		GUIStateDrawer guiStateDrawer;
		bool endLoop;
		std::thread loopThread;
		void loop() {
			guiStateDrawer.draw(state);
			std::this_thread::sleep_for(std::chrono::milliseconds(33));
		}
};
