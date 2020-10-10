#pragma once
#include "CMBase.h"
#include <string>
namespace tyr
{
	class BinaryWriter;
	class Scene;
	class ETabScenes;
	class CMScenes final : public CMBase<Scene>
	{
	public:
		CMScenes();
		~CMScenes();
		
		_NODISCARD Scene* GetScene(const std::string& sceneName) const noexcept;
		_NODISCARD Scene* GetScene(SceneID id) const noexcept;

#ifdef EDITOR_MODE
		void Save(BinaryWriter& writer) override;
		
#endif

	protected:
#ifdef EDITOR_MODE
		void OnBtnDeleteClicked(Scene* pDeletedContent) override;
		void OnBtnAddClicked(const std::string& what) override;
		void OnItemDoubleClicked(int selected) override;
#endif

	public:
		CMScenes(const CMScenes&) = delete;
		CMScenes(CMScenes&&) = delete;
		CMScenes& operator=(const CMScenes&) = delete;
		CMScenes& operator=(CMScenes&&) = delete;
	
	};
}
