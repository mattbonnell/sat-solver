// ECE 108 Lab 2 -- Sat Solver
//
// Read logical expressions in DIMACS format from files specified on the
// command line, and for each one print its name and one of "satisfiable",
// "unsatisfiable" or "tautology".

#include <stdio.h>
#include "dimacs_reader.h"

typedef struct BooleanExpression {
	int numvars;
	int numclauses;
	int ** clauses;
} BooleanExpression;

BooleanExpression * currentExpression;

int currentClauseIndex;

void init(int numvars, int numclauses) {
	// Callback function that gets passed to read_dimacs().
	// Indicates start of a new expression with up to numvars variables
	// and numclauses clauses.
	if(currentExpression != 0){
		free(currentExpression);
	}
	currentExpression = malloc(sizeof(*currentExpression));
	currentExpression->numvars = numvars;
	currentExpression->numclauses = numclauses;
	currentExpression->clauses = malloc(numclauses * sizeof(*(currentExpression->clauses)));
	currentClauseIndex = 0;
}

void add_clause(int numlits, int literals[]) {
	// Callback function that gets passed to read_dimacs().
	// The literals argument is an array of ints, numlits is its length.
	// Each literal is the index (starting from 1) of a variable.
	// Negative integers represent logical negations of variables.
	currentExpression->clauses[currentClauseIndex] = malloc((numlits + 1) * sizeof(*(currentExpression->clauses[currentClauseIndex])));
	currentExpression->clauses[currentClauseIndex][0] = numlits;
	int i;
	for(i = 1; i < numlits + 1; i++){
		currentExpression->clauses[currentClauseIndex][i] = literals[i];
	}
	currentClauseIndex++;
}

char *classify() {
	// Evaluate the current expression and return one of
	// "satisfiable", "unsatisfiable" or "tautology".
	int numVariables = currentExpression->numvars;
	int numClauses = currentExpression->numclauses;
	// int * truthValues = malloc(numVariables * sizeof(*truthValues));
	double temp = 1;
	int i;
	for(i = 0; i < numVariables; i++){
		temp *= 2;
	}
	unsigned long numPermutations = (unsigned long)(temp - 1);
	double truePermutations = 0;
	int permutation;
	for(permutation = 0; permutation <= numPermutations; permutation++){
		// for(int j = 0; j < numVariables; j++) {
		// 	truthValues[j] = i >> j & 1;
		// }
		int expressionIsTrue = 1;
		int k;
		for(k = 0; k < numClauses; k++){
			int * currentClause = currentExpression->clauses[0];
			int numlits = currentClause[0];
			int clauseIsTrue = 0;
			int l;
			for (l = 1; l < numlits + 1; l++){
				int negated = 0;
				int currentVar = currentClause[l];
				if (currentVar < 0){
					negated = 1;
					currentVar *= -1;
				}
				if (negated == 1){
					if((permutation >> (currentVar - 1)) & 1 == 0) {
						clauseIsTrue = 1;
						break;
					}
				} else {
					if((permutation >> (currentVar - 1)) & 1 == 1) {
						clauseIsTrue = 1;
						break;
					}
				}
			}
			if (clauseIsTrue == 0){
				expressionIsTrue = 0;
				break;
			}
		}
		if (expressionIsTrue == 1){
			truePermutations++;
		}
	}
	printf("\nNumber of permutations: %u True Permutations: %f\n", numPermutations, truePermutations);
	if (truePermutations == 0){
		return "unsatisfiable";
	} else if ((double)truePermutations - 1 == (double)numPermutations){
		return "tautology";
	} else {
		return "satisfiable";
	}
}

int main(int argc, char *argv[]) {
	int i;
	for (i = 1; i < argc; ++i) {
		read_dimacs(argv[i], init, add_clause);
		printf("%s: %s\n", argv[i], classify());
	}
	return 0;
}

