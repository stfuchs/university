/*
  This code is modification of the code written by Rich Sutton 12/17/00

  The code consists of two parts: 1). RL code; 2). MountainCar simulation code.
*/

#include <iostream>
#include <math.h>
#include "tiles.h"
#include "stdlib.h"

// This is Q=learning using Function Approximation: Tile-Coding
// For each action a: Q(a,s) = theta'*features(s)
// N is the number of features
#define N 3000            // number of parameters to theta, memory size
#define M 3               // number of actions


//Each tiling might consist of many tiles (see in tiles.h, and tiles.c)
#define NUM_TILINGS 10        // number of tilings in tile coding

// Global RL variables:
float Q[M];               // action values
float theta[N];             // modifyable parameter vector, aka memory, weights
float e[N];               // eligibility traces

//The features function: telling which tile in each tiling is active (binary features)
int F[M][NUM_TILINGS];        // sets of features, one for each action

// Standard RL parameters:
#define epsilon 0.0           // probability of random action
#define alpha 0.05           // step size parameter
#define lambda 0.9          // trace-decay parameters
#define gamma 1             // discount-rate parameters

// Profiles:
int episode(int max_steps);       // do one episode, return length
void load_Q();              // compute action values for current theta, F
int argmax(float Q[M]);       // compute argmax action from Q
bool with_probability(float p);     // helper - true with given probability
void load_F();            // compute feature sets for current state
void mcar_init();           // initialize car state
void mcar_step(int a);        // update car state for given action
bool mcar_goal_p ();        // is car at goal?


// The main program just does a bunch or runs, each consisting of some episodes.
// It prints out the length (number of steps) of each episode.
int main()
{
  for (int run=0; run<1; run++)
  {
    std::cout << "Beginning run #" << run << "\n";
    for (int i=0; i<N; i++) theta[i]= 0.0; // clear memory at start of each run
    for (int episode_num=0; episode_num<100; episode_num++)
      std::cout << episode(10000) <<  "\n";
  }
  return 0;
}

// Runs one episode of at most max_steps, returning episode length; see Figure 8.8 of RLAI book
int episode(int max_steps)
{
  mcar_init();                         // initialize car's state
  for (int i=0; i<N; i++) e[i] = 0.0;               // clear all traces
  load_F();                           // compute features
  load_Q();                           // compute action values
  int step = 0;                         // now do a bunch of steps
  int action = argmax(Q);                     // pick argmax action
  if (with_probability(epsilon)) action = rand() % M;       // ...or maybe pick action at random
  do
  {
    step++;
    // egilibility traces
    // update e[i]
    // note that: the feature of the current state is F[action][NUM_TILINGS]
    //homework
    //......................................
    for(int i=0; i<NUM_TILINGS; ++i)
    {
      e[F[action][i]] = 1;
    }


    //take action, observe new state, reward always = -1 when not terminal
    mcar_step(action);                    // actually take action

    float reward = -1;
    float delta = reward - Q[action];
    //std::cout <<"Delta: "<<delta;
    load_F();                         // compute features new state
    load_Q();                         // compute new state values
    //std::cout << Q[0] <<" | "<< Q[1] <<" | "<< Q[2] <<std::endl;

    action = argmax(Q);  // pick argmax action
    if (with_probability(epsilon)) action = rand() % M;  // ...or maybe pick action at random


    //compute TD error
    //homework
    delta = delta + gamma * Q[action];
    //std::cout << " => " << delta << std::endl;
    //......................................................


    // update theta (learn)
    //
    //homework
    for(int i=0; i<N; ++i)
    {
      //std::cout << alpha * delta * e[i] << std::endl;
      theta[i] = theta[i] + alpha * delta * e[i];
      e[i] = gamma * lambda * e[i];
    }
    //.......................................................


  }
  while (!mcar_goal_p() && step<max_steps);           // repeat until goal or time limit
  return step;// return episode length
}


// Compute all the action values from current F and theta
void load_Q()
{
  if (mcar_goal_p())
  {
    //if terminal state, then its terminal value = 0
    for (int a=0; a<M; a++) Q[a] = 0.0;
  }
  else
  {
    for (int a=0; a<M; a++)
    {
      //homework
      Q[a] = 0;
      for (int i=0; i<NUM_TILINGS; ++i)
      {
        Q[a] += theta[F[a][i]];

      };
      //....................................................
    }
  }
}

// Returns index (action) of largest entry in Q array, breaking ties randomly
int argmax(float Q[M])
{
  int best_action = 0;
  float best_value = Q[0];
  int num_ties = 1;                    // actually the number of ties plus 1
  for (int a=1; a<M; a++)
  {
    float value = Q[a];
    if (value >= best_value)
    {
      if (value > best_value)
      {
        best_value = value;
        best_action = a;
      }
      else
      {
        num_ties++;
        if (0 == rand() % num_ties)
        {
          best_value = value;
          best_action = a;
        }
      }
    }
  }
  return best_action;
}


// Returns TRUE with probability p
bool with_probability(float p)
{
  return p > ((float)rand()) / RAND_MAX;
}

///////////////  Mountain Car code begins here  ///////////////

// Mountain Car Global variables:
float mcar_position, mcar_velocity;

#define mcar_min_position -1.2
#define mcar_max_position 0.6
#define mcar_max_velocity 0.07      // the negative of this in the minimum velocity
#define mcar_goal_position 0.5

#define POS_WIDTH (1.7 / 8)             // the tile width for position
#define VEL_WIDTH (0.14 / 8)            // the tile width for velocity

// Compute feature sets for current car state
void load_F()
{
  float state_vars[2];
  state_vars[0] = mcar_position / POS_WIDTH;
  state_vars[1] = mcar_velocity / VEL_WIDTH;
  for (int a=0; a<M; a++)
    GetTiles(F[a],NUM_TILINGS,state_vars,2,N,a);
}

// Initialize state of Car
void mcar_init()
{
  mcar_position = -0.5;
  mcar_velocity = 0.0;
}

// Take action a, update state of car
void mcar_step(int a)
{
  mcar_velocity += (a-1)*0.001 + cos(3.0*mcar_position)*(-0.0025);
  if (mcar_velocity > mcar_max_velocity) mcar_velocity = mcar_max_velocity;
  if (mcar_velocity < -mcar_max_velocity) mcar_velocity = -mcar_max_velocity;
  mcar_position += mcar_velocity;
  if (mcar_position > mcar_max_position) mcar_position = mcar_max_position;
  if (mcar_position < mcar_min_position) mcar_position = mcar_min_position;
  if (mcar_position==mcar_min_position && mcar_velocity<0) mcar_velocity = 0;
  /*std::cout << "a=" << a
            << ", pos=" << mcar_position
            << ", vel=" << mcar_velocity << std::endl;*/
}

// Is Car within goal region?
bool mcar_goal_p ()
{
  return mcar_position >= mcar_goal_position;
}
