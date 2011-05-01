#include <iostream>

char symbol[] = "123456789";

struct Cell
{
    bool    excluded[9];
    int     num_excluded : 5;
    bool    solved;
    int     solution : 5;
};

struct Row
{
    Cell    cell[9];
};

struct Board
{
    Row     row[9];
};

struct Found
{
    int     row;
    int     col;
    int     value;
};

namespace Propagating
{
    enum  Enum
    {
        Row,
        Col,
        Block,
    };
}

int sample0[] =
{
    0, 0, 1,
    0, 1, 5,
    0, 2, 3,
    0, 4, 7,
    1, 2, 9,
    1, 3, 5,
    1, 5, 3,
    1, 8, 8,
    2, 0, 4,
    2, 3, 2,
    2, 4, 9,
    2, 5, 6,
    2, 8, 3,
    3, 0, 7,
    3, 1, 8,
    3, 4, 6,
    3, 5, 4,
    3, 6, 3,
    3, 8, 9,
    4, 3, 3,
    4, 5, 2,
    4, 6, 4,
    4, 7, 6,
    5, 0, 6,
    5, 1, 3,
    5, 2, 4,
    5, 4, 5,
    5, 6, 1,
    6, 1, 4,
    6, 5, 5,
    6, 6, 9,
    6, 7, 2,
    7, 0, 9,
    7, 2, 7,
    7, 3, 8,
    7, 4, 4,
    7, 6, 6,
    7, 7, 3,
    8, 1, 1,
    8, 2, 5,
    8, 3, 6,
    8, 7, 7,

    0,0,0
};

int sample1[] =
{
    0, 0, 5,
    0, 3, 6,
    0, 6, 7,
    1, 0, 4,
    1, 2, 6,
    1, 4, 9,
    1, 7, 8,
    2, 1, 2,
    2, 4, 8,
    2, 7, 6,
    2, 8, 3,
    3, 5, 1,
    3, 8, 6,
    4, 2, 1,
    4, 5, 3,
    5, 0, 6,
    5, 3, 2,
    6, 0, 3,
    6, 1, 8,
    6, 4, 2,
    6, 7, 9,
    7, 1, 9,
    7, 4, 7,
    7, 6, 4,
    7, 8, 2,
    8, 2, 7,
    8, 5, 9,
    8, 8, 1,

    0,0,0
};

int sample2[] =
{
    0, 1, 3,
    0, 5, 8,
    0, 6, 4,
    0, 8, 5,
    1, 0, 5,
    1, 1, 7,
    1, 4, 6,
    1, 6, 8,
    2, 5, 5,
    2, 6, 6,
    3, 2, 5,
    3, 4, 8,
    4, 0, 8,
    4, 1, 9,
    4, 7, 4,
    4, 8, 2,
    5, 4, 1,
    5, 6, 3,
    6, 2, 6,
    6, 3, 7,
    7, 2, 3,
    7, 4, 5,
    7, 7, 6,
    7, 8, 4,
    8, 0, 7,
    8, 2, 2,
    8, 3, 3,
    8, 7, 5,

    0,0,0
};

int*    samples[]   = { sample0, sample1, sample2 };
int     num_samples = 3;

void Dump(Board& board)
{
    std::cout << std::endl;
    std::cout << "+===|===|===‖===|===|===‖===|===|===+" << std::endl;
    for (int row_block = 0; row_block < 3; ++row_block)
    {
        for (int row_block_cell = 0; row_block_cell < 3; ++row_block_cell)
        {
            for (int row_cell_value = 0; row_cell_value < 3; ++row_cell_value)
            {
                std::cout << "‖";
                for (int col_block = 0; col_block < 3; ++col_block)
                {
                    for (int col_block_cell = 0; col_block_cell < 3; ++col_block_cell)
                    {
                        for (int col_cell_value = 0; col_cell_value < 3; ++col_cell_value)
                        {
                            Row&    row     = board.row[(row_block*3) + row_block_cell];
                            Cell&   cell    = row.cell[(col_block*3) + col_block_cell];
                            if (cell.solved) 
                            {
                                if (col_cell_value == 1 && row_cell_value == 1)
                                    std::cout << symbol[cell.solution];
                                else
                                    std::cout << "◦";
                            }
                            else
                            {
                                int index   = (row_cell_value*3) + col_cell_value;
                                if (cell.excluded[index])
                                    std::cout << "x";
                                else
                                    std::cout << symbol[index];
                            }
                            //after each col_cell_value
                        } //after each col_block_cell
                        if (col_block_cell < 2) std::cout << "|";
                    } //after each col_block
                    std::cout << "‖";
                } //after each row_cell_value
                if (row_cell_value < 2) std::cout << std::endl;
            } //after each row_block_cell
            if (row_block_cell < 2) 
                std::cout << std::endl 
                    << "‖---|---|---‖---|---|---‖---|---|---‖" << std::endl;
        } //after each row_block
        std::cout << std::endl 
            << "+===|===|===‖===|===|===‖===|===|===+" << std::endl;
    }
}

