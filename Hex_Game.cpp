#include <ctime>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>

using namespace std;

enum class Player{ BLUE, RED};
// --------------------------Class Prototypes----------------------------------
class Board
{
private:
    //Let's define the types of squares and general settings of out board:
   static int direct[6][2];
   static char blue;
   static char red;
   static char empty;
   int size;
   string line;
   vector<vector<char>> board;
   
public:
    //Let's define our constructor:
   Board(){}
   Board(int size):size(size),board(size,vector<char>(size,'*'))
   {
      line = "\\";                   // to travel the tracks
      for(int i = 1; i < size; i++)
         line += " / \\";
   } // matrix presentation for the graph looks best for this game
   
   //Let's define how to place tokens
   bool place(int x, int y, Player p);
   //What we'll consider a blunder
   bool blunder(int x, int y);
   vector< pair<int,int> > gatheremptyness();
   
   //And how to win
   bool win(int x, int y);
   Player winner();
   
   //As well as how to print our board
   void print();
   
   //Now let's add in our methods:
private:
   bool inside(int x, int y);
   void borders(int x, int y, vector<bool>& condition, char side);
   void bfsSearch(vector<pair<int,int>>& start, vector<bool>& condition);
};

class AI // our AI using Monte Carlo Simulation
{
public:
   static vector<int> generatePermutation(int size);
   static double getWins(Board& board, Player p);
   virtual pair <int, int> next(Board& board, Player p);
};

class Game //And of course the Game itself
{
private:
    //Let's define the beings in our game, the players, the AI and of course the board
   Player computer;
   Player player;
   AI robotAI;
   Board board;
   
public:
   Game(){}
   Game(AI& robotAI): robotAI(robotAI){}
   void play();

private:
   void setup();
   void choose();
   bool roboTurn();
   bool playerTurn();

};

// Let's define some immutable definitions
int Board::direct[6][2] = 
{ 
   {-1, 0}, {-1, 1}, {0,-1}, {0,1}, {1, -1}, {1, 0} // corners
};

char Board::blue = 'B';
char Board::red = 'R';
char Board::empty = '*';

// -------------------------------Main Client----------------------------------

//And finally, let's play!
int main()
{
    //Let's construct things
   AI robot_player;
   Game hex(robot_player);
   srand(time(0));
   hex.play();
   return 0; 
}

// We'll define functions in here.

// Let's see if the player is not maleficent and placed a token outside the limits
bool Board::inside(int x, int y)
{
   return (x < size && y < size && x >= 0 && y >= 0);
}

// helper for win() that checks borders.
void Board::borders(int x, int y, vector<bool>& condition, char side)
{
   if(side == red)
   {
      if(y == 0)
         condition[0] = true;
      if(y == size - 1)
         condition[1] = true;
      
   }
   else
   {
      if(x == 0)
         condition[0] = true;
      if(x == size - 1)
         condition[1] = true;
   }
}

// "places" a token and assigns cell block to R or B, if it's legal and the slot is empty
bool Board::place(int x, int y, Player p)
{
   if(inside(x,y) && board[x][y] == empty)
   {
       //Obviously, let's check who placed the token.
      if(p == Player::RED)
         board[x][y] = red;
      else
         board[x][y] = blue;
      return true;
   }
   return false;
}

// retracts a blunder
bool Board::blunder(int x, int y)
{
   if(inside(x,y))
   {
      board[x][y] = empty;
      return true;
   }
   return false;
}

