#include <iostream>
#include <array>
#include <algorithm>
#include <random>
#include <math.h>

template <typename T, int Dim, int Len>
using Array2d = std::array<std::array<T, Dim>, Len>;



int main()
{
    
    const int len{20};      //length of side of detector
    const int area{400};    //no. of pixels in detector
    const int dim{2};       //no. of dimensions in simulation

    //creating a 1d array of pixels, inner arrays are the coords
    Array2d<double, dim, area> pixels{};


    for(double z{13}; z < 100; z += 5)
    {

    //geberating detector
    
    double pixelWidth{z};
    double pixelHeight{9};
    //iterating through x and y coords, i corresponds to x, j corresponds to y
    for(double i{0}; i < len-2; i += 3)
    {
        //first row
        for(int j{0}; j < len; ++j)
        {
            //initialising x coord
            pixels[j + len * i][0] = j * pixelWidth;
            //initialising y coord
            pixels[j + len * i][1] = i * pixelHeight;
        }
        //offset row
        for(double k{0}; k < len; ++k)
        {
            //initialising x coord
            pixels[k + len * (i + 1)][0] = (k + 0.5) * pixelWidth;
            //initialising y coord
            pixels[k + len * (i + 1)][1] = (i + 1) * pixelHeight;
        }
        //empty row
        #if 0
        for(int l{0}; l < len; ++l)
        {
            //abstract large number
            pixels[l + len * (i + 2)][0] = 0;
            pixels[l + len * (i + 2)][1] = 0;
        }
        #endif
        
    }
    



    
    //defining bounds for random number, for gradirnet
    double upperBoundM {5};
    double lowerBoundM {-5};

    //function for generating uniform random double
    std::uniform_real_distribution<double> unifM(lowerBoundM, upperBoundM);
    std::random_device M;
    //generating the random double

    //bounds for C on y axis, so every particle enters the left of the detector
    double upperBoundC {len * pixelWidth};
    double lowerBoundC {0};

    //function for generating uniform random double
    std::uniform_real_distribution<double> unifC(lowerBoundC, upperBoundC);
    std::random_device C;
    //generating the random double

    int totalHits {0};
    int runs{100000};
    for (int p{0}; p < runs; ++p)
    {
    double gradient {unifM(M)};
    double intercept {unifC(C)};
    //std::cout << "Particle tragectory: " << gradient << " * x + " << intercept << '\n';
    

    

    //particle detection
    
    //defining the starting position of the particle
    //middle of the x row, bottom of y
    
    
    //the size of the silicon detector on each pixel(half the true height and width)
    double width{3.3};
    double hight{3e-3};
    std::vector<int> locations{};
    locations.reserve(area);
        
    //looping though each pixel in the detector
    //using y = mx + c
    //if y - mx > c, add to greater

    //to increase hits when doing real vlaues will change to x= ym + c
    //so using c = x - my

    for (int i{0}; i < len - 2; i += 3)
    {
        for(int j{0}; j < len; ++j)
        {
            int greater{0};
            int lesser{0};
            //comparing the the points at each coner of the sensor to the line of the particle
            //if its 
            if((pixels[j + i * len][0] - width) -(gradient * (pixels[j + i * len][1] + hight)) > intercept) {++greater;}
            else {++lesser;}
            if((pixels[j + i * len][0] - width) -(gradient * (pixels[j + i * len][1] - hight)) > intercept) {++greater;}
            else {++lesser;}
            if((pixels[j + i * len][0] + width) -(gradient * (pixels[j + i * len][1] - hight)) > intercept) {++greater;}
            else {++lesser;}
            if((pixels[j + i * len][0] + width) -(gradient * (pixels[j + i * len][1] + hight)) > intercept) {++greater;}
            else{++lesser;}

            if( lesser > 0 && greater > 0) { locations.push_back(j + i * len);}

        }
        
        for(int k{0}; k < len; ++k)
        {
            int greater{0};
            int lesser{0};
            //std::cout << k + (i + 1) * len << '\n';
            if((pixels[k + (i + 1) * len][0] + width) -(gradient * (pixels[k + (i + 1) * len][1] - hight)) > intercept) {++greater;}
            else {++lesser;}
            if((pixels[k + (i + 1) * len][0] - width) -(gradient * (pixels[k + (i + 1) * len][1] - hight)) > intercept) {++greater;}
            else {++lesser;}
            if((pixels[k + (i + 1) * len][0] - width) -(gradient * (pixels[k + (i + 1) * len][1] + hight)) > intercept) {++greater;}
            else {++lesser;}
            if((pixels[k + (i + 1) * len][0] + width) -(gradient * (pixels[k + (i + 1) * len][1] + hight)) > intercept) {++greater;}
            else{++lesser;}

            if( lesser > 0 && greater > 0) { locations.push_back(k + (i + 1) * len);}
        }

        
    }

    std::sort(locations.begin(), locations.end());
 


    #if 0
    
    //printing
    std::cout << "No. of collisions: " << locations.size() << '\n';
    //printing the indices of the pixels hit
    std::cout << "index of pixels collided with: " << '\n';
    for (int i{0}; i < std::size(locations); ++i) 
        std::cout << locations[i] << ", ";
    
    std::cout << "Pixels grid:\n";

    int k{0};
    for(int i{0}; i < len - 2; i += 3)
    {
        for(int j{0}; j < len; ++j)
        {
           
            if(locations[k] == j + len * i)
            {
                std::cout << "XXX" << '\t';
                ++k;
            }
            else
            {            
            std::cout << pixels[j + len * i][0] << ',' << pixels[j + len * i][1] << '\t' ;
            }
            
        }
        std::cout << '\n';
        for(int m{0}; m < len; ++m)
        {
           
            if(locations[k] == m + len * (i + 1))
            {
                std::cout << "XXX" << '\t';
                ++k;
            }
            else
            {            
            std::cout << pixels[m + len * (i + 1)][0] << ',' << pixels[m + len * (i + 1)][1] << '\t' ;
            }
            
        }
        std::cout << '\n';
        for (int n{0}; n < len; ++n)
        {
            std::cout  << "----" << '\t';
        }

        std::cout << '\n';
    }
    #endif
    totalHits += locations.size();
    }

    std::cout << "Pixel width: " << z << '\n';
    std::cout << "Total hits: " << totalHits << '\n';
    std::cout << "Average hits: " << static_cast<double>(totalHits) / runs << '\n' << '\n';
    }
    return 0;
}