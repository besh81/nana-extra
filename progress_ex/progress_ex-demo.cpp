#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/timer.hpp>
#include "progress_ex.hpp"

int main()
{
	using namespace nana;
	using namespace std;
	form fm{API::make_center(400, 420), appear::decorate<appear::taskbar>()};
	fm.div("vert margin=15"
		"<vert weight=52 <l1> <p1>> <weight=15>"
		"<vert weight=52 <l2> <p2>> <weight=15>"
		"<vert weight=52 <l3> <p3>> <weight=15>"
		"<vert weight=52 <l4> <p4>> <weight=15>"
		"<vert weight=52 <l5> <p5>> <weight=15>"
		"<vert weight=52 <l6> <p6>>"
	);
	fm.bgcolor(color{"#f7f6f5"});
	fm.caption("Extended progress widget - demo");

	vector<pair<bool, unique_ptr<progress_ex>>> progs;

	label l1{fm, "'blue' color preset, 'percentage' text mode:"};
	l1.fgcolor(color{"#555555"});
	l1.text_align(align::center, align_v::center);
	fm["l1"] << l1;

	progs.push_back(make_pair<bool, unique_ptr<progress_ex>>(true, make_unique<progress_ex>(fm)));
	progs.back().second->color_preset(progress_ex::color_presets::blue);
	progs.back().second->text_mode(progress_ex::text_modes::percentage);
	fm["p1"] << *progs.back().second;

	label l2{fm, "'green' color preset, 'value' text mode:"};
	l2.fgcolor(color{"#555555"});
	l2.text_align(align::center, align_v::center);
	fm["l2"] << l2;

	progs.push_back(make_pair<bool, unique_ptr<progress_ex>>(true, make_unique<progress_ex>(fm)));
	progs.back().second->color_preset(progress_ex::color_presets::green);
	progs.back().second->text_mode(progress_ex::text_modes::value);
	fm["p2"] << *progs.back().second;

	label l3{fm, "'violet' color preset:"};
	l3.fgcolor(color{"#555555"});
	l3.text_align(align::center, align_v::center);
	fm["l3"] << l3;

	progs.push_back(make_pair<bool, unique_ptr<progress_ex>>(true, make_unique<progress_ex>(fm)));
	progs.back().second->color_preset(progress_ex::color_presets::violet);
	progs.back().second->caption("'caption' text mode (default)");
	fm["p3"] << *progs.back().second;

	label l4{fm, "'brown' color preset, 'caption' text mode, empty caption (default):"};
	l4.fgcolor(color{"#555555"});
	l4.text_align(align::center, align_v::center);
	fm["l4"] << l4;

	progs.push_back(make_pair<bool, unique_ptr<progress_ex>>(true, make_unique<progress_ex>(fm)));
	progs.back().second->color_preset(progress_ex::color_presets::brown);
	progs.back().second->text_mode(progress_ex::text_modes::caption);
	fm["p4"] << *progs.back().second;

	label l5{fm, "'orange' color preset, 'caption' text mode:"};
	l5.fgcolor(color{"#555555"});
	l5.text_align(align::center, align_v::center);
	fm["l5"] << l5;

	progs.push_back(make_pair<bool, unique_ptr<progress_ex>>(true, make_unique<progress_ex>(fm)));
	progs.back().second->color_preset(progress_ex::color_presets::orange);
	progs.back().second->text_mode(progress_ex::text_modes::caption);
	progs.back().second->text_contrast(false);
	progs.back().second->caption("text contrast off");
	fm["p5"] << *progs.back().second;

	label l6{fm, "no color preset, 'caption' text mode:"};
	l6.fgcolor(color{"#555555"});
	l6.text_align(align::center, align_v::center);
	fm["l6"] << l6;

	progs.push_back(make_pair<bool, unique_ptr<progress_ex>>(true, make_unique<progress_ex>(fm)));
	progs.back().second->text_mode(progress_ex::text_modes::caption);
	progs.back().second->caption("nana colors (default)");
	fm["p6"] << *progs.back().second;

	timer t;
	t.interval(50);
	t.elapse([&]
	{
		for(auto &prog : progs)
		{
			if(rand() % 2) continue;
			if(prog.first)
			{
				if(prog.second->value() < prog.second->amount())
					prog.second->inc();
				else prog.first = false;
			}
			else
			{
				if(prog.second->value() > 0)
					prog.second->value(prog.second->value()-1);
				else prog.first = true;
			}
		}
	});
	t.start();

	fm.collocate();
	fm.show();
	nana::exec();
}