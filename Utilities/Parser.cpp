// author : cgnitash
// parser.cpp contains implementation of population loading scripting language


#include<iostream>
#include<string>
#include<map>
#include<unordered_map>
#include<vector>
#include<sstream>
#include<fstream>
#include<regex>
#include<numeric>
#include<random>
#include "Parser.h"

using std::cout;
using std::endl;

std::vector<std::pair<long,std::unordered_map<std::string,std::string>>>
loader::load_population(const std::string & loader_file_name){
	
	std::ifstream flines(loader_file_name);	
		if (!flines.is_open()) {
			std::cout << " error: population_loader file does not exist" << std::endl;
			exit(1);
		}

	cout << endl << "From population script \"" << loader_file_name << "\"" << endl;

	// read lines, strip out all trailing comments, and create one long string
	std::string line, all_lines = " ";
	std::regex comments("#.*");
	while (getline(flines,line)) {
		if (!line.empty()) {
			std::string clean_line = std::regex_replace(line,comments,"");
			all_lines += " " + clean_line;
		}
	}


	std::regex garb("^\\s*(load|choose|init_pop).*$"); 
	// file must start with a load/choose/oinit_pop command	
	if (!std::regex_match(all_lines,garb)) {
		cout << " error : loader file contains unrezognized text at beginning " << endl;
		exit(1);
	}

	vector<string> commands;
	// split into commands
	std::regex command("(?:load|choose|init_pop).*?(?=(?:(load|choose|init_pop))|$)");
		// match 
		// one of the keywords but don't capture
		// lookahead for another keyword(without capturing) or end of line 
	for( std::sregex_iterator end,
		i = std::sregex_iterator (all_lines.begin(), all_lines.end(),command);
			i != end; i++) {
		commands.push_back( (*i).str() ); 
	}	

	std::regex init_pop("^init_pop\\s+(\\w+)\\s*");	//syntax for init_pop
	auto initrep = 0;
	for (const auto & c : commands)  
		initrep += std::regex_match(c,init_pop); 	
	if (initrep > 1) {
		cout << " error : loader file must contain at most one init_pop command " << endl;
		exit(1);
	}


	std::regex load("^load\\s+(\\w+)\\s+with\\s?(.*)"); // syntax for load
	for (const auto & c : commands) {	// figure out which  populations are needed
			std::smatch m;
			if (std::regex_match(c,m,load)) 	
				cLOAD(m[1],m[2]);		// load population file into variable
	}

	for(const auto & file: actual_files) { // load all populations from file ONCE. 
				// this creates a SINGLE list of organisms that can be efficiently accessed
	//		std::cout << std::endl  << file << std::endl;
			generate_population(file);
	}
				

	std::regex choose("^choose\\s+(\\w+)\\s+as\\s?(.*)"); // syntax for choose 
	for (const auto & c :commands ) {	// figure out which expressions need to be parsed
		std::smatch m;
		if (std::regex_match(c,m,choose)) { 
				cCHOOSE(m[1],m[2]);				// parse them
		}
	}

	std::vector<std::pair<long,std::unordered_map<std::string,std::string>>> 
	final_population;

	for (const auto & c : commands) {
		std::smatch m;
		if (std::regex_match(c,m,init_pop)) {	// regex constructed above
			auto token = m[1].str();
			if (std::find(token_names.begin(), token_names.end(), token) 
				== token_names.end()) {
				cout << " error: undefined token " << token << endl;
				exit(1);
			}
			std::cout <<"\tinitializing population with organisms ..." << std::endl;
			for (auto  i : token_org_list[token]) {
				print_organism(i)  ; // should be written to file for replicability 
				if (i != -1)
					final_population.push_back(
						std::make_pair(i,all_organisms[i].attributes));
				else
					final_population.push_back(
						std::make_pair(i,std::unordered_map<std::string,std::string>()));
				
		//		std::cout << endl;
			}
		}
	}
			

	return final_population;
} // end loader::load_population


