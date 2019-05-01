#include <string>
#include <iostream>
#include <nana/gui.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/checkbox.hpp>

#include "propgrid.h"


namespace nana
{

cProp::cProp( cPropGrid& grid,
              const std::string& name,
              const std::string& value,
              bool bvalue  )
    : myGrid( grid )
    , myPanel( new nana::panel<true>( myGrid.parent() ) )
    , myLabel( new nana::label( *myPanel ))
    , myTextbox( new nana::textbox( *myPanel ))
    , myCheckbox( new nana::checkbox( *myPanel ))
    , myCatName( myGrid.LastCatName() )
    , myName( name )
    , myValue( value )
    , myDefaultValue( value )
    , myValueBool( bvalue )
    , myIndex( myGrid.size() )
{
    PanelLabel();
    if( myValue.length() )
    {
        myType = eType::string;
        myTextbox->move({1+myGrid.propWidth()/2,
                         1,
                         -2*myGrid.margin()+myGrid.propWidth()/2,
                         myGrid.propHeight()-2
                        });
        myTextbox->caption( myValue );
        myTextbox->events().mouse_leave([this]
        {
            if( myTextbox->caption() != myValue )
            {
                myValue = myTextbox->caption();
                myGrid.ChangeEventFunction()( *this );
            }
        });
    }
    else
    {
        myCheckbox->move({1+myGrid.propWidth()/2,1, 48,myGrid.propHeight()-2});
        myCheckbox->check( myValueBool );
        myCheckbox->events().click([this]
        {
            myValueBool = myCheckbox->checked();
            myGrid.ChangeEventFunction()( *this );
        });
        myType = eType::check;
    }
}

cProp::cProp( cPropGrid& grid,
              const std::string& name )
    : myGrid( grid )
    , myPanel( new nana::panel<true>(  myGrid.parent() ) )
    , myLabel( new nana::label( *myPanel ))
    , myCheckbox( new nana::checkbox( *myPanel ))
    , myName( name )
    , myIndex( myGrid.size() )
    , myType( eType::category )
{
    int h = myGrid.propHeight();
    myPanel->move({ 3, (h * myIndex), myGrid.propWidth(), 2 * h });
    myLabel->move({ 20, 3, myGrid.propWidth()/2,2*h-4 });
    myLabel->caption( myName );
    myLabel->fgcolor( colors::blue );
    myLabel->typeface( paint::font( "Segoe UI", 14 ) );
    myCheckbox->move( {1,h/2, 14,myGrid.propHeight()-2} );
    myCheckbox->check( true );
    myCheckbox->events().click([this]
    {
        myGrid.Expand( myName, myCheckbox->checked() );
    });
    nana::drawing dw{*myPanel};
    dw.draw([this](paint::graphics& graph)
    {
        int h = 2 * myGrid.propHeight() - 1;
        int w = -1 + myGrid.propWidth();
        graph.line( {0,h}, {w,h},colors::black);
        graph.line( {w-1,0}, {w-1,h},colors::black);
        graph.line( {w,0}, {w,h},colors::black);
    });
}

cProp::cProp(
    cPropGrid& grid,
    const std::string& name,
    const std::vector< std::string >& vChoice
)
    : myGrid( grid )
    , myPanel( new nana::panel<true>(  myGrid.parent() ) )
    , myLabel( new nana::label( *myPanel ))
    , myCombox( new nana::combox( * myPanel ))
    , myName( name )
    , myIndex( myGrid.size() )
    , myType( eType::choice )
    , myCatName( myGrid.LastCatName() )
{
    PanelLabel();
    myCombox->move({1+myGrid.propWidth()/2,1, -1+myGrid.propWidth()/2,myGrid.propHeight()-2});
    for( const auto& s : vChoice )
        myCombox->push_back( s );
    myCombox->events().text_changed([this]
    {
        myValue = myCombox->caption();
        myGrid.ChangeEventFunction()( *this );
    });
}

cProp::cProp(
    cPropGrid& grid,
    const std::string& name,
    std::function<void()> f
)
    : myGrid( grid )
    , myPanel( new nana::panel<true>(  myGrid.parent() ) )
    , myLabel( new nana::label( *myPanel ))
    , myButton( new nana::button( *myPanel ))
    , myName( name )
    , myIndex( myGrid.size() )
    , myType( eType::button )
    , myCatName( myGrid.LastCatName() )
{
    PanelLabel();
    myButton->move({1+myGrid.propWidth()/2,1,
                    myGrid.propHeight()-2,myGrid.propHeight()-2
                   });
    myButton->caption(" ... ");
    myButton->events().click([f]
    {
        f();
    });
}


void cProp::PanelLabel()
{
    myPanel->move({ myGrid.margin(),
                    (myGrid.propHeight() * myIndex),
                    myGrid.propWidth() - 2 * myGrid.margin(),
                    myGrid.propHeight()
                  });
    myLabel->move({ 1,1,
                    myGrid.propWidth()/2,
                    myGrid.propHeight()-2
                  });
    myLabel->caption( myName );

    // contsruct pop-up menu with restore default item
    // application code can add items to this menu
    myMenu.append("Restore",[this](menu::item_proxy& ip)
    {
        switch( myType )
        {
        case eType::string:
            myValue = myDefaultValue;
            myTextbox->caption( myDefaultValue );
            myGrid.ChangeEventFunction()( *this );
            break;
        }
    });

    // pop-up menu when user presses right mouse button on the label
    myLabel->events().mouse_down([this](const arg_mouse& arg)
    {
        if ( arg.right_button )
            myMenu.popup_await(myGrid.parent(), arg.pos.x, arg.pos.y);
    });

    // draw box around property
    nana::drawing dw{*myPanel};
    dw.draw([this](paint::graphics& graph)
    {
        int h = myGrid.propHeight()-1;
        int w = -1 + myGrid.propWidth();
        graph.line( {0,h}, {w,h},colors::black);
    });
}
void cProp::PanelMove( int row )
{
    int rowheight = 1;
    if( myType == eType::category )
        rowheight = 2;
    myPanel->move( { myGrid.margin(),
                     myGrid.margin() + myGrid.propHeight() * row,
                     myGrid.propWidth() - 2 * myGrid.margin(),
                     rowheight * myGrid.propHeight()
                   });
}

void cProp::visible( bool& f, int& index )
{
    if( IsCategory() )
    {
        PanelMove( index );     // category always visible
        index += 2;             // display takes two rows
        f = IsExpanded();       // control visibility of contained properties
    }
    else if( f )
    {
        PanelMove( index );     // property is visible
        index++;                // displays in one row
    }
    else
    {
        myPanel->move( { 0,0,0,0 });    // invisible property
    }
}

void cProp::tooltip( const std::string& msg )
{
    myLabel->tooltip( msg );
}


void cProp::value( const std::string& v )
{
    if( myType != eType::string )
        return;
    myValue = v;
    myTextbox->caption( v );
}

void cProp::value( bool f )
{
    if( myType != eType::check )
        return;
    myValueBool = f;
    myCheckbox->check( f );
}

std::string cProp::value()
{
    if( myType == eType::check )
    {
        if( myValueBool)
            return "true";
        else
            return "false";
    }
    return myValue;
}

cPropGrid::cPropGrid( panel<true>& parent )
    : myGridPanel( parent,
{
    0,0,parent.size().width,parent.size().height
}, {0,0,300,500} )
, myParent( parent )
{

    drawing dw{ myParent };
    dw.draw([this](paint::graphics& graph)
    {
        graph.round_rectangle( { 0,0, myParent.size().width-1,myParent.size().height-1},
                               3,3, colors::black, false, colors::black );
        graph.round_rectangle( { 1,1, myParent.size().width-2,myParent.size().height-2},
                               3,3, colors::black, false, colors::black );
    });
}

void cPropGrid::Expand( const std::string& name, bool f )
{
    for( auto p : myProp )
        if( p->name() == name )
        {
            if( ! p->IsCategory() )
                return;
            p->Expand( f );
        }
    visible();
}

void cPropGrid::visible()
{
    bool fexp = true;
    int VisibleRows = 0;
    for( auto p : myProp )
    {
        p->visible( fexp, VisibleRows );
    }

    // resize grid panel to exactly hold the visible properties
    myGridPanel.size( { propWidth(),  VisibleRows * propHeight() + 2 * margin() - 1 });

    // force grid margin redraw
    nana::API::refresh_window_tree( myParent );
}
cProp* cPropGrid::string( const std::string& name,
                          const std::string& value )
{
    myProp.emplace_back( new cProp(
                             *this,
                             name,
                             value,
                             false ) );
    visible();
    return myProp.back();
}

cProp* cPropGrid::check( const std::string& name,
                         bool value )
{
    myProp.emplace_back( new cProp(
                             *this,
                             name,
                             "",
                             value ) );
    visible();
    return myProp.back();
}

void cPropGrid::category( const std::string& name )
{
    myProp.emplace_back( new cProp(
                             *this,
                             name ) );
    myCurCatName = name;
    visible();
}

cProp* cPropGrid::choice(
    const std::string name,
    const std::vector< std::string >& vChoice )
{
    myProp.emplace_back( new cProp(
                             *this,
                             name,
                             vChoice ));
    visible();
    return myProp.back();
}

cProp* cPropGrid::button(
    std::string name,
    std::function<void()> f )
{
    myProp.emplace_back( new cProp(
                             *this,
                             name,
                             f ));
    visible();
    return myProp.back();
}

cProp * cPropGrid::find(
    const std::string& category,
    const std::string& name )
{
    for( auto p : myProp )
    {
        if( p->catName() == category && p->name() == name )
            return p;
    }
    return nullptr;
}

}
