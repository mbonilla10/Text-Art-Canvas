#include <iostream>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;


Point::Point(DrawPoint p)
{
	row = (int)round(p.row);
	col = (int)round(p.col);
}

// https://math.stackexchange.com/questions/39390/determining-end-coordinates-of-line-with-the-specified-length-and-angle
DrawPoint findEndPoint(DrawPoint start, int len, int angle)
{
	DrawPoint end;
	end.col = start.col + len * cos(degree2radian(angle));
	end.row = start.row + len * sin(degree2radian(angle));
	return end;
}


// Use this to draw characters into the canvas, with the option of performing animation
void drawHelper(char canvas[][MAXCOLS], Point p, char ch, bool animate)
{
	// Pause time between steps (in milliseconds)
	const int TIME = 50;

	// Make sure point is within bounds
	if (p.row >= 0 && p.row < MAXROWS && p.col >= 0 && p.col < MAXCOLS)
	{
		// Draw character into the canvas
		canvas[p.row][p.col] = ch;

		// If animation is enabled, draw to screen at same time
		if (animate)
		{
			gotoxy(p.row, p.col);
			printf("%c", ch);
			Sleep(TIME);
		}
	}
}


// Fills gaps in a row caused by mismatch between match calculations and screen coordinates
// (i.e. the resolution of our 'canvas' isn't very good)
void drawLineFillRow(char canvas[][MAXCOLS], int col, int startRow, int endRow, char ch, bool animate)
{
	// determine if we're counting up or down
	if (startRow <= endRow)
		for (int r = startRow; r <= endRow; r++)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
	else
		for (int r = startRow; r >= endRow; r--)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
}


// Draw a single line from start point to end point
void drawLine(char canvas[][MAXCOLS], DrawPoint start, DrawPoint end, bool animate)
{
	char ch;

	Point scrStart(start);
	Point scrEnd(end);

	// vertical line
	if (scrStart.col == scrEnd.col)
	{
		ch = '|';

		drawLineFillRow(canvas, scrStart.col, scrStart.row, scrEnd.row, ch, animate);
	}
	// non-vertical line
	else
	{
		int row = -1, prevRow;

		// determine the slope of the line
		double slope = (start.row - end.row) / (start.col - end.col);

		// choose appropriate characters based on 'steepness' and direction of slope
		if (slope > 1.8)  ch = '|';
		else if (slope > 0.08)  ch = '`';
		else if (slope > -0.08)  ch = '-';
		else if (slope > -1.8) ch = '\'';
		else ch = '|';

		// determine if columns are counting up or down
		if (scrStart.col <= scrEnd.col)
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col <= scrEnd.col; col++)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
		else
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col >= scrEnd.col; col--)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
	}
}


// Draws a single box around a center point
void drawBox(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	int sizeHalf = height / 2;
	int ratio = (int)round(MAXCOLS / (double)MAXROWS * sizeHalf);

	// Calculate where the four corners of the box should be
	DrawPoint points[4];
	points[0] = DrawPoint(center.row - sizeHalf, center.col - ratio);
	points[1] = DrawPoint(center.row - sizeHalf, center.col + ratio);
	points[2] = DrawPoint(center.row + sizeHalf, center.col + ratio);
	points[3] = DrawPoint(center.row + sizeHalf, center.col - ratio);

	// Draw the four lines of the box
	for (int x = 0; x < 3; x++)
	{
		drawLine(canvas, points[x], points[x + 1], animate);
	}
	drawLine(canvas, points[3], points[0], animate);

	// Replace the corners with a better looking character
	for (int x = 0; x < 4; x++)
	{
		drawHelper(canvas, points[x], '+', animate);
	}
}


