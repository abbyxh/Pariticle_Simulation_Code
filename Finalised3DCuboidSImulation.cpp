#include <iostream>
#include <array>
#include <algorithm>
#include <random>
#include <math.h>
#include <vector>

//this template is used to create a 2D array more simply
template <typename T, int Dim, int Vol>
using Array2d = std::array<std::array<T, Dim>, Vol>;


/*
This function puts values into an array which represent the coordniates of each pixel within the the diode

inputs: 
        pixels: an empty 2D array which is called by reference to fill in coordinate values
        len: int, the number of pixels across each side of the detector
        pixelWidth: double, the length of each pixel in the x axis
        pixelHeight: double, the length of each pixel in the y axis
        pixelDepth: double, the length of each pixel in the z axis

output:
        this is void, it 'returns' the coordinates as pixels is called by reference
*/

template <typename T, std::size_t Dim, std::size_t Vol>
void createCoords(Array2d<T, Dim, Vol>& pixels, int len, double pixelWidth, double pixelHeight, double pixelDepth)
{
    for(double z{0}; z < len ; z += 2)
    {
        for(int y{0}; y < len; ++y)
        {
            for(int x{0}; x < len; ++x)
            {
                //initialising x coord
                pixels[x + len * y + len * len * z][0] = x * pixelWidth;
                //initialising y coord
                pixels[x + len * y + len * len * z][1] = y * pixelHeight;
                //initialising z coord
                pixels[x + len * y + len * len * z][2] = z * pixelDepth;

                //offset pixels
                //initialising x coord
                pixels[x + len * y + len * len * (z + 1)][0] = (x + 0.5) * pixelWidth;
                //initialising y coord
                pixels[x + len * y + len * len * (z + 1)][1] = (y + 0.5) * pixelHeight;
                //initialising z coord
                pixels[x + len * y + len * len * (z + 1)][2] = (z + 1) * pixelDepth;
            }    
        }      
    }
}

/*
This function returns a random double value, uniformly distributed between the upper and lower bound

input:
        upper: double, the upper bound for the random number
        lower: double, the lower bound for the random number    
output:
        unif(rand): double, a randomly generated value
*/

double randomNumber(double upper, double lower)
{
    std::uniform_real_distribution<double> unif(lower, upper);
    std::random_device rand;
    return unif(rand);
}

/*
This function find the x-z coordinate for the intercept of a random line with each y plane which represents
the top and bottom of the diodes

inputs:
        planeIntercepts: an empty 2D array called by reference to hold the intercept coordinates
        len: int, the number of pixels across each side of the detector
        pixelHeight: double, the length of each pixel in the y axis
        sensorHeight: double, the length of the diodes in the y axis
        x1, y1, z1, a, b, c: double, randomly generated constant for the stright line equation

outputs:
        this function is void, but 'returns' the planeIntercepts array which is called by reference
*/

template <typename T, std::size_t Dim, std::size_t Vol>
void getIntercepts(Array2d<T, Dim, Vol>& planeIntercepts, int len, double pixelHeight, double sensorHeight,
                    double x1, double y1, double z1, double a, double b, double c)
{
    for(int i{0}; i < len; ++i)
    {
        planeIntercepts[4 * i][0] = (a / b) * ((i * pixelHeight - sensorHeight) - y1) + x1;
        planeIntercepts[4 * i][1] = (c / b) * ((i * pixelHeight - sensorHeight) - y1) + z1;

        planeIntercepts[4 * i + 1][0] = (a / b) * ((i * pixelHeight + sensorHeight) - y1) + x1;
        planeIntercepts[4 * i + 1][1] = (c / b) * ((i * pixelHeight + sensorHeight) - y1) + z1;

        planeIntercepts[4 * i + 2][0] = (a / b) * (((i + 0.5) * pixelHeight - sensorHeight) - y1) + x1;
        planeIntercepts[4 * i + 2][1] = (c / b) * (((i + 0.5) * pixelHeight - sensorHeight) - y1) + z1;

        planeIntercepts[4 * i + 3][0] = (a / b) * (((i + 0.5) * pixelHeight + sensorHeight) - y1) + x1;
        planeIntercepts[4 * i + 3][1] = (c / b) * (((i + 0.5) * pixelHeight + sensorHeight) - y1) + z1;
    }
}

/*
this function finds if any of the "plane intercepts" correspond to the coordinates of the diodes, thus finds the number of hits
from each particle, by comparing the intercept coordinates to the coordinates of the corners of each diode

inputs:
        pixels: an empty 2D array which is called by reference to fill in coordinate values
        planeIntercepts: an empty 2D array called by reference to hold the intercept coordinates
        len: int, the number of pixels across each side of the detector
        sensorWidth: double, the length of the diodes in the y axis
        sensorHeight: double, the length of the diodes in the y axis

outputs: 
        locations.size(): sint, the number of hits
*/

