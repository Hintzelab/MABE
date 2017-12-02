//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once



///********* 				Synctatic sugar for Paramter Link registration  ********/
//#define _DECL_PARAM_(Type,Name) Type Name; static shared_ptr<ParameterLink<Type>> Name##PL;
//#define _REG_PARAM_(World,Type,Name,DefaultVal,Message) shared_ptr<ParameterLink<Type>> World::Name##PL = Parameters::register_parameter( "WORLD_"#World"-"#Name , DefaultVal , Message );
//#define _INIT_PARAM_(World,Type,Name) Name = (PT == nullptr) ? Name##PL->lookup(): PT->lookup##Type( "WORLD_"#World"-"#Name );
///*Then in ExampleWorld.h  what used to be int exampleVariable; static shared_ptr<ParameterLink<int>> exampleVariablePL; now becomes simply _DECL_PARAM_(int,exampleVariable)
//And in ExampleWorld.cpp, instead of globally registering the parameter as shared_ptr<ParameterLink<int>> ExampleWorld::exampleVariablePL = Parameters::register_parameter("WORLD_ExampleWorld-exampleVariable",  42, "Example messagge for this variable"); we now simply say _REG_PARAM_(ExampleWorld, int, exampleVariable, 42, ""Example messagge for this variable")
//And in the ExampleWorld constructor, instead of looking it up like exampleVariable = (PT == nullptr) ? exampleVariablePL->lookup(): PT->lookupInt("WORLD_ExampleWorld-exampleVariable"); we can say _INIT_PARAM_(ExampleWorldWorld, Int, exameVariable)
//*/
///********* 	End of 			Synctatic sugar for Paramter Link registration  ********/




#include "Utilities.h"
//#include "AssertWithMessage.h"
#include <type_traits>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <set>
#include <memory>
#include <vector>

using namespace std;

//template<class T1, class T2, class Pred = std::less<T2> >
//struct sort_pair_first {
//	bool operator()(const std::pair<T1, T2>&left, const std::pair<T1, T2>&right) {
//		Pred p;
//		return p(left.first, right.first);
//	}
//};
//
//template<class T1, class T2, class Pred = std::less<T2> >
//struct sort_pair_second {
//	bool operator()(const std::pair<T1, T2>&left, const std::pair<T1, T2>&right) {
//		Pred p;
//		return p(left.second, right.second);
//	}
//};

//// this is here so we can use to string and it will work even if we give it a string as input
//inline string to_string(string val) {
//	return val;
//}
//
//template<typename T1, typename T2>
//bool HaveSameType(T1, T2) {
//	return is_same<T1, T2>();
//}
//
//inline string get_var_typename(const bool&) {
//	return "bool";
//}
//inline string get_var_typename(const string&) {
//	return "string";
//}
//inline string get_var_typename(const int&) {
//	return "int";
//}
//inline string get_var_typename(const double&) {
//	return "double";
//}

//inline vector<string> nameSpaceToNameParts(const string& nameSpace) {
//	string localNameSpace = nameSpace;
//	vector<string> nameParts;
//	bool done = (nameSpace.size() == 0);
//	bool nameSpaceValid = true;
//	while (!done) {
//		if (localNameSpace.size() > 2) {  // name space name must end with :: so must have more then 2 characters
//			int i = 0;
//			while ((localNameSpace[i] != ':' || localNameSpace[i + 1] != ':') && (i < (int) localNameSpace.size() - 2)) {
//				i++;
//			}
//			if (!(localNameSpace[i] == ':' && localNameSpace[i + 1] == ':')) {  // if there is not "::" at the end of the name space part
//				nameSpaceValid = false;
//				done = true;
//			} else {  // found "::"
//				nameParts.push_back(localNameSpace.substr(0, i + 2));
//				localNameSpace = localNameSpace.substr(i + 2, localNameSpace.size());
//				if (localNameSpace.size() == 0) {  // if after cutting of part there is nothing left
//					done = true;
//				}
//			}
//		} else {  // if the name space name part is less then 3 characters... ie, must be at least "*::"
//			nameSpaceValid = false;
//			done = true;
//		}
//	}
//	if (!nameSpaceValid) {
//		cout << "  Error::in ParametersTable::pointToNestedTable(string nameSpace). name space is invalid.\n  Can't parse \"" << localNameSpace << "\"\n   Parameter name space must end in ::\nExiting." << endl;
//		exit(1);
//	}
//	return nameParts;
//}

//// Put an arbitrary value to the target variable, return false on conversion failure
//template<class T>
//static bool stringToValue(const string& source, T& target) {
//	std::stringstream ss(source);
//	ss >> target;
//	if (ss.fail()) {
//		return false;
//	} else {
//		string remaining;
//		ss >> remaining;
//		// stream failure means nothing left in stream, which is what we want
//		return ss.fail();
//	}
//}

class AbstractParametersEntry {
protected:
	bool local = 0;
	bool saveOnFileWrite = false;

public:

	inline void setSaveOnFileWrite(bool _saveOnFileWrite = true) {
		saveOnFileWrite = _saveOnFileWrite;
	}

	inline bool saveOnWrite() {
		return saveOnFileWrite;
	}

	virtual inline bool isLocal() {
		return local;
	}

	virtual ~AbstractParametersEntry() = default;

