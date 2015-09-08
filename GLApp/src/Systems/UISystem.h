#pragma once

#include "3rdparty/entityx/System.h"
#include "3rdparty/rde/vector.h"
#include "3rdparty/rde/rde_string.h"

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

	const rde::vector<UIComponent*>& getSortedUIComponents() const { return m_uiComponents; }

private:

	void onUIComponentRemoved(UIComponent* component);

private:

	rde::vector<UIComponent*> m_uiComponents;
};
