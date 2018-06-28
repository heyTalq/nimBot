

/**
 * @file NimLearner.cpp
 * CS 225 - Fall 2017
 */

#include "NimLearner.h"


/**
 * Constructor to create a game of Nim with `startingTokens` starting tokens.
 *
 * This function creates a graph, `g_` representing all of the states of a
 * game of Nim with vertex labels "p#-X", where:
 * - # is the current player's turn; p1 for Player 1, p2 for Player2
 * - X is the tokens remaining at the start of a player's turn
 *
 * For example:
 *   "p1-4" is Player 1's turn with four (4) tokens remaining
 *   "p2-8" is Player 2's turn with eight (8) tokens remaining
 *
 * All legal moves between states are created as edges with initial weights
 * of 0.
 *
 * @param startingTokens The number of starting tokens in the game of Nim.
 */
NimLearner::NimLearner(unsigned startingTokens) : g_(true) {

	bool startVert = false;
	for(int i = (int)startingTokens; i >= 0 ; --i){
		if(startVert == false){
			startingVertex_ =  g_.insertVertex("p1-" + std::to_string(startingTokens));
			startVert = true;
		}else{
		 g_.insertVertex("p1-" + std::to_string(i) );
		}
		 g_.insertVertex("p2-" + std::to_string(i));
	}


/*
	startingVertex_ =  g_.insertVertex("p1-" + std::to_string(startingTokens));
	g_.insertVertex("p2-" + std::to_string(startingTokens));
	for(int i = (int)startingTokens - 1; i >= 0 ; --i){
		g_.insertVertex("p1-" + std::to_string(i) );
		g_.insertVertex("p2-" + std::to_string(i));
	}
*/

	for(unsigned i = startingTokens; i >= 2; --i){
		g_.insertEdge(g_.getVertexByLabel("p1-" + std::to_string(i)), 
				g_.getVertexByLabel("p2-" + std::to_string(i-1)));

		g_.setEdgeWeight(g_.getVertexByLabel("p1-" + std::to_string(i)),
                                g_.getVertexByLabel("p2-" + std::to_string(i-1)), 0);

		g_.insertEdge(g_.getVertexByLabel("p1-" + std::to_string(i)),
				g_.getVertexByLabel("p2-" + std::to_string(i-2)));
		
		g_.setEdgeWeight(g_.getVertexByLabel("p1-" + std::to_string(i)),
                                g_.getVertexByLabel("p2-" + std::to_string(i-2)),0);

		g_.insertEdge(g_.getVertexByLabel("p2-" + std::to_string(i)),
				g_.getVertexByLabel("p1-" + std::to_string(i-1)));

		g_.setEdgeWeight(g_.getVertexByLabel("p2-" + std::to_string(i)),
                                g_.getVertexByLabel("p1-" + std::to_string(i-1)), 0);

		 g_.insertEdge(g_.getVertexByLabel("p2-" + std::to_string(i)),
                                g_.getVertexByLabel("p1-" + std::to_string(i-2)));

		g_.setEdgeWeight(g_.getVertexByLabel("p2-" + std::to_string(i)),
                                g_.getVertexByLabel("p1-" + std::to_string(i-2)), 0);
	}



	g_.insertEdge(g_.getVertexByLabel("p1-" + std::to_string(1)),
                                g_.getVertexByLabel("p2-" + std::to_string(0)));

	g_.setEdgeWeight(g_.getVertexByLabel("p1-" + std::to_string(1)),
                                g_.getVertexByLabel("p2-" + std::to_string(0)),0);
	
	g_.insertEdge(g_.getVertexByLabel("p2-" + std::to_string(1)),
                                g_.getVertexByLabel("p1-" + std::to_string(0)));		

	g_.setEdgeWeight(g_.getVertexByLabel("p2-" + std::to_string(1)),
                                g_.getVertexByLabel("p1-" + std::to_string(0)), 0);
}

/**
 * Plays a random game of Nim, returning the path through the state graph
 * as a vector of `Edge` classes.  The `origin` of the first `Edge` must be
 * the vertex with the label "p1-#", where # is the number of starting
 * tokens.  (For example, in a 10 token game, result[0].origin must be the
 * vertex "p1-10".)
 *
 * @returns A random path through the state space graph.
 */
std::vector<Edge> NimLearner::playRandomGame() const {
  vector<Edge> path;
	int currPos = (g_.getVertices().size() - 2) / 2;
//	std::cout<< g_.getVertices().size() << endl;
	int random;
	if(currPos <= 1){
		random = 1;
	}else{
		 random = rand() % 2 + 1;
	}
	bool p1First = false;
	path.push_back(g_.getEdge(g_.getVertexByLabel("p1-" + std::to_string(currPos)),
			g_.getVertexByLabel("p2-" + std::to_string(currPos - random))));
	currPos -= random;
	p1First = true;
	
	while(currPos != 0 ){
		if(currPos <= 1){
               		 random = 1;
	        }else{
              	 	random = rand() % 2 + 1;
       		 }
		
		if(p1First == true){
			path.push_back(g_.getEdge(g_.getVertexByLabel("p2-" + std::to_string(currPos)),
                        g_.getVertexByLabel("p1-" + std::to_string(currPos - random))));
			p1First = false;
		}else{
			 path.push_back(g_.getEdge(g_.getVertexByLabel("p1-" + std::to_string(currPos)),
                        g_.getVertexByLabel("p2-" + std::to_string(currPos - random))));
			p1First = true;
		}
		currPos -= random;
	}	
  return path;
}


/*
 * Updates the edge weights on the graph based on a path through the state
 * tree.
 *
 * If the `path` has Player 1 winning (eg: the last vertex in the path goes
 * to Player 2 with no tokens remaining, or "p2-0", meaning that Player 1
 * took the last token), then all choices made by Player 1 (edges where
 * Player 1 is the source vertex) are rewarded by increasing the edge weight
 * by 1 and all choices made by Player 2 are punished by changing the edge
 * weight by -1.
 *
 * Likewise, if the `path` has Player 2 winning, Player 2 choices are
 * rewarded and Player 1 choices are punished.
 *
 * @param path A path through the a game of Nim to learn.
 */
void NimLearner::updateEdgeWeights(const std::vector<Edge> & path) {
	bool p1Won = false;
	if(path.back().dest == g_.getVertexByLabel("p2-" + std::to_string(0))){
		p1Won = true;
	}
	
	int playerCount = 0;
	
	if(p1Won){
	for(int i = 0 ; i< (int)path.size(); i++){
		if(playerCount % 2 == 0){
			g_.setEdgeWeight(path[i].source, path[i].dest, path[i].weight + 1);
			playerCount++;
		}else{
			g_.setEdgeWeight(path[i].source, path[i].dest, path[i].weight - 1);
			playerCount++;
		}
		}
	}else{
		for(int i = 0 ; i< (int) path.size(); i++){
                if(playerCount	% 2 == 0){
			g_.setEdgeWeight(path[i].source, path[i].dest, path[i].weight - 1);
                        playerCount++;
                }else{
			g_.setEdgeWeight(path[i].source, path[i].dest, path[i].weight + 1);
                        playerCount++;
                }
		}
	}
	
}


/**
 * Returns a constant reference to the state space graph.
 *
 * @returns A constant reference to the state space graph.
 */
const Graph & NimLearner::getGraph() const {
  return g_;
}
