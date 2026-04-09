#pragma once

#ifdef RESCUEDLL_EXPORTS
#define RESCUEDLL_API __declspec(dllexport)
#else
#define RESCUEDLL_API __declspec(dllimport)
#endif

#include <random>
#include <type_traits>

constexpr float scr_width{ 800.0f };
constexpr float scr_height{ 650.0f };

constexpr float sky{ 50.0f };
constexpr float ground{ 550.0f };

float nature_speed{ 1.0f };

constexpr int ERR_PTR{ 3001 };
constexpr int ERR_INDEX{ 3002 };
constexpr int ERR_PARAM{ 3003 };
constexpr int ERR_UNK{ 3004 };

enum class dirs {
	left = 0, right = 1, up = 2, down = 3, up_left = 4, up_right = 5,
	down_left = 6, down_right = 7, stop = 8
};
enum class assets { field = 0, ground = 1, civilian = 2, supply = 3 };
enum class meteors { big = 0, mid = 1 };
enum class bonus { points = 0, repair = 1, armor = 2, gun = 3 };

enum class todo { move = 0, shoot = 1, patrol = 2 };

struct RESCUEDLL_API FPOINT
{
	float x{ 0 };
	float y{ 0 };
};
struct RESCUEDLL_API FRECT
{
	float left{ 0 };
	float up{ 0 };
	float right{ 0 };
	float down{ 0 };
};

struct RESCUEDLL_API EXPLOSION
{
	float x{ 0 };
	float y{ 0 };

	int max_frames = 23;
	int frame_delay = 3;
	int max_frame_delay = 3;
};

namespace dll
{
	class RESCUEDLL_API EXCEPTION
	{
	private:
		int err_code = 0;
		
	public:
		EXCEPTION(int what_happened);

		const wchar_t* get_error();
	};

