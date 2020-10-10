#include "tyrpch.h"
#include "ContentManager.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <filesystem>
#include <direct.h>

#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "BinStructureHelpers.h"

#include "Font.h"
#include "Texture.h"
#include "Animation.h"
#include "Animator.h"
#include "Scene.h"


#include "StringManipulation.h"
#include "./Editor/ETabItem.h"
#include "TyrException.h"

#include "CMTextures.h"
#include "CMScenes.h"
#include "CMAnimations.h"
#include "CMAnimators.h"

#include "GameContext.h"
#include "SceneManager.h"

#define CONTENT_PATH "./TyrBin/Content.tyr"
#define ANIMATION_SUFFIX ".tyrAnimation"

tyr::ContentManager* tyr::ContentManager::pInstance = nullptr;


tyr::ContentManager::ContentManager()
	: m_IsInitialized(false)
	, m_DataFolder("")
	, m_pCMTextures(nullptr)
	, m_pCMScenes(nullptr)
	, m_pCMAnimations(nullptr)
	, m_pCMAnimators(nullptr)
	, m_pContext(nullptr)
#ifdef EDITOR_MODE
	, m_SelectedContentWindow(ContentWindow::None)
#endif
{
}

tyr::ContentManager::~ContentManager()
{
	SAFE_DELETE(m_pCMTextures);
	SAFE_DELETE(m_pCMScenes);
	SAFE_DELETE(m_pCMAnimations);
	SAFE_DELETE(m_pCMAnimators);
	
	std::for_each(m_pFonts.begin(), m_pFonts.end(),[](auto* p) {SAFE_DELETE(p)});
}



tyr::ContentManager* tyr::ContentManager::GetInstance()
{
	if(!pInstance)
	{
		pInstance = new ContentManager();
	}
	return pInstance;
}


void tyr::ContentManager::Initialize(GameContext* pContext)
{
	m_pContext = pContext;
	BinaryReader reader(CONTENT_PATH);

	if (!reader.IsOpen())
		THROW_ERROR(L"Could not open file");
	
	ULONG64 header = reader.Read<ULONG64>();
	if (header != 0x30140623)
	{
		THROW_ERROR(L"This is not the contentFile");
	}

	m_DataFolder = reader.Read<std::string>();
	
	m_SceneFolder = reader.Read<std::string>();
	m_TextureFolder = reader.Read<std::string>();
	m_FontFolder = reader.Read<std::string>();

	m_AnimationFolder = reader.Read<std::string>();
	m_AnimatorFolder = reader.Read<std::string>();

	ContentType type = reader.Read<ContentType>();
	while(type != ContentType::End)
	{
		//all types (except end) have a size and a size
		UINT size = reader.Read<UINT>();

		switch (type)
		{
		case ContentType::Texture:
			{
				m_pCMTextures = new CMTextures();
				m_pCMTextures->Resize(size);
				const std::string absoluteTextureFolder = m_DataFolder + m_TextureFolder;
				for (UINT i{ 0 }; i < size; i++)
				{
					std::string name = reader.Read<std::string>();
					m_pCMTextures->InsertAt(i, new Texture(absoluteTextureFolder, name));
				}

				break;
			}

		case ContentType::Font:
			LoadFont("Fonts/Arcade_20.fnt");

			if(size > 0)
				THROW_ERROR(L"Font is not implemented yet");

			break;
		case ContentType::Animation:
			{
				m_pCMAnimations = new CMAnimations();
				m_pCMAnimations->Resize(size);

				const std::string absoluteAnimationFolder = GetAbsoluteAnimationFolder();
				for (UINT i{ 0 }; i < size; i++)
				{
					std::string animationName = reader.Read<std::string>();
					
					m_pCMAnimations->InsertAt(i, Animation::Create(absoluteAnimationFolder + animationName));
				}


				break;
			}
		case ContentType::Scenes:
			{
				m_pCMScenes = new CMScenes();
			
				if(size > 0)
				{
					m_pCMScenes->Resize(size);

					for (UINT i{ 0 }; i < size; i++)
					{
						std::string sceneName = reader.Read<std::string>();
						m_pCMScenes->InsertAt(i, new Scene(sceneName, GetAbsoluteSceneFolder()));
					}
				}
#ifdef EDITOR_MODE
				else //Default Empty Scene
				{
					m_pCMScenes->Resize(1);
					m_pCMScenes->InsertAt(0, Scene::GenerateNew("New tyrScene", GetAbsoluteSceneFolder()));
				}
#endif

				break;
			}
		case ContentType::Animators:
			{
				m_pCMAnimators = new CMAnimators();
				m_pCMAnimators->Resize(size);

				const std::string absoluteAnimationFolder = GetAbsoluteAnimatorFolder();
				for (UINT i{ 0 }; i < size; i++)
				{
					std::string animatorName = reader.Read<std::string>();
					m_pCMAnimators->InsertAt(i, Animator::Create(absoluteAnimationFolder + animatorName));
				}

				break;
			}

		case ContentType::End:
			THROW_ERROR(L"Type end should not trigger switch case");
		default:
			THROW_ERROR(L"This Type is invalid");
			;
		}

		type = reader.Read<ContentType>();
	}

	
}
tyr::Scene* tyr::ContentManager::GetCurrentScene() const noexcept
{
	return m_pContext->pSceneManager->GetCurrentScene();
}

