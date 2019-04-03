#include <string>
#include <iostream>
#include <nana/gui.hpp>

#include "propgrid.h"

int main()
{
    using namespace nana;

    form fm( 0, nana::size{ 600, 500 } );

    cPropGrid pg( fm );
    pg.category( "Strings" );
    pg.string( "test1", "1" )->tooltip( "helpful description" );
    pg.string( "test2", "2" );
    pg.string( "test3", "3" );

    pg.category( "Bools" );
    pg.check( "test4", false );
    pg.check( "test5", true );

    pg.category("cat");
    pg.string( "test6", "false" );
    pg.check( "test7", true );

    pg.category( "choices" );
    pg.choice( "Select ", {"A","B","C"});

    pg.Collapse("Bools");

    pg.change_event([]( cProp& prop )
    {
        std::cout
            << "Property "  << prop.Name()
            << " in "       << prop.CatName()
            << " value is " << prop.Value()
            << "\n";
    });


    //Expose the form.
    fm.show();
    //Pass the control of the application to Nana's event
    //service. It blocks the execution for dispatching user
    //input until the form is closed.
    exec();
}

