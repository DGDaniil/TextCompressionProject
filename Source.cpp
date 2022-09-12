#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <streambuf>
#include <cctype>
#include<sstream>
#include<string>
#include<bitset>
#include<cmath>
using namespace std;

struct File {
	char name;
	int length;

	bool isLeaf;
	File* right;
	File* left;

};

vector<pair<char, int>> get_frequencies(string arr);
string readFileIntoString(const string& path);

void heapify(File** arr, int n, int i);
File** build_heap(File** arr, int n);
void printFiles(File** arr, int n);
File* removeSmallest(File** arr, int& n);
void insertNode(File** arr, int& n, File* Key);

File* OptimalBMT(File** arr, int n);

File* encode(File** arr, int n);

void printTree(File* n);

//vector<pair<string, int>> getCodes(File* n);

void printCodes(File* root, unordered_map<char, string>& localMap, int top, string arr);

string cleanCodes(string input);

string convertcharBin(char let);
string convertBin(string seq, int last);

void traverse(File* tr, string huff, string fileName);


// Начало гланвого кода


int main()
{
	char userResponse;
	char resume = 'N';
	bool compress;
	string pathInputFile;
	string pathOutputFile;

	do
	{
		system("cls");
		cout
			<< "Welcome"
			<< endl
			<< "1. Compression"
			<< endl
			<< "2. Decompression"
			<< endl;
		cin >> userResponse;
		while (userResponse != '1' && userResponse != '2')
		{
			cout << "Invalid choice! Please enter a valid choice: ";
			cin >> userResponse;
		} 

		if (userResponse == '1')
		{
			cout << "Please enter the path to the file to be compressed: ";
			cin >> pathInputFile;
			cout << endl << "Please enter the path for the output file: ";
			cin >> pathOutputFile;

			string text = readFileIntoString(pathInputFile);
			 //cout << "printing text..." << endl << text << endl;
			 vector<pair<char, int>> data = get_frequencies(text);

			 int SIZE = data.size();
			 File** heap = new File * [SIZE];


			 /*for(int i=0; i<data.size(); i++) {
			 	cout << data[i].first << " " << data[i].second << endl;
			 }*/

			 //cout << "before Initialiation" << endl;
			 for (int i = 0; i < data.size(); i++) {
			 	heap[i] = new File;
			 	heap[i]->name = data[i].first;
			 	heap[i]->length = data[i].second;
			 	heap[i]->left = NULL;
			 	heap[i]->right = NULL;
			 	heap[i]->isLeaf = true;

			 }
			 string temp = "----------------------------------------------------------------------";
			 //temp.resize(10);
			 //cout << "before encode!" << endl;
			 File* sol2 = encode(heap, data.size());
			 //cout << "before umap declaration!" << endl;
			 unordered_map<char, string> umap;
			 //cout << "before printCodes!" << endl;


			 printCodes(sol2, umap, 0, temp);
			// cout << "Printing umap..." << endl;
			 //unordered_map<char, string>::iterator it;
			 for (auto x : umap) {
			 	umap[x.first] = cleanCodes(x.second);
			 	//cout << x.first << ":" << umap[x.first] << ", " << umap[x.first].size() << endl;
			 }

			 string binRep;

			 ofstream fout(pathOutputFile, ios::out | ios::binary);
			 //ofstream codes("zippedcoded.txt");	//debugging!

			 for (size_t i = 0; i < text.length(); i++)
			 {
			 	char letter = text[i];

			 	string bin = umap[letter];
			 	//binRep.append(bin);
			 	binRep += umap[letter];
				//codes << umap[letter];

			 	//cout << "bin = " << bin << endl;

			 	//system("pause");
			 	//zipFile.write((char*)& bin, sizeof(1));

			 }
			 for (auto x : umap)
			 {
			 	/*if (x.first == '\n')
			 	{
			 		cout << "endline code = " << x.second << endl;
			 	}*/
			 	fout << int(x.first) << " " << x.second << endl;
			 }
			 fout << binRep.length() % 8 << endl;
			 fout << "-1"<<endl;
			 for (int i = 0; i < binRep.length(); i+=8)
			 {
			 	int symbol = stoi(binRep.substr(i, 8), nullptr, 2);
			 	//bitset<1> b(binRep.substr(i, 7));
			 	//cout << "b = " << b << endl;
			 	fout << char(symbol);
			 	//system('pause');
			 }
			 //fout << binRep;

			fout.close();

			double L_avg = 0;
			for (size_t i = 0; i < data.size(); i++)
			{
				L_avg += data[i].second * umap[data[i].first].length();
			}
			
			L_avg /= text.length();
			L_avg /= 8;

			double eta = 0;
			for (size_t i = 0; i < data.size(); i++)
			{
				double p_i = data[i].second;
				p_i /= double(text.length());
				eta += -1* p_i * log2(p_i);
			}

			eta = eta / (L_avg*8);

			cout << "\nCompression Ratio = " << L_avg << endl;
			cout << "Effeciency = " << eta << endl;
			cout << endl;

		}

		if (userResponse =='2')
		{
			cout << "Please enter the path to the file to be decompressed: ";
			cin >> pathInputFile;
			cout << endl << "Please enter the path for the output file: ";
			cin >> pathOutputFile;

			ifstream openCompressed;
			openCompressed.open(pathInputFile, ios::binary);

			int x;
			char letter;
			string code;
			string temp;
			string temp1;
			vector <string> row;

			File* tree = new File;
			tree->left = NULL;
			tree->right = NULL;
			tree->name = '$';
			tree->isLeaf = false;

			string compressedText;
			int lastCode;

			//string codedFile = readFileIntoString("zippedcoded.txt");

			//ofstream copy;
			//copy.open("copyZip.txt");

			while (true) {
				getline(openCompressed, temp, '\n');
				//cout << temp << endl;
				if (temp != "-1") {
					if (temp.length() == 1) { lastCode = stoi(temp); }
					else {
						//cout << temp << endl;
						stringstream s(temp);
						row.clear();
						while (getline(s, temp1, ' '))
						{
							row.push_back(temp1);
						}

						letter = char(stoi(row[0]));
						code = row[1];

						//cout << letter << " " << code << " " << code.length() << endl;

						File* temp = tree;

						for (int i = 0; i < code.length(); i++)
						{
							if (i != code.length() - 1) {
								if (code[i] == '0') {
									if (temp->left != NULL) {
										temp = temp->left;
									}
									else {
										temp->left = new File;
										temp->left->name = '$';
										temp->left->right = NULL;
										temp->left->left = NULL;
										temp->left->isLeaf = false;
										temp = temp->left;
									}

								}
								else {
									if (temp->right != NULL) {
										temp = temp->right;
									}
									else {
										temp->right = new File;
										temp->right->name = '$';
										temp->right->right = NULL;
										temp->right->left = NULL;
										temp->right->isLeaf = false;
										temp = temp->right;
									}
								}
							}
							else if (i == code.length() - 1) {
								if (code[i] == '0') {
									if (temp->left != NULL) {
										temp = temp->left;
									}
									else {
										temp->left = new File;
										temp->left->name = letter;
										temp->left->right = NULL;
										temp->left->left = NULL;
										temp->left->isLeaf = true;
										temp = temp->left;
									}

								}
								else {
									if (temp->right != NULL) {
										temp = temp->right;
									}
									else {
										temp->right = new File;
										temp->right->name = letter;
										temp->right->right = NULL;
										temp->right->left = NULL;
										temp->right->isLeaf = true;
										temp = temp->right;
									}
								}
							}
						}
					}
				}
				else {
					string abs;
					while (getline(openCompressed, abs, '\n')) {
						compressedText += abs;
						compressedText += '\n';
					}
					break;
				}
			}

			//string sheteema = convertBin(compressedText, lastCode);
			//copy << convertBin(compressedText, lastCode);

			//for (size_t i = 0; i < codedFile.length(); i++)
			//{
			//	if (codedFile[i] != sheteema[i])
			//		//cout << i << " conflict" << endl;
			//}



			traverse(tree, convertBin(compressedText, lastCode), pathOutputFile);

		}
		

		cout << "Done! Would you like to continue? (Y/N): ";
		cin >> resume;

	} while (resume == 'Y');

	// string text = readFileIntoString("test1.txt");
	// //cout << "printing text..." << endl << text << endl;
	// vector<pair<char, int>> data = get_frequencies(text);

	// int SIZE = data.size();
	// File** heap = new File * [SIZE];


	// /*for(int i=0; i<data.size(); i++) {
	// 	cout << data[i].first << " " << data[i].second << endl;
	// }*/

	// cout << "before Initialiation" << endl;
	// for (int i = 0; i < data.size(); i++) {
	// 	heap[i] = new File;
	// 	heap[i]->name = data[i].first;
	// 	heap[i]->length = data[i].second;
	// 	heap[i]->left = NULL;
	// 	heap[i]->right = NULL;
	// 	heap[i]->isLeaf = true;

	// }
	// string temp = "----------------------------------------------------------------------";
	// //temp.resize(10);
	// cout << "before encode!" << endl;
	// File* sol2 = encode(heap, data.size());
	// cout << "before umap declaration!" << endl;
	// unordered_map<char, string> umap;
	// cout << "before printCodes!" << endl;


	// printCodes(sol2, umap, 0, temp);
	// cout << "Printing umap..." << endl;
	// //unordered_map<char, string>::iterator it;
	// for (auto x : umap) {
	// 	umap[x.first] = cleanCodes(x.second);
	// 	cout << x.first << ":" << umap[x.first] << ", " << umap[x.first].size() << endl;
	// }

	// //cout << umap['a'] << "," << umap['a'].size() << endl;
	// /*string tempo = cleanCodes("0110-------");

	// cout << tempo << "," << tempo.size();*/
	// //string test1 = "1101";
	// //ofstream fout;
	// //fout.open("tesssst.txt");
	// //cout << "printing test1..." << endl;
	// //for (size_t i = 0; i < test1.length(); ++i)
	// //{
	// //	int val = int(test1[i]);

	// //	(val % 2) ? fout << '1' : fout << '0';
	// //	//bitset<1> b(test1.c_str()[i]);
	// //	//fout << b;
	// //	//fout << bs;
	// //	//cout << bs << endl;
	// //}
	// //fout.close();]
	// //ofstream zipFile("zippedFile.txt", ios::out | ios::binary);

	// string binRep;

	// ofstream fout("zippedFile.txt", ios::out | ios::binary);
	// ofstream codes("zippedcoded.txt");

	// for (size_t i = 0; i < text.length(); i++)
	// {
	// 	char letter = text[i];

	// 	string bin = umap[letter];
	// 	//binRep.append(bin);
	// 	binRep += umap[letter];
	//	codes << umap[letter];

	// 	//cout << "bin = " << bin << endl;

	// 	//system("pause");
	// 	//zipFile.write((char*)& bin, sizeof(1));

	// }
	// for (auto x : umap)
	// {
	// 	/*if (x.first == '\n')
	// 	{
	// 		cout << "endline code = " << x.second << endl;
	// 	}*/
	// 	fout << int(x.first) << " " << x.second << endl;
	// }
	// fout << binRep.length() % 8 << endl;
	// fout << "-1"<<endl;
	// for (int i = 0; i < binRep.length(); i+=8)
	// {
	// 	int symbol = stoi(binRep.substr(i, 8), nullptr, 2);
	// 	//bitset<1> b(binRep.substr(i, 7));
	// 	//cout << "b = " << b << endl;
	// 	fout << char(symbol);
	// 	//system('pause');
	// }
	// //fout << binRep;

	//fout.close();



	/*ifstream openCompressed;
	openCompressed.open("zippedFile.txt", ios::binary);

	int x;
	char letter;
	string code;
	string temp;
	string temp1;
	vector <string> row;

	File* tree = new File;
	tree->left = NULL;
	tree->right = NULL;
	tree->name = '$';
	tree->isLeaf = false;

	string compressedText;
	int lastCode;

	string codedFile = readFileIntoString("zippedcoded.txt");

	ofstream copy;
	copy.open("copyZip.txt");

	while (true) {
		getline(openCompressed, temp, '\n');
		cout << temp << endl;
		if (temp != "-1") {
			if (temp.length() == 1) { lastCode = stoi(temp); } 
			else {
				cout << temp << endl;
				stringstream s(temp);
				row.clear();
				while (getline(s, temp1, ' '))
				{
					row.push_back(temp1);
				}

				letter = char(stoi(row[0]));
				code = row[1];

				cout << letter << " " << code << " " << code.length() << endl;

				File* temp = tree;

				for (int i = 0; i < code.length(); i++)
				{
					if (i != code.length() - 1) {
						if (code[i] == '0') {
							if (temp->left != NULL) {
								temp = temp->left;
							}
							else {
								temp->left = new File;
								temp->left->name = '$';
								temp->left->right = NULL;
								temp->left->left = NULL;
								temp->left->isLeaf = false;
								temp = temp->left;
							}

						}
						else {
							if (temp->right != NULL) {
								temp = temp->right;
							}
							else {
								temp->right = new File;
								temp->right->name = '$';
								temp->right->right = NULL;
								temp->right->left = NULL;
								temp->right->isLeaf = false;
								temp = temp->right;
							}
						}
					}
					else if (i == code.length() - 1) {
						if (code[i] == '0') {
							if (temp->left != NULL) {
								temp = temp->left;
							}
							else {
								temp->left = new File;
								temp->left->name = letter;
								temp->left->right = NULL;
								temp->left->left = NULL;
								temp->left->isLeaf = true;
								temp = temp->left;
							}

						}
						else {
							if (temp->right != NULL) {
								temp = temp->right;
							}
							else {
								temp->right = new File;
								temp->right->name = letter;
								temp->right->right = NULL;
								temp->right->left = NULL;
								temp->right->isLeaf = true;
								temp = temp->right;
							}
						}
					}
				}
			}
		}
		else {
			string abs;
			while (getline(openCompressed, abs, '\n')) {
				compressedText += abs;
				compressedText += '\n';
			}
			break;
		}
	}
	
	string sheteema = convertBin(compressedText, lastCode);
	copy << convertBin(compressedText, lastCode);
	
	for (size_t i = 0; i < codedFile.length(); i++)
	{
		if (codedFile[i] != sheteema[i])
			cout << i << " conflict" << endl;
	}
	


	traverse(tree, convertBin(compressedText, lastCode), "unzippedfile.txt");*/
	return 0;
}