void tyr::ContentManager::SetCurrentScene(SceneID id)
{
	m_pContext->pSceneManager->SetCurrentScene(id);
}

#ifdef EDITOR_MODE

//PUBLIC
void tyr::ContentManager::RenderEditor()
{

	EMainMenuBarItem();
	EMainWindow();
	ESettingsContentPath();

}

void tyr::ContentManager::EditorTextureDropDown(const char* imGuiID, TextureID& textureID)
{
	m_pCMTextures->ItemDropDown(imGuiID, textureID);
	
}

void tyr::ContentManager::EditorAnimationDropDown(const char* imGUiID, AnimationID& animationID)
{
	m_pCMAnimations->ItemDropDown(imGUiID, animationID);
}

void tyr::ContentManager::EditorAnimatorDropDown(const char* imGUiID, AnimatorID& animatorID)
{
	m_pCMAnimators->ItemDropDown(imGUiID, animatorID);
}

void tyr::ContentManager::Save()
{
	BinaryWriter writer(CONTENT_PATH);

	//Binsctructure
	// Long double -> Header (JorikWeymansTyr hashed via Adler32 to this value)
	// String -> DataFolder;
	// String -> SceneFolder;
	// String -> AnimationFolder;
	// String -> TextureFolder
	//
	// ContentType::Texture
	// amount of textures
	// foreach(texture.Name)
	//
	// ContentType::Animation
	// amount of animations
	// foreach(animation.Name)
	//
	// ContentType::End (Marking EOF)

	ULONG64 header = 0x30140623;
	writer.Write(header); //Header
	
	writer.WriteString(m_DataFolder);
	
	writer.Write(m_SceneFolder);
	writer.Write(m_TextureFolder);
	writer.Write(m_FontFolder);
	writer.Write(m_AnimationFolder);
	writer.Write(m_AnimatorFolder);
	
	m_pCMTextures->Save(writer);
	m_pCMScenes->Save(writer);
	m_pCMAnimations->Save(writer);
	m_pCMAnimators->Save(writer);
	
	writer.Write(ContentType::Font);
	writer.Write(static_cast<UINT>(0 /*m_pFonts.size()*/));
	//std::for_each(m_pFonts.begin(), m_pFonts.end(), [&writer](Font* f) { writer.Write(f->GetName()); });

	


	
	writer.Write(ContentType::End);
	
}

