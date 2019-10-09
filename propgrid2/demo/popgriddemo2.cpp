#include <string>
#include <iostream>
#include <sstream>
#include <nana/gui.hpp>

#include "propgrid.h"

int main()
{
    using namespace nana;

    form fm( 0, nana::size{ 400, 300 } );
    //form fm( 0, nana::size{ 400, 600 } );
    fm.caption("PropGrid2 demo 2");

    //  Construct property grid
    panel<true> pnl( fm, { 20, 20, 300, 600 });
    cPropGrid pg( pnl );
    pg.category( "Strings" );

    // Add 20 properties
    for( int k = 0; k < 20; k++ )
    {
        // add property ( label and textbox )
        pg.string(
            "test" + std::to_string(k),   // label caption
            std::to_string(k) );         // textbox initial value
    }

    // Add label to form, outside the initial form size
    nana::label lbtest( fm );
    lbtest.move( {10, 650, 60,30});
    lbtest.caption("test");

    //Expose the form.
    fm.show();
    //Pass the control of the application to Nana's event service.
    exec();
}

