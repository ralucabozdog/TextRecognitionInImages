// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include "math.h"
#include "OpenCVApplication.h"
#include <queue>
#include <random>
#include <stack>
#include <string.h>

using namespace std;


//#define DETAILED_VERSION
//#define SIMPLE_NOISE_FILTERING
//#define SCAN_SETUP

Mat_<uchar> colorToGrayscale(Mat_<Vec3b> img) {
	Mat_<uchar> imGrayscale(img.rows, img.cols);

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			imGrayscale(i, j) = (img(i, j)[0] + img(i, j)[1] + img(i, j)[2]) / 3;
		}
	}
	
	return imGrayscale;
}

Mat_<uchar> grayscaleToBinary(Mat_<uchar> img, int threshold) {
	Mat_<uchar> imgBinary(img.rows, img.cols, (uchar)255);

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			if (img(i, j) < threshold)
				imgBinary(i, j) = 0;
		}
	}

	return imgBinary;
}

boolean isInside(Mat img, int i, int j) {
	//Mat_<uchar> img = imread("Images/flowers_24bits.bmp", 0);

	if (i >= 0 && i < img.rows && j >= 0 && j < img.cols)
		return true;
	return false;
}

int* histogram(Mat_<uchar>img) {
	int* hist = (int*)calloc(256, sizeof(int));

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			*(hist + img(i, j)) += 1;
		}
	}

	return hist;
}

float* fdp(Mat_<uchar> img) {
	float* f = (float*)calloc(256, sizeof(float));

	int* histo = histogram(img);

	for (int k = 0; k < 256; k++)
		*(f + k) = *(histo + k) / (1.0 * img.cols * img.rows);

	return f;
}

structTagImage tagBinaryImageBFS(Mat_<uchar> img, int nbNeigh) {
	structTagImage tagImageStruct;
	Mat_<int> tagImg(img.rows, img.cols, 0);
	tagImageStruct.matrix = tagImg;

	//N, V, S, E, N-V, N-E, S-V, S-E
	int diri[8] = { -1, 0, 1, 0, -1, -1, 1, 1 };
	int dirj[8] = { 0, -1, 0, 1, -1, 1, -1, 1 };

	pair<int, int> p;

	int label = 0;
	for (int j = 0; j < img.cols; j++) {
		for (int i = img.rows - 1; i >= 0; i--) {
			if (img(i, j) == 0 && tagImageStruct.matrix(i, j) == 0) {
				label++;
				queue<pair<int, int>> q;
				tagImageStruct.matrix(i, j) = label;
				q.push({ i, j });
				while (!q.empty()) {
					p = q.front();
					q.pop();
					for (int k = 0; k < nbNeigh; k++) {
						int nghbi = p.first + diri[k];
						int nghbj = p.second + dirj[k];
						if (isInside(tagImageStruct.matrix, nghbi, nghbj) && img(nghbi, nghbj) == 0 && tagImageStruct.matrix(nghbi, nghbj) == 0) {
							tagImageStruct.matrix(nghbi, nghbj) = label;
							q.push({ nghbi, nghbj });
						}
					}
				}
			}
		}
	}

	tagImageStruct.labelNumber = label;

	return tagImageStruct;
}

Mat_<uchar> boundingBox(Mat_<uchar> img) {

	int cmax = -1;
	int cmin = img.cols;
	int rmax = -1;
	int rmin = img.rows;

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			if (img(i, j) == 0) {
				if (i < rmin)
					rmin = i;
				if (i > rmax)
					rmax = i;
				if (j < cmin)
					cmin = j;
				if (j > cmax)
					cmax = j;
			}
		}
	}

	Mat_<uchar> elongationImg(rmax - rmin + 1, cmax - cmin + 1, 255);

	for (int i = rmin; i <= rmax; i++) {
		for (int j = cmin; j <= cmax; j++) {
			elongationImg(i - rmin, j - cmin) = img(i, j);
		}
	}

	return elongationImg;
}

void iterateOverDirectory(void) {
	int val = 33;

	char fileName[100] = "ImagesProjectBMPCompressedBinary/";
	char fileNameOut[100] = "ImagesProjectBMPCompressedBBB/";
	char charValue[4];

	for (int i = 0; i < 94; i++) {
		if (i < 67) {
		sprintf(charValue, "0%d", val + i);
		}
		else {
			sprintf(charValue, "%d", val + i);
		}
		
		strcat(fileName, charValue);
		strcat(fileName, ".bmp");		
		printf("%s\n", fileName);
		Mat_<uchar> templateImage = imread(fileName, 0);

		strcat(fileNameOut, charValue);
		strcat(fileNameOut, ".bmp");
		printf("%s\n", fileNameOut);
		
		imwrite(fileNameOut, boundingBox(templateImage));

		//waitKey(0);

		strcpy(fileName, "ImagesProjectBMPCompressedBinary/");
		strcpy(fileNameOut, "ImagesProjectBMPCompressedBBB/");
		strcpy(charValue, "");
		
	}

	return;
}