	virtual const bool getBool() {
		cout << "  In AbstractParametersEntry::getBool() - using abstract class method.\n  Exiting!" << endl;
		exit(1);
	}
	virtual const string getString() {
		cout << "  In AbstractParametersEntry::getString() - using abstract class method.\n  Exiting!" << endl;
		exit(1);
	}
	virtual const int getInt() {
		cout << "  In AbstractParametersEntry::getInt() - using abstract class method.\n  Exiting!" << endl;
		exit(1);
	}
	virtual const double getDouble() {
		cout << "  In AbstractParametersEntry::getDouble() - using abstract class method.\n  Exiting!" << endl;
		exit(1);
	}

	virtual const void get(bool& value, const string& name = "") {
		cout << "  In AbstractParametersEntry::get(value,name) - using abstract class method for bool with name \"" + name + "\" - suggests a type mismatch in parameter types.\n  Exiting!" << endl;
		exit(1);
	}
	virtual const void get(string& value, const string& name = "") {
		cout << "  In AbstractParametersEntry::get(value,name) - using abstract class method for string with name \"" + name + "\" - suggests a type mismatch in parameter types.\n  Exiting!" << endl;
		exit(1);
	}
	virtual const void get(int& value, const string& name = "") {
		cout << "  In AbstractParametersEntry::get(value,name) - using abstract class method for int with name \"" + name + "\" - suggests a type mismatch in parameter types.\n  Exiting!" << endl;
		exit(1);
	}
	virtual const void get(double& value, const string& name = "") {
		cout << "  In AbstractParametersEntry::get(value,name) - using abstract class method for double with name \"" + name + "\" - suggests a type mismatch in parameter types.\n  Exiting!" << endl;
		exit(1);
	}

	virtual void set(const bool & val, bool _local = true) {
		cout << "  In AbstractParametersEntry::set(value) - using abstract class method for set(bool) - suggests a type mismatch in parameter types.\n  Exiting!" << endl;
		exit(1);
	}
	virtual void set(const string & val, bool _local = true) {
		cout << "  In AbstractParametersEntry::set(value) - using abstract class method for set(string) - suggests a type mismatch in parameter types.\n  Exiting!" << endl;
		exit(1);
	}
	virtual void set(const int & val, bool _local = true) {
		cout << "  In AbstractParametersEntry::set(value) - using abstract class method for set(int) - suggests a type mismatch in parameter types.\n  Exiting!" << endl;
		exit(1);
	}
	virtual void set(const double & val, bool _local = true) {
		cout << "  In AbstractParametersEntry::set(value) - using abstract class method for set(double) - suggests a type mismatch in parameter types.\n  Exiting!" << endl;
		exit(1);
	}

	virtual void getValuePtr(shared_ptr<bool> &ptr) {
		cout << "  In AbstractParametersEntry::getValuePtr(bool) - using abstract class method.\n  Exiting!" << endl;
		exit(1);
	}
	virtual void getValuePtr(shared_ptr<string> &ptr) {
		cout << "  In AbstractParametersEntry::getValuePtr(string) - using abstract class method.\n  Exiting!" << endl;
		exit(1);
	}
	virtual void getValuePtr(shared_ptr<int> &ptr) {
		cout << "  In AbstractParametersEntry::getValuePtr(int) - using abstract class method.\n  Exiting!" << endl;
		exit(1);
	}
	virtual void getValuePtr(shared_ptr<double> &ptr) {
		cout << "  In AbstractParametersEntry::getValuePtr(double) - using abstract class method.\n  Exiting!" << endl;
		exit(1);
	}
	virtual void follow(shared_ptr<AbstractParametersEntry> &ptr) {
		cout << "  In AbstractParametersEntry::follow() - using abstract class method.\n  Exiting!" << endl;
		exit(1);
	}

	virtual string getTypeName() {
		cout << "  In AbstractParametersEntry::getTypeName() - using abstract class method.\n  Exiting!" << endl;
		exit(1);
	}

	virtual pair<string, string> getStringValuePair() {
		cout << "  In AbstractParametersEntry::getStringValuePair() - using abstract class method.\n  Exiting!" << endl;
		exit(1);
	}

	virtual void show() {
		cout << "  In AbstractParametersEntry::show() - using abstract class method.\n  Exiting!" << endl;
		exit(1);
	}
	virtual shared_ptr<AbstractParametersEntry> makelike(string value, string name = "none provided") {
		cout << "  In AbstractParametersEntry::makelike() - using abstract class method,\n  Exiting!" << endl;
		exit(1);
	}
	virtual void setExisting(string value, string name = "none provided") {
		cout << "  In AbstractParametersEntry::setExisting() - using abstract class method.\n  Exiting!" << endl;
		exit(1);
	}
};

template<typename T>
class ParametersEntry : public AbstractParametersEntry {
private:
	shared_ptr<T> valuePtr;

public:
	ParametersEntry() = default;

	ParametersEntry(T value, bool _local = true) {
		set(value, _local);
	}

	const T & get() {
		return *valuePtr;
	}

	const void get(T& value, const string& name) override {
		value = *valuePtr;
	}

	const bool getBool() override {
		cout << "  In ParametersEntry::GetBool() value associated with this ParamatersEntry is not bool.\n  Exiting!" << endl;
		exit(1);
	}

	const string getString() override {
		cout << "  In ParametersEntry::getString() value associated with this ParamatersEntry is not string.\n  Exiting!" << endl;
		exit(1);
	}

	const int getInt() override {
		cout << "  In ParametersEntry::getInt() value associated with this ParamatersEntry is not int.\n  Exiting!" << endl;
		exit(1);
	}

	const double getDouble() override {
		cout << "  In ParametersEntry::getDouble() value associated with this ParamatersEntry is not double.\n  Exiting!" << endl;
		exit(1);
	}

