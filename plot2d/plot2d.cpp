#include <iostream>
#include <algorithm>
#include <cmath>
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
    myTrace[0]->bounds(
        myMinX, myMaxX,
        myMinY, myMaxY );
    for( auto& t : myTrace )
    {
        if( t->size() > maxCount )
            maxCount = t->size();
        double txmin, txmax, tymin, tymax;
        t->bounds( txmin, txmax, tymin, tymax );
        if( txmin < myMinX )
            myMinX = txmin;
        if( txmax > myMaxX )
            myMaxX = txmax;
        if( tymin < myMinY )
            myMinY = tymin;
        if( tymax > myMaxY )
            myMaxY = tymax;
    }
    if( ! maxCount )
        return;
    if( fabs( myMaxX - myMinX) < 0.0001 )
        myXScale = 1;
    else
        myXScale = 0.9 * w / ( myMaxX - myMinX );
    if( fabs( myMaxY - myMinY ) < 0.0001 )
        myYScale = 1;
    else
        myYScale = 0.9 * h / ( myMaxY - myMinY );

    myXOffset = 0.05 * w;
    myYOffset = h + myYScale * myMinY;

    //std::cout << myMinY <<" "<< myMaxY <<" "<< myScale;
}
void trace::set( const std::vector< double >& y )
{
    if( myType != eType::plot )
        throw std::runtime_error("nanaplot error: plot data added to non plot trace");

    myY = y;

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

void trace::bounds(
    double& txmin, double& txmax,
    double& tymin, double& tymax )
{
    if( myY.size() )
    {
        if( myType == eType::realtime || myX.size() == 0 )
        {
            txmin = 0;
            txmax = myY.size();
        }
        else
        {
            auto result = std::minmax_element(
                              myX.begin(),
                              myX.end());
            txmin = *result.first;
            txmax = *result.second;
        }
        auto result = std::minmax_element(
                     myY.begin(),
                     myY.end());
        tymin = *result.first;
        tymax = *result.second;
    }
}

void trace::update( paint::graphics& graph )
{
    bool first = true;
    int xi    = 0;
    float xinc = myPlot->xinc();
    double prevX, prev;


    switch( myType )
    {
    case eType::plot:

        // loop over data points
        for( auto y : myY )
        {
            // scale
            double x  = myPlot->X2Pixel( xi );
            double ys = myPlot->Y2Pixel( y );

            if( first )
            {
                first = false;
                prevX = x;
                prev = ys;
                continue;
            }

            // draw line from previous to this data point
            graph.line(
                point(prevX, prev),
                point(x, ys),
                myColor);

            prevX = x;
            prev = ys;
            xi++;
        }
        break;

    case eType::scatter:

        for( int k = 0; k < (int)myX.size(); k++ )
        {
            graph.rectangle(
                rectangle{ myPlot->X2Pixel( myX[ k ] )-5,  myPlot->Y2Pixel( myY[ k ] )-5,
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
            double x = myPlot->X2Pixel( yidx );
            double y = myPlot->Y2Pixel( myY[ yidx ] );

            // the next data point
            // with wrap-around if the end of the vector is reached
            yidx++;
            if( yidx >= (int)myY.size() )
                yidx = 0;

            if( first )
            {
                first = false;
                prevX = x;
                prev = y;
                continue;
            }
            // draw line from previous to this data point
            graph.line(
                point( prevX, prev ),
                point( x, y ),
                myColor);

            prevX = x;;
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
    , myfGrid( false )
{
    myLabelMin = new label( myPlot->parent(),  rectangle{ 10, 10, 50, 15 } );
    myLabelMin->caption("test");
    myLabelMax = new label( myPlot->parent(),  rectangle{ 10, 10, 50, 15 } );
    myLabelMax->caption("test");
    myLabelZero = new label( myPlot->parent(),  rectangle{ 10, 10, 50, 15 } );
    myLabelZero->caption("0.0");
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

    if( mn * mx < 0 )
    {
        graph.line( point(2, ymx_px),
                    point(5, ymx_px),
                    colors::black );
        int y0_px = myPlot->Y2Pixel( 0 );
        myLabelZero->move( 5,   y0_px - 15 );
        myLabelZero->show();
        graph.line( point(2, y0_px),
                    point(5, y0_px),
                    colors::black );
        graph.line( point(2, ymn_px),
                    point(5, ymn_px),
                    colors::black );
        if( myfGrid )
            for( int k=5; k<graph.width(); k=k+10 )
            {
                graph.set_pixel(k, y0_px, colors::blue );
                graph.set_pixel(k+1, y0_px, colors::blue );
            }
    }
    else
    {
        myLabelZero->hide();
        int yinc = ( ymn_px - ymx_px ) / 4;
        for( int ky = 0; ky < 4; ky++ )
        {
            int y = ymx_px + ky * yinc;
            graph.line( point(2, y),
                        point(5, y),
                        colors::black );
            if( myfGrid )
                for( int k=5; k<graph.width(); k=k+10 )
                {
                    graph.set_pixel(k, y, colors::blue );
                    graph.set_pixel(k+1, y, colors::blue );
                }
        }
    }
}


}
}