/*Make initial grayscale images binary*/
void iterateOverDirectory1(void) {
	int val = 33;

	char fileName[100] = "ImagesProjectBMPCompressed/";
	char fileNameOut[100] = "ImagesProjectBMPCompressedBinary/";
	char charValue[4];

	for (int i = 0; i < 94; i++) {
		if (i < 67) {
			sprintf(charValue, "0%d", val + i);
		}
		else {
			sprintf(charValue, "%d", val + i);
		}

		strcat(fileName, charValue);
		strcat(fileName, ".bmp");
		printf("%s\n", fileName);
		Mat_<uchar> templateImage = imread(fileName, 0);

		strcat(fileNameOut, charValue);
		strcat(fileNameOut, ".bmp");
		printf("%s\n", fileNameOut);

		imwrite(fileNameOut, grayscaleToBinary(templateImage, 128));

		//waitKey(0);

		strcpy(fileName, "ImagesProjectBMPCompressed/");
		strcpy(fileNameOut, "ImagesProjectBMPCompressedBinary/");
		strcpy(charValue, "");

	}

	return;
}

int getMatchPercentage(Mat_<uchar> img, Mat_<uchar> pattern) {

	int matchedPixels = 0;

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			if (isInside(pattern, i, j) && img(i, j) == pattern(i, j)) {
				matchedPixels++;
			}
		}
	}

	return 100 * matchedPixels / (img.rows * img.cols);
}

structMatch getBestMatch(Mat_<uchar> img) {

	structMatch match;
	match.matchChar = '\0';
	match.matchPercentage = 0;
	int val = 33;

	char fileName[300] = "ImagesProjectBMPCompressedBBB/";
	char charValue[4];

	for (int i = 0; i < 94; i++) {
		if (i < 67) {
			sprintf(charValue, "0%d", val + i);
		}
		else {
			sprintf(charValue, "%d", val + i);
		}

		strcat(fileName, charValue);
		strcat(fileName, ".bmp");

		Mat_<uchar> templateImage = imread(fileName, 0);

		resize(templateImage, templateImage, Size(img.cols, img.rows), 0, 0, INTER_LINEAR);
		
		int localPercentage = getMatchPercentage(img, templateImage);

		if (localPercentage > match.matchPercentage) {
			match.matchPercentage = localPercentage;
			match.matchChar = atoi(charValue);
		}

		strcpy(fileName, "ImagesProjectBMPCompressedBBB/");
		strcpy(charValue, "");

	}

	return match;
}

Mat_<Vec3b> tagImageToRGBImage(Mat_<int> img) {

	Mat_<Vec3b> rgbImg(img.rows, img.cols, { 255, 255, 255 });

	default_random_engine gen;
	uniform_int_distribution<int> d(0, 255);

	Vec3b* colorArray = (Vec3b*)calloc(1000, sizeof(Vec3b));

	for (int j = 0; j < img.cols; j++) {
		for (int i = 0; i < img.rows; i++) {
			if (img(i, j) != 0) {
				if ((*(colorArray + img(i, j)))[0] || (*(colorArray + img(i, j)))[1] || (*(colorArray + img(i, j)))[2]) {
					rgbImg(i, j) = *(colorArray + img(i, j));
				}
				else {
					Vec3b randomColor{ (uchar)d(gen), (uchar)d(gen), (uchar)d(gen) };
					*(colorArray + img(i, j)) = randomColor;
					rgbImg(i, j) = randomColor;
				}
			}
		}
	}

	return rgbImg;
}