	void set(const T & value, bool _local = true) override {
		if (valuePtr == nullptr) {
			valuePtr = make_shared<T>(value);
		}
		else {
			*valuePtr = value;
		}
		local = _local;
	}

	void getValuePtr(shared_ptr<T> &ptr) override {
		if (ptr == nullptr) {
			//ASSERT(valuePtr != nullptr, "in ParametersEntry(shared_ptr<T> &ptr) :: Attempt to get shared pointer from broken pointer!");
			cout << "  In ParametersEntry::getValuePtr() - Attempt to get shared pointer from broken pointer!\n  Exiting!" << endl;
			exit(1);
		}

		ptr = valuePtr;
	}

	void follow(shared_ptr<AbstractParametersEntry> &ptr) override {
		if (ptr == nullptr) {
			cout << "  In ParametersEntry::follow() - ptr is a nullptr" << endl;
			exit(1);
		}
		shared_ptr<ParametersEntry<T>> castParametersEntry = dynamic_pointer_cast<ParametersEntry<T>>(ptr);
		if (castParametersEntry->valuePtr == nullptr) {
			cout << "  In ParametersEntry::follow() - value in ptr is a nullptr" << endl;
			exit(1);
		}
		valuePtr = castParametersEntry->valuePtr;
		local = false;
	}

	string getTypeName() {
		return get_var_typename(*valuePtr);
	}

	// return a pair of string with <value,type>
	pair<string, string> getStringValuePair() override {
		ostringstream value;
		string trimmedValue = "";
		value << fixed << *valuePtr;
		if (getTypeName() == "double") {
			bool foundDot = false;
			bool foundNonZero = false;

			int i = (int)value.str().size() - 1;
			while (i >= 0) {
				if (value.str()[i] != '0') {
					foundNonZero = true;
				}
				if (value.str()[i] == '.') {
					foundDot = true;
					if (trimmedValue == "") {
						trimmedValue = "0";
					}
				}
				if (value.str()[i] != '0' || foundNonZero) {
					trimmedValue = value.str()[i] + trimmedValue;
				}
				i--;
			}
			if (!foundDot) {
				trimmedValue = value.str();
			}
			return { trimmedValue,getTypeName() };
		}
		return { value.str(),getTypeName() };
	}

	void show() override {
		cout << "\n        Value = ";
		cout << *valuePtr << " (" << get_var_typename(*valuePtr) << ")";
		if (local) {
			cout << " is Local";
		}
		else {
			cout << " is not Local";
		}
		cout << endl;
		cout << "        saveOnFileWrite: ";
		if (saveOnFileWrite) {
			cout << "yes" << endl;
		}
		else {
			cout << "no" << endl;

		}
	}

	shared_ptr<AbstractParametersEntry> makelike(string value, string name = "none provided") override {
		auto newEntry = make_shared<ParametersEntry<T>>();
		auto tempVal = *valuePtr;
		if (load_value(value, tempVal)) {
			newEntry->valuePtr = make_shared<T>(tempVal);
		}
		else {
			cout << "  in ParametersEntry::makelike() attempting to setup a parameter with name \"" << name << "\"... string value provided to function could not be converted to type of entry!\n  Exiting" << endl;
			exit(1);
		}
		newEntry->local = true;

		return newEntry;
	}

	void setExisting(string value, string name = "none provided") override {
		auto tempVal = *valuePtr;
		if (load_value(value, tempVal)) {
			*valuePtr = tempVal;
			local = true;
		}
		else {
			cout << "  in ParametersEntry::setExisting() attempting to set a parameter with name \"" << name << "\"... string value provided to function could not be converted to type of entry!\n  Exiting" << endl;
			exit(1);
		}
	}

};

class ParametersTable {
private:
	static long long nextTableID;
	long long ID;
	string tableNameSpace;
	shared_ptr<ParametersTable> rootTable;
	shared_ptr<ParametersTable> parent;
	unordered_map<long long, shared_ptr<ParametersTable>> children;
	unordered_map<string, shared_ptr<AbstractParametersEntry>> table;
	shared_ptr<map<string, shared_ptr<ParametersTable>>> parametersTablesRegistry;
	shared_ptr<map<string, string>> parameterDocumentation;  // for each parameter name, contains documentation

	string workString;

	void setID() {
		ID = nextTableID++;
	}

	void clearParent() {
		if (parent != nullptr) {  // if there is a parent, make sure to remove this table from that parents children so that this will be deleted.
			parent->children.erase(ID);
			parent.reset();
		}
	}

	void addChild(shared_ptr<ParametersTable> _child) {
		children[_child->ID] = _child;
	}

public:
	bool neverSave = 0;

	ParametersTable() = delete;

	ParametersTable(const string& _tableNameSpace = "", shared_ptr<ParametersTable> _rootTable = nullptr) :
		tableNameSpace(_tableNameSpace), rootTable(_rootTable) {
		setID();
	}

	~ParametersTable() = default;

	long long getID() {
		return ID;
	}

	shared_ptr<AbstractParametersEntry> getEntry(const string & name) {
		if (table.find(name) != table.end()) {
			return table[name];
		}
		else {
			cout << "   in ParametersTable::getEntry :: can not find entry with name \"" << name << "\".\n  exiting." << endl;
			exit(1);
		}
	}

	string getTableNameSpace() {
		return tableNameSpace;
	}

	string getParameterType(const string& name) {
		if (rootTable->table.find(name) == table.end()) {
			return("FAIL");
		}
		else {
			return (rootTable->table[name]->getTypeName());
		}
	}

