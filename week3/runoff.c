#include "cs50.h"
#include <stdio.h>
#include <string.h>

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
        if (strcmp(candidates[i].name, name) == 0)
        {
            // 投票者の選好配列に候補者のナンバーを保持
            preferences[voter][rank] = i;
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
            int candidate_num = preferences[i][j];

            // 候補者が落選していない場合
            if (!candidates[candidate_num].eliminated)
            {
                candidates[candidate_num].votes++;
                break; // 投票確定、次の投票者へ
            }
        }
    }
}

// Print the winner of the election, if there is one
bool print_winner(void)
{
    // 投票の過半数を算出
    int majority_votes = voter_count / 2 + 1;

    for (int i = 0; i < candidate_count; i++)
    {
        // 候補者の得票数が過半数越えで選挙の勝者が存在する場合
        if (candidates[i].votes > majority_votes)
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
    // 投票者数が投票数の最大値となるため、投票者数で初期化
    int min_votes = voter_count;

    for (int i = 0; i < candidate_count; i++)
    {
        // 現在保持している最低得票数を更新する場合 かつ 候補者が落選していない場合、最低得票数を更新
        if (candidates[i].votes < min_votes && !candidates[i].eliminated)
        {
            min_votes = candidates[i].votes;
        }
    }

    return min_votes;
}

// Return true if the election is tied between all candidates, false otherwise
bool is_tie(int min)
{
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
    for (int i = 0; i < candidate_count; i++)
    {
        // 落選していないすべての候補者において、最低得票数の候補者を排除
        if (candidates[i].votes == min && candidates[i].eliminated == false)
        {
            candidates[i].eliminated = true;
        }
    }
}
