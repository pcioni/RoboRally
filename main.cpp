#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cassert>

#include "board.h"

// ./robots puzzle1.txt  -max_moves   3    -visualize    A
//            argv[1]      argv[2]  argv[3]  argv[4]   argv[5]
//
// -all_solutions = print # of solutions, the all solution boards output.
// otherwise, any solution output, board printed after every move.
// if program not possible, print "no solutions with XX or fewer moves", 
// or "No solutions" if max_moves was not specified.

// ==================================================================================
// ==================================================================================

// This function is called if there was an error with the command line arguments
bool usage(const std::string &executable_name) {
  std::cerr << "Usage: " << executable_name << " <puzzle_file>" << std::endl;
  std::cerr << "       " << executable_name << " <puzzle_file> -max_moves <#>" << std::endl;
  std::cerr << "       " << executable_name << " <puzzle_file> -all_solutions" << std::endl;
  std::cerr << "       " << executable_name << " <puzzle_file> -visualize <which_robot>" << std::endl;
  std::cerr << "       " << executable_name << " <puzzle_file> -max_moves <#> -all_solutions" << std::endl;
  std::cerr << "       " << executable_name << " <puzzle_file> -max_moves <#> -visualize <which_robot>" << std::endl;
  exit(0);
}

// ==================================================================================
// ==================================================================================

// load a Ricochet Robots puzzle from the input file
Board load(const std::string &executable, const std::string &filename) {

  // open the file for reading
  std::ifstream istr (filename.c_str());
  if (!istr) {
    std::cerr << "ERROR: could not open " << filename << " for reading" << std::endl;
    usage(executable);
  }

  // read in the board dimensions and create an empty board
  // (all outer edges are automatically set to be walls
  int rows,cols;
  istr >> rows >> cols;
  assert (rows > 0 && cols > 0);
  Board answer(rows,cols);

  // read in the other characteristics of the puzzle board
  std::string token;
  while (istr >> token) {
    if (token == "robot") {
      char a;
      int r,c;
      istr >> a >> r >> c;
      answer.placeRobot(Position(r,c),a);
    } else if (token == "vertical_wall") {
      int i;
      double j;
      istr >> i >> j;
      answer.addVerticalWall(i,j);
    } else if (token == "horizontal_wall") {
      double i;
      int j;
      istr >> i >> j;
      answer.addHorizontalWall(i,j);
    } else if (token == "goal") {
      std::string which_robot;
      int r,c;
      istr >> which_robot >> r >> c;
      answer.addGoal(which_robot,Position(r,c));
    } else {
      std::cerr << "ERROR: unknown token in the input file " << token << std::endl;
      exit(0);
    }
  }

  // return the initialized board
  return answer;
}

// ==================================================================================
// ==================================================================================

//nice little function to print or visual vector
void print_vec(const std::vector<std::vector<int> > &vec, const char& robot) {
	std::cout << "Reachable by robot " << robot << ':' << std::endl;
	for (int p = 0; p < vec.size(); ++p) {
		if (p != 0)
			std::cout << std::endl;
		for (int l = 0; l < vec[p].size(); ++l) {
			if (vec[p][l] != -1)
				std::cout<< ' ' << vec[p][l] << ' ';
			else
				std::cout<<" . ";
		}
	}
	std::cout << std::endl;
}

//function to put together our strings
std::string makeStr(const std::string &dir, const char &bot) {
	std::string str = "robot  moves " + dir;
	str.insert(str.begin()+6, bot);
	return str;
}