	static shared_ptr<ParametersTable> addTableWithParent(const string& _tableNameSpace, shared_ptr<ParametersTable> _rootTable, shared_ptr<ParametersTable> _parent) {
		shared_ptr<ParametersTable> newTable = make_shared<ParametersTable>(_tableNameSpace, _rootTable);
		if (_parent == nullptr) {  //this will be a root table
			newTable->parametersTablesRegistry = make_shared<map<string, shared_ptr<ParametersTable>>>();
			newTable->parameterDocumentation = make_shared<map<string, string>>();// for each parameter name, contains documentation
		}
		else {
			newTable->parent = _parent;
			newTable->parent->addChild(newTable);
			newTable->parametersTablesRegistry = _parent->parametersTablesRegistry;
			newTable->parameterDocumentation = _parent->parameterDocumentation;
		}
		(*newTable->parametersTablesRegistry)[_tableNameSpace] = newTable;
		return newTable;
	}

	static shared_ptr<ParametersTable> makeTable(const string& _tableNameSpace = "", shared_ptr<ParametersTable> _rootTable = nullptr) {
		if (_rootTable == nullptr) {
			shared_ptr<ParametersTable> newTable = addTableWithParent(_tableNameSpace, _rootTable, nullptr);
			newTable->rootTable = newTable;
			return newTable;
		}
		else if ((*_rootTable->parametersTablesRegistry).find(_tableNameSpace) != (*_rootTable->parametersTablesRegistry).end()) {
			cout << "  Warning :: call to makeTable. table with nameSpace: \"" << _tableNameSpace << "\" already exists. Ignoring..." << endl;
			return (*_rootTable->parametersTablesRegistry)[_tableNameSpace];
		}
		else {
			shared_ptr<ParametersTable> searchTable = nullptr;
			vector<string> nameSpaceParts = nameSpaceToNameParts(_tableNameSpace);
			int index = 0;
			bool looking = true;
			string nameSpace = "";
			while (index < (int)nameSpaceParts.size() && looking) {  // as long as we keep finding tables in the registry, keep checking down
				nameSpace += nameSpaceParts[index];
				auto s = (*_rootTable->parametersTablesRegistry).find(nameSpace);
				if (s != (*_rootTable->parametersTablesRegistry).end()) {
					searchTable = s->second;  // found a table, keep looking to see if there is one farther down
				}
				else {
					looking = false;  // we did not find a table! searchTable is the last table we found (or nullptr) and paramaterName is in a table we must make
				}
				index++;
			}
			if (searchTable == nullptr) {
				searchTable = _rootTable;
			}
			shared_ptr<ParametersTable> newTable = addTableWithParent(nameSpace, _rootTable, searchTable);
			searchTable = newTable;
			while (index < (int)nameSpaceParts.size()) {
				nameSpace += nameSpaceParts[index];
				newTable = addTableWithParent(nameSpace, _rootTable, searchTable);
				searchTable = newTable;
				index++;
			}
			return searchTable;
		}
	}

	// looks for a table with name space _tableNameSpace. if found, return shared ptr to table, if not, create table and return shared ptr
	shared_ptr<ParametersTable> getTable(const string& _tableNameSpace) {
		if ((*rootTable->parametersTablesRegistry).find(_tableNameSpace) != (*rootTable->parametersTablesRegistry).end()) {
			return (*rootTable->parametersTablesRegistry)[_tableNameSpace];
		}
		else {
			return makeTable(_tableNameSpace, rootTable);
		}
	}

	// looks for a table with name space _tableNameSpace; if not found error out.
	shared_ptr<ParametersTable> lookupTable(const string& _tableNameSpace) {
		if ((*parametersTablesRegistry).find(_tableNameSpace) == (*parametersTablesRegistry).end()) {
			cout << "  In ParametersTable::lookupTable()  - Attempt to lookup nonexistent table with name \"" + _tableNameSpace + "\". Exiting! " << endl;
			exit(1);
		}
		return (*parametersTablesRegistry)[_tableNameSpace];
	}

	// finds table where name exists, starting at searchTable and working up to root. If not found, terminate.
	shared_ptr<ParametersTable> findTableWithNamedParameter(const string& name, shared_ptr<ParametersTable> searchTable) {
		bool found = false;
		while (searchTable != nullptr && !found) {  // while findTable is valid and we have not found the value
			//cout << "searching" << searchTable->tableNameSpace << " for " << name << endl;
			if (searchTable->table.find(name) == searchTable->table.end()) {  // if the value is not here, point to the parent.
				searchTable = searchTable->parent;
			}
			else if (!searchTable->table[name]->isLocal()) {  // if the value is here, but is not local, point to the parent.
				searchTable = searchTable->parent;
			}
			else {  // if the value is here and is local, then set found = true
				found = true;
			}
		}
		return searchTable;
		// searchTable either points to a valid table which contains a local value for the named parameter or is a nullptr if not found
	}

	//return a shared_ptr to Entry in this table with name. If not found, search for it higher.
	shared_ptr<ParametersEntry<bool>> lookupBoolEntry(const string& name) {
		if (table.find(name) == table.end()) {
			lookupBool(name);
		}
		return dynamic_pointer_cast<ParametersEntry<bool>>(table[name]);
	}

	shared_ptr<ParametersEntry<string>> lookupStringEntry(const string& name) {
		if (table.find(name) == table.end()) {
			lookupString(name);
		}
		return dynamic_pointer_cast<ParametersEntry<string>>(table[name]);
	}

