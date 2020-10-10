#include "tyrpch.h"
#include "CMScenes.h"
#include <direct.h>
#include "StringManipulation.h"
#include <filesystem>
#include "Scene.h"
#include "BinaryWriter.h"
#include "BinStructureHelpers.h"
#include "ContentManager.h"
#include "GameContext.h"

#include "Editor/EUI.h"
#include "Editor/EWindowSouth.h"
#include "Editor/ETabScenes.h"

tyr::CMScenes::CMScenes()
	:CMBase("CMScenes", "New scene name", 200.f)
{
}

tyr::CMScenes::~CMScenes()
{
	std::for_each(m_pContent.begin(), m_pContent.end(), [](Scene* s) {SAFE_DELETE(s)});
	m_pContent.clear();
}


tyr::Scene* tyr::CMScenes::GetScene(const std::string& sceneName) const noexcept
{
	auto found = std::find_if(m_pContent.begin(), m_pContent.end(), [&sceneName](Scene* s) { return s->GetName() == sceneName; });
	if (found != m_pContent.end())
	{
		return *found;
	}
	return nullptr;
}

tyr::Scene* tyr::CMScenes::GetScene(SceneID id) const noexcept
{
	if(id < m_pContent.size())
	{
		return m_pContent.at(id);
	}
	return nullptr;
}

#ifdef EDITOR_MODE

void tyr::CMScenes::Save(BinaryWriter& writer)
{
	writer.Write(ContentType::Scenes);
	writer.Write(static_cast<UINT>(m_pContent.size()));
	std::for_each(m_pContent.begin(), m_pContent.end(), [&writer](Scene* s) { writer.Write(s->GetName()); });
}

void tyr::CMScenes::OnBtnDeleteClicked(Scene* pDeletedContent)
{
	if (CONTENT_MANAGER->GetCurrentScene() == pDeletedContent)
	{
		CONTENT_MANAGER->SetCurrentScene(0);
	}
	
	SAFE_DELETE(pDeletedContent);

	CONTENT_MANAGER->GetContext()->pEditorUI->GetWindow<EWindowSouth>()->GetTabItem<ETabScenes>()->CreateTabItems();
}
void tyr::CMScenes::OnBtnAddClicked(const std::string& what)
{
	Scene* pScene = Scene::GenerateNew(what, ContentManager::GetInstance()->GetAbsoluteSceneFolder());
	m_pContent.emplace_back(pScene);

	CONTENT_MANAGER->GetContext()->pEditorUI->GetWindow<EWindowSouth>()->GetTabItem<ETabScenes>()->CreateTabItems();
}
void tyr::CMScenes::OnItemDoubleClicked(int selected)
{
	CONTENT_MANAGER->SetCurrentScene(selected);
}


#endif

