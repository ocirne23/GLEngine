#pragma once

#include "EASTL/string.h"

class UIFrame;

class UIComponent
{
public:
	friend class UISystem;

	enum class ELayer
	{
		Layer0, Layer1, Layer2
	};

public:

	UIComponent(const eastl::string& jsonFilePath, ELayer layer);
	~UIComponent();
	
	ELayer getLayer() const { return m_layer; }
	const UIFrame& getUIFrame() const { return *m_uiFrame; }
	
private:

	ELayer m_layer       = ELayer::Layer0;
	UIFrame* m_uiFrame   = NULL;
	UISystem* m_uiSystem = NULL; // Set by UISystem
};