	shared_ptr<ParametersEntry<int>> lookupIntEntry(const string& name) {
		if (table.find(name) == table.end()) {
			lookupInt(name);
		}
		return dynamic_pointer_cast<ParametersEntry<int>>(table[name]);
	}

	shared_ptr<ParametersEntry<double>> lookupDoubleEntry(const string& name) {
		if (table.find(name) == table.end()) {
			lookupDouble(name);
		}
		return dynamic_pointer_cast<ParametersEntry<double>>(table[name]);
	}

	template<typename T>
	void lookup(const string& name, T& value) {
		// check for the name in this table
		if (table.find(name) != table.end()) {  // if this table has entry called name
			table[name]->get(value, name);

		}
		else {
			shared_ptr<ParametersTable> searchTable;// used to determine which ancestor table we are looking at
			if (parent != nullptr) {
				searchTable = parent;
			}
			else {
				searchTable = rootTable;
			}
			searchTable = findTableWithNamedParameter(name, searchTable);  // find the name in the closest ancestor table with named parameter
			if (searchTable == nullptr) {
				cout << "  ERROR! :: in ParametersTable::lookup(const string& name, bool& value) - could not find \"" << tableNameSpace << name << "\" in parameters tables! Exiting!" << endl;
				exit(1);
			}
			searchTable->table[name]->get(value, name);// assign value
			table[name] = make_shared<ParametersEntry<T>>();
			table[name]->follow(searchTable->table[name]);
		}
	}

	inline bool lookupBool(const string& name) {
		bool value;
		lookup(name, value);
		return value;
	}
	inline string lookupString(const string& name) {
		string value;
		lookup(name, value);
		return value;
	}
	inline int lookupInt(const string& name) {
		int value;
		lookup(name, value);
		return value;
	}
	inline double lookupDouble(const string& name) {
		double value;
		lookup(name, value);
		return value;
	}

	inline bool lookupBool(const string& name, const string& _tableNameSpace) {
		if (parametersTablesRegistry->find(_tableNameSpace) == parametersTablesRegistry->end()) {
			cout << "  ERROR! :: in ParametersTable::lookupBool(const string& name) - could not find requested tableNameSpace\"" << _tableNameSpace << "\" in parameters tables! Exiting!" << endl;
			exit(1);
		}
		bool value;
		(*parametersTablesRegistry)[_tableNameSpace]->lookup(name, value);
		return value;
	}
	inline string lookupString(const string& name, const string& _tableNameSpace) {
		if (parametersTablesRegistry->find(_tableNameSpace) == parametersTablesRegistry->end()) {
			//ASSERT(parametersTablesRegistry->find(_tableNameSpace)!=parametersTablesRegistry->end(),"  ERROR! :: in ParametersTable::lookupstring(const string& name) - could not find requested tableNameSpace\"" << _tableNameSpace << "\" in parameters tables! Exiting!");
			cout << "  ERROR! :: in ParametersTable::lookupstring(const string& name) - could not find requested tableNameSpace\"" << _tableNameSpace << "\" in parameters tables! Exiting!" << endl;
			exit(1);
		}
		string value;
		(*parametersTablesRegistry)[_tableNameSpace]->lookup(name, value);
		return value;
	}
	inline int lookupInt(const string& name, const string& _tableNameSpace) {
		if (parametersTablesRegistry->find(_tableNameSpace) == parametersTablesRegistry->end()) {
			//ASSERT(parametersTablesRegistry->find(_tableNameSpace)!=parametersTablesRegistry->end(),"  ERROR! :: in ParametersTable::lookupInt(const string& name) - could not find requested tableNameSpace\"" << _tableNameSpace << "\" in parameters tables! Exiting!");
			cout << "  ERROR! :: in ParametersTable::lookupInt(const string& name) - could not find requested tableNameSpace\"" << _tableNameSpace << "\" in parameters tables! Exiting!" << endl;
			exit(1);
		}
		int value;
		(*parametersTablesRegistry)[_tableNameSpace]->lookup(name, value);
		return value;
	}
	inline double lookupDouble(const string& name, const string& _tableNameSpace) {
		if (parametersTablesRegistry->find(_tableNameSpace) == parametersTablesRegistry->end()) {
			//ASSERT(parametersTablesRegistry->find(_tableNameSpace)!=parametersTablesRegistry->end(),"  ERROR! :: in ParametersTable::lookupDouble(const string& name) - could not find requested tableNameSpace\"" << _tableNameSpace << "\" in parameters tables! Exiting!");
			cout << "  ERROR! :: in ParametersTable::lookupDouble(const string& name) - could not find requested tableNameSpace\"" << _tableNameSpace << "\" in parameters tables! Exiting!" << endl;
			exit(1);
		}
		double value;
		(*parametersTablesRegistry)[_tableNameSpace]->lookup(name, value);
		return value;
	}

