#include "tyrpch.h"
#include "CMAnimations.h"

#include "Animation.h"

#include <algorithm>
#include "ContentManager.h"
#include "BinaryWriter.h"
#include "BinStructureHelpers.h"

#include "GameContext.h"
#include "Editor/EUI.h"
#include "Editor/EWindowSouth.h"
#include "Editor/ETabAnimations.h"

tyr::CMAnimations::CMAnimations()
	:CMBase("CMAnimations", "New Animation name", 110.f)
{
}

tyr::Animation* tyr::CMAnimations::GetAnimation(const std::string& animationName) const
{
	auto found = std::find(m_pContent.begin(), m_pContent.end(), animationName);
	if (found != m_pContent.end())
	{
		return *found;
	}

	return nullptr;
}

tyr::Animation* tyr::CMAnimations::GetAnimation(AnimationID id) const
{
	if (id >= m_pContent.size()) return nullptr;

	return m_pContent[id];
}

AnimationID tyr::CMAnimations::GetAnimationID(const std::string& animationName) const
{
	auto found = std::find(m_pContent.begin(), m_pContent.end(), animationName);

	if (found != m_pContent.end())
	{
		return static_cast<AnimationID>(std::distance(m_pContent.begin(), found));
	}

	return 0; //When animation is not found, return the first one
}

AnimationID tyr::CMAnimations::GetAnimationID(Animation* pAnimation) const noexcept
{
	auto found = std::find(m_pContent.begin(), m_pContent.end(), pAnimation);

	if (found != m_pContent.end())
	{
		return static_cast<AnimationID>(std::distance(m_pContent.begin(), found));
	}

	return 0; //When animation is not found, return the first one
}
#ifdef EDITOR_MODE


void tyr::CMAnimations::Save(BinaryWriter& writer)
{
	writer.Write(ContentType::Animation);
	writer.Write(static_cast<UINT>(m_pContent.size()));
	std::for_each(m_pContent.begin(), m_pContent.end(), [&writer](Animation* a) { writer.Write(a->GetName()); });
}

void tyr::CMAnimations::OnBtnDeleteClicked(Animation* pDeletedContent)
{
	SAFE_DELETE(pDeletedContent);
	GenerateTabItems();
}
void tyr::CMAnimations::OnBtnAddClicked(const std::string& what)
{
	m_pContent.emplace_back(Animation::GenerateNew(what));
	GenerateTabItems();
}
void tyr::CMAnimations::OnItemDoubleClicked(int selected)
{
	CONTENT_MANAGER->GetContext()->pEditorUI->GetWindow<EWindowSouth>()->
	GetTabItem<ETabAnimations>()->OpenAnimationEditorWindow(selected);


}

void tyr::CMAnimations::GenerateTabItems()
{
	CONTENT_MANAGER->GetContext()->pEditorUI->GetWindow<EWindowSouth>()->
	GetTabItem<ETabAnimations>()->CreateTabItems();

}

#endif

