#include <string>
#include <iostream>
#include <nana/gui.hpp>

#include "propgrid.h"

int main()
{
    using namespace nana;

    form fm( 0, nana::size{ 600, 500 } );

    cPropGrid pg( fm );
    pg.Add("Strings");
    pg.Add( "test1", "1" );
    pg.Add( "test2", "2" );
    pg.Add( "test3", "3" );
    pg.Add("Bools");
    pg.AddCheck( "test4", false );
    pg.AddCheck( "test5", true );
    pg.Add("cat");
    pg.Add( "test6", "false" );
    pg.AddCheck( "test7", true );

    pg.Collapse("Bools");

    pg.SetChangeFunction([]( cProp& prop )
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