//PRIVATE
void tyr::ContentManager::EMainMenuBarItem()
{
	if (SDXL_ImGui_Selectable("ContentManager", false, SDXL_ImGuiSelectableFlags_DontClosePopups, SDXL::Float2(100, 20)))
	{
		m_OpenContentManager = !m_OpenContentManager;
		if (m_OpenContentManager)
		{
			SDXL_ImGui_SetNextWindowPos(SDXL::Float2{ 100.f,100.f });
			SDXL_ImGui_SetNextWindowSize(SDXL::Float2{ 500.f, 300.f });
		}
	}

	
}

void tyr::ContentManager::EMainWindow()
{
	if (m_OpenContentManager)
	{

		SDXL_ImGui_SetNextWindowSize(SDXL::Float2(500.f, 500.f));
		if (SDXL_ImGui_Begin("ContentManager##ContentManager", &m_OpenContentManager, SDXL_ImGuiWindowFlags_NoSavedSettings | SDXL_ImGuiWindowFlags_MenuBar | SDXL_ImGuiWindowFlags_NoResize))
		{
			EMenuBar();

			EContentSelectorWindow();
			SDXL_ImGui_SameLine();
			
		}


		ERenderContentWindow();

		



		SDXL_ImGui_End();

	}
}

void tyr::ContentManager::EContentSelectorWindow()
{
	
	const SDXL_ImGuiSelectableFlags flag = SDXL_ImGuiSelectableFlags_AllowDoubleClick | SDXL_ImGuiSelectableFlags_DontClosePopups;
	
	if (SDXL_ImGui_BeginChild("child#ContentManager", SDXL::Float2(100, 0), true))
	{
		if (SDXL_ImGui_Selectable("Textures##ContentManager", m_SelectedContentWindow == ContentWindow::Textures, flag))
		{
			m_SelectedContentWindow = ContentWindow::Textures;
		}
		if (SDXL_ImGui_Selectable("Scenes##ContentManager", m_SelectedContentWindow == ContentWindow::Scenes, flag))
		{
			m_SelectedContentWindow = ContentWindow::Scenes;
		}

		if (SDXL_ImGui_Selectable("Animations##ContentManager", m_SelectedContentWindow == ContentWindow::Animations, flag))
		{
			m_SelectedContentWindow = ContentWindow::Animations;
		}
		
		if (SDXL_ImGui_Selectable("Animators##ContentManager", m_SelectedContentWindow == ContentWindow::Animators, flag))
		{
			m_SelectedContentWindow = ContentWindow::Animators;
		}
		SDXL_ImGui_EndChild();
	}


}

void tyr::ContentManager::EMenuBar()
{
	if (SDXL_ImGui_BeginMenuBar())
	{
		if (SDXL_ImGui_BeginMenu("Settings##ContentManger"))
		{
			if (SDXL_ImGui_MenuItem("File paths"))
			{

				m_OpenFilePathSettings = true;


				strcpy_s(m_CharDataPath, m_DataFolder.c_str());
				strcpy_s(m_CharSceneFolder, m_SceneFolder.c_str());
				strcpy_s(m_CharTextureFolder, m_TextureFolder.c_str());
				strcpy_s(m_CharAnimationFolder, m_AnimationFolder.c_str());
				strcpy_s(m_CharAnimatorFolder, m_AnimatorFolder.c_str());
			}
			SDXL_ImGui_EndMenu();
		}
		if(SDXL_ImGui_MenuItem("Save##ContentManagerMenu"))
		{
			Save();
		}
		SDXL_ImGui_EndMenuBar();
	}
}

