#pragma once

class trigger
{
public:
	trigger();
	void reset();

	// if this...
	void set_time(int ms);
	void set_distance(double distance);
	void set_door_open();
	void set_door_close();
	void change_distance(double changedis);
	void sotetsu_rinkai(bool rinkai);

	// then that...
	bool is_enable(int detail_index) const;
	void output_company(int* p_panel) const;
	void announce_here(int* p_sound);

	//ini
	int ini_wait;
	int ini_com1;
	int ini_com2;
	int ini_com;
	int ini_ann1;
	int ini_ann2;
	int ini_so1;
	int ini_so2;
	int ini_so3;
	int ini_so4;
	int ini_so5;
	int ini_jr1;
	int ini_jr2;
	int ini_jr3;
	int ini_jr4;
	int ini_jr5;
	int ini_reset;

private:
	int company_;
	double distance_; // [m]
	bool door_opened_;
	int countdown_; // [ms]
	int last_time_; // [ms]
	bool announce_here_;
	bool announce_here2_;
	bool announce_reset_;
	double changedis_;
	bool rinkai_;
};
