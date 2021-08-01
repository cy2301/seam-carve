#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"

using namespace std;

Pixel** createImage(int width, int height) {
  cout << "Start createImage... " << endl;
  
  // Create a one dimensional array on the heap of pointers to Pixels 
  //    that has width elements (i.e. the number of columns)
  Pixel** image = new Pixel*[width];
  
  bool fail = false;
  
  for (int i=0; i < width; ++i) { // loop through each column
    // assign that column to a one dimensional array on the heap of Pixels
    //  that has height elements (i.e. the number of rows)
    image[i] = new Pixel[height];
    
    if (image[i] == nullptr) { // failed to allocate
      fail = true;
    }
  }
  
  if (fail) { // if any allocation fails, clean up and avoid memory leak
    // deallocate any arrays created in for loop
    for (int i=0; i < width; ++i) {
      delete [] image[i]; // deleting nullptr is not a problem
    }
    delete [] image; // dlete array of pointers
    return nullptr;
  }
  
  // initialize cells
  //cout << "Initializing cells..." << endl;
  for (int row=0; row<height; ++row) {
    for (int col=0; col<width; ++col) {
      //cout << "(" << col << ", " << row << ")" << endl;
      image[col][row] = { 0, 0, 0 };
    }
  }
  cout << "End createImage... " << endl;
  return image;
}

void deleteImage(Pixel** image, int width) {
  cout << "Start deleteImage..." << endl;
  // avoid memory leak by deleting the array
  for (int i=0; i<width; ++i) {
    delete [] image[i]; // delete each individual array placed on the heap
  }
  delete [] image;
  image = nullptr;
}

int* createSeam(int length) {
  int* A = new int[length]{0};
  return A;
}

void deleteSeam(int* seam) {
  delete[] seam;
}

bool loadImage(string filename, Pixel** image, int width, int height) {
	ifstream ifs(filename);
	
	if (!ifs.is_open())
	{
		cout << "Error: failed to open input file - " << filename << endl; 
		return false;
	}
	
	char type[3];
	ifs >> type;
	if ((toupper(type[0]) != 'P') || (type[1] != '3')) { // check that type is correct
		cout << "Error: type is " << type << " instead of P3" << endl;
		return false;
	}
	
	int w = 0;
	int h = 0;
	ifs >> w;
	if (!ifs) {
		cout << "Error: read non-integer value" << endl;
		return false;
	}
	else if (w != width)
	{
		cout << "Error: input width (" << width << ") does not match value in file ("<< w << ")" << endl;
		return false;
	}
	ifs >> h;
	if (!ifs) {
		cout << "Error: read non-integer value" << endl;
		return false;
	}
	else if (h != height)
	{
		cout << "Error: input height (" << height << ") does not match value in file ("<< h << ")" << endl;
		return false;
	}
	
	int colorMax = 0;
	ifs >> colorMax;
	if (!ifs) 
	{
		cout << "Error: read non-integer value" << endl;
		return false;
	}
	else if (colorMax != 255)
	{
		cout << "Error: invalid color value " << colorMax << endl;
		return false;
	}
	
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			ifs >> image[j][i].r;
			if (!ifs)  // if the code is put in a fail state it's because of either eof and it keeps trying to read or a non-integer
			{
				if (ifs.eof())  // if it hits the fail state early this runs
				{
					cout << "Error: not enough color values" << endl;
					return false;
				}
				cout << "Error: read non-integer value" << endl;
				return false;
			}
			if (image[j][i].r > 255 || image[j][i].r < 0) // checks if the color value being read is within acceptable range
			{
				cout << "Error: invalid color value " << image[j][i].r << endl;
				return false;
			}
			ifs >> image[j][i].g;
			if (!ifs)
			{
				if (ifs.eof())
				{
					cout << "Error: not enough color values" << endl;
					return false;
				}
				cout << "Error: read non-integer value" << endl;
				return false;
			}
			if (image[j][i].g > 255 || image[j][i].g < 0)
			{
				cout << "Error: invalid color value " << image[j][i].g << endl;
				return false;
			}
			ifs >> image[j][i].b;
			if (!ifs)
			{
				if (ifs.eof())
				{
					cout << "Error: not enough color values" << endl;
					return false;
				}
				cout << "Error: read non-integer value" << endl;
				return false;
			}
			if (image[j][i].b > 255 || image[j][i].b < 0)
			{
				cout << "Error: invalid color value " << image[j][i].b << endl;
				return false;
			}
		}
	}
	
	int test = 0;  // variable to see if we got extra pixels left
	
	while (!ifs.eof())  // if the file is still not at the eof and there's values to be put in test
	{
		if (ifs >> test) 
		{
			cout << "Error: too many color values" << endl;
			return false;
		}
	}
	
	return true;
}