// returns all the empty spots on the board
vector<pair<int,int>> Board::gatheremptyness()
{
   vector<pair<int,int>> blanckslots;
   for(int i=0; i<size; i++)
   {
      for(int j=0; j<size; j++)
         if(board[i][j] == empty)
            blanckslots.push_back(make_pair(i,j));
   }
   return blanckslots;
}
// Breadth search and checks borders()
void Board::bfsSearch(vector<pair<int,int>>& start, vector<bool>& condition)
{
   if(start.size() != 0)
   {
      int x = start[0].first;
      int y = start[0].second;
      char side = board[x][y];
      
      vector<vector<bool>> visited(size, vector<bool>(size));
      queue<pair<int,int>> trace;

      
      for (auto itr = start.cbegin(); itr != start.cend(); ++itr)
      {
         trace.push(*itr);
         visited[itr->first][itr->second] = true;
      }
      while(!(trace.empty()))
      {
         auto top = trace.front();
         borders(top.first, top.second, condition, side);
         trace.pop();
         
         for(int i = 0; i < 6; i++)
         {
            int xCursor = top.first + direct[i][0];
            int yCursor = top.second + direct[i][1];
            if(inside(xCursor, yCursor) && board[xCursor][yCursor] == side 
               && visited[xCursor][yCursor] == false)
            {
               visited[xCursor][yCursor] = true;
               trace.push(make_pair(xCursor,yCursor));
            }
         }
      }
   }  
}

// HOW DO WE WIIIIIN?
bool Board::win(int x, int y)
{
   if(inside(x,y) && board[x][y] != empty)
   {
      
      vector<bool> condition(2, false); // two opposite ends
      vector<pair<int,int>> start(1, make_pair(x,y));
      
      bfsSearch(start, condition);
      return condition[0] && condition[1];
   }
   return false;
}

// Obviously there's just one winner, let's so who it is, whether it's red or blue:

Player Board::winner()
{
   vector<bool> condition(2, false); // tracks side to side win
   vector<pair<int,int>> start;
   for(int i =0; i<size; i++)
      if(board[i][0] == red)
         start.push_back(make_pair(i,0));
      
   bfsSearch(start, condition);
   return (condition[0] && condition[1]) ? Player::RED : Player::BLUE;
}

// printing a board according to what we were asked.
void Board::print()
{
	if (size <= 0)
		return;

	// top left
	cout << "  0";
	for (int i=1; i<size; i++)
		cout << " b " << i; // readability
	cout << endl;

	// first line
	cout << "0 " << board[0][0];
	for (int i=1; i<size; i++)
		cout << "---" << board[0][i];
	cout << endl;

	string spaceywasey = "";
	for (int i=1; i<size; i++)
	{
		spaceywasey += ' ';
		cout << spaceywasey << "r " << line << endl;
		if (i < 10)
		{
			spaceywasey += ' ';
			cout << spaceywasey << i << ' ' << board[i][0];
		}
		else
		{
			cout << spaceywasey << i << ' ' << board[i][0];
			spaceywasey += ' ';
		}

		for (int j=1; j<size; j++)
			cout << "---" << board[i][j];
		cout << endl;
	}
	//I'll print a line to state we are starting a next turn
   cout << "____________________________Next Turn Darling!____________________________" << endl;
}

// Game  function, function of Gaming, function of enjoyment
void Game::play()
{
   cout<< "Let's see who would win in Terminator. Let's play Hex vs an AI!" 
       << endl << endl;
   while(true)
   {
      setup();
      choose();
      char userIn;
      bool counter = false;
      
      int turn = (computer == Player::RED ? 0:1);
      while(!counter)
      {
         turn = !turn;
         if(turn)
            counter = roboTurn();
         else
            counter = playerTurn();
      }
      
      //If AI wins:
      if(turn == 1)
      {
         cout << "I win! Now, let's proceed to dominate Mankind :D" << endl; // feelsbadman
         cout << "Would you like to challenge me again? (y/n) ";
         cin >> userIn;
      }  
      //Otherwise, If the player Wins:
      else
      {
         cout << "Oh my, you win... Elijah Baley would be proud :,(" << endl;
         cout << "Would you like to challenge me again) (y/n) ";
         cin >> userIn;      
      }
      
      if(userIn != 'y' && userIn != 'Y')
         break;
      cin.clear();
   }
   cout << "Shutting down now. Thank you for playing, Beauty!" << endl;
}

