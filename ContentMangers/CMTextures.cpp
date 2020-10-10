#include "tyrpch.h"
#include "CMTextures.h"
#include <algorithm>
#include "TyrException.h"
#include "Texture.h"
#include "StringManipulation.h"
#include "ContentManager.h"
#include "BinStructureHelpers.h"
#include "BinaryWriter.h"

tyr::CMTextures::CMTextures()
	:CMBase("CMTextures", "TEXTURE_NAME.png", 219.f)
{
	
}

TextureID tyr::CMTextures::LoadTexture(const std::string& dataFolder, const std::string& name)
{
	auto found = std::find(m_pContent.begin(), m_pContent.end(), name);

	if (found != m_pContent.end())
	{
		return static_cast<TextureID>(std::distance(m_pContent.begin(), found));
	}

	try
	{
		auto pTemp = new Texture(dataFolder, name);
		m_pContent.emplace_back(pTemp);
		return static_cast<TextureID>(m_pContent.size() - 1);
	}
	catch (TyrException& e)
	{
		MessageBoxW(NULL, e.what(), L"Error", MB_ICONERROR);
		return 0;
	}
}

tyr::Texture* tyr::CMTextures::GetTexture(TextureID id) const
{
	if (id >= m_pContent.size()) return nullptr;

	return m_pContent[id];
}

#ifdef EDITOR_MODE

void tyr::CMTextures::Save(BinaryWriter& writer)
{
	writer.Write(ContentType::Texture);
	writer.Write(static_cast<UINT>(m_pContent.size()));
	std::for_each(m_pContent.begin(), m_pContent.end(), [&writer](Texture* t) { writer.Write(t->GetName()); });
}

void tyr::CMTextures::OnBtnDeleteClicked(Texture* pDeletedContent)
{
	SAFE_DELETE(pDeletedContent);
}
void tyr::CMTextures::OnBtnAddClicked(const std::string& what)
{
	CONTENT_MANAGER->LoadTexture(what);
}
void tyr::CMTextures::OnItemSelected(int selected)
{
	if (SDXL_ImGui_Begin("Texture##", nullptr, SDXL_ImGuiWindowFlags_AlwaysAutoResize))
	{
		auto di = m_pContent[selected]->GetDimension();

		SDXL_ImGui_Image(m_pContent[selected]->SDXL(), { di.x, di.y }, SDXL::Float2{ 0.f, 0.f }, SDXL::Float2{ 1.f, 1.f });

	}
	SDXL_ImGui_End();
}

/*
void tyr::CMTextures::BtnRemoveSelectedTexture(int& selected)
{
	UNREFERENCED_PARAMETER(selected);
	if (SDXL_ImGui_Button("Remove Selected##TextureContentManager"))
	{

		if (selected == -1) return;
		SDXL_ImGui_OpenPopup("Are you sure?##TextureContentManager");

	}

	SDXL_ImGui_SetNextWindowSize(SDXL::Float2(400.f, 100.f));
	if (SDXL_ImGui_BeginPopupModal("Are you sure?##TextureContentManager", nullptr, SDXL_ImGuiWindowFlags_NoMove | SDXL_ImGuiWindowFlags_NoResize))
	{
		std::string what = "This cannot be undone!";
		std::string whatSecond = "Texture: " + m_pContent[selected]->GetName() + " will be deleted!";
		SDXL_ImGui_Text(what.c_str());
		SDXL_ImGui_Text(whatSecond.c_str());

		if (SDXL_ImGui_Button("Never Mind##TextureContentManager", SDXL::Float2(180.f, 20.f)))
		{
			SDXL_ImGui_CloseCurrentPopup();
		}
		SDXL_ImGui_SameLine();
		if (SDXL_ImGui_Button("Yes, I understand", SDXL::Float2(180.f, 20.f)))
		{
			const std::string WhatUnloaded = "[UNLOADED] " + m_pContent[selected]->GetName();
			if (selected != static_cast<int>(m_pContent.size()) - 1)
			{
				Texture* pToDelete = m_pContent[selected];
				for (int i = selected; i < static_cast<int>(m_pContent.size() - 1); i++)
				{
					m_pContent[i] = m_pContent[i + 1];
				}

				m_pContent[m_pContent.size() - 1] = pToDelete;

			}

			SAFE_DELETE(m_pContent[selected]);
			m_pContent.erase(std::remove(m_pContent.begin(), m_pContent.end(), m_pContent[selected]));


			ContentManager::GetInstance()->Save();
			SDXL_ImGui_ConsoleLog(WhatUnloaded.c_str());
			selected = -1;
			SDXL_ImGui_CloseCurrentPopup();
		}

		SDXL_ImGui_EndPopup();
	}
}
*/


#endif
