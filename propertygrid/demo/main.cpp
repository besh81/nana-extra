
#include <nana/gui.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>


// propertygrid.h : contains the propertygrid object
#include "propertygrid.h"

// pgitems.h : contains few common properties and can be used as example to derive your own custom property
#include "pgitems.h"



int main()
{
	// create form and layout
	nana::form fm(0, nana::size{ 600, 500 });
	fm.caption("propertygrid : DEMO");

	nana::place pl{ fm };
	pl.div("vert <weight=45 margin=5 title><<margin=5 main><vert margin=5 gap=2 arrange=[30,20,variable] side>>");

	nana::label title{ fm };
	pl["title"] << title;
	title.typeface(nana::paint::font("", 15, { 400, false, false, false }));
	title.caption("propertygrid : DEMO");
	title.text_align(static_cast<nana::align>(1), static_cast<nana::align_v>(0));

	nana::propertygrid propgrid{ fm };
	pl["main"] << propgrid;

	nana::checkbox chk{ fm };
	pl["side"] << chk;
	chk.caption("Show/Hide IBOX");
	chk.check(true);

	nana::label lbl{ fm };
	pl["side"] << lbl;
	lbl.caption("on property_changed :");

	nana::textbox txt{ fm };
	pl["side"] << txt;
	txt.editable(false);


	// HERE starts the propertygrid configuration ...

	// append EMPTY category
	// EMPTY category (if present) is always shown as first category, CANNOT be expanded or collapsed
	auto cat0 = propgrid.append("");

	// append string property
	cat0.append(nana::propertygrid::pgitem_ptr(new nana::pg_string("string", "ABC")));

	// append int property
	cat0.append(nana::propertygrid::pgitem_ptr(new nana::pg_string_int("int", "-100")));

	// append unsigned int property
	cat0.append(nana::propertygrid::pgitem_ptr(new nana::pg_string_uint("unsigned int", "100")));

	// append "First" category
	auto cat1 = propgrid.append("First");

	// append string with button property
	cat1.append(nana::propertygrid::pgitem_ptr(new nana::pg_string_button("string + button", "Ciao")));

	// append choice property
	auto ip = cat1.append(nana::propertygrid::pgitem_ptr(new nana::pg_choice("choice")));
	auto pgchoiche = static_cast<nana::pg_choice*>(ip._m_pgitem());
	pgchoiche->set({ "One", "Two", "3" });
	pgchoiche->option(0);

	// append check property
	cat1.append(nana::propertygrid::pgitem_ptr(new nana::pg_check("check", true)));

	// append "Second" category
	auto cat2 = propgrid.append("Second");

	// append color property
	cat2.append(nana::propertygrid::pgitem_ptr(new nana::pg_color("color", "255,0,0")));

	// connect the property_change event
	propgrid.events().property_changed([&txt](const nana::arg_propertygrid& arg) {
		auto ip = arg.item;
		txt.append("[cat: " + std::to_string(ip.pos().cat) + ", pos: " + std::to_string(ip.pos().item) + "] ", true);
		txt.append("   label: " + ip.label()  + ", value: " + ip.value() + "\n", true);
	});

	// connect show/hide propertygrid ibox event
	chk.events().checked([&propgrid](const nana::arg_checkbox& arg) {
		propgrid.ibox_show(arg.widget->checked());
	});



	pl.collocate();
	fm.show();
	nana::exec();
}
