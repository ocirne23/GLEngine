#include "Systems/UISystem.h"

#include "3rdparty/rde/rde_string.h"
#include "3rdparty/rde/sort.h"
#include "Components/UIComponent.h"
#include "Systems/RenderSystem.h"
#include "UI/UIFrame.h"

UISystem::~UISystem()
{
	for (UIComponent* uiComponent : m_uiComponents)
		delete uiComponent->m_uiFrame;
}

void UISystem::update(entityx::EntityManager& a_entities, entityx::EventManager& a_events, entityx::TimeDelta a_dt)
{
}

void UISystem::configure(entityx::EventManager& a_eventManager)
{
	a_eventManager.subscribe<entityx::ComponentAddedEvent<UIComponent>>(*this);
}

void UISystem::receive(entityx::ComponentAddedEvent<UIComponent>& a_uiComponentAddedEvent)
{
	auto& component = a_uiComponentAddedEvent.component;
	// We keep track of a sorted list of components so the render system can draw them in order 
	m_uiComponents.push_back(component.get());
	rde::quick_sort(m_uiComponents.begin(), m_uiComponents.end(), [](UIComponent* l, UIComponent* r) {
		return l->getLayer() < r->getLayer();
	});
}

void UISystem::reloadJsonFiles()
{
	for (UIComponent* uiComponent : m_uiComponents)
	{
		UIFrame* frame = uiComponent->m_uiFrame;
		frame->initialize(frame->getJsonFilePath().c_str());
	}
}

void UISystem::updateLayout()
{
	for (UIComponent* uiComponent : m_uiComponents)
		uiComponent->m_uiFrame->updateLayout();
}

void UISystem::onUIComponentRemoved(UIComponent* component)
{
	m_uiComponents.remove(component);
}

