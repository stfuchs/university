#include <stdlib.h>
#include <Ors/roboticsCourse.h>
#include <Gui/opengl.h>
//#include <MT/gauss.h>
#include <Gui/plot.h>


void getControlJacobian(arr& B, CarSimulator & S, const arr & u,const arr & x){
  double carLength = S.L;
  B.resize(3,2);
  //insert correct Jacobian here
  B(0,0) = cos(x(2));
  B(1,0) = sin(x(2));
  B(2,0) = 1./carLength * tan(u(1));
  B(0,1) = 0;
  B(1,1) = 0;
  B(2,1) = u(0)/(carLength*cos(u(1))*cos(u(1)));

  B *= S.tau;
}

//ok, works
void Kalman(){
  //double theta = 0.3;
  
  CarSimulator Sim;
  Sim.gl->watch();
  
  arr u(2),y_meassured;
  
  arr s(3),S(3,3); //kalman estimates
  arr shat,Shat,dW;
  
  s(0) = Sim.x; s(1) = Sim.y; s(2) = Sim.theta; //initial at true state
  S.setDiag(0.1); //which nosy constant
  
  arr W(4,4);  W.setDiag(Sim.observationNoise);
  arr Q(3,3);  Q.setDiag(Sim.dynamicsNoise);
  
  arr A(3,3),a;  A.setDiag(1);
  
  for(uint t=0;t<1000;t++){
    u = ARR(.1, .2); //control signal
    Sim.step(u);
    Sim.getRealNoisyObservation(y_meassured);
    
    //get the linear observation model
    arr C,y_mean;
    Sim.getObservationJacobianAtState(C, s);
    Sim.getMeanObservationAtState(y_mean, s);

    //get the linear control model
    arr B;
    getControlJacobian(B, Sim, u, s);
    a = B*u;
    shat = A*s + a;
    Shat = Q + A*S*~A;
    arr K = Shat*~C*inverse(W + C*Shat*~C);
    S = Shat - K*C*Shat;
    s = shat + K*(y_meassured - y_mean);
    //Kalman filter

    //code to display a covariance
    Sim.gaussiansToDraw.resize(1);
    Sim.gaussiansToDraw(0).A=S.sub(0,1,0,1);
    Sim.gaussiansToDraw(0).a=s.sub(0,1);
    
    //tracking error
    cout << "estim error " <<maxDiff(s, ARR(Sim.x, Sim.y, Sim.theta)) << endl;
  }
}



int main(int argc,char **argv){
  Kalman();
  return 0;
}
