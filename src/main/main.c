// ECE 108 Lab 2 -- Sat Solver
//
// Read logical expressions in DIMACS format from files specified on the
// command line, and for each one print its name and one of "satisfiable",
// "unsatisfiable" or "tautology".

#include <stdio.h>
#include <stdlib.h>
#include "dimacs_reader.h"

typedef struct BooleanExpression {
	int numvars;
	int numclauses;
	int unsatisfiableByInspection;
	int ** clauses;
} BooleanExpression;

BooleanExpression * currentExpression;

int currentClauseIndex;

int * unaryClauses;
int numOfUnaryClauses;

void init(int numvars, int numclauses) {
	// Callback function that gets passed to read_dimacs().
	// Indicates start of a new expression with up to numvars variables
	// and numclauses clauses.
	if(currentExpression != 0){
		free(currentExpression);
		free(unaryClauses);
	}
	numOfUnaryClauses = 0;
	unaryClauses = malloc(100 * sizeof(*unaryClauses));
	currentExpression = malloc(sizeof(*currentExpression));
	currentExpression->numvars = numvars;
	currentExpression->numclauses = numclauses;
	currentExpression->clauses = malloc(numclauses * sizeof(*(currentExpression->clauses)));
	currentExpression->unsatisfiableByInspection = 0;
	currentClauseIndex = 0;
}

void add_clause(int numlits, int literals[]) {
	// Callback function that gets passed to read_dimacs().
	// The literals argument is an array of ints, numlits is its length.
	// Each literal is the index (starting from 1) of a variable.
	// Negative integers represent logical negations of variables.
	if(currentExpression->unsatisfiableByInspection){
		return;
	}
	if (numlits == 1){
		int i;
		for(i=0; i < numOfUnaryClauses; i++){
			if(unaryClauses[i] == -1 * literals[0]) {
				currentExpression->unsatisfiableByInspection = 1;
				return;
			}
		}
		unaryClauses[numOfUnaryClauses++] = literals[0];
	}
	currentExpression->clauses[currentClauseIndex] = malloc((numlits + 1) * sizeof(*(currentExpression->clauses[currentClauseIndex])));
	currentExpression->clauses[currentClauseIndex][0] = numlits;
	int i;
	for(i = 1; i < numlits + 1; i++){
		currentExpression->clauses[currentClauseIndex][i] = literals[i-1];
	}
	currentClauseIndex++;
}

char *classify() {
	// Evaluate the current expression and return one of
	// "satisfiable", "unsatisfiable" or "tautology".
	if (currentExpression->unsatisfiableByInspection){
		return "unsatisfiable";
	}
	int numVariables = currentExpression->numvars;
	int numClauses = currentExpression->numclauses;
	int numPermutations = 1 << numVariables;
	int truePermutations = 0;
	int falsePermutations = 0;
	int permutation;
	for(permutation = 0; permutation < numPermutations; permutation++){
		int expressionIsTrue = 1;
		int k;
		for(k = 0; k < numClauses; k++){
			int * currentClause = currentExpression->clauses[k];
			int numlits = currentClause[0];
			int clauseIsTrue = 0;
			int l;
			for (l = 1; l < numlits + 1; l++){
				int negated = 0;
				int currentVarIndex = currentClause[l];
				if (currentVarIndex < 0){
					negated = 1;
					currentVarIndex *= -1;
				}
				int currentVarValue = (permutation >> (currentVarIndex - 1)) & 1;
				if (negated == 1){
					if(currentVarValue == 0) {
						clauseIsTrue = 1;
						break;
					}
				} else {
					if(currentVarValue == 1) {
						clauseIsTrue = 1;
						break;
					}
				}
			}
			if (clauseIsTrue == 0){
				if(truePermutations > 0){
					return "satisfiable";
				}
				expressionIsTrue = 0;
				falsePermutations++;
				break;
			}
		}
		if (expressionIsTrue == 1){
			if(falsePermutations > 0){
				return "satisfiable";
			}
			truePermutations++;
		}
	}
	if (truePermutations == 0){
		return "unsatisfiable";
	} else if (truePermutations == numPermutations){
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