	template<typename T>class BAG
	{
	private:
		size_t m_size{ 0 };
		T* m_ptr{ nullptr };
		size_t m_pos{ 0 };

	public:
		BAG() :m_size{ 1 }, m_ptr{ reinterpret_cast<T*>(calloc(1, sizeof(T))) } {};
		BAG(size_t lenght) :m_size{ lenght }, m_ptr{ reinterpret_cast<T*>(calloc(lenght, sizeof(T))) } {};
		BAG(BAG& other)
		{
			if (!other.m_ptr)throw EXCEPTION(ERR_PTR);
			else
			{
				m_size = other.m_size;
				m_ptr = reinterpret_cast<T*>(calloc(m_size, sizeof(T)));

				if (!m_ptr)throw EXCEPTION(ERR_PTR);
				else if (other.m_pos > 0)
				{
					for (size_t i = 0; i < other.m_pos; ++i)
					{
						m_ptr[i] = other.m_ptr[i];
						++m_pos;
					}
				}
			}
		}
		BAG(BAG&& other)
		{
			if (!other.m_ptr)throw EXCEPTION(ERR_PTR);
			else
			{
				m_ptr = other.m_ptr;
				m_size = other.m_size;
				m_pos = other.m_pos;

				other.m_ptr = nullptr;
			}
		}

		~BAG()
		{
			free(m_ptr);
		}

		bool empty() const
		{
			return(m_pos == 0);
		}
		size_t size() const
		{
			return m_pos;
		}
		size_t capacity() const
		{
			return m_size;
		}

		void clear()
		{
			free(m_ptr);
			m_size = 1;
			m_pos = 0;
			m_ptr = reinterpret_cast<T*>(calloc(m_size, sizeof(T)));
		}

		BAG& operator=(BAG& other)
		{
			if (!other.m_ptr)throw EXCEPTION(ERR_PTR);
			else if (m_ptr != other.m_ptr)
			{
				free(m_ptr);

				m_size = other.m_size;
				m_pos = other.m_pos;

				m_ptr = reinterpret_cast<T*>(calloc(m_size, sizeof(T)));

				if (other.m_pos > 0)
					for (size_t i = 0; i < other.m_pos; ++i)m_ptr[i] = other.m_ptr[i];
			}

			return *this;
		}
		BAG& operator=(BAG&& other)
		{
			if (!other.m_ptr)throw EXCEPTION(ERR_PTR);
			else
			{
				free(m_ptr);

				m_size = other.m_size;
				m_pos = other.m_pos;
				m_ptr = other.m_ptr;

				other.m_ptr = nullptr;
			}

			return *this;
		}

		T& operator[](size_t index)
		{
			if (index < 0 || index >= m_pos)throw EXCEPTION(ERR_INDEX);
			if (!m_ptr)throw EXCEPTION(ERR_PTR);

			return m_ptr[index];
		}

		void push_back(T element)
		{
			if (!m_ptr)throw EXCEPTION(ERR_PTR);
			else
			{
				if (m_pos + 1 <= m_size)
				{
					m_ptr[m_pos] = element;
					++m_pos;
				}
				else
				{
					++m_size;
					m_ptr = reinterpret_cast<T*>(realloc(m_ptr, m_size * sizeof(T)));
					if(!m_ptr)throw EXCEPTION(ERR_PTR);
					else
					{
						m_ptr[m_pos] = element;
						++m_pos;
					}
				}
			}
		}
		void push_back(T* element)
		{
			if (!m_ptr)throw EXCEPTION(ERR_PTR);
			else
			{
				if (m_pos + 1 <= m_size)
				{
					m_ptr[m_pos] = *element;
					++m_pos;
				}
				else
				{
					++m_size;
					m_ptr = reinterpret_cast<T*>(realloc(m_ptr, m_size * sizeof(T)));
					if (!m_ptr)throw EXCEPTION(ERR_PTR);
					else
					{
						m_ptr[m_pos] = *element;
						++m_pos;
					}
				}
			}
		}

		void push_front(T element)
		{
			if (!m_ptr)throw EXCEPTION(ERR_PTR);
			else
			{
				if (m_pos + 1 <= m_size)
				{
					for (size_t count = m_pos; count > 0; --count)m_ptr[count] = m_ptr[count - 1];
					*m_ptr = element;
					++m_pos;
				}
				else
				{
					++m_size;

					m_ptr = reinterpret_cast<T*>(realloc(m_ptr, m_size * sizeof(T)));
					if (!m_ptr)throw EXCEPTION(ERR_PTR);
					else
					{
						for (size_t count = m_pos; count > 0; --count)m_ptr[count] = m_ptr[count - 1];
						*m_ptr = element;
						++m_pos;
					}
				}
			}
		}
		void push_front(T* element)
		{
			if (!m_ptr)throw EXCEPTION(ERR_PTR);
			else
			{
				if (m_pos + 1 <= m_size)
				{
					for (size_t count = m_pos; count > 0; --count)m_ptr[count] = m_ptr[count - 1];
					*m_ptr = *element;
					++m_pos;
				}
				else
				{
					++m_size;

					m_ptr = reinterpret_cast<T*>(realloc(m_ptr, m_size * sizeof(T)));
					if (!m_ptr)throw EXCEPTION(ERR_PTR);
					else
					{
						for (size_t count = m_pos; count > 0; --count)m_ptr[count] = m_ptr[count - 1];
						*m_ptr = *element;
						++m_pos;
					}
				}
			}
		}

		void erase(size_t index)
		{
			if (index < 0 || index >= m_pos)throw EXCEPTION(ERR_INDEX);
			if (!m_ptr)throw EXCEPTION(ERR_PTR);
			else
			{
				if (m_pos == 0)clear();
				else
				{
					for (size_t count = index; count < m_pos - 1; ++count)m_pos[count] = m_pos[count + 1];
					--m_pos;
				}
			}
		}

		void insert(size_t index, T element)
		{
			if (index < 0 || index > m_pos)throw EXCEPTION(ERR_INDEX);
			if (!m_ptr)throw EXCEPTION(ERR_PTR);

			if (m_pos + 1 <= m_size)
			{
				for (size_t count = m_pos; count > index; --count)m_ptr[count] = m_ptr[count - 1];
				m_ptr[index] = element;
				++m_pos;
			}
			else
			{
				++m_size;
				m_ptr = realloc(m_ptr, m_size * sizeof(T));
				if (!m_ptr)throw EXCEPTION(ERR_PTR);
				else
				{
					for (size_t count = m_pos; count > index; --count)m_ptr[count] = m_ptr[count - 1];
					m_ptr[index] = element;
					++m_pos;
				}
			}
		}
	};

	template<typename T> concept PRIMES = std::is_same<T, char>::value ||
		std::is_same<T, wchar_t>::value || std::is_same<T, int>::value || std::is_same<T, float>::value
		|| std::is_same<T, double>::value || std::is_same<T, long>::value || std::is_same<T, short>::value;

	////////////////////////////////////////////////////
	
	template<PRIMES T>bool Sort(BAG<T>Bag, bool ascending)
	{
		if (Bag.empty())return false;
		
		bool ok = false;

		while (!ok)
		{
			ok = true;

			for (size_t count = 0; count < Bag.size() - 1; ++count)
			{
				if (Bag[count] > Bag[count + 1])
				{
					T temp = Bag[count];
					Bag[count] = Bag[count + 1];
					Bag[count + 1] = temp;
					ok = false;
				}
			}
		}

		return true;
	}

	class RESCUEDLL_API RANDIT
	{
	private:
		std::mt19937* twister{ nullptr };

	public:
		RANDIT();
		~RANDIT();

		int operator()(int min, int max);
		float operator()(float min, float max);
	};

	class RESCUEDLL_API PROTON
	{
	protected:
		float _width{ 0 };
		float _height{ 0 };

		float move_sx{ 0 };
		float move_ex{ 0 };
		float move_sy{ 0 };
		float move_ey{ 0 };

		float slope{ 0 };
		float intercept{ 0 };

		bool ver_dir = false;
		bool hor_dir = false;

