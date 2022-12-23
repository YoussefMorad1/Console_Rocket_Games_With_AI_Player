#include <iostream>
#include <sstream>
#include <conio.h>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

class Player {
protected:
    char symbol, side;
public:
    Player(char symbol, char side) : symbol(symbol), side(side) {
    };

    void set_symbol(char x) {
        symbol = x;
    }

    char get_symbol() {
        return symbol;
    }

    char get_side() {
        return side;
    }

    virtual char get_move(bool first = true);

    virtual bool is_computer() { return false; }
};

char Player::get_move(bool first) {
    if (first) {
        ostringstream s;
        s << "Press the number of " << (side == 'u' ? "column" : "row") << " to move your rocket..\n";
        cout << s.str();
    }
    return getch();
}


class Board {
    char **ptr, emptyChar = '.';
    int sz;
    vector<char> row_col_names;
    Player *p1, *p2; // player 1 UP, player2 LEFT
    int computer_player; // 0 -> no computer player , 1 -> player1 is computer , 2 -> player 2 is computer
    vector<vector<char>> myVec;

    char idx_to_row_symbol(int idx);

    int row_symbol_to_idx(char in);

    bool is_valid_row_name(char in);

    int get_cur_pos(char in, char side);

    char get_input(Player *p, bool first = true);

    void draw_init_board();

    void restart_game();

public:
    vector<vector<char>> &get_board_as_vector() {
        for (int i = 0; i < sz; ++i) {
            for (int j = 0; j < sz; ++j) {
                myVec[i][j] = ptr[i][j];
            }
        }
        return myVec;
    }

    void print();

    Board(int n = 5, int computer_player = 0, char symbol1 = 'v', char symbol2 = '>');

    vector<char> get_row_col_names() const {
        return row_col_names;
    }

    Player *get_player1() { // Up-side
        return p1;
    }

    Player *get_player2() {
        return p2;
    }

    // returns index of next move else returns -1
    int get_next_move(char in, char side);

    int get_backward_move(char in, char side);

    void make_move(Player *p, char ch, bool first = true);

    void undo_move(Player *p, char ch);

    bool is_winner(Player *p);

    bool found_valid_move(Player *p);

    void play();

    ~Board();
};


class Computer_Player : public Player {
    Board *board;

    map<vector<vector<char>>, pair<int, char>> mp[2];

    bool is_good_state(Player *, Player *);

    bool is_bad_state(Player *, Player *);

public:
    Computer_Player(char symbol, char side, Board *board) : Player(symbol, side), board(board) {
    }

    pair<int, char> find_good_move(Player *, Player *);

    virtual char get_move(bool first = true);

    virtual bool is_computer() { return true; }
};


/* old AI algorithm 

 bool Computer_Player::is_bad_state(Player *curPlayer, Player *opponent) {
    if (board->is_winner(opponent)) // extra condition (already handeled in is_good_state)
        return true;
    if (find_good_move(curPlayer, opponent) == '*')
        return true;
    return false;
}

bool Computer_Player::is_good_state(Player *curPlayer, Player *opponent) {
    if (board->is_winner(curPlayer))
        return true;
    else
        return is_bad_state(opponent, curPlayer);
}
char Computer_Player::find_good_move(Player *curPlayer, Player *opponent) {
    char& ret = mp[curPlayer->get_side()=='u'][board->get_board_as_vector()];
    if(ret != 0)
        return ret;

    vector<char> names = board->get_row_col_names();
    for (char &x: names) {
        // if no move can be done here continue
        if (board->get_next_move(x, curPlayer->get_side()) == -1)
            continue;

        board->make_move(curPlayer, x, true);
//        board -> print();
        if (is_good_state(curPlayer, opponent)) {
            board->undo_move(curPlayer, x);
            return ret =  x;
        }
        board->undo_move(curPlayer, x);
    }
    return ret = '*';
}

char Computer_Player::get_move(bool first) {
    Player *opponent;
    if (this->get_side() == 'u')
        opponent = board->get_player2();
    else
        opponent = board->get_player1();

    char res = find_good_move(this, opponent);

    if (res != '*')
        return res;

    // choose any valid move if no one is good
    vector<char> names = board->get_row_col_names();
    for (char &x: names) {
        if (board->get_next_move(x, side) != -1)
            return x;
    }
    // not reachable as I call this function only when a move is guaranteed
    return '*';
}

 */

