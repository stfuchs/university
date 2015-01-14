#include <stdlib.h>
#include <Ors/roboticsCourse.h>
#include <Gui/opengl.h>

//resample a set of particles to become a set of unit-weight particles
void resample(arr& X, arr& W){
  uintA s;          // resample indices
  SUS(W,X.d0,s);    // Stochastic Universal Sampling
  arr Xnew(X.d0,3); // memorize old particles
  for(uint i=0;i<X.d0;i++) Xnew[i] = X[s(i)];
  X = Xnew;
  W = 1./X.d0;
}

void update(arr& X, arr const& u, double noise, double L = 1.)
{
  double l_inv = 1./L;
  for(uint i=0;i<X.d0;++i)
  {
    X(i,0) += u(0) * cos(X(i,2));
    X(i,1) += u(0) * sin(X(i,2));
    X(i,2) += u(0) * l_inv * tan(u(1));
  }
  rndGauss(X, noise, true);
}

void weight(arr& W, arr const& yt, arr const& yt_xt, double inv_sigma_sqr)
{
  W = arr(yt_xt.d0);
  double w_sum=0;
  for (uint i=0;i<yt_xt.d0;++i)
  {
    arr d = yt-yt_xt.row(i);
    W(i) = exp(-.5 * inv_sigma_sqr * (~d*d)(0) );
    w_sum += W(i);
  }
  if (w_sum == 0)
  {
    cout << "Warning: weights sum to 0, applying uniform weight" << endl;
    W = ones(yt_xt.d0)/double(yt_xt.d0);
  }
  else
    W = W/w_sum;
}

int main(int argc,char **argv){
  CarSimulator S;
  arr u(2),y_meassured;
  const uint N = 100;

  arr X = rand(N,3); for(uint i=0; i<N;++i) X(i,2) = X(i,2)*2.*M_PI - M_PI;
  //arr X = zeros(N,3);
  arr W = ones(N)/double(N);

  double sigma;
  if (argc == 1) sigma = .5;
  else sigma = atof(argv[1]);
  double inv_sigma_sqr_obs = 1./(sigma*sigma);
  //you have access to:
  //S.observationNoise (use when evaluating a particle likelihood)
  //S.dynamicsNoise (use when propagating a particle)
  S.particlesToDraw=X;
  S.gl->watch();
  for(uint t=0;t<1000;t++){
    u = ARR(.1, .2); //control signal
    S.step(u);
    S.getRealNoisyObservation(y_meassured);
    
    //1) resample weighted particles
    resample(X,W);
    //2) ``propagate'' each particle using the system dynamics (see internals of step function of CarSimulator)
    //   add noise to an array is done using   rndGauss(X, S.dynamicsNoise, true);
    update(X, u, S.dynamicsNoise, S.L);

    //3) compute the likelihood weights for each particle
    //   to evaluate the likelihood of the i-th particle use this:
    //   S.getTrueLandmarksInState(y_at_particle, X(i,0), X(i,1), X(i,2));
    //   and then compare to y_meassured (using a Gauss function with sdv 0.5) to compute the likelihood
    //   don't for get to normalize weights
    arr y_at_particles(N,y_meassured.d0);
    for (uint i=0; i<N; ++i)
    {
      arr y_i;
      S.getMeanObservationAtState( y_i, ARR( X(i,0),X(i,1),X(i,2) ) );
      for(uint j=0; j<y_meassured.d0; ++j) y_at_particles(i,j) = y_i(j);
    }

    weight(W,y_meassured, y_at_particles, inv_sigma_sqr_obs );
    //to draw some particles use this (X is a n-times-3 array storing the particles)
    arr x_true(1,3);
    x_true(0,0) = S.x;
    x_true(0,1) = S.y;
    x_true(0,2) = S.theta;
    S.particlesToDraw=X;
    
    //cout <<u <<endl <<y_meassured <<endl;
    if(argc==3 && std::string(argv[2])=="--pause") S.gl->watch();
  }
  return 0;
}