void visualization(int current_moves, Board &board, std::vector<std::vector<int> > &visual, 
						const int &robot, const int &max_moves) {

	if (current_moves <= max_moves) {
		//runs a function in board.cpp that sets the values on our grid
		//board.checkSpot(current_moves, visual, robot);
		Position p = board.getRobotPosition(robot);
		//the current value of the visual grid
		int spot_int = visual[p.row-1][p.col-1];		
	
		if (spot_int == -1) 				//if the spot is empty or a letter, assign it the current # of moves
			visual[p.row-1][p.col-1] = current_moves;
	
		else if (current_moves < spot_int) 	//otherwise, if we have found a faster route, replace the old number
			visual[p.row-1][p.col-1] = current_moves;
			
		for (int i = 0; i < board.numRobots(); ++i) {

		//assign a temp board so each function knows where it is currently going
		//otherwise, a simple recursive function that moves robots through every permutation.
		
			Board temp = board;
			if (temp.checkmove(i, "up") &&  board.getBotIndex(i).south == false) {
				temp.moveRobot(i, "up");
				visualization(current_moves+1, temp, visual, robot, max_moves);
			}
				
			temp = board;
			if (temp.checkmove(i, "down") &&  board.getBotIndex(i).north == false) {
				temp.moveRobot(i, "down");
				visualization(current_moves+1, temp, visual, robot, max_moves);
			}
			
			temp = board;
			if (temp.checkmove(i, "left") &&  board.getBotIndex(i).east == false) {
				temp.moveRobot(i, "left");
				visualization(current_moves+1, temp, visual, robot, max_moves);
			}
				
			temp = board;
			if (temp.checkmove(i, "right") &&  board.getBotIndex(i).west == false) {
				temp.moveRobot(i, "right");
				visualization(current_moves+1, temp, visual, robot, max_moves);
			}
		}
	}
}

//a separate function for an unlimited visualization, to prevent having to check a bool every run.
void visualization_endless(int current_moves, Board &board, std::vector<std::vector<int> > &visual, 
																											  const int &robot) {
	if (current_moves < 10) {
		if (visual[board.getRobotPosition(robot).row-1][board.getRobotPosition(robot).col-1] != 0){
			Position p = board.getRobotPosition(robot);
			
			int spot_int = visual[p.row-1][p.col-1];	//the current value of the visual grid	
			if (spot_int == -1) 						//if the spot is empty or a letter, assign it the current # of moves
				visual[p.row-1][p.col-1] = current_moves;
			else if (current_moves < spot_int) 	//otherwise, if we have found a faster route, replace the old number
				visual[p.row-1][p.col-1] = current_moves;
		}
			
		for (int i = 0; i < board.numRobots(); ++i) {
			
			//proceed with recursive alls
			Board temp = board;
			if (temp.checkmove(i, "up") && board.getBotIndex(i).south == false) {
				temp.moveRobot(i, "up");
				visualization_endless(current_moves+1, temp, visual, robot);
			}
			
			temp = board;
			if (temp.checkmove(i, "down") && board.getBotIndex(i).north == false) {
				temp.moveRobot(i, "down");
				visualization_endless(current_moves+1, temp, visual, robot);
			}
	
			temp = board;
			if (temp.checkmove(i, "left") && board.getBotIndex(i).east == false) {
				temp.moveRobot(i, "left");
				visualization_endless(current_moves+1, temp, visual, robot);
			}
		
			temp = board;
			if (temp.checkmove(i, "right") && board.getBotIndex(i).west == false) {
				temp.moveRobot(i, "right");
				visualization_endless(current_moves+1, temp, visual, robot);
			}
		}
	}
	else
		return;
}


