/*
Lab 2 - Store that handles shopping carts
Authored by Sachin Nediyanchath
Submission date : 06/29/2018
*/

#include <algorithm>
#include <bitset>
#include <cmath>
#include <fstream>
#include <iostream>
#include <deque>
#include <regex>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

//----------------------------------------------------------------------------------------------
// Node (Generic programming for Product info)
template<class X>
struct Node 
{
	X data;

	X getdata() const { return data; }
	virtual void setdata(X change_data) { data = change_data; }
	virtual void show_info() = 0;
};

// typedef objects
struct Product_name : Node<string>
{
	void setdata(string change_data) { data = change_data; }
	void show_info() { cout << "The product's name is" << getdata() << endl; }
};

struct Product_price : Node<float>
{
	void setdata(float change_data) { change_data = trunc(change_data * 1000) / 1000; data = change_data; }
	void show_info() { cout << "The product's price is" << getdata() << endl; }
};

/*
Product class inheriting from Node class
Accessors and mutators along with initial/parameter-based/copy constructors and destructor
*/
class Product
{
private:
	Product_name name;
	Product_price price;
	//Product* placeholder;
public:
	//Accessors
	string getname() const { return name.getdata(); }
	float getprice() const { return price.getdata(); }
	// Mutators
	void setname(string change_name) { name.setdata(change_name); }
	void setprice(float change_price) { price.setdata(change_price); }
	// Initial constructor
	Product() { setname("TEMP_PRODUCT"); setprice(0.00); }
	// Parameter-based constructor
	Product(string new_name, float new_price) { setname(new_name); setprice(new_price); }
	// Copy constructor
	Product(const Product &old_Product) { setname(old_Product.getname()); setprice(old_Product.getprice()); }
	// Destructor
	~Product() { }
	// Display info of individual product!
	void showinfo() { cout << "Name, Price: " << getname() << ", " << getprice() << endl; }
};

/*
Reader class (virtual class)
This class will be used in ProductList/BarcodeList/Shop classes
*/
class Reader
{
	// display the error message if text file is not in directory
	virtual void info_file_MIA() = 0;	
	virtual void read_file() = 0;
};

/*
ProductList class that inherit Product features
This class will store various products from reading XML file
This will be used in conjuction with Cart class to retrieve
information for products
*/
class ProductList : public Reader
{
private:
	list<Product> List;
public:
	ProductList() { read_file(); }
	// Read Products.XML and push data into a stack and later a queue depending on tags.
	// At the end, the data will be pushed into a list of Products
	void read_file();
	void info_file_MIA()
	{
		cerr << "The file is not in the directory... Must have been moved" << endl;
		system("pause");
		exit(1);
	};
	// XML partitioner
	void xml_breaker(ifstream & line, stack<string> & tag, deque<string> & product);
	list<Product> give_list() const { return List; }
};

// This will read Products.xml and fill the product list through xml_breaker function
void ProductList::read_file()
{
	// Initialize a stack that will hold the product objects which hold the info 
	stack<string> TagStack;
	// Initialize a queue that will hold all data Nodes (name,price)
	deque<string> Productinfo;

	ifstream read_map("Products.xml");

	if (read_map.is_open())
	{
		cout << "File is found! Reading file now...\n";
		while (!read_map.eof())
		{
				xml_breaker(read_map, TagStack, Productinfo);
		}
	}
	else
	{
		info_file_MIA();
	}
	read_map.close();
}

// Uses Regex to get BarCodeList and Product tags
void match(string text, string express, stack<string> & tag, bool & flag, bool & nest_tag)
{
	sregex_token_iterator end;
	regex pattern(express);
	for (sregex_token_iterator pos(text.begin(),text.end(),pattern); pos != end; ++pos)
	{
		if ((*pos).length() > 0)
		{
			if (static_cast<string>(*pos)[0] == 0x2F)
			{
				tag.pop();
			}
			else if (static_cast<string>(*pos)[0] != 0x20)
			{
				if (!flag)
				{
					tag.push(*pos);
					flag = true;
				}
				else if (flag && !nest_tag)
				{
					tag.push(*pos);
					nest_tag = true;
				}
			}
		}
	}
}

