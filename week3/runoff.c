#include "cs50.h"
#include <stdio.h>

// Max voters and candidates
#define MAX_VOTERS 100
#define MAX_CANDIDATES 9

// preferences[i][j] is jth preference for voter i
int preferences[MAX_VOTERS][MAX_CANDIDATES];

// Candidates have name, vote count, eliminated status
typedef struct
{
    string name;
    int votes;
    bool eliminated;
}
candidate;

// Array of candidates
candidate candidates[MAX_CANDIDATES];

// Numbers of voters and candidates
int voter_count;
int candidate_count;

// Function prototypes
bool vote(int voter, int rank, string name);
void tabulate(void);
bool print_winner(void);
int find_min(void);
bool is_tie(int min);
void eliminate(int min);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: runoff [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX_CANDIDATES)
    {
        printf("Maximum number of candidates is %i\n", MAX_CANDIDATES);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
        candidates[i].eliminated = false;
    }

    voter_count = get_int("Number of voters: ");
    if (voter_count > MAX_VOTERS)
    {
        printf("Maximum number of voters is %i\n", MAX_VOTERS);
        return 3;
    }

    // Keep querying for votes
    for (int i = 0; i < voter_count; i++)
    {

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            // Record vote, unless it's invalid
            if (!vote(i, j, name))
            {
                printf("Invalid vote.\n");
                return 4;
            }
        }

        printf("\n");
    }

    // Keep holding runoffs until winner exists
    while (true)
    {
        // Calculate votes given remaining candidates
        tabulate();

        // Check if election has been won
        bool won = print_winner();
        if (won)
        {
            break;
        }

        // Eliminate last-place candidates
        int min = find_min();
        bool tie = is_tie(min);

        // If tie, everyone wins
        if (tie)
        {
            for (int i = 0; i < candidate_count; i++)
            {
                if (!candidates[i].eliminated)
                {
                    printf("%s\n", candidates[i].name);
                }
            }
            break;
        }

        // Eliminate anyone with minimum number of votes
        eliminate(min);

        // Reset vote counts back to zero
        for (int i = 0; i < candidate_count; i++)
        {
            candidates[i].votes = 0;
        }
    }
    return 0;
}

// Record preference if vote is valid
bool vote(int voter, int rank, string name)
{
    for (int i = 0; i < candidate_count; i++)
    {
        // 投票者の入力が有効の場合
        if (candidates[i].name == name)
        {
            preferences[voter][rank] = rank;

            return true;
        }
    }

    // 投票者の入力が無効の場合
    return false;
}

// Tabulate votes for non-eliminated candidates
void tabulate(void)
{
    for (int i = 0; i < voter_count; i++)
    {
        for (int j = 0;  j < candidate_count; j++)
        {
            // 落選者には投票できないため、次点で落選していない最優先候補者の候補者に投票する
            if (candidates[preferences[i][j]].eliminated)
            {
                break;
            }

            // 落選していなければそのまま投票
            candidates[preferences[i][j]].votes++;
        }
    }
    return;
}

// Print the winner of the election, if there is one
bool print_winner(void)
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        // 候補者の得票数が過半数越えで選挙の勝者が存在する場合
        if (candidates[i].votes > (int) voter_count / 2)
        {
            printf("%s\n", candidates[i].name);

            return true;
        }
    }

    // 選挙の勝者がいない場合
    return false;
}

// Return the minimum number of votes any remaining candidate has
int find_min(void)
{
    // TODO
    int min_votes = 0;

    for (int i = 0; i < candidate_count; i++)
    {
        // 現在保持している最低得票数を更新する場合 かつ 候補者が落選していない場合、最低得票数を更新
        if (candidates[i].votes < min_votes && candidates[i].eliminated == false)
        {
            min_votes = candidates[i].votes;
        }
    }

    return min_votes;
}

// Return true if the election is tied between all candidates, false otherwise
bool is_tie(int min)
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        // 落選していないすべての候補者の投票数が同じでない場合
        if (candidates[i].votes != min && candidates[i].eliminated == false)
        {
            return false;
        }
    }

    // すべての候補者の投票数が同じである場合
    return true;
}

// Eliminate the candidate (or candidates) in last place
void eliminate(int min)
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        // 落選していないすべての候補者において、最低得票数の候補者を排除
        if (candidates[i].votes == min && candidates[i].eliminated == false)
        {
            candidates[i].eliminated = true;
        }
    }
    return;
}
