#pragma once
#include <vector>
#include <map>

namespace tyr
{
	class EAnimator;
	class Animation;
	class Rect;
	class Connection;
	class Animator final
	{
	public:
		~Animator();

	
		void Update(float elapsed);

		void SetFloat(const std::string& variable, float value);
		void SetBool(const std::string& variable, bool value);

		void Initialize();
		
		bool IsAtEnd() const;
		const Rect& GetCurrentAnimation() const;

		static Animator* Create(const std::string& path);

		const std::string& GetName() const noexcept { return m_Name; }

		Animator* CreateCopy();
		
#ifdef EDITOR_MODE
		void Save();
		static Animator* GenerateNew(const std::string& name);
#endif
	private:
		friend bool operator==(Animator* lhs, const std::string& rhs); //rhs == dataPath that gets hashed on creation
		friend EAnimator;
		Animator();
		void SetAnimation(AnimationID id);
		AnimationID GetAnimationID(Animation* pAnimation) const;
		
		std::string m_Name;
		std::vector<Connection*> m_pConnections;
			
		Animation* m_pCurrent; //weak pointer
		std::map<AnimationID, Animation*> m_pAnimations;

		void ReloadAnimations();

	public:
		Animator(const Animator&) = delete;
		Animator(Animator&&) = delete;
		Animator& operator=(const Animator&) = delete;
		Animator& operator=(Animator&&) = delete;
	};


}
#pragma once
