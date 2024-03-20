/**
 * @initial_sim_3D.cpp
 * 
 * @brief To produce a 2D array of 3D coordinates to describe the centre of the top face of a cylinder.
 *  Then to create a randomised line (particle) and see if that line intersects the cylinders.
 * 
 * @author Abigail Harrison
*/
#include <array>
#include <vector>
#include <iostream>
#include <random>

/***
 * Creates a 2D array.
 * 
 * @param T coordinate data inputed into the array
 * @param Row coordinate data inputed into the array
 * @param Col coordinate data inputed into the array
*/
template <typename T, std::size_t Row, std::size_t Col>
using Array2d = std::array<std::array<T, Col>, Row>;

/***
 * Creates a 2D array to describe sets of 3D coordinates referring to the centre of the top 
 * face of cylindrical diodes.
 * Then creates a random 3D line and determines whether the 3D line intersected the diodes.
 * 
 * @param double_len_z the length of the diode in the z direction (thickness)
*/
void runs(double diode_len_z)
{
    // define parameters for the simulation: number of diode active area, distance between diodes (in sequence)
    // number of rows of diodes

    const double length_x {2};
    const double length_y {2};
    const double length_z {2};

    //diode sizes
    const double diode_len_x {0.238};
    const double diode_len_y {0.238};
    //const double diode_len_z {4e-3};
    const int diode {10};
    
    //create an array of zeros to be altered in the next loop
    Array2d<double, diode*diode*diode, 3> arr {{
        {}}};

    double x {0};
    double y {0};
    double z {0};
    int count {0};
    int z_counter {1};
    double z_val {0};

    //go through each set of coordinates in the loop
    //the loop itself was based off a pattern I found for even lengths of diodes

    //For example if the changing lengths were 1 for 4x4x4
    //x=0000,1111,2222,3333... , y=1234,1234,1234..., z=0101,1010,0101...
    //Once this pattern had occurred for the first two layers, the process could be repeated with x=0 and y=0
    //and z increased to 2 (so the next pattern would be x=0000... y=1234..., z=2323,3232...)

    for (int i=0; i<diode*diode*diode; ++i) {
        arr[i][0] = x;
        arr[i][1] = y;
        arr[i][2] = z;

        count += 1;
        y += length_y + diode_len_y; //set increasing length of y for each loop
    
        if ((i+1)%diode == 0 && i!=0) { //if you have doen one loop of the y values, set y back to 0 and increase x
            x += (length_x + diode_len_x)/2;
            y = 0;
        }
        if (count == 2*diode) { //if counter is double the diode it resets
            count = 0; 
        }
        if (count < diode) { //if counter less than diode
            if (i%2 == 0) { //alternate between z values
                z = z_val + length_z + diode_len_z;
            }
            else { 
                z = z_val;
            }
        }
        else {
            if (i%2 == 0) {//if the counter is greater than the diode but less than double the diode, swap the alternating z values
                z = z_val;
            }
            else {
                z = z_val + length_z + diode_len_z;
            }
        }

        //if two rows have been completed then increase z (to do the next two rows) but reset x and y to 0
        if (diode%2==0 && (2*diode*diode*z_counter)/(i+1)==1 && (2*diode*diode*z_counter)%(i+1)==0) {
            x = 0; 
            y = 0;
            z_counter += 1;
            z_val = z + 2*(length_z + diode_len_z);
            z = z_val;
        }
    }

    double hits {0}; //record the number of hits
    int runs{100000}; //amount of random lines
    for (int particle{1}; particle<=runs; ++particle)
    {
        std::uniform_real_distribution<double> a(-100, 100);
        std::random_device A;
        std::uniform_real_distribution<double> b(-100, 100);
        std::random_device B;
        std::uniform_real_distribution<double> c(-100, 100);
        std::random_device C;
        std::uniform_real_distribution<double> x0(0, diode*(length_x+diode_len_x));
        std::random_device X1;
        std::uniform_real_distribution<double> y0(0, diode*(diode_len_y+length_y));
        std::random_device Y1;
        std::uniform_real_distribution<double> z0(0, diode*(diode_len_z+length_z));
        std::random_device Z1;
        //3D random lines are created for the general 3D line form
        double a1 {a(A)};
        double b1 {b(B)};
        double c1 {c(C)};
        double x2 {x0(X1)};
        double y2 {y0(Y1)}; 
        double z2 {z0(Z1)};

        for (auto& val: arr)
        {
            //look at x-y plane
            //find perepedicular distance between center point of diode and 3D line in x-y plane 
            double l_distance {(fabs((b1*val[0]) - (a1*val[1]) + ((y2*a1)-(x2*b1))))/sqrt((a1*a1)+(b1*b1))};

            //calculate z-values of the line for the x and y points at the centre of the diode
            double z_from_x {c1*((val[0]-x2)/a1)+z2};
            double z_from_y {c1*((val[1]-y2)/b1)+z2};

            //if the perpendicular length is smaller than the radius (using len_x but could use len_y as they are the same (aka they are both the radius))
            //and if z values are between the thickness of the diode then record a count
            if (l_distance<=diode_len_x && ((z_from_x>val[2] && z_from_x<val[2]+diode_len_z) || (z_from_y>val[2] && z_from_y<val[2]+diode_len_z)))
            {
                hits +=1;
            }
            else
            {
                continue;
            }
        }  
    }
    //record and print results
    double prob_hit {hits/(runs*diode*diode*diode)}; 
    double ratio_hit {hits/runs}; 
    //std::cout<<"The probability each diode is hit: "<<prob_hit; 
    //std::cout<<"\nThe ratio of hits to number of runs: "<<ratio_hit<<'\n';
    std::cout<<ratio_hit<<", ";
}   

/***
 * Sets up the different experiments that were being looked at by inputing different values into the
 * runs function.
*/
int main()
{
    for (double i=0.1; i<=5; i = i+0.1)
        std::cout<<i<< ", ";

    std::cout<<'\n';
    for (double i=0.1; i<=5; i = i+=0.1){
        runs(i);
    }

    return 0;
}