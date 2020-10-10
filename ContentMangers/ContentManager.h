#pragma once
#include <vector>

#define CONTENT_MANAGER ContentManager::GetInstance()
namespace tyr
{
	class GameContext;
	struct TabItem;
	class Font;
	class Texture;
	class Animation;
	class TextureComp;
	class Animator;
	
	class CMTextures;
	class CMScenes;
	class CMAnimations;
	class CMAnimators;
	class Scene;
	class ContentManager final
	{
	private:
		enum class ContentWindow : int
		{
			None = -1, Textures = 0, Scenes = 1, Animations = 2, Animators = 3,
		};
		
	public:
		static ContentManager* GetInstance();
		
		//This file is an engine predetermined file
		void Initialize(GameContext* pContext); 
		
		static void Destroy();

		TextureID   LoadTexture(const std::string& path);
		FontID      LoadFont(const std::string& path);

		_NODISCARD Texture*    GetTexture(TextureID id) const; 
		_NODISCARD Font const* GetFont(FontID id) const;
		
		_NODISCARD Animation*  GetAnimation(AnimationID id) const;
		_NODISCARD Animation*  GetAnimationCopy(AnimationID id) const; //The caller is in charge of deleting the copy
		_NODISCARD Animation*  GetAnimation(const std::string& animationName) const; //this does not load the animation, returns pointer to an existing animation (else nullptr)
		_NODISCARD AnimationID GetAnimationID(const std::string& animationName) const;
		_NODISCARD AnimationID GetAnimationID(Animation* pAnimation) const noexcept;

		_NODISCARD Animator* GetAnimator(AnimatorID id) const noexcept;
		_NODISCARD Animator* GetAnimatorCopy(AnimatorID id) const; //The caller is in charge of deleting the copy
		_NODISCARD Animator* GetAnimator(const std::string& animatorName) const;
		
		_NODISCARD Scene* GetCurrentScene() const noexcept;

		
		_NODISCARD const std::string& GetDataFolder() const noexcept { return m_DataFolder; }
		_NODISCARD std::string GetAbsoluteSceneFolder() const        { return m_DataFolder + m_SceneFolder; }
		_NODISCARD std::string GetAbsoluteAnimationFolder() const    { return m_DataFolder + m_AnimationFolder; }
		_NODISCARD std::string GetAbsoluteAnimatorFolder() const     { return m_DataFolder + m_AnimatorFolder; }
		
		_NODISCARD CMScenes* GetCMScenes() const noexcept         { return m_pCMScenes; }
		_NODISCARD CMAnimations* GetCMAnimations() const noexcept { return m_pCMAnimations; }
		_NODISCARD CMAnimators* GetCMAnimators() const noexcept   { return m_pCMAnimators; }
		
		void SetCurrentScene(SceneID id);

#ifdef EDITOR_MODE
		_NODISCARD GameContext* GetContext() const noexcept { return m_pContext; }
		void RenderEditor(); //called in EMenuBar RenderEditor()
		
		void EditorTextureDropDown(const char* imGuiID, TextureID& textureID);
		void EditorAnimationDropDown(const char* imGUiID, AnimationID& animationID);
		void EditorAnimatorDropDown(const char* imGUiID, AnimatorID& animatorID);
		
		void Save();

		friend TextureComp;
#endif
	private:
		ContentManager();
		~ContentManager();

		
		static ContentManager* pInstance;
		bool m_IsInitialized;
		std::string m_DataFolder,m_SceneFolder, m_TextureFolder, m_FontFolder, m_AnimationFolder, m_AnimatorFolder;
		
		CMTextures* m_pCMTextures;
		CMScenes*   m_pCMScenes;
		CMAnimations* m_pCMAnimations;
		CMAnimators* m_pCMAnimators;
		std::vector<Font*>    m_pFonts;
		GameContext* m_pContext; //Weak ptr
#ifdef EDITOR_MODE
		
		bool m_OpenContentManager = false, m_OpenFilePathSettings = false, m_AreContentSettingsOpen = true;
		ContentWindow m_SelectedContentWindow;
		char m_CharDataPath[256]{},
			 m_CharSceneFolder[30]{},
			 m_CharTextureFolder[30]{},
			 m_CharAnimationFolder[30]{},
			 m_CharAnimatorFolder[30]{};
		
		void EMainMenuBarItem();
		void EMainWindow();
		void EMenuBar();
		void EContentSelectorWindow();
		
		void ESettingsContentPath();

		void ERenderContentWindow() const;
		
		void CreateFolders() const;
		
#endif
		
	public:
		ContentManager(const ContentManager&) = delete;
		ContentManager(ContentManager&&) = delete;
		ContentManager& operator=(const ContentManager&) = delete;
		ContentManager& operator=(ContentManager&&) = delete;
	};


}
