#include <nana/gui.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/scroll.hpp>

namespace nana {

/** Panel specialization to display small visible scrollable part of the panel

    The scrolled panel is large and mostly hidden.  The widgets are placed on this panel.

    The visible panel is small and transparent, allows user to see a small part of the scrolled panel.

    The visible panel is surrounded by scroll bars which control what part of scrolled panel is seen.

    The visible panel and scroll bars are constructed automatically, to the dimensions specifed in the scrolled panel constructor

 */

class panel_scrolled : public nana::panel<false>
{
public:

    /** CTOR

        @param[in] parent window
        @param[in] visible_panel size of visible window, including scroll bars
        @param[in] scrolled_panel size of area over which window moves

    The window parent/child arrangement:

    <pre>

    parent
        <----- myPanelAndScrolls
                          <----------Visible
                                    <----------------BASE ( scrolled panel )
                          <----------Vertical Scrol
                          <--------- Horizontal Scroll
    </pre>

    */
    panel_scrolled(
        const nana::window& parent,
        const nana::rectangle& visible_panel,
        const nana::rectangle& scrolled_panel )
        : myVisibleAndScrolls( parent, true ),
          myVisible( myVisibleAndScrolls, true ),
          myScrollHoriz( myVisibleAndScrolls, true ),
          myScrollVert( myVisibleAndScrolls, true ),
          myPlace( myVisibleAndScrolls )
    {
        // create base panel over which scrollable window moves
        create( myVisible, true );
        move( scrolled_panel );

        // place scrolling window and scrolls in position on parent
        myVisibleAndScrolls.move( visible_panel );

        // Set scroll ammounts to the full extent of the scrolled area
        myScrollHoriz.amount( scrolled_panel.width  - visible_panel.width );
        myScrollVert.amount(  scrolled_panel.height - visible_panel.height );

        // register scroll event handlers
        myScrollHoriz.events().value_changed([&]
        {
            move(
                -myScrollHoriz.value(),
                -myScrollVert.value() );
        });
        myScrollVert.events().value_changed([&]
        {
            move(
                -myScrollHoriz.value(),
                -myScrollVert.value() );
        });

        // arrange visible panel with scrolls at right and bottom
        myPlace.div("<vert <<panel><scroll_vert weight=16>> <scroll_horiz weight=16>>>");
        myPlace["panel"] << myVisible;
        myPlace["scroll_vert"] << myScrollVert;
        myPlace["scroll_horiz"] << myScrollHoriz;
        myPlace.collocate();

    }
private:
    nana::panel<false> myVisibleAndScrolls;   /// Window showing scrolling window and scrolls
    nana::panel<false> myVisible;           ///< Part of scrolled panel that is visible
    nana::scroll< false > myScrollHoriz;
    nana::scroll< true  > myScrollVert;
    nana::place myPlace;
};
}