void 
loader::print_organism ( long i) {

	if (i != -1) 
		std::cout  << "\t\tID: " << all_organisms[i].orig_ID 
			<< " from file " << all_organisms[i].from_file << std::endl; 
	else
		std::cout << "\t\trandom default organism" << endl;
} // end loader::print_organism


void 
loader::cLOAD(const std::string & file_view_name,const std::string & files) { // semantics of load 
		
	if (std::find(variable_names.begin(), variable_names.end(), file_view_name) 
		!= variable_names.end()) {
			std::cout << " error: multiple definitions of " << file_view_name << std::endl;
			exit(1);
	}
 

	std::vector<std::string> var_files;
	std::set<std::string> uniq_var_files;
	std::string f;
	for(std::stringstream ss(files); ss >> f ; ) {  // for each filename (possibly wildcarded)
		auto exp_files = expand_files(f);		// expand the filename
		if (exp_files.empty()) {
			cout << " warning: " << f << " doesn't not match any files" << endl;
			exit(1);
		}
		actual_files.insert(exp_files.begin(),exp_files.end()); // add files to global file list
		uniq_var_files.insert(exp_files.begin(),exp_files.end());
		var_files.insert(var_files.end(),exp_files.begin(),exp_files.end());
	}	

	if (var_files.size() != uniq_var_files.size()) {
		cout << " warning: " << file_view_name << " loaded with duplicate populations " << endl;
		exit(1);
	}

	variable_names.push_back(file_view_name);  // add variable name to global list
	variable_files[file_view_name] = var_files;	//  update list of files assoc with file_view_name

}  // end loader::cLOAD


void 
loader::cCHOOSE(const std::string & token, const std::string & expression) { // semantics of choose

	if (std::find(variable_names.begin(), variable_names.end(), token) 
			!= variable_names.end()  
	   || std::find(token_names.begin(), token_names.end(), token) 
			!= token_names.end()) {
		cout << " error: multiple definitions of " << token << endl;
		exit(1);
	}

	token_org_list[token] = parse_expression(expression);
		 // parse expression and add orgs assoc with token
	token_names.push_back(token);	// then add token name to global token list

} // end loader::cPARSE


std::vector<long> 
loader::parse_expression ( std::string  expr) {

	std::vector<std::pair<std::string,std::string>> local_tk_stack;

//NOTE: no check for balanced parans??
	expr = "(" + expr + ")"; // makes the top of local stack evaluate to the correct expression 
//	std::regex par("\\(([\\w\\s\\+]*)\\)");
	std::regex par("\\(([^\\(\\)]*)\\)");
	std::smatch m;
	while (std::regex_search(expr,m, par)) {			// resolve parans by ...
        std::string new_tk = tk_name + std::to_string(tk_counter++);	// creating new token
		token_names.push_back(new_tk);    					// adding token to global token list
        local_tk_stack.push_back({new_tk,m[1].str()});		// adding token to local stack of tokens
        expr = m.prefix().str() + " " + new_tk + " " + m.suffix().str(); 
											// replacing paran expr with token
   }	// repeat for all parans ..
	
	
	//evaluate tokens in local stack
	for (const auto & tk : local_tk_stack) {
 		std::vector<long> pop;
		std::regex plus("(.+?)(?:\\+|$)");	
		std::string tkn = tk.second;
		for( std::sregex_iterator end,
			 i = std::sregex_iterator (tkn.begin(), tkn.end(), plus);
				i != end; i++) {	//	evaluate each sub expression 
			std::smatch sub_expr = *i;
			std::string token = sub_expr[1].str();
			token.erase(std::remove_if(token.begin(),token.end(), ::isspace), token.end());
			auto p = std::find(token_names.begin(),token_names.end(),token)
								 != token_names.end() ?
				token_org_list[token] :	// expression is already evaluated token
			 	parse_token(sub_expr[1].str());	// expression needs to be parsed
			
			pop.insert(pop.end(),p.begin(),p.end());
		}
		token_org_list[tk.first] = pop; // add population to global tokens
	}
			
	return token_org_list[local_tk_stack.back().first]; // return population at top of stack

} // end  loader::parse_expression



