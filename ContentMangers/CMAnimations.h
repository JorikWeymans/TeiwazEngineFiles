#pragma once
#include "CMBase.h"
namespace tyr
{
	class Animation;
	class BinaryWriter;
	class CMAnimations final : public CMBase<Animation>
	{
	public:
		CMAnimations();
		~CMAnimations() = default;

		_NODISCARD Animation*  GetAnimation(const std::string& animationName) const;
		_NODISCARD Animation*  GetAnimation(AnimationID id) const;
		_NODISCARD AnimationID GetAnimationID(const std::string& animationName) const;
		_NODISCARD AnimationID GetAnimationID(Animation* pAnimation) const noexcept;

		
#ifdef EDITOR_MODE
		void Save(BinaryWriter& writer) override;
#endif
	protected:
#ifdef EDITOR_MODE
		void OnBtnDeleteClicked(Animation* pDeletedContent) override;
		void OnBtnAddClicked(const std::string& what) override;
		void OnItemDoubleClicked(int selected) override;

	private:
		static void GenerateTabItems();
#endif

	public:
		CMAnimations(const CMAnimations&) = delete;
		CMAnimations(CMAnimations&&) = delete;
		CMAnimations& operator=(const CMAnimations&) = delete;
		CMAnimations& operator=(CMAnimations&&) = delete;
	};


}
