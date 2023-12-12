/*
* This program allows display and editing of text art (also called ASCII art).
*/

#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;


int main()
{
	char canvas[MAXROWS][MAXCOLS];
	char previousCanvas[MAXROWS][MAXCOLS];

	char input;

	int colInput = 0;
	int rowInput = 0;

	char newChr;
	char oldChr;

	bool animate = false;

	initCanvas(canvas);
	initCanvas(previousCanvas);

	do
	{
		displayCanvas(canvas);
		cout << "<E>dit / <M>ove / <R>eplace / <D>raw / <C>lear / <U>ndo / <L>oad / <S>ave / <Q>uit: ";
		cin >> input;
		cin.ignore();
		input = toupper(input);
		//Clears options line
		clearLine(MAXROWS + 1, MAXCOLS + 40);

		//User puts in input
		switch (input)
		{
		case 'E':
			copyCanvas(previousCanvas, canvas);
			cout << "Press <ESC> to stop editing";
			editCanvas(canvas);
			break;
		case 'M':
			copyCanvas(previousCanvas, canvas);
			cout << "Enter columns to move: ";
			cin >> colInput;
			cout << "Enter rows to move: ";
			cin >> rowInput;
			moveCanvas(canvas, rowInput, colInput);
			break;
		case 'D':
			menuTwo(canvas, previousCanvas, animate);
			break;
		case 'C':
			copyCanvas(previousCanvas, canvas);
			initCanvas(canvas);
			break;
		case 'R':
			copyCanvas(previousCanvas, canvas);
			cout << "Enter character to replace: ";
			cin.get(oldChr);
			cin.ignore();
			cout << "Enter character to replace with: ";
			cin.get(newChr);
			cin.ignore();
			replace(canvas, oldChr, newChr);
			break;
		case 'U':
			copyCanvas(canvas, previousCanvas);
			break;
		case 'L':
			copyCanvas(previousCanvas, canvas);
			cout << "Enter the filename (don't enter 'txt'): ";
			loadCanvas(canvas);
			break;
		case 'S':
			cout << "Enter the filename (don't enter 'txt'): ";
			saveCanvas(canvas);
			break;
		}

		//Clears instructions from options
		clearLine(MAXROWS + 2, MAXCOLS + 40);
		clearLine(MAXROWS + 3, MAXCOLS + 40);
		clearLine(MAXROWS + 4, MAXCOLS + 40);

	} while (input != 'Q');

	return 0;
}

void gotoxy(short row, short col)
{
	COORD pos = { col, row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


void clearLine(int lineNum, int numOfChars)
{
	// Move cursor to the beginning of the specified line on the console
	gotoxy(lineNum, 0);

	// Write a specified number of spaces to overwrite characters
	for (int x = 0; x < numOfChars; x++)
		cout << " ";

	// Move cursor back to the beginning of the line
	gotoxy(lineNum, 0);
}


void replace(char canvas[][MAXCOLS], char oldCh, char newCh)
{
	//Replaces old characters with new ones
	for (int i = 0; i < MAXROWS; i++)
	{
		for (int j = 0; j < MAXCOLS; j++)
		{
			if (canvas[i][j] == oldCh)
			{
				canvas[i][j] = newCh;
			}
		}
	}
}


void editCanvas(char canvas[][MAXCOLS])
{
	char input;
	int row = 0, col = 0;

	// Moves cursor to row,col and then get
	// a single character from the keyboard
	gotoxy(row, col);
	input = _getch();

	while (input != ESC)
	{
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
			canvas[row][col] = input;
			cout << input;
		}
		gotoxy(row, col);
		input = _getch();
	}

}


void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue)
{
	char tempCanvas[MAXROWS][MAXCOLS];
	initCanvas(tempCanvas);

	//Loops through columns and makes sure it is in range
	for (int i = 0; i < MAXCOLS; i++)
	{
		if (!((i - colValue) >= MAXCOLS || (i - colValue) < 0))
		
			//Loops through rows and makes sure it is in range
		for (int j = 0; j < MAXROWS; j++)
		{
			if (!((j - rowValue) >= MAXROWS || (j - rowValue) < 0))
			{
				tempCanvas[j][i] = canvas[j - rowValue][i - colValue];
			}
		}
	}

	for (int i = 0; i < MAXROWS; i++)
	{
		for (int j = 0; j < MAXCOLS; j++)
		{
			canvas[i][j] = tempCanvas[i][j];
		}
	}
}


void initCanvas(char canvas[][MAXCOLS])
{
	//fills array with blank characters
	for (int i = 0; i < MAXROWS; i++)
	{
		for (int j = 0; j < MAXCOLS; j++)
		{
			canvas[i][j] = ' ';
		}
	}
}


void displayCanvas(char canvas[][MAXCOLS])
{
	gotoxy(0, 0);

	// Draws the outline border
	for (int i = 0; i < MAXROWS; i++)
	{
		for (int j = 0; j < MAXCOLS; j++)
		{
			cout << canvas[i][j];
		}
		cout << "|" << endl;
	}
	for (int i = 0; i < MAXCOLS; i++)
	{
		cout << "-";
	}
	cout << endl;
}

//Transfers characters from one array to another
void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS])
{
	for (int i = 0; i < MAXROWS; i++)
	{
		for (int j = 0; j < MAXCOLS; j++)
		{
			to[i][j] = from[i][j];
		}
	}
}


void saveCanvas(char canvas[][MAXCOLS])
{
	char input[FILENAMESIZE];
	char path[FILENAMESIZE];

	ofstream outFile;

	cin.getline(input, FILENAMESIZE - 15);
	if (cin.fail())
	{
		cin.clear();
		cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	}

	snprintf(path, FILENAMESIZE, "SavedFiles\\%s.txt", input);

	// Checks to see if the file can be written
	outFile.open(path);
	if (!outFile)
	{
		cerr << "ERROR: File cannot be written." << endl;
		system("pause");
		return;
	}

	for (int i = 0; i < MAXROWS; i++)
	{
		for (int j = 0; j < MAXCOLS; j++)
		{
			outFile << canvas[i][j];
		}
		outFile << "\n";
	}
	outFile.close();

	cout << "File saved!" << endl;
	system("pause");
}


void loadCanvas(char canvas[][MAXCOLS]) 

{
	char input[FILENAMESIZE];
	char path[FILENAMESIZE];
	char buffer[MAXCOLS + 1];

	ifstream inFile;

	cin.getline(input, FILENAMESIZE - 15);
	if (cin.fail())
	{
		cin.clear();
		cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	}

	snprintf(path, FILENAMESIZE, "SavedFiles\\%s.txt", input);
	
	// Checks to see if the file can be loaded within the canvas 
	inFile.open(path);
	if (!inFile)
	{
		cerr << "ERROR: File cannot be read." << endl;
		system("pause");
		return;
	}

	initCanvas(canvas);

	int i = 0;
	int j = 0;

	inFile.getline(buffer, MAXCOLS + 1);
	for (int i = 0; i < MAXROWS && !inFile.eof(); i++)
	{
		// Checks to see if the characters go outside the canvas and clear error to continue running.
		if (inFile.fail())
		{
			inFile.clear();
			inFile.ignore((numeric_limits<streamsize>::max)(), '\n');
		}
		for (int j = 0; j < MAXCOLS && buffer[j] != '\0'; j++)
		{
			canvas[i][j] = buffer[j];
		}

		inFile.getline(buffer, MAXCOLS + 1);
	}
	inFile.close();

}