std::vector<long> 
loader::parse_token(std::string token) {	//semantics of population choosing 

	std::regex mis_p("\\(|\\)");
	if (std::regex_search(token,mis_p)) {
		cout << " error: misplaced parantheses in token " 
			<< token << endl;	// could error ever be caught here?
		exit(1);
	}

	std::string postk = 
	"^\\s*("
		"((best|worst|random)\\s+\\d+\\s+(from|from_each)\\s+\\w+(\\s+by\\s+\\w+)?)"
				"|"
		"(all_of\\s+\\w+)"
				"|"
		"(rand\\s+\\d+)"
				"|"
		"(copy\\s+\\d+\\s+of\\s+\\w+)"
				"|"
		"(fill\\s+\\d+\\s+\\w+)"
				"|"
		"(as_many_of\\s+\\w+\\s+as_are_in\\s+\\w+)"
	")\\s*$";
	std::regex possible_tokens(postk);
	std::smatch choice;
	if (!std::regex_match(token,choice,possible_tokens)) {
		cout << " error: syntax error while trying to resolve "
			<< endl << token << endl;
		exit(1);
	}


	std::regex allof("^\\s*all_of\\s+(\\w+)\\s*$");
// all_of <name>
// name is a file_view, in which case all organisms from file are added to population 
// name is a user-defined token, in which case previously constructed population is returned
	std::smatch ma;
	if (std::regex_match(token,ma,allof)) {
		auto name = ma[1].str();
		if (token_org_list.find(name) != token_org_list.end()) {
			return 	token_org_list[name];
		}
		else
		if (std::find(variable_names.begin(),variable_names.end(),name)
					 != variable_names.end()) {
			std::vector<long> pop;
			for (const auto & vf : variable_files[name]) {
				auto p = file_contents[vf]; 
				std::vector<long> sinf(p.second - p.first + 1);
				std::iota(sinf.begin(),sinf.end(),p.first);
				pop.insert(pop.end(),sinf.begin(),sinf.end());
			}
		
			return pop;		 
		}
		else {
			cout << " error: unrecognised token " << name << endl;
			exit(1);
		}

	}


	std::regex rand("^\\s*rand\\s+(\\d+)\\s*$");
// rand <number>
//	creates number of random organisms
	std::smatch mr;
	if (std::regex_match(token,mr,rand)) {
		auto number = std::stol(mr[1].str());
		std::vector<long> pop(number,-1);
		return pop;
	}


	std::regex fill("^\\s*fill\\s+(\\d+)\\s+(\\w+)\\s*$");
// rand <number>
//	creates number of random organisms
	std::smatch mf;
	if (std::regex_match(token,mf,fill)) {
		auto name = mf[2].str();
		auto number = std::stol(mf[1].str());
		if (token_org_list.find(name) != token_org_list.end()) {
			std::vector<long> pop(token_org_list[name]);
			int sz = pop.size();
			if (sz > number) {
				cout << " error: cannot use fill if expression has more than " 
					<< "number" << " organisms" << endl;
				exit(1);
			}
			for(number-=sz;number>0;number--)
				pop.push_back(-1);
		
			return pop;
		}
		else {
			cout << " error: unrecognised token " << name << endl;
			exit(1);
		}

	}


	std::regex asmany("^\\s*as_many_of\\s+(\\w+)\\s+as_are_in\\s+(\\w+)\\s*$");
// rand <number>
//	creates number of random organisms
	std::smatch masm;
	if (std::regex_match(token,masm,asmany)) {
		auto from = masm[2].str();
		auto to =  masm[1].str();
		if (token_org_list.find(from) == token_org_list.end()) {
			cout << " error: unrecognised token " << from << endl;
			exit(1);
		}
		if (token_org_list.find(to) == token_org_list.end()) {
			cout << " error: unrecognised token " << to << endl;
			exit(1);
		}
		std::vector<long> fromp(token_org_list[from]),
						  	top(token_org_list[to]);
		int fromsz = fromp.size();
		int tosz = top.size();
		if (fromsz > tosz) {
			cout << " error: " << from << " has more organisms than " << to << endl;
			exit(1);
		} 
		std::shuffle(top.begin(),top.end(),std::random_device()); 
			// yuck, also must be fixed for replicability
		std::vector<long> pop(top.begin(),top.begin() + fromsz);
		return pop;
	}

	
	std::regex copy("^\\s*copy\\s+(\\d+)\\s+of\\s+(\\w+)\\s*$");
// copy <number> of <pop_expression>
// number copies of organisms from pop_expression are added
// copy <number> of <file_view>	 // NOPE
// number copies of organisms from all files in file_view are added  // NOPE
	std::smatch mc;
	if (std::regex_match(token,mc,copy)) {
		auto number = std::stol(mc[1].str());
		auto name = mc[2].str();
		if (token_org_list.find(name) != token_org_list.end()) {
			std::vector<long> pop, orig(token_org_list[name]);
			for(;number>0;number--)
				pop.insert(pop.end(),orig.begin(),orig.end());
			return pop;
		}
		/*
		else
		if (std::find(variable_names.begin(),variable_names.end(),name)
					 != variable_names.end()) {
			std::vector<long> pop;
			for (const auto & vf : variable_files[name]) {
				auto p = file_contents[vf]; 
				std::vector<long> orig(p.second - p.first + 1);
				std::iota(orig.begin(),orig.end(),p.first);
				for(;number>0;number--)
					pop.insert(pop.end(),orig.begin(),orig.begin());
			}
			return pop;
		}
		*/
		else {
			cout << " unrecognised token " << name << endl;
			exit(1);
		}
	
	}


	std::vector<long> pop;
	std::string sel_reg =
		"^\\s*(best|worst|random)"
		"\\s+(\\d+)\\s+"
		"(from|from_each)\\s+(\\w+)"
		"(?:\\s+by\\s+(\\w+))?"
		"\\s*$"; 
	std::regex select(sel_reg);
	std::smatch ms; 
											// best, worst, random 
	if (std::regex_match(token,ms,select)) {
	//	cout << "found ... " << ms.size() << endl;	
	//	for (auto x:ms) 
	//		cout << x << endl;

		std::string selection = ms[1].str();
		long number = std::stol(ms[2].str());
		std::string selection_method = ms[3].str();
		std::string resource = ms[4].str();
		std::string attribute = ms[5].str();
		
		std::vector<long> from_pop;

		if (selection_method == "from_each") {
			if (std::find(variable_names.begin(),variable_names.end(),resource) 
				== variable_names.end()) {
				cout << " error: can only do from_each for file_view, and " << resource << " is not a file_view" << endl;
				exit(1);
			}	
			std::vector<long> pop;
			for (const auto & vf : variable_files[resource]) {
				if ( selection != "random" && 
						std::find(file_attributes[vf].begin(),file_attributes[vf].end()
						,attribute) == file_attributes[vf].end()) {
				 	cout << " error: file  " << vf << " does not have attribute "
						 << attribute << endl;
					exit(1);
				}	
				auto p = file_contents[vf]; 
				if (p.second-p.first+1 < number) {
				 	cout << " error: file  " << vf 
						<< " does not have enough organisms "  << endl;
					exit(1);
				}	

				std::vector<long> orig(p.second - p.first + 1);
				std::iota(orig.begin(),orig.end(),p.first);
				if ( selection == "random") {
					std::shuffle(orig.begin(),orig.end(),std::random_device()); 
						// yuck, also must be fixed for replicability
					pop.insert(pop.end(),orig.begin(),orig.begin() + number);
				}
				else {
					std::sort(orig.begin(),orig.end(), [&](long lhs,long rhs) {
						return std::stod(all_organisms[lhs].attributes[attribute]) 
								<  std::stod(all_organisms[rhs].attributes[attribute]) ; 
					});
					if (selection == "worst") {
						pop.insert(pop.end(),orig.begin(),orig.begin() + number);
					}
					else if (selection == "best") {
						std::reverse(orig.begin(),orig.end());
						pop.insert(pop.end(),orig.begin(),orig.begin() + number);
					}
				}
			}
			return pop;
		}
		else { // selection_method == "from"
			/*
			if ( selection != "random" &&
					std::find(variable_names.begin(),variable_names.end(),
							resource) != variable_names.end()) {
				for (const auto & vf : variable_files[resource]) {
					if (std::find(file_attributes[vf].begin(),file_attributes[vf].end(),
						attribute) == file_attributes[vf].end()) {
				 		cout << " error: file  " << vf << " does not have attribute " 
							 << attribute << endl;
						exit(1);
					}	
				auto p = file_contents[vf]; 
				std::vector<long> sinf(p.second - p.first + 1);
				std::iota(sinf.begin(),sinf.end(),p.first);
				from_pop.insert(from_pop.end(),sinf.begin(),sinf.end());
				}	
			}
			else 
			*/
			if (token_org_list.find(resource) != token_org_list.end()) 
				from_pop = token_org_list[resource];
			else {
				cout << " error: unrecognised token " << resource << endl;
				exit(1);
			}
		}

		if (from_pop.size() < number) {
			cout << " error: cannot pick " << number << " organisms from " << token << endl;
			exit(1);
		}
		for (auto o : from_pop) {
			if (all_organisms[o].attributes.find(attribute) 
				== all_organisms[o].attributes.end()) {
				cout << "  error: organisms  from " << all_organisms[o].from_file << 
						" do not have attribute " << attribute << endl;
				exit(1);
			}
			if (o == -1) {
				cout << " error: random organisms cannot be used for selection " << endl;
				exit(1);
			}
		}	
			
	
		std::vector<long> pop;	
		if ( selection == "random") {
			std::shuffle(from_pop.begin(),from_pop.end(),std::random_device()); 
				// yuck, also must be fixed for replicability
			pop.insert(pop.end(),from_pop.begin(),from_pop.begin() + number);
		}
		else {
			std::sort(from_pop.begin(),from_pop.end(),[&](long lhs,long rhs){ 
				return std::stod(all_organisms[lhs].attributes[attribute]) 
						<  std::stod(all_organisms[rhs].attributes[attribute]) ; 
			});
			if (selection == "worst") {
				pop.insert(pop.end(),from_pop.begin(),from_pop.begin() + number);
			}
			else 
			if (selection == "best") {
				std::reverse(from_pop.begin(),from_pop.end());
				pop.insert(pop.end(),from_pop.begin(),from_pop.begin() + number);
			}
		}
	return pop;
	} // end if regex_search(token,m,select)


	if (true) {
		cout << "OOPS, something's gone wrong with regex for expressions (THIS is a BUG)" << endl;
		exit(1);
	}
	
} // end loader::parse_token