// Regex to retrieve info for nested tags, such as name and price of products 
void match_nest(string text, string express, string & product_part, stack<string> & tag, deque<string> & product, bool & num_tag, int & count, Product & prod)
{
	string toupper(string & name);

	size_t os;
	float num = 0.00;
	string temp;

	sregex_token_iterator end;
	regex pattern(express);
	for (sregex_token_iterator pos(text.begin(), text.end(), pattern); pos != end; ++pos)
	{
		if ((*pos).length() > 0)
		{
			if (static_cast<string>(*pos)[0] == 0x2F)
			{
				count++;
				product.push_back(toupper(product_part));
				product_part = "";
				tag.pop();
				num_tag = true;
				if (count == 2)
				{
					prod.setname(product.front());
					product.pop_front();
					num = stof(product.front(), &os);
					num = round(num*100)/100;
					prod.setprice(num);
					product.pop_front();
					//if (product.empty()) cout << "This queue is empty yo" << endl;
					count = 0;
					num_tag = false;
				}
			}
			else if (static_cast<string>(*pos)[0] != 0x20)
			{
				if (tag.size() == 3) product_part += *pos;
				else tag.push(*pos);
			}
		}
	}
}

// Converts all product names into upper case
string toupper(string & name)
{
	string final;
	for (int i = 0; i < (int)name.size(); i++)
	{
		final += toupper(name[i]);
	}
	return final;
}

