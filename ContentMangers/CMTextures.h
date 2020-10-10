#pragma once
#include "CMBase.h"

namespace tyr
{
	class BinaryWriter;
	class ContentManager;
	class Texture;
	
	class CMTextures final : public CMBase<Texture>
	{
	public:
		CMTextures();
		~CMTextures() = default;

		TextureID LoadTexture(const std::string& dataFolder, const std::string& name);
		Texture* GetTexture(TextureID id) const;
		
#ifdef EDITOR_MODE
		void Save(BinaryWriter& writer) override;
#endif
		
	protected:
#ifdef EDITOR_MODE
		void OnBtnDeleteClicked(Texture* pDeletedContent) override;
		void OnBtnAddClicked(const std::string& what) override;
		void OnItemSelected(int selected) override;
#endif

		
	public:
		CMTextures(const CMTextures&) = delete;
		CMTextures(CMTextures&&) = delete;
		CMTextures& operator=(const CMTextures&) = delete;
		CMTextures& operator=(CMTextures&&) = delete;
	};


}