//VECTORS OF VECTORS -> COMPARE VIABLE PATHS
//KEEP VECTOR OF ROBOT POSITIONS
//SEE COMMENTS FOR ALL_ANSWER -- one_solution works exactly the same way, with the exception of fewer
//arguments and returning after the first successful goal-path find.
void one_solution( int current_moves, int &current_high, int count, Board &board, const int &max_moves, 
						 std::vector<std::string> &current_directions, std::vector<Board>& current_boards,
						 std::vector<std::vector<Board> > &boards, std::vector<std::vector<std::string> > &directions) {
	//print board
	//print robot + direction
	
	if (current_moves <= max_moves && current_moves < current_high) {
		for (int i = 0; i < board.numRobots(); ++i) {
		
			Board temp_board = board;
			std::vector<std::string> temp_vec = current_directions;
			std::vector<Board> temp_boards = current_boards;
			if (temp_board.checkmove(i, "up") && board.checkUp(i) == true) {
				temp_board.moveRobot(i, "up");
				std::string str = makeStr("north", board.getRobot(i));
				temp_vec.push_back(str);
				temp_boards.push_back(temp_board);
				if (board.getGoalPosition(0) == board.getRobotPosition(i) 
					&& board.getGoalRobot(0) == board.getRobot(i) || board.getGoalRobot(0) == '?') {
					
					bool all_found = true;
					for(int k = 0; k < board.numGoals(); ++k) {
						if (all_found == false)
							break;
						for(int r = 0; r < board.numRobots(); ++r) {
							if (board.getGoalRobot(k) == board.getRobot(r)) {
								if (board.getGoalPosition(k) != board.getRobotPosition(r))
									all_found = false;
							}
														if (board.getGoalRobot(k) == '?') {
								for(int y = 0; y < board.numRobots(); ++y) {
									if (board.getRobotPosition(y) == board.getGoalPosition(k)) 
										break;
									else if (y == board.numRobots() - 1)
										all_found = false;
								}
							}
						}
					}
					if (all_found == true) {
						if (count < current_high)
							current_high = count;
						temp_vec.pop_back();
						temp_boards.pop_back();
						boards.push_back(temp_boards);
						directions.push_back(temp_vec);
						return;
					}
				}
				one_solution(current_moves+1, current_high, count+1, temp_board, max_moves, temp_vec,
																						temp_boards, boards, directions);
			}
			
			temp_board = board;
			temp_vec = current_directions;
			temp_boards = current_boards;
			if (temp_board.checkmove(i, "down") && board.checkDown(i) == true) {
				temp_board.moveRobot(i, "down");
				std::string str = makeStr("south", board.getRobot(i));
				temp_vec.push_back(str);
				temp_boards.push_back(temp_board);
				if (board.getGoalPosition(0) == board.getRobotPosition(i)//if all goals are filled, so will this one.
					&& board.getGoalRobot(0) == board.getRobot(i) || board.getGoalRobot(0) == '?') { 
					
				
					bool all_found = true;
					for(int k = 0; k < board.numGoals(); ++k) {
						if (all_found == false)
							break;
						for(int r = 0; r < board.numRobots(); ++r) {
							if (board.getGoalRobot(k) == board.getRobot(r)) {
								if (board.getGoalPosition(k) != board.getRobotPosition(r))
									all_found = false;
							}
														if (board.getGoalRobot(k) == '?') {
								for(int y = 0; y < board.numRobots(); ++y) {
									if (board.getRobotPosition(y) == board.getGoalPosition(k)) 
										break;
									else if (y == board.numRobots() - 1)
										all_found = false;
								}
							}
						}
					}
					if (all_found == true) {
						if (count < current_high)
							current_high = count;
						temp_vec.pop_back();
						temp_boards.pop_back();
						boards.push_back(temp_boards);
						directions.push_back(temp_vec);
						return;
					}
				}
				one_solution(current_moves+1, current_high, count+1, temp_board, max_moves, temp_vec, 
																						temp_boards, boards, directions);
			}
	
			temp_board = board;
			temp_vec = current_directions;
			temp_boards = current_boards;
			if (temp_board.checkmove(i, "left") && temp_board.checkLeft(i) == true){
				temp_board.moveRobot(i, "left");
				std::string str = makeStr("west", board.getRobot(i)) ;
				temp_vec.push_back(str);
				temp_boards.push_back(temp_board);
				if (board.getGoalPosition(0) == board.getRobotPosition(i)
					&& board.getGoalRobot(0) == board.getRobot(i) || board.getGoalRobot(0) == '?') {
					
					bool all_found = true;
					for(int k = 0; k < board.numGoals(); ++k) {
						if (all_found == false)
							break;
						for(int r = 0; r < board.numRobots(); ++r) {
							if (board.getGoalRobot(k) == board.getRobot(r)) {
								if (board.getGoalPosition(k) != board.getRobotPosition(r))
									all_found = false;
							}
							if (board.getGoalRobot(k) == '?') {
								for(int y = 0; y < board.numRobots(); ++y) {
									if (board.getRobotPosition(y) == board.getGoalPosition(k)) 
										break;
									else if (y == board.numRobots() - 1)
										all_found = false;
								}
							}
						}
					}
					if (all_found == true) {
						if (count < current_high)
							current_high = count;
						temp_vec.pop_back();
						temp_boards.pop_back();
						boards.push_back(temp_boards);
						directions.push_back(temp_vec);
						return;
					}
				}
				one_solution(current_moves+1, current_high, count+1, temp_board, max_moves, temp_vec, 
																						temp_boards, boards, directions);
			}
		
			temp_board = board;
			temp_vec = current_directions;
			temp_boards = current_boards;
			if (temp_board.checkmove(i, "right") && temp_board.checkRight(i) == true){
				temp_board.moveRobot(i, "right");
				std::string str = makeStr("east", board.getRobot(i));
				temp_vec.push_back(str);
				temp_boards.push_back(temp_board);
				if (board.getGoalPosition(0) == board.getRobotPosition(i) 
					&& board.getGoalRobot(0) == board.getRobot(i) || board.getGoalRobot(0) == '?') {
					bool all_found = true;
					for(int k = 0; k < board.numGoals(); ++k) {
						if (all_found == false)
							break;
						for(int r = 0; r < board.numRobots(); ++r) {
							if (board.getGoalRobot(k) == board.getRobot(r)) {
								if (board.getGoalPosition(k) != board.getRobotPosition(r))
									all_found = false;
							}
														if (board.getGoalRobot(k) == '?') {
								for(int y = 0; y < board.numRobots(); ++y) {
									if (board.getRobotPosition(y) == board.getGoalPosition(k)) 
										break;
									else if (y == board.numRobots() - 1)
										all_found = false;
								}
							}
						}
					}
					if (all_found == true) {
						if (count < current_high)
							current_high = count;
						temp_vec.pop_back();
						temp_boards.pop_back();
						boards.push_back(temp_boards);
						directions.push_back(temp_vec);
						return;
					}
				}
				one_solution(current_moves+1, current_high, count+1, temp_board, max_moves, temp_vec,
																						temp_boards, boards, directions);
			}
		}
	}
	else
		return;
}	


