#include <array>
#include <iostream>
#include <random>

//use template to create a 2d array

template <typename T, std::size_t Row, std::size_t Col>
using Array2d = std::array<std::array<T, Col>, Row>;

int main()
{
    // define parameters for the simulation: number of diode active area, distance between diodes (in sequence)
    // number of rows of diodes
    double length_x {30};
    double length_y {60};
    double diode_len {0.236};
    const int n_diode {10}; //must use const values to make array
    const int n_rows {10};

    // from these values created array of coordinates for centre of circle diodes
    //use loop and if statements to create positions of diodes
    
    Array2d<double, n_diode*n_rows, 2> arr {{
        {}}};

    double x {0};
    double y {0};
    int count {0};
    int row {0};

    for (auto& val: arr)
    {
        count += 1;
        val[1] = y; 
        val[0] = x;
        x += length_x+ diode_len; //add to x the length of the diode and space bewteen the diodes
    
        if (count%n_diode == 0) //if even then offset rows
        {
            row+=1;
            if (row%2==0)
                x=0; //no offset
            else
                x=(diode_len+length_x)/2; //offset

            y += length_y+diode_len; //change the y length the same as the x length
        }
    }  


    double hits {0}; //counter for the number of hits that occurr
    double runs{100000}; //number of loops (randomised lines) or particles that will be ran
    for (int particle{1}; particle<=runs; ++particle)
    {
        //create randomised line using random number generator in form ax+by+c=0
        std::uniform_real_distribution<double> a(-10, 10);
        std::random_device A;
        std::uniform_real_distribution<double> b(-10, 10);
        std::random_device B;
        std::uniform_real_distribution<double> c(0, n_diode*(length_x+diode_len));
        std::random_device C;
        double a1 {a(A)};
        double b1 {b(B)};
        double c1 {c(C)};

        //loop goes through values and works out if a hit has been recorded
        for (auto& val: arr)
        {
            double x_cent {val[0]};
            double y_cent {val[1]};

            //formular to find perpendicular distance between centre of circle and line
            double l_distance {(fabs((a1*x_cent) + (b1*y_cent) + c1))/sqrt((a1*a1)+(b1*b1))};

            if (l_distance<=diode_len/2)
            {
                hits +=1;
            }
            else
                {continue;}
        }   
    }
    double prob_hit {hits/(runs*n_diode*n_rows)}; 
    double ratio_hit {hits/runs}; 
    std::cout<<"The probability each diode is hit: "<<prob_hit; 
    std::cout<<"\nThe ratio of hits to number of runs: "<<ratio_hit; //print out results
    return 0;
}