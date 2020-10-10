#pragma once
#include "CMBase.h"

namespace tyr
{
	class Animator;
	class CMAnimators final: public CMBase<Animator>
	{
	public:
		CMAnimators();
		~CMAnimators() = default;

		_NODISCARD Animator* GetAnimator(AnimatorID id) const noexcept;
		_NODISCARD Animator* GetAnimator(const std::string& animatorName) const;
		
#ifdef EDITOR_MODE
		void Save(BinaryWriter& writer) override;
#endif
	protected:
#ifdef EDITOR_MODE
		void OnBtnDeleteClicked(Animator* pDeletedContent) override;
		void OnBtnAddClicked(const std::string& what) override;
		void OnItemDoubleClicked(int selected) override;
		
	private:
		static void GenerateTabItems();
#endif

	public:
		CMAnimators(const CMAnimators&) = delete;
		CMAnimators(CMAnimators&&) = delete;
		CMAnimators& operator=(const CMAnimators&) = delete;
		CMAnimators& operator=(CMAnimators&&) = delete;
	};
}
