#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

/*****************************
*         MAP CLASS
*****************************/

class geo{
public:
  static std::vector< std::vector<char> > geo_grid;
  static std::pair<int, int> destination_pos;

  static char car;
  static char dest;
  static char open;
  static char wall;

  // organism variables
  std::pair<int, int> curr_pos;
  int facing;

  // organism sensor
  std::vector< std::vector<std::pair<int, int>> > grid_sensor;
  std::vector< int > compass;



// geo functions
  // constructions
  geo(std::string, char, char, char, char);

  // Getter functions
  std::vector< std::vector<char> > get_geo_grid;

  std::pair<int, int> get_curr_pos();
  std::pair<int, int> get_dest_pos();

  std::vector<int> get_compass();

  std::vector<int> get_sensors();
  std::vector<int> get_sides_sensor();

  // moving FUNCTIONS
  void turn(std::string);
  int move();


  // Helper functions
  char geo_char(std::pair<int, int>);


  void print_geo();

};


class geo_list{
public:
  std::vector<geo> world_maps; // geo list

  geo_list() = default;

  // modifying functions
  void add(geo);


};
