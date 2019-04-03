#include <string>
#include <iostream>
#include <nana/gui.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include <nana/gui/widgets/combox.hpp>

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
    ~cProp()
    {
        delete myTextbox;
        delete myLabel;
        delete myPanel;
    }

    void move( const nana::rectangle& r )
    {
        myPanel->move( r );
    }

    bool operator==(const cProp& other) const
    {
        return myName == other.myName;
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
    */
    void Show( bool f );

    /** Add helpful description that pops up when mouse hovers over property name
        \param[in] msg to pop up
    */
    void tooltip( const std::string& msg );

private:
    cPropGrid& myGrid;
    nana::panel<true> * myPanel;
    nana::label * myLabel;
    nana::textbox * myTextbox;
    nana::checkbox * myCheckbox;
    nana::combox * myCombox;
    std::string myCatName;
    std::string myName;
    std::string myValue;
    bool myValueBool;
    int myIndex;
    enum class eType
    {
        none,
        string,
        check,
        category,
        choice,
    } myType;

    void PanelLabel();
};
class cPropGrid
{
    friend cProp;

public:
    cPropGrid( nana::form& fm )
        : myfm( fm )
    {
        myfm.move({100,100,propWidth(),220});
    }
    ~cPropGrid()
    {
        for( auto p : myProp )
            delete p;
    }
    /** Add string property
        @param[in] name
        @param[in] value
    */
    cProp* string( const std::string& name,
              const std::string& value )
    {
        myProp.emplace_back( new cProp(
                                 *this,
                                 name,
                                 value,
                                 false ) );
        return myProp.back();
    }
    /** Add checkbox property
        @param[in] name
        @param[in] value
    */
    void check( const std::string& name,
                   bool value )
    {
        myProp.emplace_back( new cProp(
                                 *this,
                                 name,
                                 "",
                                 value ) );
    }
    /** Add category
        @param[in] name
    */
    void category( const std::string& name )
    {
        myProp.emplace_back( new cProp(
                                 *this,
                                 name ) );
        myCurCatName = name;
    }
    /** Add choice property
        \param[in] name
        \param[in] vChoice vector of possible choices
    */
    void choice(
        const std::string name,
        const std::vector< std::string >& vChoice )
    {
        myProp.emplace_back( new cProp(
                                 *this,
                                 name,
                                 vChoice ));
    }

    /** Find property by category and name
        \param[in] category name of category
        \param[in] name name of property
        \return pointer to property if found, otherwise nullptr
    */
    cProp* find( const std::string& category, const std::string& name );

    void Expand( const std::string& name, bool f = true );
    void Collapse( const std::string& name );
    nana::form& Form()
    {
        return myfm;
    }
    int propWidth()
    {
        return 300;
    }
    int propHeight()
    {
        return 22;
    }
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
                << " value is " << prop.Value()
                << "\n";
            });
    </pre>

    */
    void change_event( std::function<void( cProp& prop )> f)
    {
        myChangeEventFunction = f;
    }

    /// Number of properties in grid of all types including categories
    int size()
    {
        return (int) myProp.size();
    }

private:

    nana::form& myfm;
    std::vector< cProp* > myProp;
    std::function<void( cProp& prop )> myChangeEventFunction;
    std::string myCurCatName;

    /// Move visible properties to required screen locations when categories expand or collapse
    void CollapseAll();

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
};

}