void tyr::ContentManager::ESettingsContentPath()
{
	if (m_OpenFilePathSettings)
	{
		SDXL_ImGui_OpenPopup("ContentManager Settings");
		m_OpenFilePathSettings = false;
		m_AreContentSettingsOpen = true;

		//SDXL_ImGui_SetNextWindowPos(SDXL::Float2{ 100.f,100.f });
		SDXL_ImGui_SetNextWindowSize(SDXL::Float2{ 500.f, 300.f });
	}
	
	static bool pathHasChanged = false;
	SDXL_ImGuiWindowFlags flags = SDXL_ImGuiWindowFlags_NoMove | SDXL_ImGuiWindowFlags_NoResize;
	if (pathHasChanged) flags |= SDXL_ImGuiWindowFlags_UnsavedDocument;

	if (SDXL_ImGui_BeginPopupModal("ContentManager Settings", &m_AreContentSettingsOpen, flags))
	{
		//Data Path
		if (SDXL_ImGui_InputText("Data Path##ContentManager", m_CharDataPath, ARRAY_SIZE(m_CharDataPath)))
			pathHasChanged = true;

		std::filesystem::path path = m_CharDataPath;
		SDXL_ImGui_TextDisabled("[EPath]%s", std::filesystem::absolute(path).string().c_str());

		//Scene Folder
		if (SDXL_ImGui_InputText("Scene Folder##ContentManager", m_CharSceneFolder, ARRAY_SIZE(m_CharSceneFolder)))
			pathHasChanged = true;

		path = std::string(m_CharDataPath) + std::string(m_CharSceneFolder);
		SDXL_ImGui_TextDisabled("[EPath]%s", std::filesystem::absolute(path).string().c_str());

		//Texture Folder
		if (SDXL_ImGui_InputText("Texture folder##ContentManager", m_CharTextureFolder, ARRAY_SIZE(m_CharTextureFolder)))
			pathHasChanged = true;

		path = std::string(m_CharDataPath) + std::string(m_CharTextureFolder);
		SDXL_ImGui_TextDisabled("[EPath]%s", std::filesystem::absolute(path).string().c_str());
		
		//Animation Folder
		if (SDXL_ImGui_InputText("Animation folder##ContentManager", m_CharAnimationFolder, ARRAY_SIZE(m_CharAnimationFolder)))
			pathHasChanged = true;

		path = std::string(m_CharDataPath) + std::string(m_CharAnimationFolder);
		SDXL_ImGui_TextDisabled("[EPath]%s", std::filesystem::absolute(path).string().c_str());

		//Animator Folder
		if (SDXL_ImGui_InputText("Animator folder##ContentManager", m_CharAnimatorFolder, ARRAY_SIZE(m_CharAnimatorFolder)))
			pathHasChanged = true;

		path = std::string(m_CharDataPath) + std::string(m_CharAnimatorFolder);
		SDXL_ImGui_TextDisabled("[EPath]%s", std::filesystem::absolute(path).string().c_str());


		SDXL_ImGui_Separator();

		if (SDXL_ImGui_Button("Cancel##SettingsContentManager"))
		{
			pathHasChanged = false;
			SDXL_ImGui_CloseCurrentPopup();

		}
		SDXL_ImGui_SameLine();
		if (SDXL_ImGui_Button("Save"))
		{
			m_DataFolder = std::string(m_CharDataPath);
			m_SceneFolder = std::string(m_CharSceneFolder);
			m_AnimationFolder = std::string(m_CharAnimationFolder);
			m_AnimatorFolder = std::string(m_CharAnimatorFolder);

			CreateFolders();
			
			pathHasChanged = false;
			Save();
			SDXL_ImGui_CloseCurrentPopup();
		}

		SDXL_ImGui_EndPopup();
	}
}

void tyr::ContentManager::ERenderContentWindow() const
{
	switch(m_SelectedContentWindow)
	{
		case ContentWindow::None: break;
		case ContentWindow::Textures:
			if (SDXL_ImGui_BeginChild("TextureWindow##ContentManager", SDXL::Float2(0.f, 0.f), false))
			{
				m_pCMTextures->RenderEditor();
				SDXL_ImGui_EndChild();
			}
		break;
		case ContentWindow::Scenes: 
			if(SDXL_ImGui_BeginChild("SceneWindow##ContentManager", SDXL::Float2(0.f,0.f), false))
			{
				m_pCMScenes->RenderEditor();
				SDXL_ImGui_EndChild();
			}
		break;
		case ContentWindow::Animations:
			if (SDXL_ImGui_BeginChild("AnimationsWindow##ContentManager", SDXL::Float2(0.f, 0.f), false))
			{
				m_pCMAnimations->RenderEditor();
				SDXL_ImGui_EndChild();
			}
		break;
		case ContentWindow::Animators:
			if (SDXL_ImGui_BeginChild("AnimatorsWindow##ContentManager", SDXL::Float2(0.f, 0.f), false))
			{
				m_pCMAnimators->RenderEditor();
				SDXL_ImGui_EndChild();
			}
			break;
		default:
			SDXL_ImGui_ConsoleLogError("You have selected an invalid window");
		;
	}
}

