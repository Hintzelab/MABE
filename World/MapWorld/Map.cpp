// #include "Map.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>



/********************************************
*          CONSTRUCTOR
********************************************/
Map::Map(std::string file, char carChar, char destChar){
  /**
  * Constructor
  *
  * @param: file to create map
  **/

  // initialize variables
  car = carChar;
  dest = destChar
  std::string line;
  std::ifstream map_file (file);

  // parsing file
  if (myfile.is_open()){
    while (std::getline(myfile, line)){
      std::vector<char> parsed_line;
      std::stringstream ss(line);
      char c;
      // process line to vector of char
      while(ss >> c){
        parsed_line.push_back(c);

        if (c == car) // if character is car insert curr_pos
          curr_pos = std::make_pair(parsed_line.size()-1, map_grid.size())

        if (c == dest) // if character is destination inster destination pos
          destination_pos = std::make_pair(parsed_line.size()-1, map_grid.size())
      }
      // add vector version of line to map_grid
      map_grid.append(parsed_line);
    }
  }


}

/********************************************
*          GETTER FUNCTIONS
********************************************/
std::pair<int, int> Map::get_curr_pos(){
  return curr_pos;
}

std::pair<int, int> Map::get_dest_pos(){
  return destination_pos;
}




/********************************************
*          HELPER FUNCTIONS
********************************************/
char Map::map_char(std::pair<int, int> map_pos){
  /**
  * gets character at position from map
  *
  * @param: coordinate pair of map position
  * @return: character on map
  **/
  return map_grid[map_pos.second][map_pos.first];
}


void Map::print_map(){
  /**
  * Prints map grid
  *
  **/
  std::cout << "map: " << std::endl;
  std::string map;
    for(int i=map_grid.size()-1; i>-1; i--){
      for(int k=0; k<map_grid[i].size(); k++){
        // print organism
        if (i==curr_pos.second && k==curr_pos.first){
          // print facing position
          if(facing_pos == 0)
            std::cout << "^";
          if(facing_pos == 90)
            std::cout << ">";
          if(facing_pos == 180)
            std::cout << "v";
          if(facing_pos == 270)
            std::cout << "<";
        }

        else{
          // print destination
          if (i==destination_pos.second && k==destination_pos.first)
            std::cout << dest;
          // print map
          else
            std::cout << std::to_string(map_grid[i][k]);
        }
      }

      std::cout << std::endl;
    }
    std::cout << "current position: " << std::to_string(curr_pos.first) << ", " << std::to_string(curr_pos.second) << std::endl;
}