Mat_<uchar> autoBinary(Mat_<uchar> img, float err) {
	Mat_<uchar> binary(img.rows, img.cols, (uchar)255);

	int* histo = histogram(img);

	int imin = 0;
	int imax = 255;

	for (int i = 0; i < 256; i++) {
		if (histo[i] != 0) {
			imin = i;
			break;
		}
	}

	for (int i = 255; i >= 0; i--) {
		if (histo[i] != 0) {
			imax = i;
			break;
		}
	}

	int th = (imin + imax) / 2;

	int nth = 0;

	while (true) {
		float g1 = 0.0;
		float g2 = 0.0;
		int n1 = 0;
		int n2 = 0;

		for (int i = imin; i <= imax; i++) {
			if (i <= th) {
				g1 += i * histo[i];
				n1 += histo[i];
			}
			else {
				g2 += i * histo[i];
				n2 += histo[i];
			}
		}

		g1 = g1 / (1.0 * n1);
		g2 = g2 / (1.0 * n2);

		nth = (g1 + g2) / 2;

		if (abs(th - nth) <= err)
			break;
		th = nth;
	}

	free(histo);

	binary = grayscaleToBinary(img, nth);

	return binary;
}

structBoundingBox* getBoundingBoxes(structTagImage tag) {
	structBoundingBox* boundingBoxes = (structBoundingBox*)calloc(tag.labelNumber, sizeof(structBoundingBox));

	for (int k = 0; k < tag.labelNumber; k++) {
		boundingBoxes[k].rmin = tag.matrix.rows;
		boundingBoxes[k].cmin = tag.matrix.cols;
	}

	for (int i = 0; i < tag.matrix.rows; i++) {
		for (int j = 0; j < tag.matrix.cols; j++) {
			if (tag.matrix(i, j) != 0) {
				int k = tag.matrix(i, j);
				if (i < boundingBoxes[k - 1].rmin)
					boundingBoxes[k - 1].rmin = i;
				if (i > boundingBoxes[k - 1].rmax)
					boundingBoxes[k - 1].rmax = i;
				if (j < boundingBoxes[k - 1].cmin)
					boundingBoxes[k - 1].cmin = j;
				if (j > boundingBoxes[k - 1].cmax)
					boundingBoxes[k - 1].cmax = j;
			}
		}
	}

	return boundingBoxes;
}

int* occupiedRows(Mat_<uchar> img) {
	int* rows = (int*)calloc(img.rows, sizeof(int));

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			if (img(i, j) == 0) {
				rows[i] = 1;
				break;
			}
		}
	}

	return rows;
}

int* occupiedCols(Mat_<uchar> img, int rmin, int rmax) {
	int* cols = (int*)calloc(img.cols, sizeof(int));

	for (int j = 0; j < img.cols; j++) {
		for (int i = rmin; i <= rmax; i++) {
			if (img(i, j) == 0) {
				cols[j] = 1;
				break;
			}
		}
	}

	return cols;
}

structRow* boundRows(Mat_<uchar> img, int* rowNb) {
	/*A row is defined by at least one black pixel, followed by at least one white pixel.
	 Only the lines containing at least one black pixel are considered character rows,
	 Thus there are at most img.rows / 2  + 1 character lines in the image*/
	structRow* charRows = (structRow*)calloc(img.rows / 2 + 1, sizeof(structRow));

	int* rows = occupiedRows(img);

	int lastRow = rows[0];
	int crtRow = lastRow;
	int k = 0;

	if (lastRow == 1) {
		charRows[0].rmin = 0;
	}

	for (int i = 1; i < img.rows; i++) {
		crtRow = rows[i];

		if (crtRow == 0 && lastRow == 1) {
			charRows[k].rmax = i - 1;
			k++;
		}
		else {
			if (crtRow == 1 && lastRow == 0) {
				charRows[k].rmin = i;
			}
		}

		lastRow = crtRow;
	}

	free(rows);

	*rowNb = k;

	return charRows;
}

void getHeightLimits(Mat_<uchar> img, int* rmin, int* rmax) {
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			if (img(i, j) == 0) {
				*rmin = i;
				goto here;
			}
		}
	}

here:

	for (int i = img.rows - 1; i >= 0; i--) {
		for (int j = 0; j < img.cols; j++) {
			if (img(i, j) == 0) {
				*rmax = i;
				return;
			}
		}
	}
}