void tyr::ContentManager::CreateFolders() const
{
	std::filesystem::path path = m_DataFolder + m_SceneFolder;
	_mkdir(std::filesystem::absolute(path).string().c_str());

	path = m_DataFolder + m_TextureFolder;
	_mkdir(std::filesystem::absolute(path).string().c_str());

	path = m_DataFolder + m_FontFolder;
	_mkdir(std::filesystem::absolute(path).string().c_str());

	path = m_DataFolder + m_AnimationFolder;
	_mkdir(std::filesystem::absolute(path).string().c_str());

	path = m_DataFolder + m_AnimatorFolder;
	_mkdir(std::filesystem::absolute(path).string().c_str());
	
}


#endif
void tyr::ContentManager::Destroy()
{
	delete pInstance;
	pInstance = nullptr;
}

TextureID tyr::ContentManager::LoadTexture(const std::string& path)
{
	return m_pCMTextures->LoadTexture(m_DataFolder + m_TextureFolder, path);
}
FontID tyr::ContentManager::LoadFont(const std::string& path)
{
	auto found = std::find(m_pFonts.begin(), m_pFonts.end(), path);

	if (found != m_pFonts.end())
	{
		//return 0;
		return static_cast<FontID>(std::distance(m_pFonts.begin(), found));
		
	}

	auto pTemp = new Font(m_DataFolder, path);
	m_pFonts.emplace_back(pTemp);
	return static_cast<FontID>(m_pFonts.size() - 1);
}


tyr::Texture* tyr::ContentManager::GetTexture(TextureID id) const
{
	return m_pCMTextures->GetTexture(id);
}
tyr::Font const* tyr::ContentManager::GetFont(FontID id) const
{
	if (id >= m_pFonts.size()) return nullptr;

	return m_pFonts[id];
}
tyr::Animation* tyr::ContentManager::GetAnimation(AnimationID id) const
{
	return m_pCMAnimations->GetAnimation(id);
}

tyr::Animation* tyr::ContentManager::GetAnimationCopy(AnimationID id) const
{
	auto pOriginalAnimation = m_pCMAnimations->GetAnimation(id);
	if(pOriginalAnimation)
		return 	pOriginalAnimation->CreateCopy();

	return nullptr;
}

tyr::Animation* tyr::ContentManager::GetAnimation(const std::string& animationName) const
{
	return m_pCMAnimations->GetAnimation(animationName);
}
AnimationID tyr::ContentManager::GetAnimationID(const std::string& animationName) const
{
	return m_pCMAnimations->GetAnimationID(animationName);
}
AnimationID tyr::ContentManager::GetAnimationID(Animation* pAnimation) const noexcept
{
	return m_pCMAnimations->GetAnimationID(pAnimation);
}

tyr::Animator* tyr::ContentManager::GetAnimator(AnimatorID id) const noexcept
{
	return m_pCMAnimators->GetAnimator(id);
}

tyr::Animator* tyr::ContentManager::GetAnimatorCopy(AnimatorID id) const
{
	auto pOriginalAnimation = m_pCMAnimators->GetAnimator(id);
	if (pOriginalAnimation)
		return 	pOriginalAnimation->CreateCopy();

	return nullptr;
}

tyr::Animator* tyr::ContentManager::GetAnimator(const std::string& animatorName) const
{
	return m_pCMAnimators->GetAnimator(animatorName);
}

