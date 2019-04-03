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
    , myPanel( new nana::panel<true>( myGrid.Form() ) )
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
        myTextbox->move({1+myGrid.propWidth()/2,1, -3+myGrid.propWidth()/2,myGrid.propHeight()-2});
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
    , myPanel( new nana::panel<true>(  myGrid.Form() ) )
    , myLabel( new nana::label( *myPanel ))
    , myCheckbox( new nana::checkbox( *myPanel ))
    , myName( name )
    , myIndex( myGrid.size() )
    , myType( eType::category )
{
    int h = myGrid.propHeight();
    myPanel->move({ 0, (h * myIndex), myGrid.propWidth(), 2 * h });
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
    , myPanel( new nana::panel<true>(  myGrid.Form() ) )
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

void cProp::PanelLabel()
{
    myPanel->move({ 0, (myGrid.propHeight() * myIndex),    myGrid.propWidth(), myGrid.propHeight() });
    myLabel->move({ 1,1,                                   myGrid.propWidth()/2, myGrid.propHeight()-2 });
    myLabel->caption( myName );

    // dcontsruct pop-up menu with restore default item
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
            myMenu.popup_await(myGrid.Form(), arg.pos.x, arg.pos.y);
    });

    // draw box around property
    nana::drawing dw{*myPanel};
    dw.draw([this](paint::graphics& graph)
    {
        int h = myGrid.propHeight()-1;
        int w = -1 + myGrid.propWidth();
        graph.line( {0,h}, {w,h},colors::black);
        graph.line( {w-1,0}, {w-1,h},colors::black);
        graph.line( {w,0}, {w,h},colors::black);
    });
}

void cProp::Show( bool f )
{
    if( IsCategory() )
        return;
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

void cPropGrid::Expand( const std::string& name, bool f )
{
    for( auto p : myProp )
        if( p->Name() == name )
        {
            if( ! p->IsCategory() )
                return;
            p->Expand( f );
        }
    CollapseAll();
}

void cPropGrid::Collapse( const std::string& name )
{
    for( auto p : myProp )
        if( p->Name() == name )
        {
            if( ! p->IsCategory() )
                return;
            p->Expand( false );
        }
    CollapseAll();
}

void cPropGrid::CollapseAll()
{

    bool fexp = true;
    int index = -1;
    for( auto p : myProp )
    {
        if( p->IsCategory() )
        {
            p->Show( true );
            fexp = p->IsExpanded();
            index++;
            p->move( { 0, propHeight() * index,
                       propWidth(), 2*propHeight()
                     });
            index++;
        }
        else
        {
            p->Show( fexp );
            if( fexp )
            {
                index++;
                p->move( { 0, (propHeight() * index),
                           propWidth(), propHeight()
                         });
            }
            else
            {
                p->move( { 0,0,0,0 });
            }
        }
    }
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
