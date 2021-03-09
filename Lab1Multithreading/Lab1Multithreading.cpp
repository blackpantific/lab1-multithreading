#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>
using namespace std;

float determinant(int dim, float** matrix);
float determinantTest(int dim, int dimStart, float** matrix);

float determinantMultithread(int dim, int dimStart, float** matrix);

int main()
{

	ifstream in("matrix.txt", ios::binary);
	int size = in.seekg(0, ios::end).tellg();
	in.seekg(0);
	char* buf = new char[size + 1];
	in.read(buf, size);
	buf[size] = 0;
	char* bufIterator = buf;
	string tempString = "";

	cout << buf << endl;

	float** table;    // указатель для блока памяти для массива указателей
	float* rows;      // указатель для блока памяти для хранения информации по строкам
	int rowsCount = 0;
	
	while(rowsCount == 0)
	{
		if(*bufIterator != ' ' && (int)*bufIterator != 13 && (int)*bufIterator != 10 && *bufIterator != '\0')
		{
			tempString += *bufIterator;
			bufIterator++;
		}
		else
		{
			rowsCount = stod(tempString);
			bufIterator += 2;
		}


		
		//rowsCount = (int)buf[0] - 48;  // количество строк
	//сделать проверку
	}

	tempString = "";

	// выделяем память для двухмерного массива
	table = (float**)calloc(rowsCount, sizeof(float*));
	//rows = (float*) malloc(sizeof(float) * rowscount);

	for (int i = 0; i < rowsCount; i++)
	{
		table[i] = (float*)calloc(rowsCount, sizeof(float));//table[i] - это сам указатель на будущий массив под элементы

		int j = 0;
		while (j != rowsCount)
		{
			if ((int)*bufIterator != 32 && (int)*bufIterator != 13 && (int)*bufIterator != 10 && *bufIterator != '\0')
			{
				tempString += *bufIterator;
				bufIterator++;

			}
			else
			{
				table[i][j] = stod(tempString);
				j += 1;
				bufIterator++;
				tempString = "";
			}
			if (j == rowsCount)
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

	float start_time, end_time, tick;
	start_time = omp_get_wtime();
	
	//auto single = determinant(rowsCount, table);
	end_time = omp_get_wtime();

	auto res1 = end_time - start_time;

	start_time = omp_get_wtime();
	//auto test = determinantTest(rowsCount, rowsCount, table);//работает быстрее, чем просто determinant
	//omp_set_num_threads(8);
	auto multi = determinantMultithread(rowsCount, rowsCount, table);
	end_time = omp_get_wtime();

	auto res2 = end_time - start_time;

	//printf("Single: %f\n", single);
	//printf("Multi: %f\n", multi);
	
	int c = 5;




}

float determinant(int dim, float** matrix)
{
	float res = 0.0;

	if (dim == 2)
	{
		return res = matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1];
	}
	else
	{

		for (int i = 0; i < dim; i++)
		{
			float resFor = 0.0;
			int dimLoc = dim - 1;
			float** matrixLocal = (float**)calloc(dimLoc, sizeof(float*));

			for (int im = 0; im < dimLoc; im++)
			{
				matrixLocal[im] = (float*)calloc(dimLoc, sizeof(float));

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


			/*for (int i = 0; i < dimLoc; i++)
			{
				printf("\n");

				for (int j = 0; j < dimLoc; j++)
				{
					printf("%f \t", matrixLocal[i][j]);
				}
			}*/


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



float determinantMultithread(int dim, int dimStart, float** matrix)
{

	float res = 0.0;

	if (dim == dimStart)
	{

#pragma omp parallel
		{


			int i = 0;
#pragma omp for 
			for (i = 0; i < dim; i++)
			{
				float resFor = 0.0;
				int dimLoc = dim - 1;
				float** matrixLocal = (float**)calloc(dimLoc, sizeof(float*));

				for (int im = 0; im < dimLoc; im++)
				{
					matrixLocal[im] = (float*)calloc(dimLoc, sizeof(float));

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


				/*for (int i = 0; i < dimLoc; i++)
				{
					printf("\n");

					for (int j = 0; j < dimLoc; j++)
					{
						printf("%f \t", matrixLocal[i][j]);
					}
				}*/


				int sign = -1;//если false знак '-', если true знак '+'
				if (i % 2 == 0)
				{
					sign = 1;
				}


				auto d = matrix[0][i];


				resFor = sign * matrix[0][i] * determinantMultithread(dim - 1, dim, matrixLocal);

#pragma omp atomic
				res += resFor;
			}
		}
	}
	else
	{

		if (dim == 2)
		{
			return res = matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1];
		}
		else
		{
			float resLoc = 0.0;

			for (int i = 0; i < dim; i++)
			{
				float resFor = 0.0;
				int dimLoc = dim - 1;
				float** matrixLocal = (float**)calloc(dimLoc, sizeof(float*));

				for (int im = 0; im < dimLoc; im++)
				{
					matrixLocal[im] = (float*)calloc(dimLoc, sizeof(float));

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


				/*for (int i = 0; i < dimLoc; i++)
				{
					printf("\n");

					for (int j = 0; j < dimLoc; j++)
					{
						printf("%f \t", matrixLocal[i][j]);
					}
				}*/


				int sign = -1;//если false знак '-', если true знак '+'
				if (i % 2 == 0)
				{
					sign = 1;
				}


				auto d = matrix[0][i];


				resFor = sign * matrix[0][i] * determinantMultithread(dim - 1, dim, matrixLocal);
				resLoc += resFor;
			}
			return resLoc;
		}
	}



	return res;
}

float determinantTest(int dim, int dimStart, float** matrix)
{
	float res = 0.0;

	if (dim == dimStart)
	{
		for (int i = 0; i < dim; i++)
		{
			float resFor = 0.0;
			int dimLoc = dim - 1;
			float** matrixLocal = (float**)calloc(dimLoc, sizeof(float*));

			for (int im = 0; im < dimLoc; im++)
			{
				matrixLocal[im] = (float*)calloc(dimLoc, sizeof(float));

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


			//for (int i = 0; i < dimLoc; i++)
			//{
			//	printf("\n");

			//	for (int j = 0; j < dimLoc; j++)
			//	{
			//		printf("%f \t", matrixLocal[i][j]);
			//	}
			//}


			int sign = -1;//если false знак '-', если true знак '+'
			if (i % 2 == 0)
			{
				sign = 1;
			}


			auto d = matrix[0][i];


			resFor = sign * matrix[0][i] * determinantTest(dim - 1, dim, matrixLocal);
			res += resFor;
			free(matrixLocal);
		}
	}
	else
	{

		if (dim == 2)
		{
			return res = matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1];
			
			 
		}
		else
		{
			float resLoc = 0.0;
			float** matrixLocal = NULL;
			for (int i = 0; i < dim; i++)
			{
				float resFor = 0.0;
				int dimLoc = dim - 1;
				matrixLocal = (float**)calloc(dimLoc, sizeof(float*));

				for (int im = 0; im < dimLoc; im++)
				{
					matrixLocal[im] = (float*)calloc(dimLoc, sizeof(float));

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


				//for (int i = 0; i < dimLoc; i++)
				//{
				//	printf("\n");

				//	for (int j = 0; j < dimLoc; j++)
				//	{
				//		printf("%f \t", matrixLocal[i][j]);
				//	}
				//}


				int sign = -1;//если false знак '-', если true знак '+'
				if (i % 2 == 0)
				{
					sign = 1;
				}


				//auto d = matrix[0][i];


				resFor = sign * matrix[0][i] * determinantTest(dim - 1, dim, matrixLocal);
				resLoc += resFor;

				free(matrixLocal);
			}
			
			return resLoc;
		}
	}



	return res;
}