bool outputImage(string filename, Pixel** image, int width, int height) {
  cout << "Outputting image..." << endl;
  ofstream ofs;
  
  ofs.open(filename);
  if (!ofs.is_open())
  {
	  cout << "Error: failed to open output file " << filename << endl;
	  return false;
  }
  ofs << "P3" << endl;
  ofs << width << " " << height << endl;
  ofs << 255 << endl;
  for (int i = 0; i < height; i++)
  {
	  for (int j = 0; j < width; j++)
	  {
		  ofs << image[j][i].r << endl << image[j][i].g  << endl << image[j][i].b << endl;
	  }
  }
  return true;
}

int energy(Pixel** image, int column, int row, int width, int height) {
  // border pixels
	Pixel left;
	Pixel right;
	Pixel top;
	Pixel bottom;
	
	int energy = 0;
	
	// j = column
	// i = row
	if (column == 0)  // left column
	{
		right = image[column + 1][row];
		left = image[width - 1][row];
		if (row == 0)  // top left
		{
			bottom = image[0][row + 1];
			top = image[0][height - 1];
		}
		else if (row == (height - 1)) // bottom left
		{
			bottom = image[column][row - 1];
			top = image[0][0];
		}
		else  // anything along the left column not a corner
		{
			bottom = image[column][row - 1];
			top = image[column][row + 1];
		}
	}
  
	else if (column == (width - 1)) // right column
	{
		right = image[column - 1][row];
		left = image[0][row];
		if (row == 0) // top right
		{
			bottom = image[column][1];
			top = image[column][height - 1];
		}
		else if (row == (height - 1)) // bottom right
		{
			bottom = image[column][row - 1];
			top = image[column][0];
		}
		else  // anything along the right column not a corner
		{
			bottom = image[column][row - 1];
			top = image[column][row + 1];
		}
	}
	
	else // everything not on the left and right column
	{
		right = image[column + 1][row];
		left = image[column - 1][row];
		if (row == 0) // top row
		{
			bottom = image[column][row + 1];
			top = image[column][height - 1];
		}
		else if (row == (height - 1)) // bottom row
		{
			bottom = image[column][row - 1];
			top = image[column][0];
		}
		else // everything else
		{
			bottom = image[column][row - 1];
			top = image[column][row + 1];
		}
	}
	
	int xGrad = pow(right.r - left.r, 2) + pow(right.g - left.g, 2) + pow(right.b - left.b, 2);
	int yGrad = pow(top.r - bottom.r, 2) + pow(top.g - bottom.g, 2) + pow(top.b - bottom.b, 2);
	
	energy = xGrad + yGrad;
	
	return energy;
}

int loadVerticalSeam(Pixel** image, int start_col, int width, int height, int* seam) {
  seam[0] = start_col;
  int totalEnergy = energy(image, start_col, 0, width, height); // right forward left -- why
  int target_col = start_col;
  int energyLeft, energyForward, energyRight = 0;
  for (int i = 1; i < height; i++) // uses height because we're putting height number of elements into the array
  {
	  energyForward = energy(image, target_col, i, width, height);
	  if (target_col == 0)
	  {
		  energyLeft = energy(image, target_col + 1, i, width, height);
		  if (energyForward > energyLeft)
		  {
			  totalEnergy += energyLeft;
			  target_col = target_col + 1;
			  seam[i] = target_col;
		  }
		  else if (energyLeft > energyForward || energyLeft == energyForward)
		  {
			  totalEnergy += energyForward;
			  seam[i] = target_col;
		  }
	  }
	  else if (target_col == width - 1)
	  {
		  energyRight = energy(image, target_col - 1, i, width, height);
		  if (energyRight > energyForward || energyRight == energyForward)
		  {
			  totalEnergy += energyForward;
			  seam[i] = target_col;
		  }
		  else if (energyRight < energyForward)
		  {
			  totalEnergy += energyRight;
			  target_col = target_col - 1;
			  seam[i] = target_col;
		  }
	  }
	  else
	  {
		  energyRight = energy(image, target_col - 1, i, width, height);
		  energyLeft = energy(image, target_col + 1, i, width, height);
		  if ((energyForward < energyLeft && energyForward < energyRight)|| (energyForward == energyLeft && energyForward < energyRight) || 
		  (energyForward == energyRight && energyForward < energyLeft) || (energyForward == energyLeft && energyForward == energyRight))
		  {
			  totalEnergy += energyForward;
			  seam[i] = target_col;
		  }
		  else if ((energyLeft < energyForward && energyLeft < energyRight) || (energyLeft == energyRight && energyLeft < energyForward))
		  {
			  totalEnergy += energyLeft;
			  target_col = target_col + 1;
			  seam[i] = target_col;
		  }
		  else
		  {
			  totalEnergy += energyRight;
			  target_col = target_col - 1;
			  seam[i] = target_col;
		  }
	  }
  }
  return totalEnergy;
}