void ExcludeCellValue(
        Board&      board, 
        Found*      found, 
        int&        num_found, 
        int&        num_solved,
        int         row_index, 
        int         col_index, 
        int         value, 
        const char* propagating)
{
    Row&        row             = board.row[row_index];
    Cell&       cell            = row.cell[col_index];

    if (cell.solved)            return;   //don't need to propagate to found cells

    if (cell.excluded[value])   return;   //current value is already excluded

    cell.excluded[value] = true;

    switch (++cell.num_excluded)
    {
        case 9:
        {
            Dump(board);

            std::cout << std::endl <<
                "Error: over-defined board detected while propagating "
                    "value " << value << " through " << propagating << " at "
                    "col " << col_index << " at "
                    "row " << row_index << " - all values excluded!" << std::endl;
            exit(1);
        }
        case 8:
        {   //found! what's the value?
            int value           = 0;
            for (; value < 9; ++value)
                if (!cell.excluded[value]) break;   //tcohen TODO: fix fragile logic

            //mark the cell solved
            cell.solution   = value;
            cell.solved     = true;

            //push the newly discovered found for propagation
            Found&  discovered  = found[num_found++];
            discovered.row  = row_index;
            discovered.col  = col_index;
            discovered.value    = value;

            ++num_solved;
            
            break;
        }
        default:
            break;
    }
}

int CollectGivens_Interactive(Board& board, Found* found)
{
    int         num_given   = 0;
    while (true)
    {
        Found&  given       = found[num_given];

        std::cout << "Row: ";   std::cin >> given.row;
        std::cout << "Col: ";   std::cin >> given.col;
        std::cout << "Value: "; std::cin >> given.value;
        std::cout << std::endl;

        if (given.value == 0) break;
        else ++num_given;

        //translate given value to symbol index!
        --given.value;

        Row&    row         = board.row[given.row];
        Cell&   cell        = row.cell[given.col];

        cell.solved         = true;
        cell.solution       = given.value;

        Dump(board);
    }

    return num_given;
}

int CollectGivens_Data(Board& board, Found* found, int* data)
{
    int         num_given   = 0;
    while (true)
    {
        Found&  given       = found[num_given];

        given.row           = data[num_given * 3];
        given.col           = data[num_given * 3 + 1];
        given.value         = data[num_given * 3 + 2];

        if (given.value == 0) break;
        else ++num_given;

        //translate given value to symbol index!
        --given.value;

        Row&    row         = board.row[given.row];
        Cell&   cell        = row.cell[given.col];

        cell.solved         = true;
        cell.solution       = given.value;
    }

    Dump(board);

    return num_given;
}

void Solve(Board& board, Found* found, int& num_found, int& num_solved)
{
    bool        dump        = false;

    char dummy;
    std::cout << std::endl << "Show steps? ('y') "; std::cin >> dummy;
    if (dummy == 'y') dump  = true;

    bool        run         = false;        //step until user says run

    //propagate found values as exclusions
    while (num_found)
    {
        Found   current     = found[--num_found];   //predecrement because exclusions may push found(s)

        //exclude current value from current col
        for (int row_index  = 0; row_index < 9; ++row_index)
        {
            if (row_index == current.row)       continue;   //don't propagate to current cell
            ExcludeCellValue(board, found, num_found, num_solved, row_index, current.col, current.value, "column");
        }

        //exclude current value from current row
        for (int col_index  = 0; col_index < 9; ++col_index)
        {
            if (col_index == current.col)       continue;   //don't propagate to current cell
            ExcludeCellValue(board, found, num_found, num_solved, current.row, col_index, current.value, "row");
        }

        //exclude current value from current block (by truncating row/col to nearest third)
        int     min_row     = (int)(current.row * 1/3) * 3;
        int     min_col     = (int)(current.col * 1/3) * 3;
        for (int row_index  = min_row; row_index < min_row + 3; ++row_index)
            for (int col_index = min_col; col_index < min_col + 3; ++col_index)
                if (row_index == current.row && col_index == current.col) continue; //don't propagate to current cell
                else
                    ExcludeCellValue(board, found, num_found, num_solved, row_index, col_index, current.value, "block");

        if (dump)
        {
            std::cout << std::endl << 
                "Propagated exclusions for found "
                    "row: "     << current.row << ", "
                    "col: "     << current.col << ", "
                    "value: "   << current.value << std::endl;

            Dump(board);

            if (!run)
            {
                char dummy;
                std::cout << "'r' to run, any other key to step. "; std::cin >> dummy;
                if (dummy == 'r') run = true;
            }
        }
    }

}
int main(int argc, char * const argv[])
{
    Board       board;
    memset(&board, 0, sizeof(Board));

    int         num_solved  = 0;

    Dump(board);

    // setup with givens

    Found found[81];
    int         num_found   = 0;

    char input;
    std::cout << std::endl 
        << "Select a sample (0-" << num_samples-1 << ") or 'i' to specify interactively: ";
    std::cin >> input;
    if (input == 'i')
    {
        std::cout << std::endl 
            << "For each given, input row, column, value.  Zero value to end and solve." << std::endl;
        num_found               = CollectGivens_Interactive(board, found);
    }
    else
    {
        char    array[]         = {input, 0};
        char*   string          = array;
        int     index           = atoi(string);
        num_found               = CollectGivens_Data(board, found, samples[index]);
    }
    num_solved                 += num_found;    //all givens are trivially found and solved

    // solve

    Solve(board, found, num_found, num_solved);
    
    Dump(board);

    if (num_solved < 81)
    {
        std::cout << std::endl << "Error: under-defined board detected.  I'm stuck!" << std::endl;
        return 1;
    }

    std::cout << std::endl << "Success!" << std::endl;

    return 0;
}