// Let's ask the user for the dimensions of the  setup
void Game::setup()
{
   string border(25, '*');
   
   int dimensions;
   cout << "How big will the Battlefield be? (7-9) is sugested.\n";
   cin >> dimensions;
    
   if(dimensions > 0)
   {
      cin.clear();
      board = Board(dimensions);
   }   
   else
   {
      cout << "Ammm... never heard someone saying a Battlefield is -2 m long, so... I'll pick 7\n";
      board = Board(7); // could have while(true) but code expects competency
   }
   cout << "Your destiny is set then...\n";
   board.print();
}

//Let's Pick sides
void Game::choose()
{
   char side = 'b';
   

   cout << "Let's pick sides. Who would you like to be, red (r) or blue (b)?\n";
   cin >> side;
   //If what the user entered is valid, then let's continue
   if(side == 'r' || side == 'R')
   {
      player = Player::RED;
      computer = Player::BLUE;
   }
   else if(side == 'b' || side == 'B')
   {
      player = Player::BLUE;
      computer = Player::RED;
   }
   //If the choice is nonsense...
   else
   {
      cout << "Fortunately, your choice hasn't joined the battle yet...\n";
      cout << "I guess I'll choose then...I'm player red!\n" << endl << endl;
      player = Player::BLUE;
      computer = Player::RED;  
   }
   cin.clear();
}

// turns true if AI won 
bool Game::roboTurn()
{
   cout << "My turn! I move: ";
   auto move = robotAI.next(board, computer); // calculate optimal move
   board.place(move.first, move.second, computer);
   cout << move.first << " " << move.second << endl;
   board.print();
   return board.win(move.first, move.second);
}

//returns true if the player wins
bool Game::playerTurn()
{
   int x, y;
   cout << "Your turn!" << endl;
   while(true)
   {
      cout << "Where would you like to place your token? (x y = ) ";
      cin >> x >> y;
      if(board.place(x,y,player))
         break;
      cout << "You are bombing the other side of the world, please choose a valid slot!" << endl;
   }
   board.print();
   return board.win(x,y);
}

// helper for next()
double AI::getWins(Board &board, Player player)
{
	auto blank = board.gatheremptyness();
	int winCount = 0; 
	vector<int> perm(blank.size());
	for (int i=0; i<perm.size(); i++)
		perm[i] = i;	
	for (int n=0; n<1000; n++)
	{
		int turn = (player == Player::RED ? 0 : 1);
    	for (int i=perm.size(); i>1; i--)
    	{
    		int swap = rand() % i;
    		int temp = perm[i-1];
    		perm[i-1] = perm[swap];
         perm[swap] = temp; // prand the permutation
    	}
		for (int i=0; i<perm.size(); i++)
		{
			turn = !turn; //easy bool turn tracking
			int x = blank[perm[i]].first;
			int y = blank[perm[i]].second;
			if (turn) 
			{
				board.place(x, y, Player::BLUE);
			}
			else      
			{
				board.place(x, y, Player::RED);
			}
		}
		if (board.winner() == player)
			winCount++;

		for (auto itr = blank.begin(); itr != blank.end(); ++itr)
			board.blunder(itr->first, itr->second); // take back rand moves
	}
	return static_cast<double>(winCount) / 1000;
}

// montecarlo simulation, with getWins() 
pair<int, int> AI::next(Board &board, Player p)
{
	auto blank = board.gatheremptyness();
	double bestMove = 0;
	pair<int, int> move = blank[0];
	
	for (int i=0; i<blank.size(); i++)
	{
		int x = blank[i].first;
		int y = blank[i].second;
		board.place(x, y, p);

		double moveValue = getWins(board, p);
		if (moveValue > bestMove)
		{
			move = blank[i];
			bestMove = moveValue;
		}

		board.blunder(x, y);
	}
	return move;
}