int loadHorizontalSeam(Pixel** image, int start_row, int width, int height, int* seam) {
  seam[0] = start_row;
  int totalEnergy = energy(image, 0, start_row, width, height);
  int target_row = start_row;
  int energyLeft, energyForward, energyRight = 0;
  for (int i = 1; i < width; i++)
  {
	  energyForward = energy(image, i, target_row, width, height);
	  if (target_row == 0)
	  {
		  energyRight = energy(image, i, target_row + 1, width, height);
		  if (energyForward > energyRight)
		  {
			  totalEnergy += energyRight;
			  target_row = target_row + 1;
			  seam[i] = target_row;
		  }
		  else if (energyRight > energyForward || energyRight == energyForward)
		  {
			  totalEnergy += energyForward;
			  seam[i] = target_row;
		  }
	  }
	  else if (target_row == height - 1)
	  {
		  energyLeft = energy(image, i, target_row - 1, width, height);
		  if (energyLeft > energyForward || energyLeft == energyForward)
		  {
			  totalEnergy += energyForward;
			  seam[i] = target_row;
		  }
		  else if (energyLeft < energyForward)
		  {
			  totalEnergy += energyLeft;
			  target_row = target_row - 1;
			  seam[i] = target_row;
		  }
	  }
	  else
	  {
		  energyLeft = energy(image, i, target_row - 1, width, height);
		  energyRight = energy(image, i, target_row + 1, width, height);
		  if ((energyForward < energyRight && energyForward < energyLeft)|| (energyForward == energyRight && energyForward < energyLeft) || 
		  (energyForward == energyLeft && energyForward < energyRight) || (energyForward == energyRight && energyForward == energyLeft))
		  {
			  totalEnergy += energyForward;
			  seam[i] = target_row;
		  }
		  else if ((energyLeft < energyForward && energyLeft < energyRight) || (energyRight == energyLeft && energyLeft < energyForward))
		  {
			  totalEnergy += energyLeft;
			  target_row = target_row - 1;
			  seam[i] = target_row;
		  }
		  else
		  {
			  totalEnergy += energyRight;
			  target_row = target_row + 1;
			  seam[i] = target_row;
		  }
	  }
  }
  return totalEnergy;
}

int* findMinVerticalSeam(Pixel** image, int width, int height) {
  int* minSeam = createSeam(height);
  int minEnergy = loadVerticalSeam(image, 0, width, height, minSeam);
  int currentEnergy = 0;
  for (int j = 1; j < width; j++)
  {
	  int* currentSeam = createSeam(height);
	  currentEnergy = loadVerticalSeam(image, j, width, height, currentSeam);
	  if (currentEnergy < minEnergy)
	  {
		  minEnergy = currentEnergy;
		  deleteSeam(minSeam);
		  minSeam = currentSeam;
	  }
	  else
	  {
		  deleteSeam(currentSeam);
	  }
  }
  return minSeam;
}
  
  /*
  int minEnergy = INT32_MAX;
  int* A = createSeam(height);
  int* minSeam = createSeam(height);
  int currentEnergy = 0;
  for (int j = 0; j < width; j++) 
  {
	  currentEnergy = loadVerticalSeam(image, j, width, height, A);  
	  // reminder that A is a pointer that's updated each time
	  if (currentEnergy < minEnergy)
	  {
		  minEnergy = currentEnergy;
		  for (int i = 0; i < height; i++)  // copies contents of array A into minSeam
		  {
			  minSeam[i] = A[i];
		  }
	  }
  }
  delete[] A;
  return minSeam;
  */

int* findMinHorizontalSeam(Pixel** image, int width, int height) {
	int* minSeam = createSeam(width);
	int minEnergy = loadHorizontalSeam(image, 0, width, height, minSeam);
	int currentEnergy = 0;
	for (int i = 1; i < height; i++)
	{
	  int* currentSeam = createSeam(width);
	  currentEnergy = loadHorizontalSeam(image, i, width, height, currentSeam);
	  if (currentEnergy < minEnergy)
	  {
		  minEnergy = currentEnergy;
		  deleteSeam(minSeam);
		  minSeam = currentSeam;
	  }
	  else
	  {
		  deleteSeam(currentSeam);
	  }
	}
	return minSeam;
	/*
  int minEnergy = INT32_MAX;
  int* A = createSeam(width);
  int* minSeam = createSeam(width);
  int currentEnergy = 0;
  for (int i = 0; i < height; i++) 
  {
	  currentEnergy = loadHorizontalSeam(image, i, width, height, A);  
	  // reminder that A is a pointer that's updated each time
	  if (currentEnergy < minEnergy)
	  {
		  minEnergy = currentEnergy;
		  for (int j = 0; j < width; j++)
		  {
			  minSeam[j] = A[j];
		  }
	  }
  }
  delete[] A;
  return minSeam;
  */
}

void removeVerticalSeam(Pixel** image, int width, int height, int* verticalSeam) {
  for (int i = 0; i < height; i++)
  {
	  for (int j = verticalSeam[i]; j < width - 1; j++)
	  {
		  image[j][i] = image[j+1][i];
	  }
  }
}

void removeHorizontalSeam(Pixel** image, int width, int height, int* horizontalSeam) {
  for (int j = 0; j < width; j++)
  {
	  for (int i = horizontalSeam[j]; i < height - 1; i++)
	  {
		  image[j][i] = image[j][i+1];
	  }
  }
}