char is_it[5][5] = {{'@', '.', '.', '.', '@'},
                    {'.', '>', 'v', 'v', '.'},
                    {'.', '.', '>', '.', '.'},
                    {'.', '.', '>', '.', '.'},
                    {'@', 'v', '.', '.', '@'}};

// choosing the best_move out of all no good moves
// getting the number of good_moves

// الفرق بين النود اللي هاخدها بين أقرانها
// وبين إزاي هقارن بينها وبين أقرانها
// the sum of ways to win from this state the bigger is better
// always maxmizing? or it's sum of children?

// choose the node with the "maximum" score "sum of children"

pair<int, char> Computer_Player::find_good_move(Player *curPlayer, Player *opponent) {
    pair<int, char> &ret = mp[curPlayer->get_side() == 'u'][board->get_board_as_vector()];
    if (ret.second != 0)
        return ret;

    if (board->is_winner(curPlayer))// || !board->found_valid_move(opponent))
        return ret = make_pair(1, '*');
    if (board->is_winner(opponent))// || !board->found_valid_move(curPlayer))
        return ret = make_pair(-1, '*');


    vector<char> names = board->get_row_col_names();
    int best_next_move_score = INT_MIN, current_state_score = 0;
    char next_move = 0;
    for (char &x: names) {
        // if no move can be done here continue
        if (board->get_next_move(x, curPlayer->get_side()) == -1)
            continue;
        board->make_move(curPlayer, x, true);
        /* checking a specific state
        bool ok = 1;
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                if(board->get_board_as_vector()[i][j] != is_it[i][j])
                    ok = 0;
            }
        }
        if(ok){
            board->print();
        }
        */
        int score = -1 * find_good_move(opponent, curPlayer).first;
        current_state_score += score;
        board->undo_move(curPlayer, x);
        if (score > best_next_move_score) {
            best_next_move_score = score;
            next_move = x;
        }
    }
    pair<int, char> res = make_pair(current_state_score, next_move);
//    res.first = current_state_score;
//    res.second = (next_move == 0 ? '*' : next_move);
    return ret = res;
}

char Computer_Player::get_move(bool first) {
    Player *opponent;
    if (this->get_side() == 'u')
        opponent = board->get_player2();
    else
        opponent = board->get_player1();

    pair<int, char> res = find_good_move(this, opponent);

    return res.second;

//    // choose any valid move if no one is good
//    vector<char> names = board->get_row_col_names();
//    for (char &x: names) {
//        if (board->get_next_move(x, side) != -1)
//            return x;
//    }
//    // not reachable as I call this function only when a move is guaranteed
//    return '*';
}

void Board::draw_init_board() {
    for (int i = 0; i < sz; ++i) {
        for (int j = 0; j < sz; ++j) {
            ptr[i][j] = emptyChar;
            if (i == 0 && j > 0 && j < sz - 1)
                ptr[i][j] = p1->get_symbol();
            if (j == 0 && i > 0 && i < sz - 1)
                ptr[i][j] = p2->get_symbol();
        }
    }
    ptr[0][0] = ptr[0][sz - 1] = ptr[sz - 1][sz - 1] = ptr[sz - 1][0] = '@';
}

