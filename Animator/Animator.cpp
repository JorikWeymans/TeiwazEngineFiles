#include "tyrpch.h"
#include "Animator.h"
#include <algorithm>
#include "Animation.h"
#include "ContentManager.h"

#include "BinaryWriter.h"
#include "BinaryReader.h"
#include "TyrException.h"
#include "Connection.h"
#define ANIMATOR_SUFFIX ".tyrAnimator"


//TODO: Should all the connections be checked every frame? Right now a connection gets checked when it gets set and it seem to work fine
tyr::Animator::Animator()
	: m_Name("")
	, m_pCurrent(nullptr)
{	
}

tyr::Animator::~Animator()
{
	std::for_each(m_pConnections.begin(), m_pConnections.end(), [](auto& a) {delete a; a = nullptr; });
	m_pConnections.clear();
	std::for_each(m_pAnimations.begin(), m_pAnimations.end(), [](auto pA)
		{
			SAFE_DELETE(pA.second);
		});
	m_pAnimations.clear();
}

void tyr::Animator::SetAnimation(AnimationID id)
{
	m_pCurrent = m_pAnimations[id];
}

AnimationID tyr::Animator::GetAnimationID(Animation* pAnimation) const
{
	auto found = std::find_if(m_pAnimations.begin(), m_pAnimations.end(), [pAnimation](auto pA) { return pA.second == pAnimation; });

	if(found != m_pAnimations.end())
		return found->first;

	return 0;
}

void tyr::Animator::ReloadAnimations()
{
	std::for_each(m_pAnimations.begin(), m_pAnimations.end(), [](auto pA)
		{
			SAFE_DELETE(pA.second);
		});

	m_pAnimations.clear();

	for (auto pcon : m_pConnections)
	{
		if (m_pAnimations[pcon->lhs] == nullptr)
			m_pAnimations[pcon->lhs] = CONTENT_MANAGER->GetAnimationCopy(pcon->lhs);

		if (m_pAnimations[pcon->rhs] == nullptr)
			m_pAnimations[pcon->rhs] = CONTENT_MANAGER->GetAnimationCopy(pcon->rhs);
		
	}
}

void tyr::Animator::Update(float elapsed)
{
	m_pCurrent->Update(elapsed);
}

void tyr::Animator::SetFloat(const std::string& variable, float value)
{
	for(auto pcon : m_pConnections)
	{
		if (pcon->pVariable->GetName() != variable) continue;
		if (pcon->pVariable->GetType() != VariableType::Float) continue;
		if (pcon->lhs != GetAnimationID(m_pCurrent)) continue;


		if(pcon->pVariable->DoEquation(value))
			SetAnimation(pcon->rhs);
		
	}
}

void tyr::Animator::SetBool(const std::string& variable, bool value)
{
	for (auto pcon : m_pConnections)
	{
		if (pcon->pVariable->GetName() != variable) continue;
		if (pcon->pVariable->GetType() != VariableType::Bool) continue;
		if (pcon->lhs != GetAnimationID(m_pCurrent)) continue;


		if (pcon->pVariable->DoEquation(value))
			SetAnimation(pcon->rhs);


	}
	
}

void tyr::Animator::Initialize()
{
	//m_pCurrent = CONTENT_MANAGER->GetAnimation(m_pConnections[0]->lhs);

	ReloadAnimations();
	SetAnimation(m_pConnections[0]->lhs);

	m_pCurrent->Reset();

}

bool tyr::Animator::IsAtEnd() const
{
	return m_pCurrent->IsAtEnd();
}

const tyr::Rect& tyr::Animator::GetCurrentAnimation() const
{
	return m_pCurrent->GetCurrentAnimation();
}

tyr::Animator* tyr::Animator::Create(const std::string& path)
{
	BinaryReader reader(path + ANIMATOR_SUFFIX);

	const ULONG64 header = reader.Read<ULONG64>();
	if (header != 0x6efd095e)
		THROW_ERROR(L"This is not an animator");

	Animator* pTheAnimator = new Animator();
	pTheAnimator->m_Name = reader.ReadString();
	pTheAnimator->m_pCurrent = CONTENT_MANAGER->GetAnimation(reader.Read<AnimationID>());
	const UINT connectionSize = reader.Read<UINT>();
	pTheAnimator->m_pConnections.resize(connectionSize);

	for(UINT i{0}; i < connectionSize; ++i)
	{
		pTheAnimator->m_pConnections[i] = Connection::Create(reader);
	}



	
	return pTheAnimator;
}
tyr::Animator* tyr::Animator::CreateCopy()
{
	auto pReturn = new Animator();
	pReturn->m_Name = this->m_Name;

	const int size = static_cast<int>(this->m_pConnections.size());
	pReturn->m_pConnections.resize(size);

	for(int i{0}; i < size; i++)
	{
		pReturn->m_pConnections[i] = new Connection(this->m_pConnections[i]);
	}
	
	return pReturn;
}




#ifdef EDITOR_MODE

void tyr::Animator::Save()
{
	std::stringstream ss;
	ss << CONTENT_MANAGER->GetAbsoluteAnimatorFolder();
	ss << m_Name;
	ss << ANIMATOR_SUFFIX;


	//Binary structure
	// Long double -> Header (JorikWeymansTyrAnimator hashed via Adler32 to this value)
	// std::string -> m_Name
	// AnimationID -> m_pCurrent(pointer to AID)
	// UINT -> ConnectionSize
	// for each connection
	//   AnimationID  -> lhs
	//   AnimationID  -> rhs
	//   std::string  -> var::m_Name
	//   VariableType -> var::m_Type
	//   Equation     -> var::m_Equation
	//   bool/float   -> var::b/fComparatorValue (depended on what type it is.
	
	BinaryWriter writer(ss.str());

	if(writer.IsOpen())
	{
		const ULONG64 header = 0x6efd095e;
		writer.Write(header);
		writer.Write(m_Name);
		writer.Write(CONTENT_MANAGER->GetAnimationID(m_pCurrent));
		writer.Write(static_cast<UINT>(m_pConnections.size()));
		std::for_each(m_pConnections.begin(), m_pConnections.end(), [&writer](Connection* pC) { pC->Save(writer); });

		SDXL_ImGui_ConsoleLog("Animator is saved");
	}
	else
		SDXL_ImGui_ConsoleLogWarning("Could not save the Animator");
}


tyr::Animator* tyr::Animator::GenerateNew(const std::string& name)
{
	Animator* pReturnAnimator = new Animator();
	pReturnAnimator->m_Name = name;
	pReturnAnimator->m_pConnections.emplace_back(new Connection(0, 0, new AnimatorVariable("default", false, Equation::Equal)));
	pReturnAnimator->Save();
	
	return pReturnAnimator;
}
#endif

bool tyr::operator==(Animator* lhs, const std::string& rhs)
{
	if (!lhs) return false;

	return lhs->m_Name == rhs;
}