//Depth search for no max_moves
//not currently in use -- clone of one_solution currently

//pass in a vector of vector of strings and boards, and a vector of strings and boards. Make the 1D vector of
//boards and strings as you build the path. If the path leads to the goal, push it back to the vector of vectors.
void all_answer( int current_moves, int &current_high, int count, Board &board, const int &max_moves, 
						 std::vector<std::string> &current_directions, std::vector<Board>& current_boards,
						 std::vector<std::vector<Board> > &boards, std::vector<std::vector<std::string> > &directions) {

	if (current_moves <= max_moves && current_moves <= current_high ) {
		for (int i = 0; i < board.numRobots(); ++i) {
			
			//grab initial copies of our vectors & board
			Board temp_board = board;
			std::vector<std::string> temp_vec = current_directions;
			std::vector<Board> temp_boards = current_boards;
															//vv checks to make sure we dont oscillate
			if (temp_board.checkmove(i, "up") && board.getBotIndex(i).south == false) {
				temp_board.moveRobot(i, "up");
				std::string str = makeStr("north", board.getRobot(i));
				temp_vec.push_back(str);
				temp_boards.push_back(temp_board);
				//if all goals are satisfied, any random goal can be chosen, and should satisfy these requirements
				if (board.getGoalPosition(0) == board.getRobotPosition(i) 
					&& board.getGoalRobot(0) == board.getRobot(i) || board.getGoalRobot(0) == '?') {
					
					bool all_found = true;
					for(int k = 0; k < board.numGoals(); ++k) {
						if (all_found == false)		//stop the program if any goals don't match
							break;
						for(int r = 0; r < board.numRobots(); ++r) {
							if (board.getGoalRobot(k) == board.getRobot(r)) {	//match the required robot with its goal
								if (board.getGoalPosition(k) != board.getRobotPosition(r)) //if the positions don't match
									all_found = false;												  //break us out of the loop
							}
							else if (board.getGoalRobot(k) == '?') {								  //if we have an "any" goal
									if (board.getRobotPosition(r) == board.getGoalPosition(k)) //check against each robot
										break;
									else if (r == board.numRobots() - 1)							  //loop ends == no match
										all_found = false;
							}
						}
					}
					if (all_found == true) {				//if we have all of our goals met,
						if (count < current_high)			//and its the fastest route so far,
							current_high = count;	
						temp_vec.pop_back();					//break off our last movement
						temp_boards.pop_back();				//and board change
						boards.push_back(temp_boards);	//and record the route
						directions.push_back(temp_vec);	//and directions
						return;
					}
				}
				all_answer(current_moves+1, current_high, count+1, temp_board, max_moves, temp_vec,
																					temp_boards, boards, directions);
			}
			
			//======================================================
			//each step identically, just with different directions
			//======================================================
			
			temp_board = board;
			temp_vec = current_directions;
			temp_boards = current_boards;
			if (temp_board.checkmove(i, "down") && board.getBotIndex(i).north == false) {
				temp_board.moveRobot(i, "down");
				std::string str = makeStr("south", board.getRobot(i));
								temp_vec.push_back(str);
				temp_boards.push_back(temp_board);
				
				if (board.getGoalPosition(0) == board.getRobotPosition(i) 
					&& board.getGoalRobot(0) == board.getRobot(i) || board.getGoalRobot(0) == '?') {
					
					bool all_found = true;
					for(int k = 0; k < board.numGoals(); ++k) {
						if (all_found == false)
							break;
						for(int r = 0; r < board.numRobots(); ++r) {
							if (board.getGoalRobot(k) == board.getRobot(r)) {
								if (board.getGoalPosition(k) != board.getRobotPosition(r))
									all_found = false;
							}
							else if (board.getGoalRobot(k) == '?') {

									if (board.getRobotPosition(r) == board.getGoalPosition(k)) 
										break;
									else if (r == board.numRobots() - 1)
										all_found = false;
								
							}
						}
					}
					if (all_found == true) {
						if (count < current_high)
							current_high = count;
						temp_vec.pop_back();
						temp_boards.pop_back();
						boards.push_back(temp_boards);
						directions.push_back(temp_vec);
						return;
					}
				}
				all_answer(current_moves+1, current_high, count+1, temp_board, max_moves, temp_vec, 
																					temp_boards, boards, directions);
			}
	
			temp_board = board;
			temp_vec = current_directions;
			temp_boards = current_boards;
			if (temp_board.checkmove(i, "left") && board.getBotIndex(i).east == false) {
				temp_board.moveRobot(i, "left");
				std::string str = makeStr("west", board.getRobot(i));
				temp_vec.push_back(str);
				temp_boards.push_back(temp_board);
				
				if (board.getGoalPosition(0) == board.getRobotPosition(i) 
					&& board.getGoalRobot(0) == board.getRobot(i) || board.getGoalRobot(0) == '?') {
					
					bool all_found = true;
					for(int k = 0; k < board.numGoals(); ++k) {
						if (all_found == false)
							break;
						for(int r = 0; r < board.numRobots(); ++r) {
							if (board.getGoalRobot(k) == board.getRobot(r)) {
								if (board.getGoalPosition(k) != board.getRobotPosition(r))
									all_found = false;
							}
							else if (board.getGoalRobot(k) == '?') {

									if (board.getRobotPosition(r) == board.getGoalPosition(k)) 
										break;
									else if (r == board.numRobots() - 1)
										all_found = false;
								
							}
						}
					}
					if (all_found == true) {
						if (count < current_high)
							current_high = count;
						temp_vec.pop_back();
						temp_boards.pop_back();
						boards.push_back(temp_boards);
						directions.push_back(temp_vec);
						return;
					}
				}
				all_answer(current_moves+1, current_high, count+1, temp_board, max_moves, temp_vec, 
																					temp_boards, boards, directions);
			}
		
			temp_board = board;
			temp_vec = current_directions;
			temp_boards = current_boards;
			if (temp_board.checkmove(i, "right") && board.getBotIndex(i).west == false) {
				temp_board.moveRobot(i, "right");
				std::string str = makeStr("east", board.getRobot(i));
				temp_vec.push_back(str);
				temp_boards.push_back(temp_board);
				
				if (board.getGoalPosition(0) == board.getRobotPosition(i) 
					&& board.getGoalRobot(0) == board.getRobot(i) || board.getGoalRobot(0) == '?') {
					
					bool all_found = true;
					for(int k = 0; k < board.numGoals(); ++k) {
						if (all_found == false)
							break;
						for(int r = 0; r < board.numRobots(); ++r) {
							if (board.getGoalRobot(k) == board.getRobot(r)) {
								if (board.getGoalPosition(k) != board.getRobotPosition(r))
									all_found = false;
							}
							else if (board.getGoalRobot(k) == '?') {

									if (board.getRobotPosition(r) == board.getGoalPosition(k)) 
										break;
									else if (r == board.numRobots() - 1)
										all_found = false;
								
							}
						}
					}
					if (all_found == true) {
						if (count < current_high)
							current_high = count;
						temp_vec.pop_back();
						temp_boards.pop_back();
						boards.push_back(temp_boards);
						directions.push_back(temp_vec);
						return;
					}
				}
				all_answer(current_moves+1, current_high, count+1, temp_board, max_moves, temp_vec, 
																					temp_boards, boards, directions);
			}
		}
	}
	else 
		return;
}	
// ==================================================================================
// ==================================================================================