	// set value in this table, or named table if provided (creates if needed) - will also set this value in rootTable if the value does not exist
	// add or overwrite to this table and add to root if not there.
	template<typename T>
	void setParameter(const string& name, const T& value, const string& _tableNameSpace = "'", bool _saveOnFileWrite = false) {
		string localTableNameSpace = (_tableNameSpace == "'") ? tableNameSpace : _tableNameSpace;
		//cout << "in setParameter :: tableNameSpace: " << tableNameSpace << "   localTableNameSpace: " << localTableNameSpace << endl;
		if (localTableNameSpace != tableNameSpace) {  // if this table is not the table we are writing to...nameSpaceToNameParts
			if ((*parametersTablesRegistry).find(localTableNameSpace) != (*parametersTablesRegistry).end()) {  // if the table we are writing to exists...
				(*parametersTablesRegistry)[localTableNameSpace]->setParameter(name, value, localTableNameSpace, _saveOnFileWrite);// go to the table and set the value
			}
			else {  // if the table we are writing to does not exist...
				makeTable(localTableNameSpace, rootTable)->setParameter(name, value, localTableNameSpace, _saveOnFileWrite);// make the table and set value in table
			}
		}
		else {  // if this is the table we are writing to...
			if (table.find(name) != table.end()) {  // if this table has entry called name
				if (table[name]->isLocal()) {
					table[name]->set(value);
				}
				else { // if this paramerter (in this table) is not local, then we must make a new local entry in this table!
					table[name] = make_shared<ParametersEntry<T>>();
					table[name]->set(value);
				}
			}
			else {  // this table does not have entry called name
				table[name] = make_shared<ParametersEntry<T>>(value);
				if (parent != nullptr) {  // if this is not a root table...
					if (rootTable->table.find(name) == rootTable->table.end()) {  // if this value is not yet set in the root table. create it there also
						rootTable->table[name] = make_shared<ParametersEntry<T>>(value);
					}
				}
			}
			vector<ParametersTable> checklist;  // make a checklist of all children tables from the current table
			for (auto c : children) {
				checklist.push_back(*c.second);
			}
			while (checklist.size() > 0) {
				ParametersTable checkTable = checklist[checklist.size() - 1];  // get last element in checklist
				checklist.pop_back();
				if (checkTable.table.find(name) != checkTable.table.end()) {  // if this table has entry called name
					if (!checkTable.table[name]->isLocal()) {  // if it's not local, then we need to change where it's looking
						cout << "checkTable name: " << checkTable.tableNameSpace << endl;
						checkTable.table[name]->follow(table[name]);
						for (auto c : checkTable.children) {  // and add it's children to checklist
							checklist.push_back(*c.second);
						}
					}  // if isLocal we don't do anything! (if children are referencing, they are referencing here)
				}
				else {  // if name is not here, add children to checklist (a child may be referencing above.)
					for (auto c : checkTable.children) {  // and add it's children to checklist
						checklist.push_back(*c.second);
					}
				}
			}
			if (_saveOnFileWrite) {
				table[name]->setSaveOnFileWrite(true);
			}
			//return table[name];
		}

	}

	// setExistingParameter is used to set the value of a parameter in this table, but only if that parameter already exists.
	// the name / value pair is string / string, and the value is converted baised on the type of the parameter as it has already been defined.
	void setExistingParameter(string name, string value) {
		if (table.find(name) != table.end() && table[name]->isLocal()) { // if this table has entry called name and it is local
			table[name]->setExisting(value, name);
		}
		else { // name not found in this table, or it is here, but not local, check root
			if (rootTable->table.find(name) != table.end()) { // Good, it's in the root table, we will need to add it here!
				auto newEntry = rootTable->table[name]->makelike(value, name); // get entry from root table (this will get us the type!)
				table[name] = newEntry; // assign it in this table
				///// This is copied from setParameter --- it makes sure that children of this table know to look here!
				vector<ParametersTable> checklist;  // make a checklist of all children tables from the current table
				for (auto c : children) {
					checklist.push_back(*c.second);
				}
				while (checklist.size() > 0) {
					ParametersTable checkTable = checklist[checklist.size() - 1];  // get last element in checklist
					checklist.pop_back();
					if (checkTable.table.find(name) != checkTable.table.end()) {  // if this table has entry called name
						if (!checkTable.table[name]->isLocal()) {  // if it's not local, then we need to change where it's looking
							//checkTable.table[name]->set(name,false);
							checkTable.table[name]->follow(table[name]);
							for (auto c : checkTable.children) {  // and add it's children to checklist
								checklist.push_back(*c.second);
							}
						}  // if isLocal we don't do anything! (if children are referencing, they are referencing here)
					}
					else {  // if name is not here, add children to checklist (a child may be referencing above.)
						for (auto c : checkTable.children) {  // and add it's children to checklist
							checklist.push_back(*c.second);
						}
					}
				}
				///// end of copy from setParameter

			}
			else { // This value does not exist in the system!
				cout << "  in ParametersTable::setExistingParameter():: attempting to set parameter with name \"" << name << "\" and value \"" << value << "\", but that name is not registered! Please check the name.\n  Exiting." << endl;
				exit(1);
			}
		}
	}

