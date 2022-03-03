#include "SetUpGUI.h"


UserInterface::UserInterface(ApplicationGraphics* graphics)
{

	ImGui::CreateContext();
	ImGui_ImplDX11_Init(graphics->GetDevice(), graphics->GetContext());
	ImGui_ImplWin32_Init(graphics->getWnd());
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.WantCaptureMouse = true;
}

UserInterface::~UserInterface()
{

}

void UserInterface::FrameBegin()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

}

void UserInterface::FrameEnd()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void UserInterface::ShutDown()
{
}


