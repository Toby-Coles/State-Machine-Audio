#pragma once
#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_win32.h"
#include "Imgui\imgui_impl_dx11.h"
#include "ApplicationGraphics.h"

class UserInterface
{
public:
	UserInterface() = default;
	UserInterface(ApplicationGraphics* Graphics);
	~UserInterface();

	void FrameBegin();
	void FrameEnd();
	void ShutDown();


private:

};