	// attempt to delete a value from this table also, remove any non-local children who are relying on this value.
	// if removing from root, delete all paramaters with this name in table.
	void deleteParameter(const string& name) {
		if (table.find(name) == table.end()) {
			//ASSERT((table.find(name) != table.end()), "  ERROR! :: attempt to remove non-existent \"" << tableNameSpace << name << "\". Exiting");
			cout << "  ERROR! In ParametersTable::deleteParameter(name) - attempt to remove non-existent \"" << tableNameSpace << name << "\". Exiting" << endl;
			exit(1);
		}
		if (tableNameSpace == "") {  // if we are in the root table
			(*parameterDocumentation).erase(name);
			// remove parameter from all children
			vector<shared_ptr<ParametersTable>> checklist;
			for (auto c : children) {
				checklist.push_back(c.second);
			}
			while (checklist.size() > 0) {
				if (checklist[0]->table.find(name) != checklist[0]->table.end()) {
					checklist[0]->table.erase(name);
				}
				for (auto c : checklist[0]->children) {
					checklist.push_back(c.second);
				}
				swap(checklist.front(), checklist.back());
				checklist.pop_back();
			}
		}
		else {  // this is not a root table, only remove parameters from child tables if not local and looking at this table.
			vector<shared_ptr<ParametersTable>> checklist;
			for (auto c : children) {
				if (c.second->table.find(name) != c.second->table.end()) {  // if child has name...
					if (!c.second->table[name]->isLocal()) {  // and childs name is not local...
						c.second->table.erase(name);// delete name and ...
						checklist.push_back(c.second);// add child to checklist
					}  // if child has name and it is local, don't do anything. (if any children are not local, they will be looking here)
				}
				else {  // if child does not have name...
					checklist.push_back(c.second);// add child to list (so that it's children are checked.
				}
			}
			while (checklist.size() > 0) {
				for (auto c : checklist[0]->children) {
					if (c.second->table.find(name) != c.second->table.end()) {  // if child has name...
						if (!c.second->table[name]->isLocal()) {  // and childs name is not local...
							c.second->table.erase(name);// delete name and ...
							checklist.push_back(c.second);// add child to checklist
						}  // if child has name and it is local, don't do anything. (if any children are not local, they will be looking here)
					}
					else {  // if child does not have name...
						checklist.push_back(c.second);// add child to list (so that it's children are checked.
					}
				}
				swap(checklist.front(), checklist.back());
				checklist.pop_back();
			}
		}
		table.erase(name);
	}

	// deleteParamatersTable deletes table with nameSpace. Any children of this table are also removed.
	// note, external shared_ptrs to this table (or others in the tree) may still be valid, and will
	// cause the tables to remain active. This should not be a problem, as objects leave scope the tables will
	// auto delete. Also note that shared_ptrs to parameterEntries will still be valid, and this is the desired behavior
	void deleteParamatersTable() {
		while (children.size() > 0) {
			children.begin()->second->deleteParamatersTable();
		}
		clearParent();
		rootTable.reset();
		(*parametersTablesRegistry).erase(tableNameSpace);
	}

	// set the value in this table (create is needed) (and in root - see set). also assigns usage data, only registered parameters can be saved to files
	void setSaveOnFileWrite(const string& name, bool setting = true) {
		if (table.find(name) != table.end()) {
			table[name]->setSaveOnFileWrite(setting);
		}
		else {
			cout << "  Warning :: call to setSaveOnFileWrite. parameter \"" << tableNameSpace << name << "\" not found. Ignoring..." << endl;
		}
	}

	void setDocumentation(const string& name, const string& _documentation) {
		if (table.find(name) != table.end()) {
			(*parameterDocumentation)[name] = _documentation;
		}
		else {
			cout << "  Warning :: call to set documentation. parameter \"" << tableNameSpace << name << "\" not found. Ignoring..." << endl;
		}
	}

	template<typename T>
	shared_ptr<ParametersEntry<T>> register_parameter(const string& name, const T& default_value, const string& documentation) {
		if (table.find(name) != table.end()) {  // if this table has entry called name
			cout << "  WARRNING!! Attempting to register \"" << tableNameSpace << name << "\", but it already exists!\n      Updating with new values\n";
		}
		setParameter(name, default_value, rootTable->tableNameSpace);
		setSaveOnFileWrite(name);
		setDocumentation(name, documentation);
		return dynamic_pointer_cast<ParametersEntry<T>>(table[name]);
	}

	vector<shared_ptr<ParametersTable>> getChildren() {
		vector<shared_ptr<ParametersTable>> kids;
		for (auto c : children) {
			kids.push_back(c.second);
		}
		return kids;
	}

	void parametersToSortedList(map<string, vector<string>>& sorted_parameters) {

		sorted_parameters.clear();
		ostringstream build_string;
		string workingNameSpace, workingCategory, workingParameterName, workingParameterType;
		pair<string, string> workingValue;

		for (auto p : table) {
			if (p.second->saveOnWrite()) {
				build_string.str("");
				int i = 0;
				while (p.first[i] != '-' && i < (int)p.first.size()) {  // find "-", separator between category and name
					i++;
				}
				if (i >= (int)p.first.size() - 1) {  // if there is not a "-" and at least one character after that... there is a problem.
					cout << "  ERROR :: in ParametersTable::parametersToSortedList(), found misformatted parameter with name \"" << p.first << "\". Possibly missing category. Exiting!" << endl;
					exit(1);
				}
				workingCategory = p.first.substr(0, i);
				workingParameterName = p.first.substr(i + 1, p.first.size());

				workingParameterType = getParameterType(p.first);

				//WRITE GET STRING OF VALUE!
				workingValue = p.second->getStringValuePair();
				build_string << fixed << workingParameterName << " = " << workingValue.first;


				build_string << " @@@# (" << workingValue.second << ") " << (*parameterDocumentation)[p.first];
				sorted_parameters[workingCategory].push_back(build_string.str());
			}
		}

		for (auto c : sorted_parameters) {
			sort(sorted_parameters[c.first].begin(), sorted_parameters[c.first].end());
		}
	}

