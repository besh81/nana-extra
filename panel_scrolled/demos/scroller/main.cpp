#include <iostream>

#include "panel_scrolled.h"


int main()
{
    // construct application form
    nana::form fm( nana::rectangle( 100,100, 300, 300 ));

    // construct large panel with scrolling window
    const int width_scrolled_panel   = 600;
    const int height_scrolled_panel  = 600;
    const int width_scrolling_window = 100;
    const int height_scrolling_window = 100;
    nana::panel_scrolled S (
        fm,
    { 50, 50, width_scrolling_window, height_scrolling_window },        // size and location of scrolling window on form
    { 0,   0, width_scrolled_panel,   height_scrolled_panel });       // size of base panel over which scrolling window moves


    // add a big label to be scrolled over
    nana::label lbl( S, nana::rectangle(
                         0,0,
                         width_scrolled_panel, height_scrolled_panel));
    lbl.caption(
        "This is a very very very very very wide label\n"
        " This is a very very very very very wide label with 2nd line....................................................................................\n"
        " This is a very very very very very wide label and 3rd line\n"
        " This is a very very very very very wide label and 4th line\n"
        " This is a very very very very very wide label and 5th line\n"
        " This is a very very very very very wide label and 6th line\n"
        " This is a very very very very very wide label and 7th line\n"
        " This is a very very very very very wide label and 8th line\n"
        " This is a very very very very very wide label and 9th line");


    // show & run
    fm.show();
    nana::exec();

}
