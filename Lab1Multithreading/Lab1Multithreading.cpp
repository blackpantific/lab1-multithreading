#include <iostream>
#include <fstream>
#include <string>
using namespace std;

double determinant(int dim, double** matrix);

double determinantMultithread(int dim, double** matrix);

int main()
{

    ifstream in("matrix.txt", ios::binary);
    int size = in.seekg(0, ios::end).tellg();
    in.seekg(0);
    char* buf = new char[size + 1];
    in.read(buf, size);
    buf[size] = 0;
    char* bufIterator = buf+3;
    string tempString = "";

    cout << buf << endl;

    double** table;    // указатель для блока памяти для массива указателей
    double* rows;      // указатель для блока памяти для хранения информации по строкам

    int rowsCount = (int)buf[0] - 48;  // количество строк
    //сделать проверку

    // выделяем память для двухмерного массива
    table = (double**) calloc(rowsCount, sizeof(double*));
    //rows = (double*) malloc(sizeof(double) * rowscount);

    for (int i = 0; i < rowsCount; i++)
    {
        table[i] =(double*) calloc(rowsCount, sizeof(double));//table[i] - это сам указатель на будущий массив под элементы

        int j = 0;
    	while(j!=rowsCount)
    	{
    		if((int)*bufIterator != 32 && (int)*bufIterator != 13 && (int)*bufIterator != 10 && *bufIterator != '\0')
    		{
                tempString += *bufIterator;
                bufIterator++;
    			
    		}else
    		{
                table[i][j] = stod(tempString);
                j += 1;
                bufIterator++;
                tempString = "";
    		}
    		if(j==rowsCount)
    		{
                bufIterator++;
    		}
    		
    	}
    }

 /*   for (int i = 0; i < rowsCount; i++)
    {
        printf("\n");

        for (int j = 0; j < rowsCount; j++)
        {
            printf("%f \t", table[i][j]);
        }
    }*/


    auto single = determinant(rowsCount, table);

    auto multi = determinantMultithread(rowsCount, table);
	
    int c = 5;

    


}

double determinant(int dim, double** matrix)
{
    double res = 0.0;

    if(dim == 2)
    {
        return res = matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1];
    }
    else
    {

        for (int i = 0; i < dim; i++)
        {
            double resFor = 0.0;
            int dimLoc = dim - 1;
            double** matrixLocal = (double**)calloc(dimLoc, sizeof(double*));

            for (int im = 0; im < dimLoc; im++)
            {
                matrixLocal[im] = (double*)calloc(dimLoc, sizeof(double));

                int ifContinue = 0;

                for (int col = 0; col < dim; col++)
                {
                    if (col == i)
                    {
                        ifContinue = 1;
                        continue;
                    }
                    else
                    {
                        if (ifContinue == 1)
                        {
                            matrixLocal[im][col - 1] = matrix[im + 1][col  /*+ ifContinue*/];
                        }
                        else
                        {
                            matrixLocal[im][col] = matrix[im + 1][col  /*+ ifContinue*/];
                        }

                    }
                }



            }


            for (int i = 0; i < dimLoc; i++)
            {
                printf("\n");

                for (int j = 0; j < dimLoc; j++)
                {
                    printf("%f \t", matrixLocal[i][j]);
                }
            }


            int sign = -1;//если false знак '-', если true знак '+'
            if (i % 2 == 0)
            {
                sign = 1;
            }


            auto d = matrix[0][i];


            resFor = sign * matrix[0][i] * determinant(dim - 1, matrixLocal);
            res += resFor;
        }
    }

        return res;
}

double determinantMultithread(int dim, double** matrix)
{
    double res = 0.0;

    if (dim == 2)
    {
        return res = matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1];
    }
    else
    {

        for (int i = 0; i < dim; i++)
        {
            double resFor = 0.0;
            int dimLoc = dim - 1;
            double** matrixLocal = (double**)calloc(dimLoc, sizeof(double*));

            for (int im = 0; im < dimLoc; im++)
            {
                matrixLocal[im] = (double*)calloc(dimLoc, sizeof(double));

                int ifContinue = 0;

                for (int col = 0; col < dim; col++)
                {
                    if (col == i)
                    {
                        ifContinue = 1;
                        continue;
                    }
                    else
                    {
                        if (ifContinue == 1)
                        {
                            matrixLocal[im][col - 1] = matrix[im + 1][col  /*+ ifContinue*/];
                        }
                        else
                        {
                            matrixLocal[im][col] = matrix[im + 1][col  /*+ ifContinue*/];
                        }

                    }
                }



            }


            for (int i = 0; i < dimLoc; i++)
            {
                printf("\n");

                for (int j = 0; j < dimLoc; j++)
                {
                    printf("%f \t", matrixLocal[i][j]);
                }
            }


            int sign = -1;//если false знак '-', если true знак '+'
            if (i % 2 == 0)
            {
                sign = 1;
            }


            auto d = matrix[0][i];


            resFor = sign * matrix[0][i] * determinant(dim - 1, matrixLocal);
            res += resFor;
        }
    }

    return res;
}

