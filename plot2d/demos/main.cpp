#include <iostream>
#include <nana/gui.hpp>
#include "plot2d.h"

int main()
{

    using namespace nana;

    try
    {
        form fm;

        // construct plot to be drawn on form
        plot::plot thePlot( fm );
        thePlot.Grid( true );

        // construct plot traces
        plot::trace& t1 = thePlot.AddStaticTrace();
        plot::trace& t2 = thePlot.AddStaticTrace();

        // provide some data for first trace
        std::vector< double > d1 { 10, 15, 20, 25, 30, 25, 20, 15, 10 };
        t1.set( d1 );

        // plot in blue
        t1.color( colors::blue );

        // provide data for second trace
        std::vector< double > d2 { 20, 30, 40, 50, 60, 50, 40, 30, 20 };
        t2.set( d2 );

        // plot in red
        t2.color( colors::red );

        // show and run
        fm.show();
        exec();
    }
    catch( std::runtime_error& e )
    {
        msgbox mb( e.what() );
        mb();
    }
}

