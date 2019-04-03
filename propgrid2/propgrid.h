#include <string>
#include <iostream>
#include <nana/gui.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/checkbox.hpp>

namespace nana
{

class cPropGrid;

class cProp
{
public:
    cProp( cPropGrid& grid,
           const std::string& catName,
           const std::string& name,
           const std::string& value,
           bool bvalue,
           int index  );
    cProp( cPropGrid& grid,
           const std::string& name,
           int index  );
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
    std::string Value()
    {
        return myValue;
    }
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
    /** Show property
        @param[in] f true to show, false to hide
    */
    void Show( bool f );

private:
    cPropGrid& myGrid;
    nana::panel<true> * myPanel;
    nana::label * myLabel;
    nana::textbox * myTextbox;
    nana::checkbox * myCheckbox;
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
    } myType;
};
class cPropGrid
{
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
    void Add( const std::string& name,
              const std::string& value )
    {
        myProp.emplace_back( new cProp(
                                 *this,
                                 myCurCatName,
                                 name,
                                 value,
                                 false,
                                 (int)myProp.size() ) );
    }
    /** Add checkbox property
        @param[in] name
        @param[in] value
    */
    void AddCheck( const std::string& name,
                   bool value )
    {
        myProp.emplace_back( new cProp(
                                 *this,
                                 myCurCatName,
                                 name,
                                 "",
                                 value,
                                 (int)myProp.size() ) );
    }
    /** Add category
        @param[in] name
    */
    void Add( const std::string& name )
    {
        myProp.emplace_back( new cProp(
                                 *this,
                                 name,
                                 (int)myProp.size() ) );
        myCurCatName = name;
    }
    std::vector< cProp* >::iterator Find( const std::string name );
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
    /** Function called wnen changed property loses focus
        @pram[in] f function to be called
    */
    void SetChangeFunction( std::function<void( cProp& prop )> f)
    {
        myf = f;
    }
    std::function<void( cProp& prop )> ChangeFunction()
    {
        return myf;
    }

private:
    nana::form& myfm;
    std::vector< cProp* > myProp;
    std::function<void( cProp& prop )> myf;
    std::string myCurCatName;

    void CollapseAll();
};

}
