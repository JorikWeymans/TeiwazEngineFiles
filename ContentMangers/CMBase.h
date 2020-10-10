#pragma once
#include <vector>
#include "ContentManager.h"
#include "StringManipulation.h"
namespace tyr
{
	class BinaryWriter;
	template <class T>
	class CMBase
	{
	public:
		explicit CMBase(const std::string& label, const std::string& addHint, float selectorItemWidth)
#ifdef EDITOR_MODE
			: m_DeleteLabel("Delete##" + label)
			, m_UpLabel    ("UP##"     + label)
			, m_DownLabel  ("DOWN##"   + label)
			, m_AddLabel   ("##"       + label)
			, m_AddHint(addHint)
			, m_SelectorItemWidth(selectorItemWidth)
			{
#else
			{
		   //We don't need parameters when not in editor mode, but leave them for ease of use
			UNREFERENCED_PARAMETER(label);
			UNREFERENCED_PARAMETER(addHint);
			UNREFERENCED_PARAMETER(selectorItemWidth);
#endif
		    }
			
		virtual ~CMBase()
		{
			std::for_each(m_pContent.begin(), m_pContent.end(), [](auto* pC) {SAFE_DELETE(pC)});
		}

		_NODISCARD auto Begin()        noexcept { return m_pContent.begin(); }
		_NODISCARD auto End()          noexcept { return m_pContent.end(); }
		_NODISCARD auto CBegin() const noexcept { return m_pContent.cbegin(); }
		_NODISCARD auto CEnd()   const noexcept { return m_pContent.cend(); }


		void Resize(unsigned int newSize)
		{
			m_pContent.resize(newSize, nullptr);
		}
		void InsertAt(unsigned int index, T* pData) noexcept
		{
			m_pContent.at(index) = pData;
		}
		
#ifdef EDITOR_MODE
		void RenderEditor()
		{
			PreRender();
			
			static int selectedScene = -1;

			ShowContent(selectedScene);

			BtnDeleteContent(selectedScene);
			BtnMoveContentUp(selectedScene);
			BtnMoveContentDown(selectedScene);

			BtnAddContent();

			PostRender();
		}
		virtual void Save(BinaryWriter& writer) = 0;
		void ItemDropDown(const char* imGuiID, UINT&selectedID)
		{
			
			const UINT contentSize = static_cast<UINT>(m_pContent.size());

			//When content gets removed the id can exceed the array,  when it does, clamp this
			if(selectedID >= contentSize)
				selectedID = (contentSize ==0) ? 0 : contentSize - 1; //don't go negative

			
			const char* currentItem = m_pContent[selectedID]->GetName().c_str();
			SDXL_ImGui_SetNextItemWidth(m_SelectorItemWidth);
			if (SDXL_ImGui_BeginCombo(imGuiID, currentItem, SDXL_ImGuiComboFlags_HeightLargest)) // The second parameter is the label previewed before opening the combo.
			{
				for (UINT n = 0; n < contentSize; n++)
				{
					bool IsSelected = (currentItem == m_pContent[n]->GetName().c_str());

					if (SDXL_ImGui_Selectable(m_pContent[n]->GetName().c_str(), IsSelected))
						selectedID = n;
					if (IsSelected)
						SDXL_ImGui_SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
				}
				SDXL_ImGui_EndCombo();
			}
		}
		
#endif	
	protected:
		std::vector<T*> m_pContent;
#ifdef EDITOR_MODE
		virtual void PreRender() {}
		virtual void PostRender(){}
		
		
		//Child classes need to delete this pDeleteContent, it gets removed out of the array but still exists as a pointer
		virtual void OnBtnDeleteClicked(T* pDeletedContent) = 0;
		virtual void OnBtnAddClicked(const std::string& what) = 0;
		virtual void OnItemSelected(int selected) { UNREFERENCED_PARAMETER(selected); }
		virtual void OnItemDoubleClicked(int selected) { UNREFERENCED_PARAMETER(selected); }

		//T needs function GetName() -> std::string
		void ShowContent(int& selected)
		{
			SDXL_ImGui_Text("ID\tName");

			for (int i{ 0 }; i < static_cast<int>(m_pContent.size()); ++i)
			{
				std::string tag = FormatString(" %i\t%s", i, m_pContent[i]->GetName().c_str());

				if (SDXL_ImGui_Selectable(tag.c_str(), selected == i, SDXL_ImGuiSelectableFlags_DontClosePopups))
				{
					selected = i;
				}
				if (SDXL_ImGui_IsItemHovered() && SDXL_ImGui_IsMouseDoubleClicked(SDXL_ImGuiMouseButton_Left))
				{

					OnItemDoubleClicked(selected);
				}
			}
			
			if(selected !=-1)
				OnItemSelected(selected);
			
			SDXL_ImGui_Separator();
		}
		void BtnDeleteContent(int& selected)
		{
			SDXL_ImGui_Text("            "); SDXL_ImGui_SameLine();
			if (SDXL_ImGui_Button(m_DeleteLabel.c_str()))
			{
				if (selected > -1 && m_pContent.size() > 1)
				{
					auto deleteThis = m_pContent[selected];
					m_pContent[selected] = *(m_pContent.end() - 1);

					//When deleting the last one, -- selected to avoid "array out of range"
					if (m_pContent.size() - 1 == static_cast<UINT>(selected))
						selected--;
					
					m_pContent.pop_back();

					
					OnBtnDeleteClicked(deleteThis);
					CONTENT_MANAGER->Save();
				}
			}
		}
		void BtnMoveContentUp(int& selected)
		{
			SDXL_ImGui_SameLine();
			if (SDXL_ImGui_Button(m_UpLabel.c_str()))
			{
				if (selected > 0)
				{
					auto temp = m_pContent[selected - 1];
					m_pContent[selected - 1] = m_pContent[selected];
					m_pContent[selected] = temp;
					selected--;
					CONTENT_MANAGER->Save();

				}
			}
		}
		void BtnMoveContentDown(int& selected)
		{
			SDXL_ImGui_SameLine();
			if (SDXL_ImGui_Button(m_DownLabel.c_str()))
			{
				if (selected >= 0 && selected < static_cast<int>(m_pContent.size() - 1))
				{
					auto temp = m_pContent[selected + 1];
					m_pContent[selected + 1] = m_pContent[selected];
					m_pContent[selected] = temp;
					selected++;
					CONTENT_MANAGER->Save();

				}
			}
		}
		void BtnAddContent()
		{
			static char newContent[40];


			SDXL_ImGui_Text("      "); SDXL_ImGui_SameLine();
			SDXL_ImGui_SetNextItemWidth(200.f);
			SDXL_ImGui_InputTextWithHint(m_AddLabel.c_str(), m_AddHint.c_str(), newContent, ARRAY_SIZE(newContent));
			SDXL_ImGui_SameLine();
			if (SDXL_ImGui_Button("Add##ContentManager"))
			{
				const auto what = std::string(newContent);
				if (!what.empty())
				{
					OnBtnAddClicked(newContent);
					CONTENT_MANAGER->Save();
				}
					


			}
		}
#endif
	private:
#ifdef EDITOR_MODE
		std::string m_DeleteLabel, m_UpLabel, m_DownLabel, m_AddLabel;
		std::string m_AddHint;
		float m_SelectorItemWidth;
#endif
		
	public:
		CMBase(const CMBase&) = delete;
		CMBase(CMBase&&) = delete;
		CMBase& operator=(const CMBase&) = delete;
		CMBase& operator=(CMBase&&) = delete;
		
	};


}