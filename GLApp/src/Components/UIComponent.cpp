#include "Components/UIComponent.h"

#include "Graphics/UI/UIFrame.h"
#include "Systems/UISystem.h"

UIComponent::UIComponent(const eastl::string& a_jsonFilePath, ELayer a_layer)
	: m_layer(a_layer)
{
	m_uiFrame = new UIFrame();
	m_uiFrame->initialize(a_jsonFilePath.c_str());
}

UIComponent::~UIComponent()
{
	m_uiSystem->onUIComponentRemoved(this);
}