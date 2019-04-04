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

void cProp::visible( bool& f, int& index )
{
    if( IsCategory() )
    {
        myPanel->move( { myGrid.margin(),
                         myGrid.margin() + myGrid.propHeight() * index,
                         myGrid.propWidth() - 2 * myGrid.margin(),
                         2 * myGrid.propHeight()
                       });
        index += 2;
        f = IsExpanded();
        return;
    }
    if( f )
    {
        myLabel->show();
        switch( myType )
        {
        case eType::string:
            myTextbox->show();
            break;
        case eType::check:
            myCheckbox->show();
            break;
        case eType::choice:
            myCombox->show();
            break;
        }
        myPanel->move( { myGrid.margin(),
                         myGrid.margin() + myGrid.propHeight() * index,
                         myGrid.propWidth() - 2 * myGrid.margin(),
                         myGrid.propHeight()
                       });
        index++;
    }
    else
    {
        myLabel->hide();
        switch( myType )
        {
        case eType::string:
            myTextbox->hide();
            break;
        case eType::check:
            myCheckbox->hide();
            break;
        case eType::choice:
            myCombox->hide();
            break;
        }
        myPanel->move( { 0,0,0,0 });
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

cPropGrid::cPropGrid( panel<true>& parent )
    : myParent( parent, {0,0,parent.size().width,parent.size().height}, {0,0,300,500} )
{

    drawing dw{myParent};
    dw.draw([this](paint::graphics& graph)
    {
        graph.round_rectangle( { 0,1, propWidth()-1,myVisibleHeight-1},
                               3,3, colors::black, false, colors::black );
        graph.round_rectangle( { 1,1, propWidth()-2,myVisibleHeight-2},
                               3,3, colors::black, false, colors::black );
    });
}

void cPropGrid::Expand( const std::string& name, bool f )
{
    for( auto p : myProp )
        if( p->Name() == name )
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
    myVisibleHeight = 0;
    for( auto p : myProp )
    {
        p->visible( fexp, myVisibleHeight );
    }
    // convert from rows to pixels
    myVisibleHeight *= propHeight();
    myVisibleHeight += 2 * margin() - 1;

    myParent.size( {propWidth(),myVisibleHeight});

    // force grid margin redraw
    nana::API::refresh_window ( myParent );
}
    cProp* cPropGrid::string( const std::string& name,
                   const std::string& value )
    {
        myProp.emplace_back( new cProp(
                                 *this,
                                 name,
                                 value,
                                 false ) );
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
    return myProp.back();
}

void cPropGrid::category( const std::string& name )
{
    myProp.emplace_back( new cProp(
                             *this,
                             name ) );
    myCurCatName = name;
}

cProp* cPropGrid::choice(
    const std::string name,
    const std::vector< std::string >& vChoice )
{
    myProp.emplace_back( new cProp(
                             *this,
                             name,
                             vChoice ));
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
    return myProp.back();
}

cProp * cPropGrid::find(
    const std::string& category,
    const std::string& name )
{
    std::string cat;
    for( auto p : myProp )
    {
        if( p->IsCategory() )
        {
            cat = p->Name();
            continue;
        }
        if( cat != category )
            continue;
        if( p->Name() == name )
            return p;
    }
    return nullptr;
}

}