Board::Board(int n, int computer_player, char symbol1, char symbol2)
        : sz(n), computer_player(computer_player) {
    if (n > 64) {
        throw invalid_argument("Too Big Size of Board!");
    }
    ptr = new char *[n];
    for (int i = 0; i < n; ++i) {
        ptr[i] = new char[n];
    }

    myVec.resize(sz);
    for (int i = 0; i < sz; ++i) {
        myVec[i].resize(sz);
    }

    if (computer_player == 1 || computer_player == 3)
        p1 = new Computer_Player(symbol1, 'u', this);
    else
        p1 = new Player(symbol1, 'u');

    if (computer_player == 2 || computer_player == 3)
        p2 = new Computer_Player(symbol2, 'l', this);
    else
        p2 = new Player(symbol2, 'l');

    // Draw the initial Board
    draw_init_board();

    // Create the name of rows and columns
    for (int i = 1; i <= n - 2; ++i) {
        row_col_names.push_back(idx_to_row_symbol(i));
    }

}

Board::~Board() {
    delete p1;
    delete p2;
    for (int i = 0; i < sz; ++i) {
        delete[] ptr[i];
    }
    delete[] ptr;
}

char Board::idx_to_row_symbol(int idx) {
    if (idx <= 10)
        // from real index to name      ->     idx - 10 + 'a'
        return (idx + '0' - 1);
    else if (idx <= 36)
        // from real index to name      ->     idx - 10 + 'a'
        return (idx - 11 + 'a');
    else if (idx <= 62)
        // from real index to name      ->     idx - 10 + 'a'
        return (idx - 37 + 'A');
    else
        throw invalid_argument("Out Of boundaries index to idx_to_row_symbol function!");
}

int Board::row_symbol_to_idx(char in) {
    int idx = -1;
    if (isdigit(in))
        // from name to real index       ->      name + '0'
        idx = in - '0' + 1;
    else if (islower(in))
        // from name to real index       ->      name - 'a' + 10
        idx = in - 'a' + 11;
    else if (isupper(in))
        // from name to real index       ->      name - 'A' + 36
        idx = in - 'A' + 37;
    return idx;
}

bool Board::is_valid_row_name(char in) {
    return find(row_col_names.begin(), row_col_names.end(), in) != row_col_names.end();
}

int Board::get_cur_pos(char in, char side) {
    int idx = row_symbol_to_idx(in);
    if (side == 'u') {
        char sym = p1->get_symbol();
        for (int i = 0; i < sz; ++i) {
            if (ptr[i][idx] == sym) {
                return i;
            }
        }
    } else {
        char sym = p2->get_symbol();
        for (int i = 0; i < sz; ++i) {
            if (ptr[idx][i] == sym) {
                return i;
            }
        }
    }
}

int Board::get_next_move(char in, char side) {
    int idx = row_symbol_to_idx(in);
    if (side == 'u') {
        int curPos = get_cur_pos(in, side);
        if (curPos + 1 < sz && ptr[curPos + 1][idx] == emptyChar)
            return curPos + 1;
        if (curPos + 2 < sz && ptr[curPos + 2][idx] == emptyChar)
            return curPos + 2;
    } else {
        int curPos = get_cur_pos(in, side);
        if (curPos + 1 < sz && ptr[idx][curPos + 1] == emptyChar)
            return curPos + 1;
        if (curPos + 2 < sz && ptr[idx][curPos + 2] == emptyChar)
            return curPos + 2;
    }
    return -1;
}

int Board::get_backward_move(char in, char side) {
    int idx = row_symbol_to_idx(in);
    if (side == 'u') {
        int curPos = get_cur_pos(in, side);
        if (curPos - 1 >= 0 && ptr[curPos - 1][idx] == emptyChar)
            return curPos - 1;
        if (curPos - 2 >= 0 && ptr[curPos - 2][idx] == emptyChar)
            return curPos - 2;
    } else {
        int curPos = get_cur_pos(in, side);
        if (curPos - 1 >= 0 && ptr[idx][curPos - 1] == emptyChar)
            return curPos - 1;
        if (curPos - 2 >= 0 && ptr[idx][curPos - 2] == emptyChar)
            return curPos - 2;
    }
    return -1;
}

