#include <string>
#include <iostream>
#include <nana/gui.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/widgets/button.hpp>

#include <nana/gui.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/scroll.hpp>

namespace nana
{

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
        panel<false>::move( scrolled_panel );

        // place scrolling window and scrolls in position on parent
        myVisibleAndScrolls.move( visible_panel );

        // Set scroll ammounts to the full extent of the scrolled area
        scroll_amount();

        // register scroll event handlers
        myScrollHoriz.events().value_changed([&]
        {
            panel<false>::move(
                -myScrollHoriz.value(),
                -myScrollVert.value() );
        });
        myScrollVert.events().value_changed([&]
        {
            panel<false>::move(
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
    /// Change size of scrolled panel
    void size( const nana::size& s )
    {
        panel<false>::size( s );
        scroll_amount();
    }
    void move( const rectangle& r)
    {
        throw std::runtime_error("move disabled for scrolled_panel, use size instead");
    }
private:
    nana::panel<false> myVisibleAndScrolls;   /// Window showing scrolling window and scrolls
    nana::panel<false> myVisible;           ///< Part of scrolled panel that is visible
    nana::scroll< false > myScrollHoriz;
    nana::scroll< true  > myScrollVert;
    nana::place myPlace;

    /// Adjust scrolling amounts
    void scroll_amount()
    {
        int x = panel<false>::size().width  - myVisible.size().width;
        if( x < 0 )
            x = 0;
        int y = panel<false>::size().height - myVisible.size().height;
        if( y < 0 )
            y = 0;
        myScrollHoriz.amount( x );
        myScrollVert.amount( y );
    }
};
}




namespace nana
{

class cPropGrid;

/** \brief Display and edit an individual name-value property

There are no public constructors.  Application code should not attempt to construct a property.
Rather, use the methods of cPropGrid to add new properties: cPropGrid::string(), cPropGrid::category, etc.
*/
class cProp
{
public:

    ~cProp()
    {
        delete myTextbox;
        delete myLabel;
        delete myPanel;
    }
    /// \brief Get Name of property
    std::string name()
    {
        return myName;
    }
    /// \brief Get value of string property
    std::string value()
    {
        return myValue;
    }
    bool valueBool()
    {
        return myValueBool;
    }

    /** \brief Set value of string property
        \param[in] v value to set
    */
    void value( const std::string& v );

    /// \brief get name of category that contains property.
    std::string CatName()
    {
        return myCatName;
    }

    /** \brief Add helpful description that pops up when mouse hovers over property name
        \param[in] msg to pop up
    */
    void tooltip( const std::string& msg );

    /** \brief Get menu which pops-up when user presses mouse right button on property label

    Use this to add items to the menu

    <pre>
        pg.find( "category_name", "property_name" )->menu().append("Test",[](menu::item_proxy& ip)
        {
            msgbox m("popup menu extra");
            m.show();
        });
    </pre>
    */
    menu& menu()
    {
        return myMenu;
    }

private:

    friend cPropGrid;

    cPropGrid& myGrid;
    nana::panel<true> * myPanel;
    nana::label * myLabel;
    nana::textbox * myTextbox;
    nana::checkbox * myCheckbox;
    nana::combox * myCombox;
    nana::button * myButton;
    nana::menu myMenu;
    std::string myCatName;
    std::string myName;
    std::string myValue;
    std::string myDefaultValue;
    bool myValueBool;
    int myIndex;
    enum class eType
    {
        none,
        string,
        check,
        category,
        choice,
        button,
    } myType;

    cProp( cPropGrid& grid,
           const std::string& name,
           const std::string& value,
           bool bvalue );
    cProp( cPropGrid& grid,
           const std::string& name );
    cProp( cPropGrid& grid,
           const std::string& name,
           const std::vector< std::string >& vChoice );
    cProp( cPropGrid& grid,
           const std::string& name,
           std::function<void()> f);

    /// Common constructor for all properties except category typw
    void PanelLabel();

    /** \brief Make properties in category visible or not
    \param[in] f true id properties are to be visible
    */
    void Expand( bool f )
    {
        if( myType != eType::category )
            return;
        myCheckbox->check( f );
    }

    bool IsExpanded()
    {
        return myCheckbox->checked();
    }
    /** Show property when category is expanded or collapsed
        \param[in] f true to show, false to hide
        \param[out] f true if expanded category
        \param[in] index of preceding visible properties
        \param[out] index incremented index
    */
    void visible( bool& f, int& index );

    bool IsCategory()
    {
        return myType == eType::category;
    }
};

/// A class to hold properties and display them for editing in a grid

class cPropGrid
{
    friend cProp;

public:
    /** \brief CTOR
        \param[in] parent panel where grid will be seen

        The parent panel may be smaller than the property grid with all categories expanded
        Scroll bars allow user to move through entire visible grid using parent panel to view segments of the grid
    */
    cPropGrid( panel<true>& parent );

    ~cPropGrid()
    {
        for( auto p : myProp )
            delete p;
    }
    /** \brief Add string property.
        \param[in] name
        \param[in] value
        \return pointer to property
    */
    cProp* string(
        const std::string& name,
        const std::string& value );

    /** \brief Add checkbox property
        \param[in] name
        \param[in] value
        \return pointer to property
    */
    cProp* check(
        const std::string& name,
        bool value );

    /** \brief Add category
        \param[in] name
    */
    void category(
        const std::string& name );

    /** \brief Add choice property
        \param[in] name
        \param[in] vChoice vector of possible choices
        \return pointer to property
    */
    cProp* choice(
        const std::string name,
        const std::vector< std::string >& vChoice );

    /** \brief Add button property
            \param[in] name
            \param[in] f function to call when button clicked
            \return pointer to property
    */
    cProp* button(
        const std::string name,
        std::function<void()> f );

    /** \brief Find property by category and name
        \param[in] category name of category
        \param[in] name name of property
        \return pointer to property if found, otherwise nullptr
    */
    cProp* find(
        const std::string& category,
        const std::string& name );

    /** \brief Set visibility of properties in a category
        \param[in] name of category
        \param[in] f true if properties should be visible, default true
    */
    void Expand(
        const std::string& name, bool f = true );

    /** \brief Register function to be called wnen property changes value
        \param[in] f function to be called

    Function signature: void f( cProp* prop )

    Usage:

    <pre>
        cPropGrid pg( fm );
        pg.change_event([]( cProp& prop )
        {
        std::cout
            << "Property "  << prop.Name()
            << " in "       << prop.CatName()
            << " value is " << prop.value()
            << "\n";
        });
    </pre>

    */
    void change_event(
        std::function<void( cProp& prop )> f)
    {
        myChangeEventFunction = f;
    }

private:

    panel<true>& myParent;
    panel_scrolled myGridPanel;
    std::vector< cProp* > myProp;
    std::function<void( cProp& prop )> myChangeEventFunction;
    std::string myCurCatName;
    int myVisibleHeight;

    /// Move visible properties to required screen locations when categories expand or collapse
    void visible();

    /** get change event function

        \return function to be called when property value changes

    A property will call this when its values is changed by the user
    in order to invoke the stored change event function
    */
    std::function<void( cProp& prop )> ChangeEventFunction()
    {
        return myChangeEventFunction;
    }

    /// Last category name. ( Properties added will be included in this category )
    std::string LastCatName() const
    {
        return myCurCatName;
    }

    window parent()
    {
        return myGridPanel;
    }
    int propWidth()
    {
        return 300;
    }
    int propHeight()
    {
        return 22;
    }
    int margin()
    {
        return 3;
    }

    /// Number of properties in grid of all types including categories
    int size()
    {
        return (int) myProp.size();
    }
};

}