std::vector<string> 
loader::expand_files(std::string file_name) {

	std::vector<string> result;
	std::regex r1("\\*");
	file_name = std::regex_replace(file_name,r1,"\\w*");
	std::regex reg("^./" + file_name + "$");

	std::regex is_org_file("(.*)_organisms(_\\d+)?.csv$");

	std::copy_if(all_possible_file_names.begin(),all_possible_file_names.end(),
			std::back_inserter(result), [&reg,&is_org_file](const string &s) { 
					return  std::regex_match(s,reg) &&
							 std::regex_match(s,is_org_file); 
			});

	return result;
}	// end loader::expand_files
	
	

void 
loader::generate_population(std::string fn) {

	auto file_name = fn;
//cout << "\tTrying to open " << file_name << " ... " << endl;		

	auto org_file_data = get_attribute_map(file_name);
//cout << "\tSuccesfully opened " << file_name << " ... " << endl;		

	std::regex org_to_data("(.*)_organisms(_\\d+)?.csv$");
	std::smatch match_org;
	if (!std::regex_match(file_name,match_org,org_to_data)) {
		cout << " error: unrecognized file name format " << file_name
			<< endl << " Was this file generated by MABE? " << endl;
		exit(1);
	}
	file_name = std::regex_replace(file_name,org_to_data,
				match_org[1].str() + "_data" + match_org[2].str() + ".csv");

//cout << file_name << endl;		
//cout << "\tTrying to open " << file_name << " ... " << endl;		
	std::map<long,std::map<std::string,std::string>> data_file_data;
	if (std::find(all_possible_file_names.begin(),
				all_possible_file_names.end(),file_name) 
					!= all_possible_file_names.end()) { 
		data_file_data = get_attribute_map(file_name); 
	}
	else {
		cout << " warning: " << fn << " does not have a corresponding "
			<< file_name   
			<< endl << " Was this file generated by MABE? " << endl;
	}
	
	std::vector<std::string> attribute_names;
	for (const auto &att : org_file_data.begin()->second)
		attribute_names.push_back(att.first);
	if (!data_file_data.empty())
		for (const auto &att : data_file_data.begin()->second)
			attribute_names.push_back(att.first);
//cout << "broken ?" << endl;

	file_attributes[fn] = attribute_names;
	auto cur_size = all_organisms.size();
	file_contents[fn] = std::make_pair(cur_size,cur_size + org_file_data.size() -1);

	for (const auto &org_data : org_file_data) {
		organism org;
		org.orig_ID =  org_data.first;
		org.from_file = fn;
		org.attributes.insert(org_data.second.begin(),org_data.second.end());
		if (data_file_data.find(org.orig_ID) != data_file_data.end()) {
			org.has_corresponding_data_file = true;
			org.attributes.insert(data_file_data[org.orig_ID].begin(),
									data_file_data[org.orig_ID].end());
		}
		else {
			cout << " warning: org " << org.orig_ID << " from file " << fn 
				<< " does not have a corresponding entry in " << file_name  
			<< endl << " Was this file generated by MABE? " << endl;
		}
		all_organisms.push_back(org);
	}

}	// end loader::generate_population