void isolateForMatching(structBoundingBox* boundingBoxes, int nbLabels, Mat_<uchar> img, FILE* out_file) {

	int flags[100] = { 0 };
	int middles[100] = { 0 };

	for (int k = 0; k < nbLabels; k++) {
		middles[k] = boundingBoxes[k].cmin + (boundingBoxes[k].cmax - boundingBoxes[k].cmin) / 2;
	}

	int superpositionNb = 0;
	for (int k = 0; k < nbLabels; k++) {
		if (flags[k] == 0) {
			for (int i = 0; i < nbLabels; i++) {
				if (i != k && flags[i] == 0 && middles[k] < boundingBoxes[i].cmax && middles[k] > boundingBoxes[i].cmin) {
					superpositionNb++;
					flags[k] = superpositionNb;
					flags[i] = superpositionNb;
					break;
				}
			}
		}
	}

	structBoundingBox* mergedBoundingBoxes = (structBoundingBox*)calloc(superpositionNb, sizeof(structBoundingBox));

	int index = 0;
	for (int k = 0; k < nbLabels; k++) {
		if (flags[k] != 0) {
			for (int i = 0; i < nbLabels; i++) {
				if (i != k && flags[i] == flags[k]) {
					mergedBoundingBoxes[index].rmin = (boundingBoxes[i].rmin < boundingBoxes[k].rmin) ? boundingBoxes[i].rmin : boundingBoxes[k].rmin;
					mergedBoundingBoxes[index].rmax = (boundingBoxes[i].rmax > boundingBoxes[k].rmax) ? boundingBoxes[i].rmax : boundingBoxes[k].rmax;
					mergedBoundingBoxes[index].cmin = (boundingBoxes[i].cmin < boundingBoxes[k].cmin) ? boundingBoxes[i].cmin : boundingBoxes[k].cmin;
					mergedBoundingBoxes[index].cmax = (boundingBoxes[i].cmax > boundingBoxes[k].cmax) ? boundingBoxes[i].cmax : boundingBoxes[k].cmax;
					index++;
				}
			}
		}
	}
	
	index = 0;
	for (int k = 0; k < nbLabels; k++) {

		if (flags[k] == 0) {
			Mat_<uchar> charImg(boundingBoxes[k].rmax - boundingBoxes[k].rmin + 1, boundingBoxes[k].cmax - boundingBoxes[k].cmin + 1, 255);
			for (int i = boundingBoxes[k].rmin; i <= boundingBoxes[k].rmax; i++) {
				for (int j = boundingBoxes[k].cmin; j <= boundingBoxes[k].cmax; j++) {
					charImg(i - boundingBoxes[k].rmin, j - boundingBoxes[k].cmin) = img(i, j);
				}
			}

			structMatch match = getBestMatch(charImg);

#ifdef DETAILED_VERSION 
			printf("\nBest match: %c with percentage %d\n", match.matchChar, match.matchPercentage);
#ifndef SCAN_SETUP
			fprintf(out_file, "\nBest match: %c with percentage %d\n", match.matchChar, match.matchPercentage);
#endif
#else
			printf("%c", match.matchChar);
#ifndef SCAN_SETUP
			fprintf(out_file, "%c", match.matchChar);
#endif
#endif
		}
		else {
			if (flags[k] > 0) {
				Mat_<uchar> charImg(mergedBoundingBoxes[index].rmax - mergedBoundingBoxes[index].rmin + 1, mergedBoundingBoxes[index].cmax - mergedBoundingBoxes[index].cmin + 1, 255);
				for (int i = mergedBoundingBoxes[index].rmin; i <= mergedBoundingBoxes[index].rmax; i++) {
					for (int j = mergedBoundingBoxes[index].cmin; j <= mergedBoundingBoxes[index].cmax; j++) {
						charImg(i - mergedBoundingBoxes[index].rmin, j - mergedBoundingBoxes[index].cmin) = img(i, j);
					}
				}

				index++;

				structMatch match = getBestMatch(charImg);
				
#ifdef DETAILED_VERSION 
				printf("\nBest match: %c with percentage %d\n", match.matchChar, match.matchPercentage);
#ifndef SCAN_SETUP
				fprintf(out_file, "\nBest match: %c with percentage %d\n", match.matchChar, match.matchPercentage);
#endif
#else
				printf("%c", match.matchChar);
#ifndef SCAN_SETUP
				fprintf(out_file, "%c", match.matchChar);
#endif
#endif


				for (int x = 0; x < nbLabels; x++) {
					if (x != k && flags[x] == flags[k]) {
						flags[x] = -1;
					}
				}

				flags[k] = -1;
			}
		}
	}

	free(mergedBoundingBoxes);
}