// Конец главного кода


string cleanCodes(string input) {

	//cout << endl << "printing cleaned Codes!" << endl;
	string x;
	int counter = 0;
	while (input[counter] != '-')
	{
		x += input[counter];
		counter++;
	}
	return x;


}

void printCodes(File* root, unordered_map<char, string>& localMap, int top, string arr)
{

	// Assign 0 to left edge and recur
	if (root->left) {
		//cout << root->name << " " << root->isLeaf << "left" << endl;
		//arr.append("0");
		arr[top] = '0';
		printCodes(root->left, localMap, top + 1, arr);
	}

	// Assign 1 to right edge and recur
	if (root->right) {
		//cout << root->name << " " << root->isLeaf << "right" << endl;
		//arr.append("1");
		arr[top] = '1';
		printCodes(root->right, localMap, top + 1, arr);
	}

	// If this is a leaf node, then
	// it contains one of the input
	// characters, print the character
	// and its code from arr[]
	if (root->isLeaf) {

		//cout << root->name << " " << arr << endl;
		localMap[root->name] = arr;
		//printArr(arr, top);
	}
}

void heapify(File** arr, int n, int i)
{
	int largest = i;
	int l = 2 * i + 1;
	int r = 2 * i + 2;

	if (l < n && arr[l]->length < arr[largest]->length)
		largest = l;

	if (r < n && arr[r]->length < arr[largest]->length)
		largest = r;

	if (largest != i) {
		swap(arr[i], arr[largest]);
		heapify(arr, n, largest);
	}
}