// Uses both match and match_nest functions to retrieve information 
void ProductList::xml_breaker(ifstream & doc, stack<string> & tag, deque<string> & product)
{
	string line, temp, nest_data;
	bool begin_tag = false, nest_tag = false, num_tag = false;
	int count = 0;
	Product TempProduct;

	while (getline(doc, line, '\n'))
	{
		if(!nest_tag && List.empty())	match(line, "[a-zA-Z]*", tag, begin_tag, nest_tag);
		else if (!nest_tag) match(line, "[a-zA-Z/]*", tag, begin_tag, nest_tag);
		else if (num_tag) { match_nest(line, "[a-zA-Z1-9./]*", nest_data, tag, product, num_tag, count, TempProduct); nest_tag = false; }
		else match_nest(line, "[a-zA-Z /]*", nest_data, tag, product, num_tag, count, TempProduct);
		if (product.empty() && tag.size() == 2 && !nest_tag) List.push_back(TempProduct);
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Generic code for Barcode class
template <typename X, typename Y, typename Z>
struct Data {
	tuple<X, Y, Z> morning_tuple;
	// Setters
	void set(X variable_one) { get<X>(morning_tuple) = variable_one; }
	void set(Y variable_two) { get<Y>(morning_tuple) = variable_two; }
	void set(Z variable_three) { get<Z>(morning_tuple) = variable_three; }
	void settuple(X one, Y two, Z three) { set(one); set(two); set(three); }
	// Getters
	X getfirst()const { return get<X>(morning_tuple); }
	Y getsecond()const { return get<Y>(morning_tuple); }
	Z getthird()const { return get<Z>(morning_tuple); }
	// Shows the traits of tuple
	void getmorse()
	{
		X first = getfirst();
		Y second = getsecond();
		Z third = getthird();
		cout << "Tuple Summary\n" << first << "\n"
			<< second << "\n" << third << endl;
	}	
};

// This will be used in Barcode class
typedef Data<char, bitset<9>, int> BarCode;

/*
BARCODELIST
Class that will hold a list of Barcodes (tuple)
*/
class BarCodeList : public Reader
{
private:
	list<BarCode> list_barcode;
public:
	BarCodeList() { read_file(); }
	void read_file();
	void info_file_MIA()
	{
		cerr << "The file is not in the directory... Must have been moved" << endl;
		system("pause");
		exit(1);
	};
	// This is part of put_into_list function (made public, since it needs to use Morser set functions)
	void read_from_file(string& line, int & result);
	// Returns the character chart 
	list<BarCode> give_vector() { return list_barcode; };
};

// This will read b.xml and fill the product list through xml_breaker function
void BarCodeList::read_file()
{
	string line;
	int result;
	ifstream read_text("bc3of9.txt");

	if (read_text.is_open())
	{
		cout << "File is found! Reading file now...\n";

		while (!read_text.eof())
		{
			while (getline(read_text, line, '\n'))
				read_from_file(line, result);
		}
	}
	else
	{
		info_file_MIA();
	}
	read_text.close();
}

// reads a line of data from text file that also returns a result from a friend function of the class
void BarCodeList::read_from_file(string& line, int & result)
{
	bitset<9> convert_string_bit(string, int);
	int convert_bit_int(bitset<9>, int);

	int sizer;
	BarCode temp;
	stringstream ss;
	string morse, first;
	size_t size = 0;
	bitset<9> Jerry;

	if (list_barcode.size() == 44)
	{
		return;
	}

	// Feeds a line of text into a stringstream
	ss << line;
	// Convert from stringstream to string 
	first = ss.str();
	// Counts characters until hitting space
	size = (first.find('n') || first.find('w'));
	// Sets 1 of 3 traits of temp Morser 
	temp.set(first[0]);
	// This will be a symbol per usual  
	morse = first.substr(0, size);
	// This will go beyond first space to . and -'s
	morse = line.substr(size + 1, size + 8);
	sizer = (int)morse.size();
	Jerry = convert_string_bit(morse, sizer);
	// Sets 2 of 3 Morser
	temp.set(Jerry);
	// This is where one can make values of dot and dashes for each character (result)
	result = convert_bit_int(Jerry, sizer);
	// This will set the final attribute of Morse temp (integer key)
	temp.set(result);
	morse = line.substr(size + 9);
	// This will push temp into a list of Morser objects
	list_barcode.push_back(temp);
	line = "";
}

/*^^^^^^^ 1 ^^^^^5^^^^^^
This will change the bitset into int value
Used in convert_string_int
Last two parameters are references since this is embedded, so values are constantly different
^^^^^^^^1 ^^^^^5^^^^^^*/
void change_into_a_number(bitset<9> & digit, int &result, int& sizedown)
{
	for (int i = (sizedown-1); i > -1; i--)
	{
		sizedown--;
		result += ((digit[i])*(int)pow(2, sizedown));
	}
}

/*
Algorithm that changes series of n's and w's into 1 and 0
(always different due to order and variety for each character)
Example: nwwnnnwnn -> 011000100 -> 196
(Part of decoder && friend)
*/
bitset<9> convert_string_bit(string morse_code, int size)
{
	string transfer;

	for (int i = 0; i < size; i++)
	{
		if (morse_code[i] == 'n')
		{
			transfer += '0';
		}
		else if (morse_code[i] == 'w')
		{
			transfer += '1';
		}	
	}
	bitset<9>binary(transfer);
	
	return binary;
}

// 011000100 -> 196 by usage of change_into_a_number function (mentioned above)
int convert_bit_int(bitset<9> code, int size)
{
	int result = 0;

	change_into_a_number(code, result, size);

	return result;
}

//===============================================================================================================
/*
Cart class
This will hold a queue of bitset (40) that will be converted into 5 letters/symbols (1001..001 -> APRIC)
*/
class Cart 
{
private:
	int CartID;
	deque<bitset<45>> ProductID;
	int product_count;
	float price_total;
	deque<string> product;
	BarCodeList masterbarlist;
public:
	// Mutators
	void set_cartnumber(int number) { CartID = number; }
	void push_barcode(bitset<45> barbit) { ProductID.push_back(barbit); }
	void set_number_products(int number) { product_count = number; }
	void set_total(float number) { price_total = number; }
	// Accesors 
	int get_cartnumber() const { return CartID; }
	int get_number_product() const { return product_count; }
	float get_price_total() const { return price_total; }
	deque<bitset<45>> return_queue() const { return ProductID; }
	// Empty queue before repopulating
	void clear_queue_bit() { cout << "There are " << product_count << " items in this cart!" << endl; while (!ProductID.empty()) { ProductID.pop_front(); } }
	// Splitting entry into 5 parts for int value which is used to retrieve symbols/letters/numbers for product price retrieval
	void split_40_5();
	// Convert int to symbols/letters/numbers
	char make_first_five(int & num);
};

void Cart::split_40_5()
{
	char told;
	string holder, gold, bold;
	int num;

	for (int i = 0; i < (int)ProductID.size(); i++)
	{
		gold = ProductID[i].to_string();
		for (int j = 0; j < 45; j++)
		{
			holder += gold[j];
			if (j % 9 == 8)
			{
				//cout << "This is the 8-bit bro " << holder << endl;
				bitset<9>tempo(holder);
				num = (int)tempo.to_ulong();
				holder = "";
				told = make_first_five(num);
				bold += told;
			}
		}
		cout << "These are the five letters... " << bold << endl;
		product.push_back(bold);
		cout << "Onto the next one..." << endl;
		bold = "";
	}
}

// This will get the letter associated with the int value (friend)
void give_letter(int i, char &letter, list<BarCode> & holder)
{
	for (auto it : holder)
	{
		if (it.getthird() == i)
		{
			letter = it.getfirst();
			break;
		}
		else {}
	}
}

char Cart::make_first_five(int & rep_num)
{
	char letter;
	list<BarCode> temp = masterbarlist.give_vector();
	
	give_letter(rep_num, letter, temp);

	temp.clear();
	return letter;
}
//*****************************************************************************************
/*
Shop class - this class will handle Cart objects 
This will show the contents of the cart 
*/
class Shop : public Reader , Cart
{
private:
	ProductList masterprodlist;
	vector<Cart> waiting;
	//vector<bitset<40>> Products;
public:
	Shop() { read_file(); }
	void read_file();
	void info_file_MIA()
	{
		cerr << "The file is not in the directory... Must have been moved" << endl;
		system("pause");
		exit(1);
	};
	void xml_breaker(ifstream & line, stack<string> & tag);
	// This will 
	void retrieve_price(Cart & customer);
};

// This will read Products.xml and fill the product list through xml_breaker function
void Shop::read_file()
{
	// Initialize a stack that will hold the product objects which hold the info 
	stack<string> TagStack;

	ifstream read_checkout("Carts.xml");

	if (read_checkout.is_open())
	{
		cout << "File is found! Reading file now...\n";
		while (!read_checkout.eof())
		{
			xml_breaker(read_checkout, TagStack);
		}
	}
	else
	{
		info_file_MIA();
	}
	read_checkout.close();
}

// Uses Regex to get BarCodeList and Product tags
void match_x(string text, string express, stack<string> & tag, bool & flag, bool & nest_tag, Cart & carriage)
{
	sregex_token_iterator end;
	regex pattern(express);
	for (sregex_token_iterator pos(text.begin(), text.end(), pattern); pos != end; ++pos)
	{
		if ((*pos).length() > 0)
		{
			if (static_cast<string>(*pos)[0] == 0x2F)
			{
				if (static_cast<string>(*pos)[1] == 0x58) flag = false;
				tag.pop();
				return;
			}
			else if (static_cast<string>(*pos)[0] != 0x20)
			{
				if (!flag)
				{
					tag.push(*pos);
					flag = true;
				}
				else if (flag && !nest_tag)
				{
					tag.push(*pos);
					size_t os;
					int id;
					string test = static_cast<string>(*pos);
					sregex_token_iterator num_end;
					regex numpattern("[0-9]+");
					for (sregex_token_iterator nos(test.begin(), test.end(), numpattern); nos != num_end; ++nos)
					{
							cout << *nos << endl;
							id = stoi(*nos, &os);
							carriage.set_cartnumber(id);
					}
					nest_tag = true;
				}
			}
		}
	}
}

// Regex to retrieve info for nested tags, such as barcodes 
void match_nest(string text, string express, stack<string> & tag, bool & cart_flag, bool & cart_end, int & count, Cart & carriage)
{
	//size_t os;
	bool inner = false;
	string temp;

	sregex_token_iterator end;
	regex pattern(express);
	for (sregex_token_iterator pos(text.begin(), text.end(), pattern); pos != end; ++pos)
	{
		if (inner)
		{
			string test = static_cast<string>(*pos);
			sregex_token_iterator num_end;
			regex numpattern("[0-1]+");
			for (sregex_token_iterator nos(test.begin(), test.end(), numpattern); nos != num_end; ++nos)
			{
				bitset<45> grump(static_cast<string>(*nos));
				carriage.push_barcode(grump);
				inner = false;
			}
		}
		else if ((*pos).length() > 0)
		{
			if (static_cast<string>(*pos)[0] == 0x2F)
			{
				if(static_cast<string>(*pos)[1] != 0x43) count++;
				else { cart_flag = false; cart_end = true; }
				tag.pop();
				return;
			}
			else if (static_cast<string>(*pos)[0] != 0x20 && !inner)
			{
				tag.push(*pos);
				inner = true;
			}
		}
	}
}

// Uses both match and match_nest functions to retrieve information 
void Shop::xml_breaker(ifstream & doc, stack<string> & tag)
{
	string line, temp, nest_data;
	bool begin_tag = false, cart_tag = false, nest_tag = false, cart_end = false;
	int count = 0;
	Cart TempCart;

	while (getline(doc, line, '\n'))
	{
		if (!begin_tag)	match_x(line, "[a-zA-Z]*", tag, begin_tag, cart_tag, TempCart);
		else if (begin_tag&&!cart_tag) match_x(line, "[a-zA-Z0-9/]*", tag, begin_tag, cart_tag, TempCart);
		else if (cart_tag) { match_nest(line, "[a-zA-Z0-9./]*", tag, cart_tag, cart_end, count, TempCart);  }
		if (count > 0) { TempCart.set_number_products(count);  };
		if (cart_end && begin_tag) { waiting.push_back(TempCart); count = 0; TempCart.split_40_5();TempCart.clear_queue_bit(); cart_end = false; }
	}
}

void Shop::retrieve_price(Cart & customer)
{
	/*
	This function is suppose to use Binary_search to find the product name in masterprodlist, and then its associated price will be retrieved
	and added to the total of Cart that is in reference
	*/
}

//=========================================================================================
// This does not retrieve price info, but this is close. This does not have binary search 
int main()
{
	Shop timothy;

	return 0;
}