	public:
		FPOINT start{};
		FPOINT end{};
		FPOINT center{};

		float x_rad{ 0 };
		float y_rad{ 0 };

		dirs dir = dirs::stop;

		PROTON();
		PROTON(float _first_x, float _first_y);
		PROTON(float _first_x, float _first_y, float _first_width, float _first_height);

		virtual ~PROTON() {};

		void set_edges();
		void new_dims(float _new_width, float _new_height);

		void new_width(float _new_width);
		void new_height(float _new_height);

		void set_path(float _target_x, float _target_y);

		float get_target_x()const;
		float get_target_y()const;
		float get_width()const;
		float get_height()const;
	};

	class RESCUEDLL_API FIELDS :public PROTON
	{
	private:
		float _speed = nature_speed;

		FIELDS(assets _what, float _sx, float _sy);

	public:
		assets type = assets::field;
		
		static FIELDS* create(assets what, float sx, float sy);

		bool move(dirs dir, float gear);

		void Release();
	};

	class RESCUEDLL_API ASSETS :public PROTON
	{
	private:
		int frame = 0;
		int max_frames = 0;
		int frame_delay = 0;
		int max_frame_delay = 0;

		float _speed = nature_speed;

		RANDIT _Rand{};

		ASSETS(assets _what, float _sx, float _sy);

	public:
		assets type = assets::civilian;

		static ASSETS* create(assets what, float sx, float sy);

		bool move(dirs dir, float gear);

		int get_frame();

		void Release();
	};

	class RESCUEDLL_API SHOTS :public PROTON
	{
	private:
		float _speed = 5.0f;

		SHOTS(float _first_x, float _first_y, float _end_x, float _end_y);

	public:
		int damage = 10;

		bool move(float gear);

		void Release();

		static SHOTS* create(float first_x, float first_y, float end_x, float end_y);
	};

	class RESCUEDLL_API METEORS :public PROTON
	{
	private:
		int max_delay = 4;
		int frame = 0;
		float _speed = 1.0f;

		METEORS(meteors _type, float _first_x, float _first_y, float _end_x, float _end_y);

	public:
		meteors type{ meteors::big };
		
		bool move(float gear);

		void Release();

		static METEORS* create(meteors type, float first_x, float first_y, float end_x, float end_y);
	};

	class RESCUEDLL_API GUN :public PROTON
	{
		int max_delay = 100;
		int delay = 100;

		int frame_delay = 13;
		int frame = 0;

		float _speed{ nature_speed };

		GUN(float _sx, float _sy);

	public:
		int damage = 50;

		bool move(dirs dir, float gear);

		int attack();

		int get_frame();

		void Release();

		static GUN* create(float sx, float sy);
	};

	class RESCUEDLL_API HERO :public PROTON
	{
	private:
		int max_frames = 3;
		int frame_delay = 20;
		int max_frame_delay = 20;
		int frame = 0;

		float _speed = 10.0f;

		HERO(float _sx, float _sy);

	public:
		int damage = 5;
		int armor = 1;
		int lifes = 150;

		void move(float gear);
		
		int get_frame();
		
		void Release();

		static HERO* create(float sx, float sy);
	};

	class RESCUEDLL_API EVIL:public PROTON
	{
	private:
		int max_frames = 3;
		int frame_delay = 22;
		int max_frame_delay = 22;
		int frame = 0;

		float _speed = 0.8f;

		int attack_delay = 80;
		int max_attack_delay = 80;
		
		EVIL(float _sx, float _sy);

	public:
		todo current_action = todo::patrol;

		int lifes = 120;
		int armor = 1;
		int damage = 5;

		float see_range = 300.0f;
		float shoot_range = 150.0f;

		bool move(float ex, float ey, float gear);

		int attack();
		int get_frame();

		void Release();

		static EVIL* create(float sx, float sy);
	};

	struct RESCUEDLL_API BONUS
	{
		float sx{ 0 };
		float sy{ 0 };
		float ex{ 32.0f };
		float ey{ 32.0f };
		
		float opacity{ 1.0f };

		void set_opacity()
		{
			static float duration = 3.1f;
			duration -= 0.1f;
			opacity = duration / 3.0f;
		}

		void set_edges()
		{
			ex = sx + 32.0f;
			ey = sy + 32.0f;
		}
	};

	// FUNCTIONS *****************************************

	RESCUEDLL_API float Distance(FPOINT first, FPOINT second);

	RESCUEDLL_API void Sort(BAG<FPOINT>& bag, FPOINT target);
	
	RESCUEDLL_API int IntroFrame();

	RESCUEDLL_API bool Intersect(FPOINT first_center, FPOINT second_center, float first_xrad, float second_xrad,
		float first_yrad, float second_yrad);

	RESCUEDLL_API bool Intersect(FRECT first, FRECT second);

	RESCUEDLL_API todo AINextMove(EVIL& my_unit, BAG<FPOINT>& civils, BAG<FPOINT>& shots, BAG<FPOINT>& powerups, FPOINT hero);
}