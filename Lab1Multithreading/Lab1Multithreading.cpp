#include <iostream>
#include <fstream>
#include <string>
#include <omp.h>
#include "conio.h"
using namespace std;

float determinant(int dim, float** matrix);
float determinantTest(int dim, int dimStart, float** matrix);

float determinantNew(int i, int j, bool* excludeColumns, int dim, int dimStart, float** matrix);

float determinantMultithread(int dim, int dimStart, float** matrix);
float determinantMultithreadTest(int dim, int dimStart, float** matrix);

long double LUDecomposition(float** matrix, float** lu, int n);
long double LUDecompositionMultithread(float** matrix, float** lu, int n);
float LUDecompositionMultithreadMod(float** matrix, float** lu, int n);
float LUDecompositionMod(float** matrix, float** lu, int n);
float test(void);

int main(int argc, char** argv)
{
	char* bufIterator;
	char* buf;
	float** table;
	int NUM_OF_THREADS = 0;

	/*for (size_t i = 0; i < argc; i++)
	{
		printf("%s", argv[i]);
	}*/

	float** lu;
	int rowsCount = 0;
	try
	{
		string sNUM_OF_THREADS = argv[2];
		NUM_OF_THREADS = atoi(argv[2]);
		string file = argv[1];
 		ifstream in(/*"matrix.txt"*/argv[1], ios::binary);//matrix.txt
		int size = in.seekg(0, ios::end).tellg();
		if (size == -1)
			throw "File is empty";
		in.seekg(0);
		buf = new char[size + 1];
		in.read(buf, size);
		buf[size] = 0;
		bufIterator = buf;
		in.close();
		string tempString = "";



		try
		{
			while (rowsCount == 0)
			{
				if (*bufIterator != ' ' && (int)*bufIterator != 13 && (int)*bufIterator != 10 && *bufIterator != '\0')
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
		}
		catch (...)
		{
			cout << "File has incorrect data" << endl;
			return 1;
		}
		tempString = "";

		// выделяем память для двухмерного массива
		table = (float**)calloc(rowsCount, sizeof(float*));
		lu = (float**)calloc(rowsCount, sizeof(float*));


		for (int i = 0; i < rowsCount; i++)
		{
			table[i] = (float*)calloc(rowsCount, sizeof(float));//table[i] - это сам указатель на будущий массив под элементы
			lu[i] = (float*)calloc(rowsCount, sizeof(float));

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
					if (tempString == "")
					{
						throw "Wrong number exception";
					}
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



	}
	catch (const char* msg)
	{
		std::cout << msg << std::endl;
		return 1;
	}

	float start_time, end_time, tick;

	if(NUM_OF_THREADS == -1)
	{
		start_time = omp_get_wtime();
		long double resSingle = LUDecomposition(table, lu, rowsCount);
		end_time = omp_get_wtime();
		auto timeSingle = end_time - start_time;
		printf("Determinant: %g\n", resSingle);
		printf("\nTime (%i thread(s)): %f ms\n", NUM_OF_THREADS, timeSingle);
		
	}else if(NUM_OF_THREADS == 0)
	{
		start_time = omp_get_wtime();
		long double resMulti = LUDecompositionMultithread(table, lu, rowsCount);
		end_time = omp_get_wtime();
		auto timeMulti = end_time - start_time;
		printf("Determinant: %g\n", resMulti);
		printf("\nTime (%i thread(s)): %f ms\n", NUM_OF_THREADS, timeMulti);
	}else if(NUM_OF_THREADS >0)
	{
		omp_set_num_threads(NUM_OF_THREADS);
		start_time = omp_get_wtime();
		long double resMulti = LUDecompositionMultithread(table, lu, rowsCount);
		end_time = omp_get_wtime();
		auto timeMulti = end_time - start_time;
		printf("Determinant: %g\n", resMulti);
		printf("\nTime (%i thread(s)): %f ms\n", NUM_OF_THREADS, timeMulti);
	}
	
	



	_getch(); 

	for (int i = 0; i < rowsCount; i++)
	{
		free(table[i]);
		free(lu[i]);
	}
	free(table);
	free(lu);
	free(buf);
	return 0;
}


float test(void)
{
	int a = 0;
#pragma omp parallel private(a)
	{
		a = 0;
		a += 1;
	}
	return a;
}

long double LUDecompositionMultithread(float** matrix, float** lu, int n)
{
	float sum;
	for (int i = 0; i < n; i++)
	{

#pragma omp parallel private(sum)
		{

			//int num = omp_get_num_threads();
			int j = 0;
#pragma omp for schedule(static)
			for (j = i; j < n; j++)
			{
				sum = 0;
				for (int k = 0; k < i; k++)
					sum += lu[i][k] * lu[k][j];
				lu[i][j] = matrix[i][j] - sum;
			}
#pragma omp for schedule(static)
			for (j = i + 1; j < n; j++)
			{
				sum = 0;
				for (int k = 0; k < i; k++)
					sum += lu[j][k] * lu[k][i];
				lu[j][i] = (1 / lu[i][i]) * (matrix[j][i] - sum);
			}
		}
	}

	long double res = 1.0;

	for (int i = 0; i < n; i++)
	{
		res *= lu[i][i];
	}
	return res;
}


float LUDecompositionMultithreadMod(float** matrix, float** lu, int n)
{
	float sum;
	for (int i = 0; i < n; i++)
	{
#pragma omp parallel private(sum)
		{
			int j = 0;
#pragma omp for
			for (j = i; j < n; j++)
			{
				sum = 0;
				for (int k = 0; k < i; k++)
					sum += lu[i][k] * lu[k][j];
				lu[i][j] = matrix[i][j] - sum;
				if (j + 1 != n)
				{
					int locJ = j + 1;
					sum = 0;
					for (int k = 0; k < i; k++)
						sum += lu[locJ][k] * lu[k][i];
					lu[locJ][i] = (1 / lu[i][i]) * (matrix[locJ][i] - sum);
				}
			}
		}
	}

	auto res = 1.0;

	for (int i = 0; i < n; i++)
	{
		res *= lu[i][i];
	}
	return res;
}

long double LUDecomposition(float** matrix, float** lu, int n)
{
	float sum = 0;
	for (int i = 0; i < n; i++)
	{
		for (int j = i; j < n; j++)
		{
			sum = 0;
			for (int k = 0; k < i; k++)
				sum += lu[i][k] * lu[k][j];
			lu[i][j] = matrix[i][j] - sum;
		}
		for (int j = i + 1; j < n; j++)
		{
			sum = 0;
			for (int k = 0; k < i; k++)
				sum += lu[j][k] * lu[k][i];
			lu[j][i] = (1 / lu[i][i]) * (matrix[j][i] - sum);
		}
	}

	long double res = 1.0;

	for (int i = 0; i < n; i++)
	{
		res *= lu[i][i];
	}
	return res;
}

float LUDecompositionMod(float** matrix, float** lu, int n)
{
	float sum = 0;
	int locJ;
	for (int i = 0; i < n; i++)
	{
		for (int j = i; j < n; j++)
		{
			sum = 0;
			for (int k = 0; k < i; k++)
				sum += lu[i][k] * lu[k][j];
			lu[i][j] = matrix[i][j] - sum;
			if (j + 1 != n)
			{
				locJ = j + 1;
				sum = 0;
				for (int k = 0; k < i; k++)
					sum += lu[locJ][k] * lu[k][i];
				lu[locJ][i] = (1 / lu[i][i]) * (matrix[locJ][i] - sum);
			}

		}
	}

	auto res = 1.0;

	for (int i = 0; i < n; i++)
	{
		res *= lu[i][i];
	}
	return res;
}


float determinantNew(int i, int j, bool* excludeColumns, int dim, int dimStart, float** matrix)
{
	float res = 0.0;

	if (dim == dimStart)
	{
		for (int dimCur = 0; dimCur < dimStart; dimCur++)
		{
			float resFor = 0.0;
			bool* excludeColumnsLoc = (bool*)calloc(dimStart, sizeof(bool));
			/*for (int temp = 0; temp<dimStart; temp++)
			{
				excludeColumnsLoc[temp] = false;
			}*/

			if (dimCur == 0)
			{
				j = 1;//возможно тоже придется пихать в лок переменную при многопоточке, даже не возможно, а точно придется
			}
			else
			{
				j = 0;//возможно тоже придется пихать в лок переменную при многопоточке, даже не возможно, а точно придется
			}

			excludeColumnsLoc[dimCur] = true;


			int sign = -1;//если false знак '-', если true знак '+'
			if (dimCur % 2 == 0)
			{
				sign = 1;
			}


			resFor = sign * matrix[0][dimCur] * determinantNew(i + 1, j, excludeColumnsLoc, dimStart - 1, dimStart, matrix);
			res += resFor;

			free(excludeColumnsLoc);

		}
	}
	else
	{

		if (dim == 2)
		{
			int secondColumn = 0;
			bool flag = false;
			int tempJ = j;
			while (true)
			{
				if (excludeColumns[tempJ] == false && flag == false)
				{
					flag = true;
				}
				else if (excludeColumns[tempJ] == false && flag == true)
				{
					break;
				}
				tempJ++;
			}



			return res = matrix[i][j] * matrix[i + 1][tempJ] - matrix[i + 1][j] * matrix[i][tempJ];


		}
		else
		{
			float resLoc = 0.0;
			int shiftDimCur = j;
			for (int dimCur = 0; dimCur < dim; dimCur++)
			{


				float resFor = 0.0;
				//bool* excludeColumnsLoc = excludeColumns; //(bool*)calloc(dimStart, sizeof(bool*));//
				//memcpy(excludeColumnsLoc, excludeColumns, sizeof(excludeColumns));
				int jLoc = j;


				while (true)
				{

					if (excludeColumns[dimCur + shiftDimCur] == true)
					{
						shiftDimCur++;
					}
					else
					{
						excludeColumns[dimCur + shiftDimCur] = true;
						break;
					}

				}
				//excludeColumns[dimCur + j] = true;

				if (dimCur == 0)
				{
					while (true)
					{

						if (excludeColumns[1 + jLoc] == true)
						{
							jLoc += 1;
						}
						else
						{
							jLoc = 1 + jLoc;
							break;
						}

					}

				}
				else
				{
					while (true)
					{

						if (excludeColumns[0 + jLoc] == true)
						{
							jLoc += 1;
						}
						else
						{
							//jLoc = 1 + jLoc;
							break;
						}

					}
					//jLoc = 0 + j;
				}



				int sign = -1;//если false знак '-', если true знак '+'
				if (dimCur % 2 == 0)
				{
					sign = 1;
				}


				//auto d = matrix[0][i];


				//resFor = sign * matrix[i][j+dimCur] * determinantNew(i+1, jLoc, excludeColumns, dim - 1, dimStart, matrix);
				resFor = sign * matrix[i][shiftDimCur + dimCur] * determinantNew(i + 1, jLoc, excludeColumns, dim - 1, dimStart, matrix);
				resLoc += resFor;

				//excludeColumns[dimCur + j] = false;
				excludeColumns[dimCur + shiftDimCur] = false;

				//free(excludeColumnsLoc);
			}

			return resLoc;
		}
	}



	return res;
}

float determinantMultithreadTest(int dim, int dimStart, float** matrix)
{

	float res = 0.0;

	if (dim == dimStart)
	{

#pragma omp parallel
		{
			printf("%d/%d\n", omp_get_num_threads(), omp_get_thread_num());


			int i = 0;

#pragma omp for 
			for (i = 0; i < dim; i++)
			{
				printf("%d/%d/%d\n", i, omp_get_num_threads(), omp_get_thread_num());

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



				int sign = -1;//если false знак '-', если true знак '+'
				if (i % 2 == 0)
				{
					sign = 1;
				}


				auto d = matrix[0][i];


				resFor = sign * matrix[0][i] * determinantMultithreadTest(dim - 1, dim, matrixLocal);

#pragma omp atomic
				res += resFor;
			}
		}
	}
	else
	{
		printf("Else %d/%d\n", omp_get_num_threads(), omp_get_thread_num());

		if (dim == 2)
		{
			return res = matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1];
		}
		else
		{
			float resLoc = 0.0;

			for (int i = 0; i < dim; i++)
			{
				printf("%d/%d/%d\n", i, omp_get_num_threads(), omp_get_thread_num());

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


				int sign = -1;//если false знак '-', если true знак '+'
				if (i % 2 == 0)
				{
					sign = 1;
				}


				auto d = matrix[0][i];


				resFor = sign * matrix[0][i] * determinantMultithreadTest(dim - 1, dim, matrixLocal);
				resLoc += resFor;
			}
			return resLoc;
		}
	}



	return res;
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
			for (int im = 0; im < dimLoc; im++)
			{
				free(matrixLocal[im]);
			}
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

				for (int im = 0; im < dimLoc; im++)
				{
					free(matrixLocal[im]);
				}
				free(matrixLocal);
			}

			return resLoc;
		}
	}



	return res;
}