File** build_heap(File** arr, int n)
{
	File** copy = new File * [n];
	for (int i = 0; i < n; i++) {
		copy[i] = arr[i];
	}

	for (int i = n / 2 - 1; i >= 0; i--)
		heapify(copy, n, i);


	return copy;
}

void printFiles(File** arr, int n) {
	for (int i = 0; i < n; i++) {
		cout << arr[i]->name << " " << arr[i]->length << endl;
	}
}

File* removeSmallest(File** arr, int& n)
{
	File* lastElement = arr[n - 1];
	File* temp = arr[0];
	arr[0] = lastElement;
	n--;



	heapify(arr, n, 0);

	return temp;
}

void insertNode(File** arr, int& n, File* Key)
{
	n++;

	arr[n - 1] = Key;

	heapify(arr, n, n - 1);
}

File* OptimalBMT(File** arr, int n) {
	File** heapified = build_heap(arr, n);

	File* Final = new File;
	Final->length = 0;

	while (n > 0) {
		File* f1; File* f2;
		File* f3 = new File;

		f1 = removeSmallest(heapified, n);
		if (n > 0) {
			f2 = removeSmallest(heapified, n);
			f3->name = '$';
			f3->length = f1->length + f2->length;
			insertNode(heapified, n, f3);

			Final->length += f3->length;
		}
		else {
			f3 = f1;
		}

		Final->name = f3->name;

	}


	return Final;
}


