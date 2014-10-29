#include <Ors/roboticsCourse.h>


void simpleArrayOperations(){
  arr x;
  x = ARR(.1, .2, .3);         //directly setting the array
  cout <<"x = " <<x <<endl;

  x += ARR(2., 2., 2.);        //adding to an array
  cout <<"x = " <<x <<endl;

  x *= 1.;
  cout <<"x = " <<x <<endl;

  arr y = ARR(-.3,-.2,-.1);
  y.append(x);                 //appending a vector to a vector
  cout <<"y = " <<y <<endl;

  arr M(4,3);                  //some 3 x 4 matrix
  M[0] = ARR(1, 0, 0);         //setting the first row
  M[1] = ARR(0, 1, 0);
  M[2] = ARR(0, 0, 1);
  M[3] = ARR(1, 0, 0);

  cout <<"M =\n" <<M <<endl;

  cout <<"transpose M =\n" <<~M <<endl;  //matrix transpose

  cout <<"M*x = " <<M*x <<endl;          //matrix-vector product

  cout <<"M^-1 =\n" <<inverse(M) <<endl; //matrix inverse

  M.append(M);                 //appending a matrix to a matrix
  cout <<"M =\n" <<M <<endl;
}


void openingSimulator(){
  Simulator S("man.ors");
  cout <<"joint dimensions = " <<S.getJointDimension() <<endl;

  cout <<"initial posture (hit ENTER in the OpenGL window to continue!!)" <<endl;
  S.watch();        //pause and watch initial posture

  arr q;
  S.getJointAngles(q);

  q(0) += 0.1;                 //change the first entry of q-vector
  S.setJointAngles(q);
  S.watch();
  
  q = 0.;                      //set q-vector equal zero
  S.setJointAngles(q);
  S.watch();
}


void reach(){
  Simulator S("man.ors");
  arr q,W;
  uint n = S.getJointDimension();
  S.getJointAngles(q);
  W.setId(n);  //W is equal the Id_n matrix

  cout <<"initial posture (hit ENTER in the OpenGL window to continue!!)" <<endl;
  S.watch();        //pause and watch initial posture

  arr y_target,y,yt,y0,J;
  arr X(100,2);
  S.kinematicsPos(y0,"handR");
  for(uint i=0;i<100;i++){
    //1st task:
    y_target = ARR(-0.2, -0.4, 1.1);
    S.kinematicsPos(y,"handR");  //"handR" is the name of the right hand ("handL" for the left hand)
    S.jacobianPos  (J,"handR");

    X(i,0) = i;
    X(i,1) = length(y_target - y);
    //cout << "task error: " <<  << endl;
    yt = y0 + (float(i)/100.)*(y_target-y0);
    //compute joint updates
    q += .1 * inverse(~J*J + 1e-4*W)*~J*(yt-y);//(y_target - y); 
    //NOTATION: ~J is the transpose of J
    //the 1e-4 corresponds to C=1e4*Id
    
    //sets joint angles AND computes all frames AND updates display
    S.setJointAngles(q);

    //optional: pause and watch OpenGL
    S.watch();
  }
  gnuplot(X);
}


int main(int argc,char **argv){
  MT::initCmdLine(argc,argv);

  switch(MT::getParameter<int>("mode",2)){
  case 0:
    cout << "Mode: simpleArrayOperations()" << endl;
    simpleArrayOperations();
    break;
  case 1:
    cout << "Mode: openingSimulator()" << endl;
    openingSimulator();
    break;
  case 2:
    cout << "Mode: reach()" << endl;
    reach();
    break;
//  case 3:  circle();  break;
//  case 4:  multiTask();  break;
  }

  return 0;
}