template<typename T, std::size_t Dim, std::size_t Vol, typename C, std::size_t Dim2, std::size_t Vol2>
int getHits(Array2d<T, Dim, Vol>& pixels, Array2d<C, Dim2, Vol2>& planeIntercepts, int len, double sensorWidth, double sensorDepth)
{
    std::vector<int> locations{};
    locations.reserve(2 * len);
    for(int z{0}; z < len; z += 2)
    {
        for (int y{0}; y < len; ++y)
        {
            for(int x{0}; x < len; ++x)
            {
                //for both the top an bottom planes for the sensors, adding 1 to the greater variables if the particle is > the x and y coord
                //if the particle passes through the sensor, the will not all be greater and not all less
                int greaterBottom {0};
                if((pixels[x + len * y + len * len * z][0] - sensorWidth < planeIntercepts[4 * z][0]) && (pixels[x + len * y + len * len * z][2] - sensorDepth < planeIntercepts[4 * z][1]))
                {++greaterBottom;}
                if((pixels[x + len * y + len * len * z][0] + sensorWidth > planeIntercepts[4 * z][0]) && (pixels[x + len * y + len * len * z][2] + sensorDepth > planeIntercepts[4 * z][1]))
                {++greaterBottom;}

                int greaterTop {0};
                if((pixels[x + len * y + len * len * z][0] - sensorWidth < planeIntercepts[4 * z + 1][0]) && (pixels[x + len * y + len * len * z][2] - sensorDepth < planeIntercepts[4 * z + 1][1]))
                {++greaterTop;}
                if((pixels[x + len * y + len * len * z][0] + sensorWidth > planeIntercepts[4 * z + 1][0]) && (pixels[x + len * y + len * len * z][2] + sensorDepth > planeIntercepts[4 * z + 1][1]))
                {++greaterTop;}
                
                //if the particle goes through the top or bottom plane, it has gone through the snensor
                //if((greaterBottom != 0 && greaterBottom != 4) || (greaterTop != 0 && greaterTop != 4)) 
                if((greaterBottom == 2) || ( greaterTop == 2)) 
                { 
                    locations.push_back(y);
                }

                greaterBottom = 0;
                if((pixels[x + len * y + len * len * (z + 1)][0] - sensorWidth < planeIntercepts[4 * z + 2][0]) && (pixels[x + len * y + len * len * (z + 1)][2] - sensorDepth < planeIntercepts[4 * z + 2][1]))
                {++greaterBottom;}
                if((pixels[x + len * y + len * len * (z + 1)][0] + sensorWidth > planeIntercepts[4 * z + 2][0]) && (pixels[x + len * y + len * len * (z + 1)][2] + sensorDepth > planeIntercepts[4 * z + 1][1]))
                {++greaterBottom;}

                greaterTop = 0;
                if((pixels[x + len * y + len * len * (z + 1)][0] - sensorWidth < planeIntercepts[4 * z + 3][0]) && (pixels[x + len * y + len * len * (z + 1)][2] - sensorDepth < planeIntercepts[4 * z + 3][1]))
                {++greaterTop;}
                if((pixels[x + len * y + len * len * (z + 1)][0] + sensorWidth > planeIntercepts[4 * z + 3][0]) && (pixels[x + len * y + len * len * (z + 1)][2] + sensorDepth > planeIntercepts[4 * z + 3][1]))
                {++greaterTop;}
                
                //if the particle goes through the top or bottom plane, it has gone through the snensor
                if((greaterBottom ==2 ) || (greaterTop ==2)) 
                { 
                    locations.push_back(y);
                }
            }
        }
    }
    return locations.size();
}

int main()
{
    
        const int len{14};              //length of side of detector
        const int volume{2744};         //no. of pixels in detector
        const int dim{3};               //no. of dimensions in simulation

        double pixelWidth{80};          //size of pixel in x axis
        double pixelHeight{29};         //size of pixel in y axis
        double pixelDepth{19};          //size of pixel in z axis

    
        std::vector<double> averages{};
        int sqrtSensorNo{40};
    
        double sensorWidth{0.236 * sqrtSensorNo};     //size of sensor in x axis(multiple sensors merged into 1 wider sensor)
        double sensorDepth(0.236 * sqrtSensorNo);        //size of sensor in y axis
        double sensorHeight{6e-3};  

        Array2d<double, dim, volume> pixels{};

        createCoords(pixels, len, pixelWidth, pixelHeight, pixelDepth);


        double totalHits{0};
        double runs{1000000};
        for(int p{0}; p < runs; ++p)
        {

            double x1 {randomNumber(0.9 * len * pixelWidth, 0.1 * len * pixelWidth)};
            double y1 {randomNumber(0.9 * len * pixelHeight, 0.1 * len * pixelHeight)};
            double z1 {randomNumber(0.9 * len * pixelDepth, 0.1 * len * pixelDepth)};
            double a {randomNumber(100, -100)};
            double b {randomNumber(100, -100)};
            double c {randomNumber(100, -100)};


            Array2d<double, 2, len * 4> planeIntercepts{};

            getIntercepts(planeIntercepts, len, pixelHeight, sensorHeight, x1, y1, z1, a, b, c);
            
            
            
            int numberOfHits{getHits(pixels, planeIntercepts, len, sensorWidth, sensorDepth)};
            totalHits += numberOfHits;
        }
        //std::cout << "Number of sensors: " << sensorNo << '\n';
        //std::cout << "Total hits: " << totalHits << '\n';
        //std::cout << "Average hits: " << static_cast<double>(totalHits) /static_cast<double>(runs) << '\n' << "\n\n\n";
    
        std::cout << totalHits / runs << ", ";
       
    
    return 0;
}