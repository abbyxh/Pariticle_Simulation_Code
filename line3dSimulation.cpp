#include <iostream>
#include <array>
#include <algorithm>
#include <random>
#include <math.h>

template <typename T, int Dim, int Len>
using Array2d = std::array<std::array<T, Dim>, Len>;



int main()
{
    
    /*
    defining the variables of the detector
    the first variables relate to the number of pixels
    the second lot of variables relate to the dimensions of the pixel
    the third lot of variables relate to the dimensions of the sensors on the pixel
    */

    const int len{10};              //length of side of detector
    const int volume{1000};         //no. of pixels in detector
    const int dim{3};               //no. of dimensions in simulation

    int sensorNumber {10};          //number of sensors on each pixel
    double pixelWidth{20 + 3 * static_cast<double>(sensorNumber)};  //size of pixel in x axis
    double pixelHeight{20};         //size of pixel in y axis
    double pixelDepth{20};          //size of pixel in z axis

    double sensorWidth{0.5 * sensorNumber};     //size of sensor in x axis(multiple sensors merged into 1 wider sensor)
    double sensorDepth(0.5);        //size of sensor in y axis
    double sensorHeight{3e-3};       //size of sensor in z axis

    //creating a 1d array of pixels, inner arrays are the coordinates of the center of the pixel(and sensor for this sim)
    Array2d<double, dim, volume> pixels{};



    /*
    Detector Geometry.

    creating the coordinates for each pixel and changing the corresponding value in the pixels array 
    */

    //iterating through x and y coords, i corresponds to x, j corresponds to y
    for(double i{0}; i < len - 2; i += 3)
    {
        for(int j{0}; j < len; ++j)
        {
            for(int k{0}; k < len; ++k)
            {
                //initialising x coord
                pixels[k + len * j + len * len * i][0] = k * pixelWidth;
                //initialising y coord
                pixels[k + len * j + len * len * i][1] = j * pixelHeight;
                //initialising z coord
                pixels[k + len * j + len * len * i][2] = i * pixelDepth;

                //offset pixels
                //initialising x coord
                pixels[k + len * j + len * len * (i + 1)][0] = (k + 0.5) * pixelWidth;
                //initialising y coord
                pixels[k + len * j + len * len * (i + 1)][1] = (j + 0.5) * pixelHeight;
                //initialising z coord
                pixels[k + len * j + len * len * (i + 1)][2] = i * pixelDepth;
            }    
        }      
    }
    


    /*
    Random Number Generation.

    defining the bounds for each of the randomly generated values for the particle path
    */
    
    double upperBoundx1 {0.9 * len * pixelWidth};
    double lowerBoundx1 {0.1 * len * pixelWidth};
    std::uniform_real_distribution<double> unifx1(lowerBoundx1, upperBoundx1);
    std::random_device randx1;

    double upperBoundy1 {0.9 * len * pixelHeight};
    double lowerBoundy1 {0.1 * len * pixelHeight};
    std::uniform_real_distribution<double> unify1(lowerBoundy1, upperBoundy1);
    std::random_device randy1;

    double upperBoundz1 {0.9 * len * pixelDepth};
    double lowerBoundz1 {0.1 * len * pixelDepth};
    std::uniform_real_distribution<double> unifz1(lowerBoundz1, upperBoundz1);
    std::random_device randz1;

    double upperBounda {100};
    double lowerBounda {-100};
    std::uniform_real_distribution<double> unifa(lowerBounda, upperBounda);
    std::random_device randa;

    double upperBoundb {100};
    double lowerBoundb {-100};
    std::uniform_real_distribution<double> unifb(lowerBoundb, upperBoundb);
    std::random_device randb;

    double upperBoundc {100};
    double lowerBoundc {-100};
    std::uniform_real_distribution<double> unifc(lowerBoundc, upperBoundc);
    std::random_device randc;


    /*
    looping through the particle creation and detection so can do multiple runs to get lots of
    values and find an average number of hits detectors with the same pixel and sensor variables
    */

    int totalHits {0};
    int runs{100000};

    for (int p{0}; p < runs; ++p)
    {

    //generating the new random numbers inside each loop
    double x1 {unifx1(randx1)};
    double y1 {unify1(randy1)};
    double z1 {unifz1(randy1)};
    double a {unifa(randa)};
    double b {unifb(randb)};
    double c {unifc(randc)};

    

    /*
    Particle Detection.

    by treating each sensor as 2 planes, one for the top and one for the bottom, i can determine where a particle
    (represented by a 3d line) intercepts these planes, for each y value which contains pixels, then using the corners
    of each pixel to determine if the pixel passed through the top or bottom of the pixels, which results in a hit.
    first i am looping through the locations of each plane to find the intercepts, then looping through each pixel
    to find if it hit the pixels. Note, need to only cheack each intercept with the pixels with matching y values.
    */

    std::vector<int> locations{};
    locations.reserve(len * len);

    Array2d<double, 2, len * 2> planeIntercepts{};

    //plane intercepts
    /*
    inputs: pixelHeight
            sensorHeight
            len

    outputs: planeIntercepts - if used later, probably need to define before hand and call as a reference
    */
    for(int i{0}; i < len; ++i)
    {
        planeIntercepts[2 * i][0] = (a / b) * ((i * pixelHeight - sensorHeight) - y1) + x1;
        planeIntercepts[2 * i][1] = (c / b) * ((i * pixelHeight - sensorHeight) - y1) + z1;

        planeIntercepts[2 * i + 1][0] = (a / b) * ((i * pixelHeight + sensorHeight) - y1) + x1;
        planeIntercepts[2 * i + 1][1] = (c / b) * ((i * pixelHeight + sensorHeight) - y1) + z1;
    }
        
    

    //finding hits.
    /*
    inputs: len
            pixels - call as a reference
            sensorWidth
            sensorDepth
            planeIntercepts - call as a reference
    outputs: locations - call as a reference
             or return locations.size()
    */
    for(int i{0}; i < len; ++i)
    {
        for (int j{0}; j < len; ++j)
        {
            for(int k{0}; k < len; ++k)
            {
                int greaterBottom {0};
                if((pixels[j + len * i + len * len * k][0] - sensorWidth < planeIntercepts[2 * i][0]) && (pixels[j + len * i + len * len * k][2] - sensorDepth < planeIntercepts[2 * i][1]))
                {++greaterBottom;}
                if((pixels[j + len * i + len * len * k][0] + sensorWidth < planeIntercepts[2 * i][0]) && (pixels[j + len * i + len * len * k][2] - sensorDepth < planeIntercepts[2 * i][1]))
                {++greaterBottom;}
                if((pixels[j + len * i + len * len * k][0] - sensorWidth < planeIntercepts[2 * i][0]) && (pixels[j + len * i + len * len * k][2] + sensorDepth < planeIntercepts[2 * i][1]))
                {++greaterBottom;}
                if((pixels[j + len * i + len * len * k][0] + sensorWidth < planeIntercepts[2 * i][0]) && (pixels[j + len * i + len * len * k][2] + sensorDepth < planeIntercepts[2 * i][1]))
                {++greaterBottom;}

                int greaterTop {0};
                if((pixels[j + len * i + len * len * k][0] - sensorWidth < planeIntercepts[2 * i + 1][0]) && (pixels[j + len * i + len * len * k][2] - sensorDepth < planeIntercepts[2 * i + 1][1]))
                {++greaterTop;}
                if((pixels[j + len * i + len * len * k][0] + sensorWidth < planeIntercepts[2 * i + 1][0]) && (pixels[j + len * i + len * len * k][2] - sensorDepth < planeIntercepts[2 * i + 1][1]))
                {++greaterTop;}
                if((pixels[j + len * i + len * len * k][0] - sensorWidth < planeIntercepts[2 * i + 1][0]) && (pixels[j + len * i + len * len * k][2] + sensorDepth < planeIntercepts[2 * i + 1][1]))
                {++greaterTop;}
                if((pixels[j + len * i + len * len * k][0] + sensorWidth < planeIntercepts[2 * i + 1][0]) && (pixels[j + len * i + len * len * k][2] + sensorDepth < planeIntercepts[2 * i + 1][1]))
                {++greaterTop;}

                if((greaterBottom != 0 && greaterBottom != 4) || (greaterTop != 0 && greaterTop != 4)) { locations.push_back(j);}
            }
        }
    }

    //adding up the number of hits from each iteration
    totalHits += locations.size();
    
    }

    std::cout << "Total hits: " << totalHits << '\n';
    std::cout << "Average hits: " << static_cast<double>(totalHits) / runs << "\n\n\n" ;
    
    return 0;
}