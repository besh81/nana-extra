/*
		* Copyright (c) 2014 by James Bremner
		* All rights reserved.
		*
		* Use license: Modified from standard BSD license.
		*
		* Redistribution and use in source and binary forms are permitted
		* provided that the above copyright notice and this paragraph are
		* duplicated in all such forms and that any documentation, advertising
		* materials, Web server pages, and other materials related to such
		* distribution and use acknowledge that the software was developed
		* by James Bremner. The name "James Bremner" may not be used to
		* endorse or promote products derived from this software without
		* specific prior written permission.
		*
		* THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
		* IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
		* WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <functional>
#include <vector>

namespace raven
{

/**  A 2D cubic spline : draws a set of smooth curves through points */
class cSpline
{

public:

    /**  Constructor: Calculates the spline curve coefficients

    @param[in] x  The x points
    @param[in] y  The y points

    The x points must be in increasing order,
    with a minimum delta of 1.0
    */
    cSpline(
            std::vector< double >& x,
            std::vector< double >& y );

    /** Check if input is insane

    @return true if all OK
     */
    bool IsSane()      { return (bool) ! myError; }

    /// error numbers
    enum e_error
    {
        no_error,
        x_not_ascending,
        no_input,
        not_single_valued,
    } myError;

    /** Check for error

    @return 0 if all OK, otherwise an error number

    */
    e_error IsError()   { return myError; }

    /**  Draw the spline curves

    @param[in]  the drawing function
    @param[in]  resolution The number of times to call the drawing function, default 100.

    This method will iterate between the minimum and maximum x values
    calling the drawing function 100 times with evenly spaced values of x
    and the y value of the spline curve for that x.

    Example of usage
    <pre>
    double xold = x0;
    double yold = spline.getY( x0 );
    spline.Draw( [&] ( double x, double y )
                {
                    dc.DrawLine( xold, yold, x, y );
                    xold = x;
                    yold = y;
                });

    </pre>
    */
    void Draw( std::function<void (double x, double y)> func,
              int resolution = 100 );

    /** Get the Y value of the spline curves for a particular X

    @param[in] x

    @return the y value

    */
    double getY( double x);

private:

    // The fitted points
    std::vector< double > myX;
    std::vector< double > myY;

    /// The coefficients of the spline curve between two points
    struct SplineSet
    {
        double a;   // constant
        double b;   // 1st order coefficient
        double c;   // 2nd order coefficient
        double d;   // 3rd order coefficient
        double x;   // starting x value
    };

    /// The coefficients of the spline curves between all points
    std::vector< SplineSet > mySplineSet;

    /// The minimum delta between x points, must be 1
    double myMinUniqueDelta;

    bool IsInputSane();

};
};