std::map<long,std::map<std::string,std::string>> 
	// nested maps, yuck, but workaroud till MABE DataMAP, and FileManager cleanup
loader::get_attribute_map(const std::string & fn) {
	
	std::map<long,std::map<std::string,std::string>> result;

	std::ifstream file(fn);
	if (!file.is_open()) {
		cout<< " error: unable to load" << fn << endl;
		exit(1);
	} 

	std::string attr_names;
	getline(file,attr_names);
	std::regex r("[\\w|:]+");
	std::vector<std::string> attribute_names;
	//	for(stringstream ss(attr_names) ; getline(ss,name,','); ) {
	
	for( std::sregex_iterator end,
		i = std::sregex_iterator (attr_names.begin(), attr_names.end(), r);
			i != end ; i++) {
			attribute_names.push_back( (*i).str() );
		}
//cout << endl;

	if (std::find(attribute_names.begin(), attribute_names.end(),"ID") 
		== attribute_names.end()) {
		cout << " error: no ID for organisms in file " << fn << endl;
		exit(1);
	}

    std::regex mabe_csv_regex("((\\d+)(?:,|$))|(\"\\[)|((\\d+\\]\")(?:,))"); 
//	std::regex csv("(\"[^\"]+\"|[^,]+)(,|$)"); // this triggers bug in libstdc++ for capture of more than 20k bytes 
		// checking for MABE csv-ness
	std::string org_details;
	for(;getline(file,org_details);) { 
		std::map<std::string,std::string> temp_result;
		long k =0;
		auto in_quotes = false;
		std::string quote_str = "";
		for( std::sregex_iterator end,
			 i = std::sregex_iterator (org_details.begin(), org_details.end(), 
				mabe_csv_regex);  i != end ; i++) {
			std::smatch m = *i;		

			if (m[1].length()) 
				if (in_quotes == false)
					temp_result[attribute_names[k++]] = m[2].str();
				else 
					quote_str += m[1].str();
			else 
			if (m[3].length()) {
				in_quotes = true;
				quote_str += m[3].str();
			}
			else 
			if (m[5].length()) {
				quote_str += m[5].str();
				temp_result[attribute_names[k++]] = quote_str; 
				in_quotes = false;
				quote_str = "";
			}
			else {
				cout << " error : something wrong with mabe csv-ness " << endl;
				exit(1);
			}
		}
//cout << " kk " << endl;
		result[std::stol(temp_result["ID"])] = temp_result;
	}
file.close();	
	return result;
} // end loader::get_attribute_map