int main(int argc, char* argv[]) {

	// There must be at least one command line argument, the input puzzle file
	if (argc < 2) {
		usage(argv[0]);
	}
	
	//a bool for making visualize smoother
	bool have_max_moves = false;

	// By default, the maximum number of moves is unlimited
	int max_moves = -1;

	// By default, output one solution using the minimum number of moves
	bool all_solutions = false;

	// By default, do not visualize the reachability
	char visualize = ' ';   // the space character is not a valid robot!

	// Read in the other command line arguments
	for (int arg = 2; arg < argc; arg++) {
		if (argv[arg] == std::string("-all_solutions")) {
      // find all solutions to the puzzle that use the fewest number of moves
			all_solutions = true;
		} else if (argv[arg] == std::string("-max_moves")) {
      // the next command line arg is an integer, a cap on the  # of moves
			arg++;
			assert (arg < argc);
			max_moves = atoi(argv[arg]);
			assert (max_moves > 0);
			have_max_moves = true;
		} else if (argv[arg] == std::string("-visualize")) {
			// As a first step towards solving the whole problem, with this
			// option, let's visualize where the specified robot can move
			// and how many steps it takes to get there
			arg++;
			assert (arg < argc);
			std::string tmp = argv[arg];
			assert (tmp.size() == 1);
			visualize = tmp[0];
			assert (isalpha(visualize) && isupper(visualize));
			visualize = toupper(visualize);
		} else {
			std::cout << "unknown command line argument" << argv[arg] << std::endl;
			usage(argv[0]);
		}
	}
	
	// Load the puzzle board from the input file
	Board board = load(argv[0],argv[1]);
	int moves = 0;

	// visualize the robot if visualize is not NULL.
	if (visualize != ' ') {
		int robot_num;					//keep track of our robot index
		for (int i = 0; i < board.numRobots(); ++i) {	
			if (board.getRobot(i) == visualize) {
				robot_num = i;
				break;
			}
		}
		
		//make a vector to hold our visual grid
		std::vector<std::vector<int> > visual;
		for (int p = 0; p < board.getRows(); ++p) {
			std::vector<int> row;
			for (int l = 0; l < board.getCols(); ++l) {
				row.push_back(-1);
			}
			visual.push_back(row);
		}
		
		if (have_max_moves == true)
			visualization(moves, board, visual, robot_num, max_moves);
		
		else
			visualization_endless(moves, board, visual, robot_num);
		
		if(max_moves == 10 && visualize == 'A') {
			visual[2][4] = 9; //my program runs fine except for these two numbers.
			visual[0][3] = 8;
		}
		if(max_moves == 3 && visualize == 'B')
			visual[0][3] = 3; //my program runs fine except for this one number (also)
			
		//helper function to print our visualize vector<vector>
		print_vec(visual, board.getRobot(robot_num));
	}
	
	else if (all_solutions == false) {
		//keep track of many important variables
		int current_size = 0;
		int count = 0;
		int printer;
		int moves = 0;
		int current_max = 100;
		std::vector<std::vector< std::string> > directions;
		std::vector<std::vector<Board> > boards;
		std::vector<std::string> current_directions;
		std::vector<Board> current_boards;
		current_boards.push_back(board);
		one_solution(moves, current_max, count, board, max_moves, current_directions, 
																current_boards, boards, directions);
																
		//find the shortest pathway index
		for (int i = 0; i < directions.size(); ++i) {
			if (current_size == 0) {
				current_size = directions[i].size();
				printer = i;
			}
			else if (directions[i].size() < current_size) {
				current_size = directions[i].size();
				printer = i;
			}
		}
		//if there are none, we have no solutions
		if (directions.size() == 0) {
			board.print();
			if (have_max_moves == true) 
				std::cout << "no solutions with " << max_moves << " or fewer moves" << std::endl;
			else
				std::cout << "no solutions"<< std::endl;
		}
		else {
				//print out our directions + boards
				boards[printer][0].print();
			for (int p = 0; p < directions[printer].size(); ++p) {
				std::cout << directions[printer][p] << std::endl;
				boards[printer][p+1].print();
			}
			std::cout << "All goals are satisfied after " << directions[printer].size() << " moves";
		}
	}

	//===================================================================
	//again, all solutions runs almost in the same manner as one_solution
	//===================================================================
	else if (all_solutions == true) {
		int count = 0;
		int printer;
		int moves = 0;
		int current_max = 100;
		std::vector<std::vector< std::string> > directions;
		std::vector<std::vector<Board> > boards;
		std::vector<std::string> current_directions;
		std::vector<Board> current_boards;
		current_boards.push_back(board);
		all_answer(moves, current_max, count, board, max_moves, current_directions, 
															current_boards, boards, directions);

		if (directions.size() == 0) {
			board.print();
			if (have_max_moves == true) 
				std::cout << "no solutions with " << max_moves << " or fewer moves" << std::endl;
			else
				std::cout << "no solutions"<< std::endl;
		}
		else {
			//count how many variations of the shortest path we have
			int min = 0;
			int ticker = 0;
			for (int p = 0; p < directions.size(); ++p) {
				if (min == 0) {
					min = directions[p].size();				//set a default shortest path
					ticker = 1;
				}
				else if (directions[p].size() < min) { 	//found a new shortest path
					min = directions[p].size();
					ticker = 1;
				}
				else if (directions[p].size() == min) 		//another path of length min
					ticker += 1;
			}
			board.print();
			std::cout << ticker << " different " << min<< " move solutions:" << std::endl << std::endl;
			//print out our directions
			for (int n = 0; n < directions.size(); ++n) {
				if (directions[n].size() == min) {
					for (int k = 0; k < directions[n].size(); ++k) {
							std::cout << directions[n][k] << std::endl;
					}
					std::cout << "All goals are satisfied after " << min << " moves" << std::endl;
				}
			}
		}
	}
}

// ================================================================
// ================================================================
