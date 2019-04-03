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
    , myValueBool( bvalue )
    , myIndex( myGrid.size() )
    , myType( eType::string )
{
    PanelLabel();
    if( myValue.length() )
    {
        myTextbox->move({1+myGrid.propWidth()/2,1, -1+myGrid.propWidth()/2,myGrid.propHeight()-2});
        myTextbox->caption( myValue );
        myTextbox->events().mouse_leave([this]
        {
            if( myTextbox->caption() != myValue )
            {
                myValue = myTextbox->caption();
                myGrid.ChangeFunction()( *this );
            }
        });
        myType = eType::string;
    }
    else
    {
        myCheckbox->move({1+myGrid.propWidth()/2,1, 48,myGrid.propHeight()-2});
        myCheckbox->check( myValueBool );
        myCheckbox->events().click([this]
        {
            myValueBool = myCheckbox->checked();
            myGrid.ChangeFunction()( *this );
        });
        myType = eType::check;
    }
    nana::drawing dw{*myPanel};
    dw.draw([this](paint::graphics& graph)
    {
        int w = -1 + myGrid.propWidth()/2;
        graph.line( {0,21}, {w,21},colors::black);
        graph.line( {w,0}, {w,21},colors::black);
    });
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
    myPanel->move({ 0, (h * myIndex), myGrid.propWidth(), h });
    myLabel->move({ 20,1, myGrid.propWidth()/2,h-2 });
    myLabel->caption( myName );
    myCheckbox->move( {1,1,14,myGrid.propHeight()-2} );
    myCheckbox->check( true );
    myCheckbox->events().click([this]
    {
        myGrid.Expand( myName, myCheckbox->checked() );
    });
    nana::drawing dw{*myPanel};
    dw.draw([this](paint::graphics& graph)
    {
        int w = -1 + myGrid.propWidth();
        graph.line( {0,21}, {w,21},colors::black);
        graph.line( {w,0}, {w,21},colors::black);
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
        myGrid.ChangeFunction()( *this );
    });
}

void cProp::PanelLabel()
{
    myPanel->move({ 0, (myGrid.propHeight() * myIndex),    myGrid.propWidth(), myGrid.propHeight() });
    myLabel->move({ 1,1,                                   myGrid.propWidth()/2, myGrid.propHeight()-2 });
    myLabel->caption( myName );
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
            case eType::string: myTextbox->show(); break;
            case eType::check:  myCheckbox->show(); break;
            case eType::choice: myCombox->show(); break;
        }
    }
    else
    {
        myLabel->hide();
        myTextbox->hide();
        myCheckbox->hide();
    }
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
            p->move( { 0, (22 * index),
                       propWidth(), 22
                     });
        }
        else
        {
            p->Show( fexp );
            if( fexp )
            {
                index++;
                p->move( { 0, (22 * index), propWidth(), 22 });
            }
            else
            {
                p->move( { 0,0,0,0 });
            }
        }
    }
}

}
