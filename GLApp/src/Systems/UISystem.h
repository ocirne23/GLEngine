#pragma once

#include "entityx/System.h"
#include "EASTL/vector.h"
#include "EASTL/string.h"

class UIComponent;
class UIFrame;

class UISystem : public entityx::System<UISystem>, public entityx::Receiver<UISystem>
{
public:
	friend class UIComponent;

	UISystem() {}
	~UISystem();

	void reloadJsonFiles();
	void updateLayout();
	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
	void configure(entityx::EventManager& eventManager) override;
	void receive(entityx::ComponentAddedEvent<UIComponent>& uiComponentAddedEvent);
	const eastl::vector<UIComponent*>& getSortedUIComponents() const { return m_uiComponents; }

private:

	void onUIComponentRemoved(UIComponent* component);

private:

	eastl::vector<UIComponent*> m_uiComponents;
};
