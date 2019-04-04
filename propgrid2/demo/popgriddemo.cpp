#include <string>
#include <iostream>
#include <sstream>
#include <nana/gui.hpp>

#include "propgrid.h"

int main()
{
    using namespace nana;

    form fm( 0, nana::size{ 400, 500 } );
    panel<true> pnl( fm, { 20, 20, 300, 400 });
    nana::label status( fm, nana::rectangle{ 20, 450, 200, 25 }, true );
    cPropGrid pg( pnl );
    pg.category( "Strings" );
    pg.string( "test1", "1" )->tooltip( "helpful description" );
    pg.string( "test2", "2" );
    pg.string( "test3", "3" );

    pg.category( "Bools" );
    pg.check( "test4", false );
    pg.check( "test5", true );

    pg.category("Cat");
    pg.string( "test6", "false" );
    pg.check( "test7", true );

    pg.category( "choices" );
    pg.choice( "Select", {"A","B","C"});

    pg.category( "Dialogs" );
    pg.button( "dialog", []
              {
                    msgbox m("Button Dialog");
                    m.show();
              });

    pg.Expand("Bools",false);

    pg.change_event([&status]( cProp& prop )
    {
        std::stringstream ss;
        ss        << "Property "  << prop.name()
                << " in "       << prop.catName()
                << " value is " << prop.value()
                << "\n";
        status.caption( ss.str() );
    });

    pg.find( "Strings", "test1")->value( "42" );

    pg.find( "Strings", "test2")->menu().append("Test",[](menu::item_proxy& ip)
    {
        msgbox m("test1 popup menu extra");
        m.show();
    });


    //Expose the form.
    fm.show();
    //Pass the control of the application to Nana's event
    //service. It blocks the execution for dispatching user
    //input until the form is closed.
    exec();
}

