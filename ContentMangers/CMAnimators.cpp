#include "tyrpch.h"
#include "CMAnimators.h"


#include "BinStructureHelpers.h"
#include "BinaryWriter.h"
#include "Animator.h"
#include "Editor/EUI.h"
#include "Editor/EWindowSouth.h"
#include "Editor/ETabAnimators.h"
#include "GameContext.h"

tyr::CMAnimators::CMAnimators()
	:CMBase("CMAnimator", "New Animator name", 200.f)
{
}

tyr::Animator* tyr::CMAnimators::GetAnimator(AnimatorID id) const noexcept
{
	if (id >= m_pContent.size()) return nullptr;

	return m_pContent[id];
}

tyr::Animator* tyr::CMAnimators::GetAnimator(const std::string& animatorName) const
{
	const auto found = std::find(m_pContent.begin(), m_pContent.end(), animatorName);
	if (found != m_pContent.end())
	{
		return *found;
	}

	return nullptr;
}
#ifdef EDITOR_MODE
void tyr::CMAnimators::Save(BinaryWriter& writer)
{
	writer.Write(ContentType::Animators);
	writer.Write(static_cast<UINT>(m_pContent.size()));
	std::for_each(m_pContent.begin(), m_pContent.end(), [&writer](Animator* pA) { writer.Write(pA->GetName()); });
}

void tyr::CMAnimators::OnBtnDeleteClicked(Animator* pDeletedContent)
{
	SAFE_DELETE(pDeletedContent);
	GenerateTabItems();
}

void tyr::CMAnimators::OnBtnAddClicked(const std::string& what)
{
	m_pContent.emplace_back(Animator::GenerateNew(what));
	GenerateTabItems();
}

void tyr::CMAnimators::OnItemDoubleClicked(int selected)
{
	UNREFERENCED_PARAMETER(selected);
}

void tyr::CMAnimators::GenerateTabItems()
{
	CONTENT_MANAGER->GetContext()->pEditorUI->GetWindow<EWindowSouth>()->
		GetTabItem<ETabAnimators>()->CreateTabItems();
}
#endif