void Board::print() {

    // print the name of every row
    cout << string(4, ' ');
    for (const char &x: row_col_names)
        cout << x << ' ';
    cout << endl;

    for (int i = 0; i < sz; ++i) {

        // print name of every column from 1 to n-2
        if (0 < i && i < sz - 1)
            cout << row_col_names[i - 1] << ' ';
        else
            cout << ' ' << ' ';

        // print the board content
        for (int j = 0; j < sz; ++j) {
            cout << ptr[i][j] << ' ';
        }
        cout << endl;
    }
    cout << endl;
}

bool Board::is_winner(Player *p) {
    if (p->get_side() == 'u') {
        for (int i = 1; i <= sz - 2; ++i) {
            if (ptr[sz - 1][i] != p->get_symbol())
                return false;
        }
    } else {
        for (int i = 1; i <= sz - 2; ++i) {
            if (ptr[i][sz - 1] != p->get_symbol())
                return false;
        }
    }
    return true;
}


char Board::get_input(Player *p, bool first) {
    if (!found_valid_move(p))
        return '*';
    char ch = p->get_move(first);
    if (!is_valid_row_name(ch)) {
        if (first)
            cout << "Invalid " << (p->get_side() == 'u' ? "Column" : "Row") << " Name!, Enter a valid one..\n";
        return get_input(p, false);
    }
    return ch;
}

bool Board::found_valid_move(Player *p) {
    for (char &x: get_row_col_names()) {
        if (get_next_move(x, p->get_side()) != -1)
            return true;
    }
    return false;
}

void Board::make_move(Player *p, char ch, bool first) {
    int idx = row_symbol_to_idx(ch);
    int curPos = get_cur_pos(ch, p->get_side());
    int nextMove = get_next_move(ch, p->get_side());
    if (nextMove == -1) {
        // code goes here only with Real player not Computer
        if (first)
            cout << "This stone can't be moved!, Choose another one..\n";
//        cout << "chosen: " << ch << " " << p->get_side() << endl;
        make_move(p, get_input(p, false), false);
        return;
    }
    if (p->get_side() == 'u') {
        swap(ptr[nextMove][idx], ptr[curPos][idx]);
    } else {
        swap(ptr[idx][nextMove], ptr[idx][curPos]);
    }
}

void Board::undo_move(Player *p, char ch) {
    int idx = row_symbol_to_idx(ch);
    int curPos = get_cur_pos(ch, p->get_side());
    int backPos = get_backward_move(ch, p->get_side());
    if (p->get_side() == 'u') {
        swap(ptr[backPos][idx], ptr[curPos][idx]);
    } else {
        swap(ptr[idx][backPos], ptr[idx][curPos]);
    }
}


void Board::restart_game() {
    draw_init_board();

    this->play();
}


void Board::play() {
    vector<Player *> players = {p1, p2};
    while (true) {
        system("cls");
        print();
        for (Player *p: players) {
            cout << "Player " << (p->get_side() == 'u' ? "1 v" : "2 >") << " Turn..\n";
            char ch = get_input(p);
            if (ch == '*') {
                cout << "No Valid Move for " << "Player " << (p->get_side() == 'u' ? "1 v" : "2 >")
                     << ".. This turn will be skipped\n";
                _sleep(350);
                continue;
            }
            make_move(p, ch);
            system("cls");
            print();
            if (is_winner(p)) {
                cout << "\nPlayer " << (p->get_side() == 'u' ? "1" : "2") << " Won!\n";
                cout << "Do you want to restart? choose (y or n)" << endl;
                char c = tolower(getch());
                if (c == 'y')
                    restart_game();
                return;
            }
        }
    }
}

// note when you put AI with no valid moves it crashes
int main() {
    Board game(5, 1);
    game.play();
}