// Menu for the drawing tools
void menuTwo(char canvas[][MAXCOLS], char backupCanvas[][MAXCOLS], bool& animate)
{
	char charInput;
	int numInput;
	int numInput2;

	Point startPoint(0, 0);
	Point endPoint(0, 0);
	char getPointResult;
	do
	{
		displayCanvas(canvas);

		cout << "<A>nimate: ";
		//Lets the user know whether the animate is toggled on or off in their options.
		if (animate)
		{
			cout << "Y";
		}
		else
		{
			cout << "N";
		}
		cout << " / <F>ill / <L>ine / <B>ox / <N>ested Boxes / <T>ree / <M>ain Menu: ";
		cin >> charInput;
		cin.ignore();
		charInput = toupper(charInput);
		//Clears options line
		clearLine(MAXROWS + 1, MAXCOLS + 40);

		switch (charInput)
		{
		case 'A':
			animate = !animate;
			break;
		case 'F':
			clearLine(MAXROWS + 1, MAXCOLS + 40);
			cout << "Enter character to fill with from current location / <ESC> to cancel ";
			charInput = getPoint(startPoint);

			//Allows user to cancel operation
			if (charInput != ESC)
			{
				copyCanvas(backupCanvas, canvas);
				fillRecursive(canvas, startPoint.row, startPoint.col, canvas[startPoint.row][startPoint.col], charInput, animate);
			}
			break;
		case 'L':
			clearLine(MAXROWS + 1, MAXCOLS + 40);
			cout << "Type any letter to choose start point / <ESC> to cancel";
			getPointResult = getPoint(startPoint);

			//Allows user to cancel operation
			if (getPointResult != ESC)
			{
				//Marks the spot the user chose
				cout << getPointResult;

				clearLine(MAXROWS + 1, MAXCOLS + 40);
				cout << "Type any letter to choose end point / <ESC> to cancel";
				getPointResult = getPoint(endPoint);
				//Allows user to cancel operation
				if (getPointResult != ESC)
				{
					cout << getPointResult;

					copyCanvas(backupCanvas, canvas);
					drawLine(canvas, startPoint, endPoint, animate);
				}
			}
			
			break;
		case 'B':
			clearLine(MAXROWS + 1, MAXCOLS + 40);
			cout << "Enter size: ";
			cin >> numInput;

			clearLine(MAXROWS + 1, MAXCOLS + 40);
			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel";
			charInput = getPoint(startPoint);

			//Allows user to cancel operation
			if (charInput != ESC)
			{
				if (charInput == 'c' || charInput == 'C')
				{
					startPoint.col = MAXCOLS / 2;
					startPoint.row = MAXROWS / 2;
				}
				copyCanvas(backupCanvas, canvas);
				drawBox(canvas, startPoint, numInput, animate);
			}
			break;
		case 'N':
			clearLine(MAXROWS + 1, MAXCOLS + 40);
			cout << "Enter size of largest box: ";
			cin >> numInput;

			clearLine(MAXROWS + 1, MAXCOLS + 40);
			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel";
			charInput = getPoint(startPoint);

			//Allows user to cancel operation
			if (charInput != ESC)
			{
				if (charInput == 'c' || charInput == 'C')
				{
					startPoint.col = MAXCOLS / 2;
					startPoint.row = MAXROWS / 2;
				}
				copyCanvas(backupCanvas, canvas);
				drawBoxesRecursive(canvas, startPoint, numInput, animate);
			}
			break;
		case 'T':
			clearLine(MAXROWS + 1, MAXCOLS + 40);
			cout << "Enter approximate tree height: ";
			cin >> numInput;

			clearLine(MAXROWS + 1, MAXCOLS + 40);
			cout << "Enter branch angle: ";
			cin >> numInput2;

			clearLine(MAXROWS + 1, MAXCOLS + 40);
			cout << "Type any letter to choose start point, or <C> for bottom center / <ESC> to cancel";
			charInput = getPoint(startPoint);

			//Allows user to cancel operation
			if (charInput != ESC)
			{
				if (charInput == 'c' || charInput == 'C')
				{
					startPoint.col = MAXCOLS / 2;
					startPoint.row = MAXROWS - 1;
				}
				copyCanvas(backupCanvas, canvas);
				treeRecursive(canvas, startPoint, numInput, 270, numInput2, animate);
			}

			break;
		}
	} while (charInput != 'M');
}


// Get a single point from screen, with character entered at that point
char getPoint(Point& pt)
{
	char input;
	int row = 0, col = 0;

	// Moves cursor to row,col and then get
	// a single character from the keyboard
	gotoxy(row, col);
	
	do
	{
		input = _getch();
		if (input == SPECIAL)
		{
			input = _getch();
			switch (input)
			{
			case RIGHTARROW:
				if (col + 1 < MAXCOLS)
				{
					col++;
				}

				break;
			case LEFTARROW:
				if (col > 0)
				{
					col--;
				}

				break;
			case DOWNARROW:
				if (row + 1 < MAXROWS)
				{
					row++;
				}

				break;
			case UPARROW:
				if (row > 0)
				{
					row--;
				}

				break;
			}

		}
		else if (input == '\0')
		{
			//Throws away input
			input = _getch();
		}
		else if (input >= 32 && input <= 126)
		{
			//User chooses a spot and input character is returned
			pt.col = col;
			pt.row = row;
			return input;
		}
		gotoxy(row, col);
	} while (input != ESC);

	return input;
}


// Recursively fill a section of the screen
void fillRecursive(char canvas[][MAXCOLS], int row, int col, char oldCh, char newCh, bool animate)
{
	Point startPoint(row, col);

	if ((row < 0 || row >= MAXROWS) || (col < 0 || col >= MAXCOLS))
	{
		return;
	}
	if (canvas[row][col] == oldCh)
	{
		drawHelper(canvas, startPoint, newCh, animate);
		//Fills in all adjacent points
		fillRecursive(canvas, row - 1, col, oldCh, newCh, animate);
		fillRecursive(canvas, row + 1, col, oldCh, newCh, animate);
		fillRecursive(canvas, row, col - 1, oldCh, newCh, animate);
		fillRecursive(canvas, row, col + 1, oldCh, newCh, animate);
	}

}


// Recursively draw a tree
void treeRecursive(char canvas[][MAXCOLS], DrawPoint start, int height, int startAngle, int branchAngle, bool animate)
{
	Point endPoint = findEndPoint(start, height / 3, startAngle);
	if ((start.row < 0 || start.row >= MAXROWS) || (start.col < 0 || start.col >= MAXCOLS) || (height < 3))
	{
		return;
	}
	drawLine(canvas, start, endPoint, animate);
	// Starts drawing branches
	treeRecursive(canvas, endPoint, height - 2, startAngle + branchAngle, branchAngle, animate);
	treeRecursive(canvas, endPoint, height - 2, startAngle - branchAngle, branchAngle, animate);
}


// Recursively draw nested boxes
void drawBoxesRecursive(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	//Ensures that the box isn't getting too small
	if (height < 2)
	{
		return;
	}
	drawBox(canvas, center, height, animate);
	drawBoxesRecursive(canvas, center, height - 2, animate);
}