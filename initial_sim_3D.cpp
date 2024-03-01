#include <array>
#include <vector>
#include <iostream>
#include <random>

template <typename T, std::size_t Row, std::size_t Col>
using Array2d = std::array<std::array<T, Col>, Row>;

template <typename T, std::size_t Row, std::size_t Col>
void printArray(Array2d<T, Row, Col> &arr)
{
    for (const auto& arow: arr)   // get each array row
    {
        for (const auto& e: arow) // get each element of the row
            std::cout << e << ' ';

        std::cout << '\n';
    }
}

int main()
{
    // define parameters for the simulation: number of diode active area, distance between diodes (in sequence)
    // number of rows of diodes
    double length_x {2.2};
    double length_y {2.2};
    double diode_len {1.1};
    double thickness {0.001};
    const int n_diode {10}; //must use const values to make array
    const int n_rows {10};

    // from these values created array of coordinates for centre of circle diodes
    //use loop??
    
    Array2d<double, n_diode*n_rows*n_rows, 3> arr {{
        {}}};

    double x {0};
    double y {0};
    double z {0};
    int count {0};
    int row {0};
    int v {0};

    for (auto& val: arr)
    {
        count += 1;
        val[2] = z;
        val[1] = y;
        val[0] = x;
        x += length_x+ diode_len;
    
        if (count%n_diode == 0)
        {
            row+=1;
            if (row%2==0)
                x=0;
            else
                x=(diode_len+length_x)/2;

            y += length_y+diode_len;
        }
     
        if (n_diode*n_rows == count) {
            z += (thickness + 0.01)/2;
            v += 1;
            if (v%2 == 0; ++v) {
                y=0;
            }
            else {
                y = (diode_len+length_y);
            }
            x = 0;
            count = 0;
        }
    
    }  

    printArray(arr);

    //make random line travelling through
    //ax+by+c =0

    double hits {0};
    int runs{1000000};
    for (int particle{1}; particle<=runs; ++particle)
    {
        std::uniform_real_distribution<double> a(-5, 5);
        std::random_device A;
        std::uniform_real_distribution<double> b(-5, 5);
        std::random_device B;
        std::uniform_real_distribution<double> c(-5, 5);
        std::random_device C;
        std::uniform_real_distribution<double> x0(-5, 5);
        std::random_device X1;
        std::uniform_real_distribution<double> y0(-5, 5);
        std::random_device Y1;
        std::uniform_real_distribution<double> z0(-5, 5);
        std::random_device Z1;

        //specify line using 2 3D points
        double a1 {a(A)};
        double b1 {b(B)};
        double c1 {c(C)};
        double x2 {x0(X1)};
        double y2 {y0(Y1)}; 
        double z2 {z0(Z1)};

        std::vector gradient {a1, b1, c1};
        std::vector point {x2, y2, z2};

        //std::cout<< a1 << "   "<<b1<<"   "<< c1<< '\n';

        for (auto& val: arr)
        {
            double x_cent {val[0]};
            double y_cent {val[1]};

            double l_distance {(fabs((b1*x_cent) - (a1*y_cent) + ((y2*a1)-(x2*b1))))/sqrt((a1*a1)+(b1*b1))};

            double z_from_x {c1*((x_cent-x2)/a1+z2)};
            //double z_from_y {c1*((y_cent-y2)/b1+z2)};

            if (l_distance<=diode_len/2 && z_from_x>val[2] && z_from_x<val[2]+thickness)
            {
                //std::cout << x_cent << ", " << y_cent<<'\n';
                hits +=1;
            }
            else
                {continue;}
        }   
    }
    double prob_hit {hits/(runs*n_diode*n_rows*n_rows)}; 
    double ratio_hit {hits/runs}; 
    std::cout<<"The probability each diode is hit: "<<prob_hit; 
    std::cout<<"\nThe ratio of hits to number of runs: "<<ratio_hit; 
    return 0;
}