	void show(bool showChildren = true) {
		cout << "\nTABLE: ";
		if (parent == nullptr) {
			cout << "(ROOT) " << tableNameSpace;
		}
		else {
			cout << tableNameSpace;
		}
		cout << "\n    ID: " << getID() << endl;
		if (parent != nullptr) {
			if (parent->parent == nullptr) {
				cout << "    parent name space: (ROOT) " << parent->tableNameSpace << endl;
			}
			else {
				cout << "    parent name space: " << parent->tableNameSpace << endl;
			}
		}
		else {
			cout << "    parent name space: NULLPTR (this is ROOT) " << endl;
		}
		cout << "    Children (" << children.size() << "):  ";
		for (auto c : children) {
			cout << c.second->tableNameSpace << "  ";
		}
		cout << endl;
		cout << "      Parameters in this table:" << endl;
		for (auto v : table) {  // show all parameters in this table
			cout << "      " << v.first << "  ";
			v.second->show();
			cout << "        documentation: " << (*parameterDocumentation)[v.first] << endl;
		}
		if (showChildren) {
			for (auto c : children) {
				c.second->show();
			}
		}
	}

};

template<typename T>
class ParameterLink {
public:
	string name;
	shared_ptr<ParametersEntry<T>> entry; // points to a parameters entry
	shared_ptr<ParametersTable> table; // the table that owns this entry
	map<long long, shared_ptr<ParametersEntry<T>>> entriesCache; // used to track entries in other name spaces

	ParameterLink(string _name, shared_ptr<ParametersEntry<T>> _entry, shared_ptr<ParametersTable> _table) :
		name(_name), entry(_entry), table(_table) {
	}

	~ParameterLink() = default;


	T get() {
		return entry->get();
	}

	T get(shared_ptr<ParametersTable> lookupTable) {
		//cout << "in lookup with PT    with name: " << name << endl;
		if (lookupTable == nullptr) {
			cout << "  in ParameterLink::get(lookupTable) :: while looking up \"" << name << "\", lookupTable passed is a nullptr! I can not get an ID!. exiting..." << endl;
			exit(1);
		}
		auto mapRecord = entriesCache.find(lookupTable->getID());
		if (mapRecord == entriesCache.end()) { // if the cache does not contain this table
			T lookupValue;
			lookupTable->lookup(name, lookupValue);
			entriesCache[lookupTable->getID()] = dynamic_pointer_cast<ParametersEntry<T>>(lookupTable->getEntry(name));
			return lookupValue;
		}
		return entriesCache[lookupTable->getID()]->get();
	}

	//T lookup() {
	//	return get();
	//}

	//T lookup(shared_ptr<ParametersTable> lookupTable) {
	//	return get(lookupTable);
	//}

	void set(T value) {
		table->setParameter(name, value);
		entriesCache[table->getID()] = dynamic_pointer_cast<ParametersEntry<T>>(table->getEntry(name));
	}

	void set(T value, shared_ptr<ParametersTable> lookupTable) {
		lookupTable->setParameter(name, value);
		entriesCache[lookupTable->getID()] = dynamic_pointer_cast<ParametersEntry<T>>(lookupTable->getEntry(name));
	}

	void clearCache() {
		entriesCache.clear();
	}
	
	void clearCache(shared_ptr<ParametersTable> _table) {
		auto mapRecord = entriesCache.find(_table->getID());
		if (mapRecord != entriesCache.end()) { // if the cache contain this _table(parameterstable)
			entriesCache.erase(mapRecord); // remove the entry from the entiresCache
		}
		// else do nothing, there is not entry for _table in this PL
	}

	void clearCache(vector<shared_ptr<ParametersTable>> _tables) {
		for (auto table : _tables) { // clear each parametersTables entry in the entriesCache
			clearCache(table);
		}
	}

};

class Parameters {
public:
	static shared_ptr<ParametersTable> root;

	template<typename T>
	static shared_ptr<ParameterLink<T>> register_parameter(const string& name, const T& default_value, const string& documentation) {
		if (root == nullptr) {
			root = ParametersTable::makeTable();
		}
		auto entry = root->register_parameter(name, default_value, documentation);
		auto newLink = make_shared<ParameterLink<T>>(name, entry, root);
		return newLink;
	}

	static shared_ptr<ParameterLink<bool>> getBoolLink(const string& name, shared_ptr<ParametersTable> table);
	static shared_ptr<ParameterLink<string>> getStringLink(const string& name, shared_ptr<ParametersTable> table);
	static shared_ptr<ParameterLink<int>> getIntLink(const string& name, shared_ptr<ParametersTable> table);
	static shared_ptr<ParameterLink<double>> getDoubleLink(const string& name, shared_ptr<ParametersTable> table);

	static void parseFullParameterName(const string& fullName, string& nameSpace, string& category, string& parameterName);
	static void readCommandLine(int argc, const char** argv, unordered_map<string, string>& comand_line_list, vector<string>& fileList) {
		bool temp = false;
		readCommandLine(argc, argv, comand_line_list, fileList, temp);
	}
	static void readCommandLine(int argc, const char** argv, unordered_map<string, string>& comand_line_list, vector<string>& fileList, bool& saveFiles);
	static unordered_map<string, string> readParametersFile(string fileName);
	static bool initializeParameters(int argc, const char * argv[]);
	static void saveSettingsFile(const string& nameSpace, stringstream& FILE, vector<string> categoryList, int _maxLineLength, int _commentIndent, bool alsoChildren = false, int nameSpaceLevel = 0);
	static void saveSettingsFiles(int _maxLineLength, int _commentIndent, vector<string> nameSpaceList = { "*" }, vector<pair<string, vector<string>>> categoryLists = { {"settings.cfg", {""}} });
	static void printParameterWithWraparound(stringstream& FILE, string _currentIndent, string _parameter, int _maxLineLength, int _commentIndent);
};