File* encode(File** arr, int n) {
	File** heapified = build_heap(arr, n);

	File* Final = new File;
	Final->length = 0;

	while (n > 0) {
		File* f1; File* f2;
		File* f3 = new File;

		f1 = removeSmallest(heapified, n);
		if (n > 0) {
			f2 = removeSmallest(heapified, n);
			f3->name = '$';
			f3->length = f1->length + f2->length;
			insertNode(heapified, n, f3);



			f3->left = f1;
			f3->right = f2;
			f3->isLeaf = false;

			Final->length += f3->length;
		}
		else {
			f3 = f1;
		}

		if (n == 1)
			Final = f3;
	}

	return Final;
}

void printTree(File* n) {
	if (n == NULL) {
		return;
	}
	cout << n->name << " ";
	printTree(n->left);
	printTree(n->right);

}
//
//vector<pair<string, int>> getCodes(File* n) {
//	if (n == NULL) {
//
//	}
//}

vector<pair<char, int>> get_frequencies(string arr) {

	vector<pair<char, int>> freqs;
	sort(arr.begin(), arr.end()); // array gets sorted and algorithm becomes O(n log n)

	int count = 1;
	double elem = arr[0];

	for (int i = 0; i < arr.size(); i++) {
		if (arr[i] == arr[i + 1]) {
			count++;
		}
		else {
			pair<char, int> p;
			p.first = arr[i];
			p.second = count;
			freqs.push_back(p);
			count = 1; elem = arr[i + 1];
		}
	}

	return freqs;
}

string readFileIntoString(const string& path) {
	ifstream t(path);
	string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
	return str;
}


string convertcharBin(char let)
{
	return bitset<8>(let).to_string();
}

string convertBin(string seq, int last) {
	string huffmans = "";
	for (int i = 0; i < seq.length()-2; i++)
	{
		//cout << seq[i] << " " << convertcharBin(seq[i]) << endl;
		huffmans += convertcharBin(seq[i]);
	} 

	if (last != 0) {
		string lastOne = convertcharBin(seq[seq.length() - 2]).substr(8 - last, last);
		//cout << lastOne << endl;
		huffmans += lastOne;
	}
	else {
		huffmans += convertcharBin(seq[seq.length() - 2]);
	}

	return huffmans;
}

void traverse(File* tr, string huff, string fileName)
{
	ofstream decoded;
	decoded.open(fileName);
	int i = 0;

	//cout << huff.length() << endl;

	while (i < huff.length()) {
		File* ptr = tr;
		while (!ptr->isLeaf) {
			//cout << i << endl;
			if (huff[i] == '0') {
				ptr = ptr->left;
			}
			else if (huff[i] == '1') {
				ptr = ptr->right;
			}
			i++;
		}
		decoded << ptr->name;
	}

	decoded.close();
}