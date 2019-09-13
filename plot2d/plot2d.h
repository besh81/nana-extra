#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui.hpp>

namespace nana
{
namespace plot
{
class plot;

/** \brief Single trace to be plotted

    Application code shouild not attempt to construct a trace
    Rather call one of plot::AddPointTrace, plot::AddRealTimeTrace or plot::AddStaticTrace
    which return a reference to the trace which can be configured
    and be populated with data.

    <pre>
        form fm;

         construct plot to be drawn on form

        plot::plot thePlot( fm );

         construct plot trace

        auto t1 = thePlot.AddStaticTrace();

         provide some data for  trace

        std::vector< double > d1 { 10, 15, 20, 25, 30, 25, 20, 15, 10 };
        t1.set( d1 );

         plot in blue

        t1.color( colors::blue );

         show and run

        fm.show();
        exec();
    </pre>
*/
class trace
{
public:

    /** \brief set plot data
        @param[in] y vector of data points to display

        Replaces any existing data.  Plot is NOT refreshed.
        An exception is thrown when this is called
        for a trace that is not plot type
    */
    void set( const std::vector< double >& y );

    /** \brief add new value to real time data
        @param[in] y the new data point

        An exception is thrown when this is called
        for a trace that is not real time type.
    */
    void add( double y );

    /** \brief add point to scatter trace
        @param[in] x location
        @param[in] y location

        An exception is thrown when this is called
        for a trace that is not scatter type
    */

    void add( double x, double y );

    /// set color
    void color( const colors & clr )
    {
        myColor = clr;
    }

    int size()
    {
        return (int) myY.size();
    }

private:

    friend plot;

    plot * myPlot;
    std::vector< double > myX;
    std::vector< double > myY;
    colors myColor;
    int myRealTimeNext;
    enum class eType
    {
        plot,
        realtime,
        scatter
    } myType;

    /** CTOR
    Application code should not call this constructor
    Rather call one of plot::AddPointTrace, plot::AddRealTimeTrace or plot::AddStaticTrace

    */
    trace()
        : myType( eType::plot )
    {

    }

    /// set plot where this trace will appear
    void Plot( plot * p )
    {
        myPlot = p;
    }

    /** \brief Convert trace to real time operation
    @param[in] w number of data points to display

    Data points older than w scroll off the left edge of the plot and are lost
    */
    void realTime( int w )
    {
        myType = eType::realtime;
        myRealTimeNext = 0;
        myY.clear();
        myY.resize( w );
    }

    /** \brief Convert trace to point operation for scatter plots */
    void scatter()
    {
        myType = eType::scatter;
        myY.clear();
        myX.clear();
    }

    /// min and max values in trace
    void bounds(
        double& txmin, double& txmax,
        double& tymin, double& tymax );

    /// draw
    void update( paint::graphics& graph );
};
/** \brief Draw decorated vertical line on LHS of plot for Y-axis

    This class is internal and none of its methods should be
    called by the application code
*/
class axis
{
public:
    axis( plot * p );

    ~axis()
    {
        delete myLabelMin;
        delete myLabelMax;
        delete myLabelZero;
    }

    /// draw
    void update( paint::graphics& graph );

    void Grid( bool f )
    {
        myfGrid = f;
    }

private:
    plot * myPlot;
    label * myLabelMin;
    label * myLabelMax;
    label * myLabelZero;
    bool myfGrid;
};


/** \brief Draw a 2D plot

The plot contains one or more traces.

Each trace can be of one of three types:

- Plot: succesive y-values with line drawn between them.
- Scatter: succesive x,y-values with box around each point
- Realtime: a specified number of the most recent y-values

Any number of plot and scatter traces can be shown together,
only one realtime trace may be present in a plot.

 */
class plot
{
public:

    /** \brief CTOR
        @param[in] parent window where plot will be drawn
    */
    plot( window parent );

    ~plot()
    {
        delete myAxis;
    }

    /** \brief Add static trace
        @return reference to new trace

        The data in a static trace does not change
        A line is drawn between successive points
        Specify y location only for each point.
        The points will be evenly distributed along the x-axis
    */
    trace& AddStaticTrace()
    {
        trace * t = new trace();
        t->Plot( this );
        myTrace.push_back( t );
        return *t;
    }

    /** \brief Add real time trace
        @param[in] w number of recent data points to display
        @return reference to new trace

        The data in a real time trace receives new values from time to time
        The display shows w recent values.  Older values scroll off the
        left hand side of the plot and disappear.
    */
    trace& AddRealTimeTrace( int w )
    {
        trace * t = new trace();
        t->Plot( this );
        t->realTime( w );
        myTrace.push_back( t );
        return *t;
    }

    /** \brief Add scatter trace
        @return reference to new trace

        A static trace for scatter plots
        No line between points,
          box around each point.
        Specify x AND y locations for each point.
    */
    trace& AddScatterTrace();

    /** \brief Enable display of grid markings */
    void Grid( bool enable )
    {
        myAxis->Grid( enable );
    }

    int X2Pixel( double x ) const
    {
        return myXOffset + myXScale * x;
    }
    int Y2Pixel( double y ) const
    {
        return myYOffset - myYScale * y;
    }

    float xinc()
    {
        return myXinc;
    }
    double minY()
    {
        return myMinY;
    }
    double maxY()
    {
        return myMaxY;
    }
//    double Scale()
//    {
//        return myScale;
//    }
    int XOffset()
    {
        return myXOffset;
    }
    int YOffset()
    {
        return myYOffset;
    }
    window parent()
    {
        return myParent;
    }

    void update()
    {
        API::refresh_window( myParent );
    }

    void debug()
    {
        for( auto t : myTrace )
        {
            std::cout << "debugtsize " << t->size() << "\n";
        }
    }

private:

    ///window where plot will be drawn
    window myParent;

    axis * myAxis;

    /// plot traces
    std::vector< trace* > myTrace;

    float myXinc;
    double myMinX, myMaxX;
    double myMinY, myMaxY;
    double myXScale, myYScale;
    int myXOffset;
    int myYOffset;

    /** calculate scaling factors so plot will fit in window client area
        @param[in] w width
        @param[in] h height
    */
    void CalcScale( int w, int h );

    /// arrange for the plot to be updated when needed
    void RegisterDrawingFunction();

    int MaxXPixel();

};

}
}
