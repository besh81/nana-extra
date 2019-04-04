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

namespace nana
{

class cPropGrid;

class cProp
{
public:
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
    ~cProp()
    {
        delete myTextbox;
        delete myLabel;
        delete myPanel;
    }
    bool IsCategory()
    {
        return myType == eType::category;
    }
    std::string Name()
    {
        return myName;
    }
    /** Get value of string property
        \return value
    */
    std::string value()
    {
        return myValue;
    }
    /** Set value of string property
        \param[in] v value to set
    */
    void value( const std::string& v );

    std::string CatName()
    {
        return myCatName;
    }
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

    /** Add helpful description that pops up when mouse hovers over property name
        \param[in] msg to pop up
    */
    void tooltip( const std::string& msg );

    /** Get menu which pops-up when user presses mouse right button on property label

    Use this to add items to the menu

    <pre>
        // find property and add item to the pop-up menu
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

    /// Common constructor for all properties except category typw
    void PanelLabel();
};

/// A class to hold properties and display them for editing in a grid

class cPropGrid
{
    friend cProp;

public:
    cPropGrid( panel<true>& parent );

    ~cPropGrid()
    {
        for( auto p : myProp )
            delete p;
    }
    /** Add string property
        \param[in] name
        \param[in] value
        \return pointer to property if found, otherwise nullptr
    */
    cProp* string(
        const std::string& name,
        const std::string& value );

    /** Add checkbox property
        \param[in] name
        \param[in] value
        \return pointer to property if found, otherwise nullptr
    */
    cProp* check(
        const std::string& name,
        bool value );

    /** Add category
        \param[in] name
    */
    void category(
        const std::string& name );

    /** Add choice property
        \param[in] name
        \param[in] vChoice vector of possible choices
        \return pointer to property if found, otherwise nullptr
    */
    cProp* choice(
        const std::string name,
        const std::vector< std::string >& vChoice );

    /** Add button property
            \param[in] name
            \param[in] f function to call when button clicked
            \return pointer to property if found, otherwise nullptr
    */
    cProp* button(
        const std::string name,
        std::function<void()> f );

    /** Find property by category and name
        \param[in] category name of category
        \param[in] name name of property
        \return pointer to property if found, otherwise nullptr
    */
    cProp* find(
        const std::string& category,
        const std::string& name );

    /** Set visibility of properties in a category
        \param[in] name of category
        \param[in] f true if properties should be visible, default true
    */
    void Expand(
        const std::string& name, bool f = true );

    /** Register function to be called wnen property changes value
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
        return myParent;
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