void showRows(Mat_<uchar> img, FILE* out_file) {

	int rowNb;

	structRow* charRows = boundRows(img, &rowNb);

	for (int k = 0; k < rowNb; k++) {
		int* cols = occupiedCols(img, charRows[k].rmin, charRows[k].rmax);

		structCol* charCols = (structCol*)calloc(img.cols / 2 + 1, sizeof(structCol));

		/*HERE*/
		printf("\n");
#ifndef SCAN_SETUP
		fprintf(out_file, "\n");
#endif
		int lastCol = cols[0];
		int crtCol = lastCol;
		int p = 0;

		if (lastCol == 1) {
			charCols[0].cmin = 0;
		}

		for (int j = 1; j < img.cols; j++) {
			crtCol = cols[j];

			if (crtCol == 0 && lastCol == 1) {
				charCols[p].cmax = j - 1;
				p++;
			}
			else {
				if (crtCol == 1 && lastCol == 0) {
					charCols[p].cmin = j;
				}
			}

			lastCol = crtCol;
		}

		for (int h = 0; h < p; h++) {
			if (h > 0) {
				if (charCols[h].cmin - charCols[h - 1].cmax > (charRows[k].rmax - charRows[k].rmin) / 6) {
					/*HERE*/
					printf(" ");
#ifndef SCAN_SETUP
					fprintf(out_file, " ");
#endif
				}
			}

			Mat_<uchar> partImg(charRows[k].rmax - charRows[k].rmin + 1, charCols[h].cmax - charCols[h].cmin + 1, 255);

			for (int i = 0; i < charRows[k].rmax - charRows[k].rmin + 1; i++) {
				for (int j = 0; j < charCols[h].cmax - charCols[h].cmin + 1; j++) {
					partImg(i, j) = img(charRows[k].rmin + i, charCols[h].cmin + j);
				}
			}

			int rmin, rmax;
			getHeightLimits(partImg, &rmin, &rmax);

			Mat_<uchar> boundImg(rmax - rmin + 1, charCols[h].cmax - charCols[h].cmin + 1, 255);
			for (int i = 0; i < rmax - rmin + 1; i++) {
				for (int j = 0; j < charCols[h].cmax - charCols[h].cmin + 1; j++) {
					boundImg(i, j) = partImg(rmin + i, j);
				}
			}

			//imshow("image", boundImg);
			//waitKey(0);

#ifdef SIMPLE_NOISE_FILTERING
			if (boundImg.rows * boundImg.cols > 4) {
#endif
				structMatch match = getBestMatch(boundImg);

				if (match.matchPercentage < 80) {

					structTagImage tagImage = tagBinaryImageBFS(boundImg, 8);

#ifdef DETAILED_VERSION
					imshow("Glued together", tagImageToRGBImage(tagImage.matrix));
					waitKey(0);
#endif
					structBoundingBox* boundingBoxes = getBoundingBoxes(tagImage);

					isolateForMatching(boundingBoxes, tagImage.labelNumber, boundImg, out_file);
				}
				else {

#ifdef DETAILED_VERSION
					printf("\nBest match: %c with percentage %d\n", match.matchChar, match.matchPercentage);
#ifndef SCAN_SETUP
					fprintf(out_file, "\nBest match: %c with percentage %d\n", match.matchChar, match.matchPercentage);
#endif
#else
					printf("%c", match.matchChar);
#ifndef SCAN_SETUP
					fprintf(out_file, "%c", match.matchChar);
#endif
#endif
				}
#ifdef SIMPLE_NOISE_FILTERING
			}
#endif
		}

		free(charCols);
	}

	free(charRows);
}

int main()
{
	char img_name[20];
	char img_access[100] = "Images/";

	int iteration = 1;
	char img_out_access[100] = "text_from_image_";
	char string_number[10];

	Mat_<uchar> img;
	Mat_<uchar> img2;
	Mat_<Vec3b> img1;

#ifndef SCAN_SETUP	
	while (1) {

		sprintf(string_number, "%d", iteration);
		strcat(img_out_access, string_number);
		strcat(img_out_access, ".txt");

		FILE* out_file = fopen(img_out_access, "w");

		destroyAllWindows();

		printf("\n\nEnter image name and extension to start looking for text\n\n>>Image:	");
		scanf("%s", &img_name);

		strcat(img_access, img_name);

		img1 = imread(img_access, 1);

		if (img1.rows != 0) {
			imshow("Original image", img1);
			printf("\nClose image to continue\n");
			waitKey(0);
			printf("\n\nFound text:\n");

			img = colorToGrayscale(img1);
			img2 = autoBinary(img, 0.1);

			showRows(img2, out_file);

			printf("\n\n\nYou can find the text in %s\n\n\n", img_out_access);
			fclose(out_file);
			strcpy(img_access, "Images/");
			strcpy(img_out_access, "text_from_image_");
			iteration++;
		}
		else {
			printf("\nImage not found!\n");
			strcpy(img_access, "Images/");
		}
	}

#else
	img1 = imread("Images/0scan.png", 1);
	img = colorToGrayscale(img1);
	img2 = autoBinary(img, 0.1);

	showRows(img2, NULL);
#endif
	return 0;
}