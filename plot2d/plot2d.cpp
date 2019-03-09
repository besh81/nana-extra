#include <iostream>
#include <algorithm>
#include <nana/gui.hpp>
#include "plot2d.h"
namespace nana
{
namespace plot
{

plot::plot( window parent )
    : myParent( parent )
{
    RegisterDrawingFunction();

    myAxis = new axis( this );
}

trace& plot::AddScatterTrace()
{
    trace * t = new trace();
    t->Plot( this );
    t->scatter();
    myTrace.push_back( t );
    return *t;
}

void plot::RegisterDrawingFunction()
{
    /* The drawing object is just defined as a local variable

    When the drawing object gets destroyed, the drawer functions are still alive.
    The drawing object doesn't manage the life-time of drawer functions,
    it is just for installing drawer functions.

    */
    drawing( myParent ).draw([this](paint::graphics& graph)
    {
        // check there are traces that need to be drawn
        if( ! myTrace.size() )
            return;

        // calculate scaling factors
        // so plot will fit
        CalcScale(
            graph.width(),
            graph.height() );

        // draw axis
        myAxis->update( graph );

        // loop over traces
        for( auto t : myTrace )
        {
            // draw a trace
            t->update( graph );
        }
    });
}

void plot::CalcScale( int w, int h )
{
    w *= 0.9;
    h *= 0.95;

    int maxCount = 0;
    myTrace[0]->bounds( myMinY, myMaxY );
    for( auto& t : myTrace )
    {
        if( t->size() > maxCount )
            maxCount = t->size();
        double tmin, tmax;
        t->bounds( tmin, tmax );
        if( tmin < myMinY )
            myMinY = tmin;
        if( tmax > myMaxY )
            myMaxY = tmax;
    }
    if( ! maxCount )
        return;
    myXinc = (float)w / maxCount;
    if( myMaxY == myMinY )
        myScale = 1;
    else
        myScale = 0.9 * h / ( myMaxY - myMinY );
    myXOffset = 0.05 * w;
    myYOffset = h + myScale * myMinY;

    //std::cout << myMinY <<" "<< myMaxY <<" "<< myScale;
}
void trace::set( const std::vector< double >& y )
{
    if( myType != eType::plot )
        throw std::runtime_error("nanaplot error: plot data added to non plot trace");

    myY.clear();
    for( double s : y )
        myY.push_back( s );
    //myY = y;

    std::cout << "plot::trace::set " << myY.size() << "\n";
}
void trace::add( double y )
{
    if( myType != eType::realtime )
        throw std::runtime_error("nanaplot error: realtime data added to non realtime trace");
    myY[ myRealTimeNext++ ] = y;
    if( myRealTimeNext >= (int)myY.size() )
        myRealTimeNext = 0;

    myPlot->update();
}

void trace::add( double x, double y )
{
    if( myType != eType::scatter )
        throw std::runtime_error("nanaplot error: point data added to non scatter type trace");
    myX.push_back( x );
    myY.push_back( y );
}

void trace::bounds( double& tmin, double& tmax )
{
    auto result = std::minmax_element(
                      myY.begin(),
                      myY.end());
    tmin = *result.first;
    tmax = *result.second;
}

void trace::update( paint::graphics& graph )
{
    bool first = true;
    float x    = myPlot->XOffset();
    float xinc = myPlot->xinc();
    double prev;


    switch( myType )
    {
    case eType::plot:

        // loop over data points
        for( auto y : myY )
        {
            // scale
            double ys =  myPlot->Y2Pixel( y );

            if( first )
            {
                first = false;
                prev = ys;
                continue;
            }

            // draw line from previous to this data point
            graph.line(
                point(x, prev),
                point(x+xinc, ys),
                myColor);

            x += xinc;
            prev = ys;

        }
        break;

    case eType::scatter:

        for( int k = 0; k < (int)myX.size(); k++ )
        {
            int x = (int) ( myPlot->XOffset() + xinc * myX[k] );
            graph.rectangle(
                rectangle{ x-5,  myPlot->Y2Pixel( myY[ k ] )-5,
                           10, 10 },
                false,
                myColor );
        }
        break;

    case eType::realtime:

    {

        // loop over data points

        // they are stored in a circular buffer
        // so we have to start with the oldest data point
        int yidx = myRealTimeNext;
        do
        {
            double y = myPlot->Y2Pixel( myY[ yidx ] );

            // the next data point
            // with wrap-around if the end of the vector is reached
            yidx++;
            if( yidx >= (int)myY.size() )
                yidx = 0;

            if( first )
            {
                first = false;
                prev = y;
                continue;
            }
            // draw line from previous to this data point
            graph.line(
                point( x, prev ),
                point( x+xinc, y ),
                myColor);

            x += xinc;
            prev = y;

        }

        // check for end of circular buffer
        // ( most recent point )
        while( yidx != myRealTimeNext );

    }
    break;
    }
}

axis::axis( plot * p )
    : myPlot( p )
{
    myLabelMin = new label( myPlot->parent(),  rectangle{ 10, 10, 50, 15 } );
    myLabelMin->caption("test");
    myLabelMax = new label( myPlot->parent(),  rectangle{ 10, 10, 50, 15 } );
    myLabelMax->caption("test");
}

void axis::update( paint::graphics& graph )
{
    double mn = 10 * ( myPlot->minY() / 10 );
    double mx = 10 * ( myPlot->maxY() / 10 );
    if( mx-mn < 2 )
    {
        mn = myPlot->minY();
        mx = myPlot->maxY();
    }
    int ymn_px = myPlot->Y2Pixel( mn );
    myLabelMin->caption(std::to_string(mn));
    myLabelMin->move( 5,  ymn_px );

    int ymx_px = myPlot->Y2Pixel( mx );
    myLabelMax->caption(std::to_string( mx ));
    myLabelMax->move( 5,   ymx_px - 15 );

    graph.line( point( 2, ymn_px ),
                point( 2, ymx_px ),
                colors::black );

    int yinc = ( ymn_px - ymx_px ) / 4;
    for( int ky = 0; ky < 4; ky++ )
    {
        int y = ymx_px + ky * yinc;
        graph.line( point(2, y),
                    point(5, y),
                    colors::black );
    }
}


}
}
