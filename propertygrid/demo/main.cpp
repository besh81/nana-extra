
#include <nana/gui.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/textbox.hpp>

#include "propertygrid.h"
#include "pgitems.h"




int main()
{
	nana::form fm(0, nana::size{ 600, 500 });
	nana::place pl{ fm };
	pl.div("margin=5 gap=5 field");

	nana::propertygrid propgrid{ fm };
	pl["field"] << propgrid;

	nana::textbox txt{ fm };
	pl["field"] << txt;
	txt.editable(false);


	// append category
	auto cat = propgrid.append("Standard properties");
	// append string property
	cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string("string", "ABC")));
	// append int property
	cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string_int("int", "-100")));
	// append unsigned int property
	cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string_uint("unsigned int", "100")));
	// append string with button property
	cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string_button("string + button", "Ciao")));
	// append choice property
	auto ip = cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_choice("choice")));
	auto pgchoiche = static_cast<nana::pg_choice*>(ip._m_pgitem());
	pgchoiche->push_back("One");
	pgchoiche->push_back("Two");
	pgchoiche->push_back("3");
	// append check property
	cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_check("check", true)));
	// append color property
	cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_color("color", "255,0,0")));
	// append color+inherited property
	ip = cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_color("color + inherited", "128,128,128", true)));
	ip.value("0,255,0");

	// connect the events
	propgrid.events().property_changed([&txt](const nana::arg_propertygrid& arg)
	{
		auto ip = arg.item;
		txt.append("[cat: " + std::to_string(ip.pos().cat) + ", pos: " + std::to_string(ip.pos().item) + "] ", true);
		txt.append("   label: " + ip.label() + "\n", true);
	});


	pl.collocate();
	fm.show();
	nana::exec();
}
