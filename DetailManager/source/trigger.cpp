#include "trigger.h"

#include "ats_define.hpp"
#include "debug.h"

namespace config
{
	/*
	namespace trigger
	{
		const double distance = 2100; // [m]
		const double distance2 = 210000; // [m]
		const int wait_time = 30 * 1000; // [ms]
	}
	
	namespace panel
	{
		namespace index
		{
			const int output_sotetsu = 249;
			const int output_jr = 250;
			const int output_company = 251;
		};
	};

	namespace sound
	{
		namespace index
		{
			const int announce_here = 21;
			const int announce_here2 = 22;
		};
	};
	*/
	namespace details
	{
		namespace index
		{
			const int sotetsu[] = { 5, 9 };
			const int jr[] = { 7, 8 };
		};
	};

	namespace company
	{
		const int sotetsu = 1;
		const int jr = 2;
	}
}

trigger::trigger() :
	company_(config::company::sotetsu),
	distance_(0),
	door_opened_(false),
	countdown_(0),
	last_time_(0),
	announce_here_(false),
	announce_here2_(false),
	announce_reset_(false),
	changedis_(200000),
	rinkai_(false)
{}

void trigger::reset()
{
	this->company_ = config::company::sotetsu;
	this->distance_ = 0;
	this->door_opened_ = false;
	this->countdown_ = 0;
	this->last_time_ = 0;
	this->announce_here_ = false;
	this->announce_here2_ = false;
	this->announce_reset_ = true;
	this->changedis_ = 200000;
	this->rinkai_ = false;
}
//�ǉ���
//�ύX��������ݒ�
void trigger::change_distance(const double changedis)
{
	changedis_ = changedis;
}
//�؊�������ݒ�
void trigger::sotetsu_rinkai(const bool rinkai)
{
	rinkai_ = rinkai;
	if (rinkai == true) {
		this->company_ = config::company::jr;
	}
}

//����
void trigger::set_time(const int ms)//���t���[�����s
{
	const auto dt = ms - this->last_time_;
	this->last_time_ = ms;

	if (this->distance_ < this->changedis_)
	{
		return;
	}

	if (!this->door_opened_)
	{
		return;
	}
	if (dt < 0)
	{
		// not expected
		return;
	}

	this->countdown_ -= dt;//dt��countdown�萔���z������؊�
	if (this->countdown_ < 0 && this->company_ != config::company::jr && this->rinkai_ == false)
	{
		// switching
		this->company_ = config::company::jr;
		this->door_opened_ = false;//�����I��
		this->announce_here_ = true;
	}
	if (this->countdown_ < 0 && this->company_ != config::company::sotetsu && this->rinkai_ == true)
	{
		// switching
		this->company_ = config::company::sotetsu;
		this->door_opened_ = false;//�����I��
		this->announce_here2_ = true;
	}
}

void trigger::set_distance(const double distance)//���t���[�����s
{
	if (this->distance_ == 0)//distance=0�Ȃ�i�܂菉��A�w�W�����v���j
	{
		// just after reset
		if (this->changedis_ < distance && this->rinkai_ == false)
		{
			this->company_ = config::company::jr;//�؊����������Ă����JR�ɂ��Ă��܂�
		}
		if (this->changedis_ < distance && this->rinkai_ == true)
		{
			this->company_ = config::company::sotetsu;//�؊����������Ă���Α��S�ɂ��Ă��܂�
		}
	}
	this->distance_ = distance;
}

void trigger::set_door_open()//�h�A�J�����Ɏ��s
{
	this->door_opened_ = true;
	this->countdown_ = ini_wait;
}

void trigger::set_door_close()//�h�A�����Ɏ��s
{
	this->door_opened_ = false;
}

bool trigger::is_enable(const int detail_index) const
{
	if (this->company_ != config::company::sotetsu)
	{
		/*
		for (const auto sotetsu_index : config::details::index::sotetsu)
		{
			if (sotetsu_index == detail_index)
			{
				return false;
			}
		}
		*/
		if (detail_index == ini_so1 || detail_index == ini_so2 || detail_index == ini_so3 || detail_index == ini_so4 || detail_index == ini_so5)
			return false;
	}

	if (this->company_ != config::company::jr)
	{
		/*
		for (const auto jr_index : config::details::index::jr)
		{
			if (jr_index == detail_index)
			{
				return false;
			}
		}
		*/
		if (detail_index == ini_jr1 || detail_index == ini_jr2 || detail_index == ini_jr3 || detail_index == ini_jr4 || detail_index == ini_jr5)
			return false;
	}
	/*
	if (this->company_ == config::company::rinkai)
	{
		for (const auto rinkai_index : config::details::index::rinkai)
		{
			if (rinkai_index == detail_index)
			{
				return false;
			}
		}
	}
	*/
	return true;
}

void trigger::output_company(int* p_panel) const//���t���[�����s
{
	// none
	p_panel[ini_com1] = 0;
	p_panel[ini_com2] = 0;
	//p_panel[config::panel::index::output_rinkai] = 0;
	int result = 0;
	if (this->is_enable(config::details::index::sotetsu[0]))
	//if (this->company_ == config::company::sotetsu)
	{
		// sotetsu
		p_panel[ini_com1] = 1;
		result += 1;
	}
	if (this->is_enable(config::details::index::jr[0]))
	//if (this->company_ == config::company::jr)
	{
		// jr
		p_panel[ini_com2] = 1;
		result += 2;
	}
	/*
	//if (this->is_enable(config::details::index::rinkai[0]))
	if (this->company_ == config::company::rinkai)
	{
		// rinkai
		p_panel[config::panel::index::output_rinkai] = 1;
		result += 3;
	}
	*/
	p_panel[ini_com] = result;
}

void trigger::announce_here(int* p_sound)//���t���[�����s
{
	if (this->announce_reset_)
	{
		p_sound[ini_ann1] = ATS_SOUND_STOP;
		p_sound[ini_ann2] = ATS_SOUND_STOP;
		this->announce_reset_ = false;
		return;
	}
	if (this->announce_here_)
	{
		p_sound[ini_ann1] = ATS_SOUND_PLAY;
		this->announce_here_ = false;
		return;
	}
	if (this->announce_here2_)
	{
		p_sound[ini_ann2] = ATS_SOUND_PLAY;
		this->announce_here2_ = false;
		return;
	}
	p_sound[ini_ann1] = ATS_SOUND_CONTINUE;
	p_sound[ini_ann2] = ATS_SOUND_CONTINUE;
}
